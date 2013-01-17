/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: MoveFileExA.cpp,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: rt $ $Date: 2005/09/08 16:21:23 $
 *
 *  The Contents of this file are made available subject to
 *  the terms of GNU Lesser General Public License Version 2.1.
 *
 *
 *    GNU Lesser General Public License Version 2.1
 *    =============================================
 *    Copyright 2005 by Sun Microsystems, Inc.
 *    901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License version 2.1, as published by the Free Software Foundation.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *    MA  02111-1307  USA
 *
 ************************************************************************/

//#include "macros.h"
#include <windows.h>
#include <common.h>
#include "_kernel32_apilist.h"
#include "k32ord.h"

#define	WININIT_FILENAME	"wininit.ini"
#define RENAME_SECTION		"rename"
#define BUFSIZE				32767

#define RELTOABS(x) ( (DWORD)(x) + *(DWORD*)(x) + 4 )
#define MAPHANDLE_MAPPED 0x1000

/*
 *	A bit of mapping file 9x rules:
 *	copy-on-write access is possible to file mappings created with 
 *	PAGE_WRITECOPY. Note that it makes sense only to real files.
 *	If you CreateFileMapping on INVALID_HANDLE_VALUE with PAGE_WRITECOPY
 *	you will get mapping handle which can be only mapped with FILE_MAP_READ
 *	PAGE_READONLY can be mapped only with FILE_MAP_READ, end of story
 *	PAGE_READWRITE can be mapped with any FILE_MAP_READ or FILE_MAP_WRITE
 *	but not FILE_MAP_COPY; however FILE_MAP_ALL_ACCESS is fine
 *	kernel checks exactly for it, other masks with FILE_MAP_COPY will fail.
 *
 *	Office 2003 workaround is hack for specific access mask which breaks
 *	that FILE_MAP_COPY rule
 */

#define FILE_MAP_OFFICE2003 (SECTION_QUERY | SECTION_MAP_WRITE | \
		SECTION_MAP_READ |  SECTION_MAP_EXECUTE | \
		SECTION_EXTEND_SIZE | READ_CONTROL)

#ifndef INVALID_SET_FILE_POINTER
#define INVALID_SET_FILE_POINTER	((DWORD)-1)
#endif

static DWORD FindSharedMem64K(DWORD StartAddress)
{
	MEMORY_BASIC_INFORMATION mbi;
	DWORD TryAddress = StartAddress;
	while (true)
	{
		if (TryAddress & 0x0FFFF) //64K align
			TryAddress = (TryAddress & 0xFFFF0000) + 0x10000;
		if ( !VirtualQuery((PVOID)TryAddress,&mbi,sizeof(mbi)) ) return NULL;
		if (mbi.State == MEM_FREE)
			return TryAddress;
		TryAddress = (DWORD)mbi.BaseAddress + (DWORD)mbi.RegionSize;
	}	
}

//MapHandle: maps kernel handle to object
static PVOID MapHandle(HANDLE hObject, DWORD dwFlags, DWORD dwReserved)
{
	typedef PDWORD (WINAPI *MapHandle_t)(HANDLE,DWORD,DWORD);
	static MapHandle_t KernelMapHandle;
	if (!KernelMapHandle)
		KernelMapHandle = (MapHandle_t)RELTOABS((DWORD)kexGetProcAddress(
					GetModuleHandle("KERNEL32.DLL"),"OpenVxDHandle")+0xC);
	return KernelMapHandle(hObject,dwFlags,dwReserved);
}

static LPVOID GetMapViewAddress(HANDLE hFileMapping)
{
	PVOID ret = (PVOID)-1;
	PFILEMAPPING mapobj;
	static CRITICAL_SECTION* k32lock;
	if (!k32lock)
		k32lock = (CRITICAL_SECTION*) kexGetK32Lock();
	_EnterSysLevel(k32lock);
	mapobj = (PFILEMAPPING)MapHandle(hFileMapping,MAPHANDLE_MAPPED,0);
	if (mapobj && mapobj->type == WIN98_K32OBJ_MEM_MAPPED_FILE)
		ret = mapobj->mapaddr;
	_LeaveSysLevel(k32lock);
	return ret;
}

/* MAKE_EXPORT CopyFileExA_new=CopyFileExA */
BOOL WINAPI CopyFileExA_new(LPCSTR lpExistingFileNameA, LPCSTR lpNewFileNameA, LPPROGRESS_ROUTINE lpProgressRoutine, LPVOID lpData, LPBOOL pbCancel, DWORD dwCopyFlags)
{
	return CopyFileA(lpExistingFileNameA, lpNewFileNameA, 
			(dwCopyFlags & COPY_FILE_FAIL_IF_EXISTS) != 0);
}

/* MAKE_EXPORT CreateFileA_fix=CreateFileA */
HANDLE WINAPI CreateFileA_fix(LPCSTR lpFileName, DWORD dwDesiredAccess, 
	DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, 
	DWORD dwCreationDistribution, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
	/* translate NT-specific access rights to generic ones */
	if (dwDesiredAccess & ~0xf0010100)
	{
		DWORD oldaccess = dwDesiredAccess;
		dwDesiredAccess &= 0xf0010100;
		if (oldaccess & FILE_READ_DATA)
			dwDesiredAccess |= GENERIC_READ;
		if (oldaccess & FILE_WRITE_DATA)
			dwDesiredAccess |= GENERIC_WRITE;
		if (oldaccess & FILE_EXECUTE)
			dwDesiredAccess |= GENERIC_EXECUTE;
	}
	//FILE_SHARE_DELETE is not supported
	dwShareMode &= ~FILE_SHARE_DELETE;
	// hTemplate has to be NULL on 9x
	hTemplateFile = NULL;
	// special case: overlapped I/O
	if (dwFlagsAndAttributes & FILE_FLAG_OVERLAPPED)
	{
		DWORD lasterr = GetLastError();
		HANDLE h;
		h = CreateFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, 
				dwCreationDistribution, dwFlagsAndAttributes, hTemplateFile);
		if (h != INVALID_HANDLE_VALUE)
			return h;
		SetLastError(lasterr);
		dwFlagsAndAttributes &= ~FILE_FLAG_OVERLAPPED;
	}
	return CreateFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, 
		dwCreationDistribution, dwFlagsAndAttributes, hTemplateFile);
}

/* MAKE_EXPORT FindFirstFileExA_new=FindFirstFileExA */
HANDLE WINAPI FindFirstFileExA_new( LPCSTR lpFileNameA, FINDEX_INFO_LEVELS fInfoLevelId, 
					   LPWIN32_FIND_DATAA lpFindFileDataA, FINDEX_SEARCH_OPS fSearchOp,
					   LPVOID lpSearchFilter, DWORD dwAdditionalFlags)
{
	return FindFirstFileA(lpFileNameA,lpFindFileDataA);
}

/* MAKE_EXPORT FindFirstFileExW_new=FindFirstFileExW */
HANDLE WINAPI FindFirstFileExW_new( LPWSTR lpFileNameW, FINDEX_INFO_LEVELS fInfoLevelId, 
					   LPWIN32_FIND_DATAW lpFindFileDataW, FINDEX_SEARCH_OPS fSearchOp,
					   LPVOID lpSearchFilter, DWORD dwAdditionalFlags)
{	
	return FindFirstFileW_new(lpFileNameW,lpFindFileDataW);
}

/* MAKE_EXPORT GetDiskFreeSpaceA_fix=GetDiskFreeSpaceA */
BOOL WINAPI GetDiskFreeSpaceA_fix(LPCSTR lpRootPathName, LPDWORD lpSectorsPerCluster, 
								  LPDWORD lpBytesPerSector, LPDWORD lpNumberOfFreeClusters,
								  LPDWORD lpTotalNumberOfClusters)
{
	char newRootPath[4] = { "X:\\" };
	if (lstrlenA(lpRootPathName) == 2 && lpRootPathName[1] == ':') //GetDiskFreeSpace fails on paths like C:
	{
		newRootPath[0] = lpRootPathName[0];
		lpRootPathName = newRootPath;
	}
	BOOL ret = GetDiskFreeSpaceA(lpRootPathName,lpSectorsPerCluster,lpBytesPerSector,lpNumberOfFreeClusters,lpTotalNumberOfClusters);
	if (!ret) //more suprisingly, it may fail on paths with two extensions (e.g. c:\game.exe.lnk)
	{
		char shortPath[MAX_PATH];
		if (GetShortPathName(lpRootPathName,shortPath,MAX_PATH))
		{
			//GetDiskFreeSpace will still fail on short path name, so we check drive root, having path validated
			newRootPath[0] = shortPath[0];
			ret = GetDiskFreeSpaceA(newRootPath,lpSectorsPerCluster,lpBytesPerSector,lpNumberOfFreeClusters,lpTotalNumberOfClusters);
		}
	}
	return ret;
}

/* MAKE_EXPORT GetDiskFreeSpaceExA_fix=GetDiskFreeSpaceExA */
BOOL WINAPI GetDiskFreeSpaceExA_fix(LPCSTR lpDirectoryName, PULARGE_INTEGER lpFreeBytesAvailable, 
									PULARGE_INTEGER lpTotalNumberOfBytes, PULARGE_INTEGER lpTotalNumberOfFreeBytes)
{
	//GetDiskFreeSpaceEx does not fail on paths like C: but still fails on on paths with two extensions
	BOOL ret = GetDiskFreeSpaceExA(lpDirectoryName,lpFreeBytesAvailable,lpTotalNumberOfBytes,lpTotalNumberOfFreeBytes);
	if (!ret)
	{
		char shortPath[MAX_PATH];
		if (GetShortPathName(lpDirectoryName,shortPath,MAX_PATH))
			ret = GetDiskFreeSpaceExA(shortPath,lpFreeBytesAvailable,lpTotalNumberOfBytes,lpTotalNumberOfFreeBytes);
	}
	return ret;
}

/* MAKE_EXPORT GetFileSizeEx_new=GetFileSizeEx */
BOOL WINAPI GetFileSizeEx_new(HANDLE hFile, PLARGE_INTEGER lpFileSize)
{
	DWORD h, l, lasterr;
	lasterr = GetLastError();
	l = GetFileSize(hFile, &h);
	if (l == INVALID_FILE_SIZE && GetLastError() != NO_ERROR) return FALSE;
	else 
	{
		lpFileSize->HighPart = h;
		lpFileSize->LowPart = l;
	}
	SetLastError(lasterr);
	return TRUE;
}

/* MAKE_EXPORT GetTempFileNameA_fix=GetTempFileNameA */
UINT WINAPI GetTempFileNameA_fix(LPCSTR lpPathName, LPCSTR lpPrefixString, UINT uUnique, LPTSTR lpTempFileName)
{
	static int g_tempprefix = 0;
	char temppref[2];

	if (!lpPathName) 
		lpPathName = "\\";
	if (!lpPrefixString)
	{
		g_tempprefix++;
		g_tempprefix %= 5;
		temppref[0] = '0' + g_tempprefix;
		temppref[1] = '\0';
		lpPrefixString = temppref;
	}
	return GetTempFileNameA(lpPathName,lpPrefixString,uUnique,lpTempFileName);
}

/* MAKE_EXPORT GetVolumePathNameA_new=GetVolumePathNameA */
BOOL WINAPI GetVolumePathNameA_new(LPCSTR filename, LPSTR volumepathname, DWORD buflen)
{
	//semi-stub - doesn't work with UNC
	const char *p = filename;

	if (p && tolower(p[0]) >= 'a' && tolower(p[0]) <= 'z' && p[1] ==':' && p[2] == '\\' && buflen >= 4)
	{
		volumepathname[0] = p[0];
		volumepathname[1] = ':';
		volumepathname[2] = '\\';
		volumepathname[3] = 0;
		return TRUE;
	}
	return FALSE;
}

/* MAKE_EXPORT GetVolumePathNameW_new=GetVolumePathNameW */
BOOL WINAPI GetVolumePathNameW_new(LPCWSTR filenameW, LPWSTR volumepathnameW, DWORD buflen)
{
	BOOL ret;
	file_GetCP();
	file_ALLOC_WtoA(filename);
	ALLOC_A(volumepathname, buflen);
	ret = GetVolumePathNameA_new(filenameA, volumepathnameA, buflen);
	if (ret)
		file_ABUFtoW(volumepathname, -1, buflen);
	return ret;
}

/* MAKE_EXPORT LockFileEx_new=LockFileEx */
BOOL WINAPI LockFileEx_new(
	HANDLE hFile,
	DWORD dwFlags,
	DWORD dwReserved,
	DWORD nNumberOfBytesToLockLow,
	DWORD nNumberOfBytesToLockHigh,
	LPOVERLAPPED lpOverlapped
)
{
	/* 
	 * FIXME: flags LOCKFILE_FAIL_IMMEDIATELY and LOCKFILE_EXCLUSIVE_LOCK not supported (always set), 
	 * Event signalling not supported
	 */
	if (dwReserved)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}
	return LockFile(hFile, lpOverlapped->Offset, lpOverlapped->OffsetHigh, 
			nNumberOfBytesToLockLow, nNumberOfBytesToLockHigh);
}

/* MAKE_EXPORT MapViewOfFileEx_new=MapViewOfFileEx */
LPVOID WINAPI MapViewOfFileEx_new(
	HANDLE hFileMappingObject,
	DWORD dwDesiredAccess,
	DWORD dwFileOffsetHigh,
	DWORD dwFileOffsetLow,
	SIZE_T dwNumberOfBytesToMap,
	LPVOID lpBaseAddress
)
{
	/* 
	 * With this modification MapViewOfFileEx won't fail if lpBaseAddress 
	 * doesn't point to Shared Arena - it will ignore suggested starting address
	 */
	if ((DWORD)lpBaseAddress < 0x80000000 || (DWORD)lpBaseAddress >= 0xc0000000)
	{
		lpBaseAddress = 0;
	}

	LPVOID ret = NULL;
	if ( dwDesiredAccess == FILE_MAP_OFFICE2003 )
	{
		DBGPRINTF(("KEX: Using 64K align workaround on memmap handle %p\n",
				hFileMappingObject));
		//was it already mapped?
		LPVOID oldmapaddr = GetMapViewAddress(hFileMappingObject);
		if (oldmapaddr == (LPVOID)-1)
		{
			SetLastError(ERROR_INVALID_HANDLE);
			return NULL;
		}
		if (oldmapaddr) //if it was, try that address first
			ret = MapViewOfFileEx(hFileMappingObject,FILE_MAP_ALL_ACCESS,
				dwFileOffsetHigh,dwFileOffsetLow,dwNumberOfBytesToMap,oldmapaddr);
		//search shared memory piece, skipping 64K to not deadloop
		DWORD mem64K = 0x81000000;
		while (!ret)
		{			
			mem64K = FindSharedMem64K(mem64K + 0x10000);
			if (!mem64K)
			{
				DBGPRINTF(("KEX: Failed to find memory for mapping handle %p!",
						hFileMappingObject));
				SetLastError(ERROR_NOT_ENOUGH_MEMORY);
				return NULL;
			}
			DWORD gle = GetLastError();
			SetLastError(0);
			ret = MapViewOfFileEx(hFileMappingObject,FILE_MAP_ALL_ACCESS,dwFileOffsetHigh,
				dwFileOffsetLow,dwNumberOfBytesToMap,(LPVOID)mem64K);
			if ( GetLastError() == ERROR_INVALID_PARAMETER ) //not according to plan
				return NULL;
			else
				SetLastError(gle);
		}
	}
	else
	{
		ret = MapViewOfFileEx(hFileMappingObject, dwDesiredAccess, dwFileOffsetHigh, 
				dwFileOffsetLow, dwNumberOfBytesToMap, lpBaseAddress);
	}
	
	return ret;
}


/* MAKE_EXPORT MapViewOfFile_new=MapViewOfFile */
LPVOID WINAPI MapViewOfFile_new( HANDLE hFileMappingObject, DWORD dwDesiredAccess,
  DWORD dwFileOffsetHigh, DWORD dwFileOffsetLow, SIZE_T dwNumberOfBytesToMap )
{
	return MapViewOfFileEx_new(hFileMappingObject, dwDesiredAccess, 
			dwFileOffsetHigh, dwFileOffsetLow, dwNumberOfBytesToMap, NULL);
}

/* MAKE_EXPORT MoveFileExA_new=MoveFileExA */
BOOL WINAPI MoveFileExA_new( LPCSTR lpExistingFileNameA, LPCSTR lpNewFileNameA, DWORD dwFlags )
{
	BOOL	fSuccess = FALSE;	// assume failure

	// Windows 9x has a special mechanism to move files after reboot

	if ( dwFlags & MOVEFILE_DELAY_UNTIL_REBOOT )
	{
		CHAR	szExistingFileNameA[MAX_PATH];
		CHAR	szNewFileNameA[MAX_PATH];
		lstrcpyA( szNewFileNameA, "NUL" );

		// Path names in WININIT.INI must be in short path name form

		if ( 
			GetShortPathNameA( lpExistingFileNameA, szExistingFileNameA, MAX_PATH ) &&
			(!lpNewFileNameA || GetShortPathNameA( lpNewFileNameA, szNewFileNameA, MAX_PATH ))
			)
		{
			CHAR*	szBuffer = (CHAR*) HeapAlloc( GetProcessHeap(), 0, BUFSIZE );	// The buffer size must not exceed 32K
			DWORD	dwBufLen = GetPrivateProfileSectionA( RENAME_SECTION, szBuffer, BUFSIZE, WININIT_FILENAME );

			CHAR	szRename[MAX_PATH];	// This is enough for at most to times 67 chracters
			size_t	lnRename;
			lstrcpyA( szRename, szNewFileNameA );
			lstrcatA( szRename, "=" );
			lstrcatA( szRename, szExistingFileNameA );
			lnRename = lstrlenA(szRename);

			if ( dwBufLen + lnRename + 2 <= BUFSIZE )
			{
				lstrcpyA( &szBuffer[dwBufLen], szRename );
				szBuffer[ dwBufLen + lnRename + 1 ] = 0;

				fSuccess = WritePrivateProfileSectionA( RENAME_SECTION, szBuffer, WININIT_FILENAME );
			}
			else
				SetLastError( ERROR_BUFFER_OVERFLOW );
				
			HeapFree( GetProcessHeap(), 0, szBuffer );
		}
	}
	else
	{

		fSuccess = MoveFileA( lpExistingFileNameA, lpNewFileNameA );

		if ( !fSuccess && 0 != (dwFlags & (MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING)) )
		{
			BOOL	bFailIfExist = 0 == (dwFlags & MOVEFILE_REPLACE_EXISTING);

			fSuccess = CopyFileA( lpExistingFileNameA, lpNewFileNameA, bFailIfExist );

			// In case of successfull copy do not return FALSE if delete fails.
			// Error detection is done by GetLastError()

			if ( fSuccess )
			{
				SetLastError( NO_ERROR );
				DeleteFileA( lpExistingFileNameA );
			}
		}

	}

	return fSuccess;
}

/* MAKE_EXPORT MoveFileWithProgressA_new=MoveFileWithProgressA */
BOOL WINAPI MoveFileWithProgressA_new(LPCSTR lpExistingFileName, LPCSTR lpNewFileName, LPPROGRESS_ROUTINE lpProgressRoutine, LPVOID lpData, DWORD dwFlags)
{
	return MoveFileExA_new(lpExistingFileName, lpNewFileName, dwFlags);
}

/* MAKE_EXPORT ReadFile_fix=ReadFile */
BOOL WINAPI ReadFile_fix(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead,
	LPDWORD lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped)
{
	DWORD lasterr = GetLastError();

	if (ReadFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, 
			lpOverlapped))
		return TRUE;

	if (lpOverlapped && GetLastError() == ERROR_INVALID_PARAMETER)
	{
		LONG high = lpOverlapped->OffsetHigh;
		SetLastError(lasterr);
		if ((SetFilePointer(hFile, lpOverlapped->Offset, &high, FILE_BEGIN)
				== INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR))
				return FALSE;
		ResetEvent(lpOverlapped->hEvent);
		lpOverlapped->Internal = STATUS_PENDING;
		lpOverlapped->InternalHigh = 0;
		BOOL result = 
			ReadFile(hFile, lpBuffer, nNumberOfBytesToRead, &lpOverlapped->InternalHigh, 0);
		lasterr = GetLastError();
		lpOverlapped->Internal = STATUS_WAIT_0;
		SetEvent(lpOverlapped->hEvent);
		SetLastError(lasterr);
		if (lpNumberOfBytesRead)
			*lpNumberOfBytesRead = lpOverlapped->InternalHigh;
		return result;
	}
	return FALSE;
}

/* MAKE_EXPORT SetFilePointerEx_new=SetFilePointerEx */
BOOL WINAPI SetFilePointerEx_new(HANDLE hFile, LARGE_INTEGER liDistanceToMove, PLARGE_INTEGER lpNewFilePointer, DWORD dwMoveMethod)
{
	DWORD lasterr = GetLastError();
	if ((liDistanceToMove.LowPart = SetFilePointer(hFile, liDistanceToMove.LowPart, &liDistanceToMove.HighPart, dwMoveMethod)) == INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}
	if (lpNewFilePointer) *lpNewFilePointer = liDistanceToMove;
	SetLastError(lasterr);
	return TRUE;
}

/* MAKE_EXPORT UnlockFileEx_new=UnlockFileEx */
BOOL WINAPI UnlockFileEx_new(
	HANDLE hFile,
	DWORD dwReserved,
	DWORD nNumberOfBytesToUnlockLow,
	DWORD nNumberOfBytesToUnlockHigh,
	LPOVERLAPPED lpOverlapped
)
{
	if (dwReserved)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}
	return UnlockFile(hFile, lpOverlapped->Offset, lpOverlapped->OffsetHigh,
			nNumberOfBytesToUnlockLow, nNumberOfBytesToUnlockHigh);
}

/* MAKE_EXPORT WriteFile_fix=WriteFile */
BOOL WINAPI WriteFile_fix(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, 
	LPDWORD lpNumberOfBytesWritten, LPOVERLAPPED lpOverlapped)
{
	DWORD lasterr = GetLastError();

	if (WriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, 
			lpNumberOfBytesWritten, lpOverlapped))
		return TRUE;

	if (lpOverlapped && GetLastError() == ERROR_INVALID_PARAMETER)
	{
		LONG high = lpOverlapped->OffsetHigh;
		SetLastError(lasterr);
		if ((SetFilePointer(hFile, lpOverlapped->Offset, &high, FILE_BEGIN)
				== INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR))
			return FALSE;
		ResetEvent(lpOverlapped->hEvent);
		lpOverlapped->Internal = STATUS_PENDING;
		lpOverlapped->InternalHigh = 0;
		BOOL result = 
			WriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, &lpOverlapped->InternalHigh, 0);
		lasterr = GetLastError();
		lpOverlapped->Internal = STATUS_WAIT_0;
		SetEvent(lpOverlapped->hEvent);
		SetLastError(lasterr);
		if (lpNumberOfBytesWritten)
			*lpNumberOfBytesWritten = lpOverlapped->InternalHigh;
		return result;
	}
	return FALSE;
}