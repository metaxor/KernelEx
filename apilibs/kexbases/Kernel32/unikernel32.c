/*
 *  KernelEx
 *  Copyright (C) 2006-2008, Xeno86
 *
 *  This file is part of KernelEx source code.
 *
 *  KernelEx is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published
 *  by the Free Software Foundation; version 2 of the License.
 *
 *  KernelEx is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with GNU Make; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#define _WIN32_WINDOWS 0x0410
#define ENABLE_FILEAPIS
#include "common.h"
#include <tlhelp32.h>
#include "_kernel32_apilist.h"

//MAKE_EXPORT AddAtomW_new=AddAtomW
ATOM WINAPI AddAtomW_new(LPCWSTR strW)
{
	ALLOC_WtoA(str);
	return AddAtomA(strA);
}

//MAKE_EXPORT CopyFileExW_new=CopyFileExW
BOOL WINAPI CopyFileExW_new(LPCWSTR lpExistingFileNameW, LPCWSTR lpNewFileNameW, LPPROGRESS_ROUTINE lpProgressRoutine, LPVOID lpData, LPBOOL pbCancel, DWORD dwCopyFlags)
{
	file_GetCP();
	file_ALLOC_WtoA(lpExistingFileName);
	file_ALLOC_WtoA(lpNewFileName);
	return CopyFileExA_new(lpExistingFileNameA, lpNewFileNameA, 
			lpProgressRoutine, lpData, pbCancel, dwCopyFlags);
}

//MAKE_EXPORT CopyFileW_new=CopyFileW
BOOL WINAPI CopyFileW_new(LPCWSTR lpExistingFileNameW, LPCWSTR lpNewFileNameW, BOOL bFailIfExists)
{
	file_GetCP();
	file_ALLOC_WtoA(lpExistingFileName);
	file_ALLOC_WtoA(lpNewFileName);
	return CopyFileA(lpExistingFileNameA, lpNewFileNameA, bFailIfExists);
}

//MAKE_EXPORT CreateDirectoryExW_new=CreateDirectoryExW
BOOL WINAPI CreateDirectoryExW_new(LPCWSTR templatedirW, LPCWSTR newdirW, LPSECURITY_ATTRIBUTES secattr)
{
	file_GetCP();
	file_ALLOC_WtoA(templatedir);
	file_ALLOC_WtoA(newdir);
	return CreateDirectoryExA(templatedirA, newdirA, secattr);
}

//MAKE_EXPORT CreateDirectoryW_new=CreateDirectoryW
BOOL WINAPI CreateDirectoryW_new(LPCWSTR PathW, LPSECURITY_ATTRIBUTES SecAttr)
{
	file_GetCP();
	file_ALLOC_WtoA(Path);
	return CreateDirectoryA(PathA, SecAttr);
}

//MAKE_EXPORT CreateFileW_new=CreateFileW
HANDLE WINAPI CreateFileW_new(LPCWSTR strW, DWORD access, DWORD sharemode,
	LPSECURITY_ATTRIBUTES secattr, DWORD creatdistr, DWORD flags, HANDLE temp)
{
	file_GetCP();
	file_ALLOC_WtoA(str);
	return CreateFileA_fix(strA, access, sharemode, secattr, creatdistr, flags, temp);
}


/* MAKE_EXPORT CreateProcessW_new=CreateProcessW */
BOOL WINAPI CreateProcessW_new(LPCWSTR lpApplicationNameW, LPWSTR lpCommandLineW, LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectoryW, LPSTARTUPINFOW lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation)
{
	LPWSTR lpDesktopW;
	LPWSTR lpReservedW;
	LPWSTR lpTitleW;
	STARTUPINFOA StartupInfo;

	ALLOC_WtoA(lpApplicationName);
	ALLOC_WtoA(lpCommandLine);
	ALLOC_WtoA(lpCurrentDirectory);

	memmove(&StartupInfo, lpStartupInfo, sizeof(*lpStartupInfo));

	StartupInfo.lpDesktop = NULL;
	StartupInfo.lpReserved = NULL;
	StartupInfo.lpTitle = NULL;

	lpDesktopW = lpStartupInfo->lpDesktop;
	lpReservedW = lpStartupInfo->lpReserved;
	lpTitleW = lpStartupInfo->lpTitle;

	ALLOC_WtoA(lpDesktop);
	ALLOC_WtoA(lpReserved);
	ALLOC_WtoA(lpTitle);

	StartupInfo.lpDesktop = lpDesktopA;
	StartupInfo.lpReserved = lpReservedA;
	StartupInfo.lpTitle = lpTitleA;

	return CreateProcessA(lpApplicationNameA,
						lpCommandLineA,
						lpProcessAttributes,
						lpThreadAttributes,
						bInheritHandles,
						dwCreationFlags,
						lpEnvironment,
						lpCurrentDirectoryA,
						&StartupInfo,
						lpProcessInformation);
}

//MAKE_EXPORT DefineDosDeviceW_new=DefineDosDeviceW
BOOL WINAPI DefineDosDeviceW_new(DWORD dwFlags, LPCWSTR lpDeviceNameW, 
	LPCWSTR lpTargetPathW)
{
	file_GetCP();
	file_ALLOC_WtoA(lpDeviceName);
	file_ALLOC_WtoA(lpTargetPath);
	return DefineDosDeviceA(dwFlags, lpDeviceNameA, lpTargetPathA);
}

//MAKE_EXPORT DeleteFileW_new=DeleteFileW
BOOL WINAPI DeleteFileW_new(LPCWSTR lpFileNameW)
{
	file_GetCP();
	file_ALLOC_WtoA(lpFileName);
	return DeleteFileA(lpFileNameA);
}

//MAKE_EXPORT FindAtomW_new=FindAtomW
ATOM WINAPI FindAtomW_new(LPCWSTR strW)
{
	ALLOC_WtoA(str);
	return FindAtomA(strA);
}

//MAKE_EXPORT FindFirstChangeNotificationW_new=FindFirstChangeNotificationW
HANDLE WINAPI FindFirstChangeNotificationW_new(LPCWSTR pathW, BOOL watchsubtree, DWORD filter)
{
	file_GetCP();
	file_ALLOC_WtoA(path);
	return FindFirstChangeNotificationA(pathA, watchsubtree, filter);
}

//MAKE_EXPORT FindFirstFileW_new=FindFirstFileW
HANDLE WINAPI FindFirstFileW_new(LPCWSTR strW, LPWIN32_FIND_DATAW dataW)
{
	HANDLE ret;
	char cFileNameA[MAX_PATH];
	char cAlternateFileNameA[14];
	WCHAR* cFileNameW = dataW->cFileName;
	WCHAR* cAlternateFileNameW = dataW->cAlternateFileName;
	file_GetCP();
	file_ALLOC_WtoA(str);
	ret = FindFirstFileA(strA, (LPWIN32_FIND_DATAA)dataW);
	if (ret != INVALID_HANDLE_VALUE)
	{
		strcpy(cFileNameA, ((LPWIN32_FIND_DATAA)dataW)->cFileName);
		strcpy(cAlternateFileNameA, ((LPWIN32_FIND_DATAA)dataW)->cAlternateFileName);
		file_AtoW(cFileName, sizeof(dataW->cFileName) / sizeof(WCHAR));
		file_AtoW(cAlternateFileName, sizeof(dataW->cAlternateFileName) / sizeof(WCHAR));
	}
	return ret;
}

//MAKE_EXPORT FindNextFileW_new=FindNextFileW
BOOL WINAPI FindNextFileW_new(HANDLE handle, LPWIN32_FIND_DATAW dataW)
{
	BOOL ret;
	char cFileNameA[MAX_PATH];
	char cAlternateFileNameA[14];
	WCHAR* cFileNameW = dataW->cFileName;
	WCHAR* cAlternateFileNameW = dataW->cAlternateFileName;
	ret = FindNextFileA(handle, (LPWIN32_FIND_DATAA)dataW);
	if (ret)
	{
		file_GetCP();
		strcpy(cFileNameA, ((LPWIN32_FIND_DATAA)dataW)->cFileName);
		strcpy(cAlternateFileNameA, ((LPWIN32_FIND_DATAA)dataW)->cAlternateFileName);
		file_AtoW(cFileName, sizeof(dataW->cFileName) / sizeof(WCHAR));
		file_AtoW(cAlternateFileName, sizeof(dataW->cAlternateFileName) / sizeof(WCHAR));
	}
	return ret;
}

//MAKE_EXPORT FindResourceExW_new=FindResourceExW
HRSRC WINAPI FindResourceExW_new(HMODULE hModule, LPCWSTR typeW, LPCWSTR nameW, WORD lang)
{
	LPSTR nameA, typeA;

	if (HIWORD(nameW))
	{
		_ALLOC_WtoA(name);
	}
	else 
		nameA = (LPSTR) nameW;

	if (HIWORD(typeW))
	{
		_ALLOC_WtoA(type);
	}
	else 
		typeA = (LPSTR) typeW;

	return FindResourceExA(hModule, typeA, nameA, lang);
}

//MAKE_EXPORT FindResourceW_new=FindResourceW
HRSRC WINAPI FindResourceW_new(HINSTANCE hModule, LPCWSTR name, LPCWSTR type)
{
	return FindResourceExW_new(hModule, type, name, MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL));
}

//MAKE_EXPORT FreeEnvironmentStringsW_new=FreeEnvironmentStringsW
BOOL WINAPI FreeEnvironmentStringsW_new(void* env)
{
	return HeapFree(GetProcessHeap(), 0, env);
}

//MAKE_EXPORT GetAtomNameW_new=GetAtomNameW
UINT WINAPI GetAtomNameW_new(ATOM atom, LPWSTR bufferW, int size)
{
	UINT ret;
	ALLOC_A(buffer, size * acp_mcs);
	ret = GetAtomNameA(atom, bufferA, size * acp_mcs);
	if (ret)
		ret = ABUFtoW(buffer, ret + 1, size);
	if (ret) ret--;
	return ret;
}

//MAKE_EXPORT GetCurrentDirectoryW_new=GetCurrentDirectoryW
DWORD WINAPI GetCurrentDirectoryW_new(DWORD nBufferLength, LPWSTR lpBufferW)
{
	DWORD ret;
	char lpBufferA[MAX_PATH];
	ret = GetCurrentDirectoryA(MAX_PATH, lpBufferA);
	if (ret)
	{
		file_GetCP();
		if (ret > MAX_PATH)
		{
			SetLastError(ERROR_FILENAME_EXCED_RANGE);
			return 0;
		}
		ret = file_AtoW(lpBuffer, 0);
		if (ret <= nBufferLength)
		{
			ret = file_AtoW(lpBuffer, nBufferLength);
			if (ret) ret--;
		}
	}
	return ret;
}

//MAKE_EXPORT GetDefaultCommConfigW_new=GetDefaultCommConfigW
BOOL WINAPI GetDefaultCommConfigW_new(LPCWSTR lpszNameW, LPCOMMCONFIG lpCC, LPDWORD lpdwSize)
{
	ALLOC_WtoA(lpszName);
	return GetDefaultCommConfigA(lpszNameA, lpCC, lpdwSize);
}

//MAKE_EXPORT GetDiskFreeSpaceExW_new=GetDiskFreeSpaceExW
BOOL WINAPI GetDiskFreeSpaceExW_new(LPCWSTR lpDirectoryNameW, PULARGE_INTEGER lpFreeBytesAvailableToCaller, PULARGE_INTEGER lpTotalNumberOfBytes, PULARGE_INTEGER lpTotalNumberOfFreeBytes)
{
	file_GetCP();
	file_ALLOC_WtoA(lpDirectoryName);
	return GetDiskFreeSpaceExA_fix(lpDirectoryNameA, lpFreeBytesAvailableToCaller, lpTotalNumberOfBytes, lpTotalNumberOfFreeBytes);
}

//MAKE_EXPORT GetDiskFreeSpaceW_new=GetDiskFreeSpaceW
BOOL WINAPI GetDiskFreeSpaceW_new(LPCWSTR lpRootPathNameW, LPDWORD lpSectorsPerCluster, LPDWORD lpBytesPerSector, LPDWORD lpNumberOfFreeClusters, LPDWORD lpTotalNumberOfClusters)
{
	file_GetCP();
	file_ALLOC_WtoA(lpRootPathName);
	return GetDiskFreeSpaceA_fix(lpRootPathNameA, lpSectorsPerCluster, lpBytesPerSector, lpNumberOfFreeClusters, lpTotalNumberOfClusters);
}

//MAKE_EXPORT GetEnvironmentStringsW_new=GetEnvironmentStringsW
void* WINAPI GetEnvironmentStringsW_new(void)
{
	int len;
	WCHAR* envW;
	char* envA = GetEnvironmentStringsA();
	char* ptrA = (char*) envA;
	if (!envA) return envA;
	
	do while (*ptrA++); while (*++ptrA);
	len = (int) ptrA - (int) envA;
	
	envW = (WCHAR*) HeapAlloc(GetProcessHeap(), 0, len * sizeof(WCHAR));
	if (envW)
		ABUFtoW(env, len, len);
	FreeEnvironmentStringsA(envA);
	return envW;
}

//MAKE_EXPORT GetFileAttributesExW_new=GetFileAttributesExW
BOOL WINAPI GetFileAttributesExW_new(LPCWSTR lpFileNameW, GET_FILEEX_INFO_LEVELS fInfoLevelId,
	LPVOID lpFileInformation)
{
	file_GetCP();
	file_ALLOC_WtoA(lpFileName);
	return GetFileAttributesExA(lpFileNameA, fInfoLevelId, lpFileInformation);
}

//MAKE_EXPORT GetFileAttributesW_new=GetFileAttributesW
DWORD WINAPI GetFileAttributesW_new(LPCWSTR lpFileNameW)
{
	file_GetCP();
	file_ALLOC_WtoA(lpFileName);
	return GetFileAttributesA(lpFileNameA);
}

//MAKE_EXPORT GetFullPathNameW_new=GetFullPathNameW
DWORD WINAPI GetFullPathNameW_new(LPCWSTR lpFileNameW, DWORD nBufferLength, LPWSTR lpBufferW, LPWSTR *lpFilePartW)
{
	DWORD ret;
	char lpBufferA[MAX_PATH];
	file_GetCP();
	file_ALLOC_WtoA(lpFileName);
	ret = GetFullPathNameA(lpFileNameA, MAX_PATH, lpBufferA, NULL);
	if (ret)
	{
		if (ret > MAX_PATH) 
		{
			SetLastError(ERROR_FILENAME_EXCED_RANGE);
			return 0;
		}
		ret = file_AtoW(lpBuffer, 0);
		if (ret <= nBufferLength)
		{
			ret = file_AtoW(lpBuffer, nBufferLength);
			if (ret && lpFilePartW)
			{
				if (lpBufferW[ret - 1] == '\\') *lpFilePartW = 0;
				else
				{
					LPWSTR p = lpBufferW + ret - 1;
					while ((p > lpBufferW + 2) && (*p != '\\')) p--;
					*lpFilePartW = p + 1;
				}
			}
			if (ret) ret--;
		}
	}
	return ret;
}

static int GetCPFromLocale(LCID Locale)
{
	int cp;	
	Locale = LOWORD(Locale);
	if (GetLocaleInfoA(Locale, LOCALE_IDEFAULTANSICODEPAGE | LOCALE_RETURN_NUMBER, (LPSTR)&cp, sizeof(int)))
		return cp;
	else
		return CP_ACP;
}

/* MAKE_EXPORT GetLocaleInfoW_new=GetLocaleInfoW */
int WINAPI GetLocaleInfoW_new(
	LCID Locale,
	LCTYPE LCType,
	LPWSTR lpLCData,
	int cchData
)
{
	int cp;
	int ret;
	char *buf;
	
	if ((LCType & LOCALE_RETURN_NUMBER) || (cchData == 0))
		return GetLocaleInfoA(Locale, LCType, (LPSTR) lpLCData, cchData * 2);
	if (!lpLCData)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return 0;
	}
	buf = (char*) alloca(cchData * 2);
	buf[0] = 0;
	ret = GetLocaleInfoA(Locale, LCType, buf, cchData * 2);
	if (!ret)
		return ret;
	if (LCType & LOCALE_USE_CP_ACP)
		cp = CP_ACP;
	else
		cp = GetCPFromLocale(Locale);
	ret = MultiByteToWideChar(cp, 0, buf, -1, lpLCData, cchData);
	return ret;
}
 
//MAKE_EXPORT GetLongPathNameW_new=GetLongPathNameW
DWORD WINAPI GetLongPathNameW_new(LPCWSTR lpszShortPathW, LPWSTR lpszLongPathW, DWORD cchBuffer)
{
	DWORD ret;
	char lpszLongPathA[MAX_PATH];
	file_GetCP();
	file_ALLOC_WtoA(lpszShortPath);
	ret = GetLongPathNameA(lpszShortPathA, lpszLongPathA, MAX_PATH);
	if (ret)
	{
		if (ret > MAX_PATH) 
		{
			SetLastError(ERROR_FILENAME_EXCED_RANGE);
			return 0;
		}
		ret = file_AtoW(lpszLongPath, 0);
		if (ret <= cchBuffer)
		{
			ret = file_AtoW(lpszLongPath, cchBuffer);
			if (ret) ret--;
		}
	}
	return ret;
}

//MAKE_EXPORT GetModuleFileNameW_new=GetModuleFileNameW
DWORD WINAPI GetModuleFileNameW_new(HMODULE hModule, LPWSTR lpFilenameW, DWORD nSize)
{
	DWORD ret;
	char lpFilenameA[MAX_PATH];
	if (nSize <= 0)
		return 0;
	ret = GetModuleFileNameA(hModule, lpFilenameA, MAX_PATH);
	if (ret)
	{
		file_GetCP();
		DWORD last_err = GetLastError();
		if (ret >= MAX_PATH)
		{
			SetLastError(ERROR_FILENAME_EXCED_RANGE);
			return 0;
		}
		ret = file_AtoW(lpFilename, nSize);
		if (!ret && GetLastError() == ERROR_INSUFFICIENT_BUFFER)
		{
			ret = nSize;
		}
		else if (ret) ret--;
		SetLastError(last_err);
	}
	return ret;
}

//MAKE_EXPORT GetModuleHandleW_new=GetModuleHandleW
HMODULE WINAPI GetModuleHandleW_new(LPCWSTR lpModuleNameW)
{
	file_GetCP();
	file_ALLOC_WtoA(lpModuleName);
	return GetModuleHandleA(lpModuleNameA);
}

//MAKE_EXPORT GetShortPathNameW_new=GetShortPathNameW
DWORD WINAPI GetShortPathNameW_new(LPCWSTR lpszLongPathW, LPWSTR lpszShortPathW, DWORD cchBuffer)
{
	DWORD ret;
	char lpszShortPathA[MAX_PATH];
	file_GetCP();
	file_ALLOC_WtoA(lpszLongPath);
	ret = GetShortPathNameA(lpszLongPathA, lpszShortPathA, MAX_PATH);
	if (ret)
	{
		if (ret > MAX_PATH) 
		{
			SetLastError(ERROR_FILENAME_EXCED_RANGE);
			return 0;
		}
		ret = file_AtoW(lpszShortPath, 0);
		if (ret <= cchBuffer)
		{
			ret = file_AtoW(lpszShortPath, cchBuffer);
			if (ret) ret--;
		}
	}
	return ret;
}

//MAKE_EXPORT GetStartupInfoW_new=GetStartupInfoW
VOID WINAPI GetStartupInfoW_new(LPSTARTUPINFOW lpStartupInfo)
{
	// since lpDesktop and lpTitle and lpReserved are 
	// always zero we can just execute ansi version
	GetStartupInfoA((LPSTARTUPINFOA)lpStartupInfo);
}

//MAKE_EXPORT GetSystemDirectoryW_new=GetSystemDirectoryW
UINT WINAPI GetSystemDirectoryW_new(LPWSTR lpBufferW, UINT uSize)
{
	UINT ret;
	char lpBufferA[MAX_PATH];
	ret = GetSystemDirectoryA(lpBufferA, MAX_PATH);
	if (ret)
	{
		if (ret > MAX_PATH) 
		{
			SetLastError(ERROR_FILENAME_EXCED_RANGE);
			return 0;
		}
		file_GetCP();
		ret = file_AtoW(lpBuffer, 0);
		if (ret <= uSize)
		{
			ret = file_AtoW(lpBuffer, uSize);
			if (ret) ret--;
		}
	}
	return ret;
}

//MAKE_EXPORT GetTempFileNameW_new=GetTempFileNameW
UINT WINAPI GetTempFileNameW_new(LPCWSTR lpPathNameW, LPCWSTR lpPrefixStringW, UINT uUnique, LPWSTR lpTempFileNameW)
{
	UINT ret;
	char lpTempFileNameA[MAX_PATH];
	file_GetCP();
	file_ALLOC_WtoA(lpPathName);
	file_ALLOC_WtoA(lpPrefixString);
	ret = GetTempFileNameA_fix(lpPathNameA, lpPrefixStringA, uUnique, lpTempFileNameA);
	if (ret)
	{
		file_AtoW(lpTempFileName, MAX_PATH);
	}
	return ret;
}

//MAKE_EXPORT GetTempPathW_new=GetTempPathW
DWORD WINAPI GetTempPathW_new(DWORD nBufferLength, LPWSTR lpBufferW)
{
	DWORD ret;
	char lpBufferA[MAX_PATH];
	ret = GetTempPathA(MAX_PATH, lpBufferA);
	if (ret)
	{
		if (ret > MAX_PATH) 
		{
			SetLastError(ERROR_FILENAME_EXCED_RANGE);
			return 0;
		}
		file_GetCP();
		ret = file_AtoW(lpBuffer, 0);
		if (ret <= nBufferLength)
		{
			ret = file_AtoW(lpBuffer, nBufferLength);
			if (ret) ret--;
		}
	}
	return ret;
}

//MAKE_EXPORT GetWindowsDirectoryW_new=GetWindowsDirectoryW
UINT WINAPI GetWindowsDirectoryW_new(LPWSTR lpBufferW, UINT uSize)
{
	UINT ret;
	char lpBufferA[MAX_PATH];
	ret = GetWindowsDirectoryA(lpBufferA, MAX_PATH);
	if (ret)
	{
		if (ret > MAX_PATH) 
		{
			SetLastError(ERROR_FILENAME_EXCED_RANGE);
			return 0;
		}
		file_GetCP();
		ret = file_AtoW(lpBuffer, 0);
		if (ret <= uSize)
		{
			ret = file_AtoW(lpBuffer, uSize);
			if (ret) ret--;
		}
	}
	return ret;
}

//MAKE_EXPORT GlobalAddAtomW_new=GlobalAddAtomW
ATOM WINAPI GlobalAddAtomW_new(LPCWSTR strW)
{
	ALLOC_WtoA(str);
	return GlobalAddAtomA(strA);
}

//MAKE_EXPORT GlobalFindAtomW_new=GlobalFindAtomW
ATOM WINAPI GlobalFindAtomW_new(LPCWSTR strW)
{
	ALLOC_WtoA(str);
	return GlobalFindAtomA(strA);
}

//MAKE_EXPORT GlobalGetAtomNameW_new=GlobalGetAtomNameW
UINT WINAPI GlobalGetAtomNameW_new(ATOM atom, LPWSTR bufferW, int size)
{
	UINT ret;
	ALLOC_A(buffer, size * acp_mcs);
	ret = GlobalGetAtomNameA(atom, bufferA, size * acp_mcs);
	if (ret)
		ret = ABUFtoW(buffer, ret + 1, size);
	if (ret) ret--;
	return ret;
}

//MAKE_EXPORT LoadLibraryW_new=LoadLibraryW
HINSTANCE WINAPI LoadLibraryW_new(LPCWSTR lpLibFileNameW)
{
	file_GetCP();
	file_ALLOC_WtoA(lpLibFileName);
	return LoadLibraryA(lpLibFileNameA);
}

//MAKE_EXPORT LoadLibraryExW_new=LoadLibraryExW
HINSTANCE WINAPI LoadLibraryExW_new(LPCWSTR lpLibFileNameW, HANDLE hFile, DWORD dwFlags)
{
	file_GetCP();
	file_ALLOC_WtoA(lpLibFileName);
	return LoadLibraryExA(lpLibFileNameA, hFile, dwFlags);
} 

//MAKE_EXPORT Module32FirstW_new=Module32FirstW
BOOL WINAPI Module32FirstW_new(HANDLE hSnapshot, LPMODULEENTRY32W lpmeW)
{
	BOOL ret;
	char szModuleA[MAX_MODULE_NAME32 + 1];
	char szExePathA[MAX_PATH];
	WCHAR* szModuleW = lpmeW->szModule;
	WCHAR* szExePathW = lpmeW->szExePath;
	
	if (lpmeW->dwSize < sizeof(MODULEENTRY32W))
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}
	
	lpmeW->dwSize = sizeof(MODULEENTRY32);
	
	if ((ret = Module32First(hSnapshot, (LPMODULEENTRY32)lpmeW)) != FALSE)
	{
		strcpy(szModuleA, ((LPMODULEENTRY32)lpmeW)->szModule);
		strcpy(szExePathA, ((LPMODULEENTRY32)lpmeW)->szExePath);
		AtoW(szModule, sizeof(lpmeW->szModule) / sizeof(WCHAR));
		AtoW(szExePath, sizeof(lpmeW->szExePath) / sizeof(WCHAR));
	}
	lpmeW->dwSize = sizeof(MODULEENTRY32W);
	return ret;
}

//MAKE_EXPORT Module32NextW_new=Module32NextW
BOOL WINAPI Module32NextW_new(HANDLE hSnapshot, LPMODULEENTRY32W lpmeW)
{
	BOOL ret;
	char szModuleA[MAX_MODULE_NAME32 + 1];
	char szExePathA[MAX_PATH];
	WCHAR* szModuleW = lpmeW->szModule;
	WCHAR* szExePathW = lpmeW->szExePath;
	
	if (lpmeW->dwSize < sizeof(MODULEENTRY32W))
	{
		SetLastError(ERROR_INVALID_DATA);
		return FALSE;
	}
	
	lpmeW->dwSize = sizeof(MODULEENTRY32);
	
	if ((ret = Module32Next(hSnapshot, (LPMODULEENTRY32)lpmeW)) != FALSE)
	{
		strcpy(szModuleA, ((LPMODULEENTRY32)lpmeW)->szModule);
		strcpy(szExePathA, ((LPMODULEENTRY32)lpmeW)->szExePath);
		AtoW(szModule, sizeof(lpmeW->szModule) / sizeof(WCHAR));
		AtoW(szExePath, sizeof(lpmeW->szExePath) / sizeof(WCHAR));
	}
	lpmeW->dwSize = sizeof(MODULEENTRY32W);
	return ret;
}

//MAKE_EXPORT MoveFileExW_new=MoveFileExW
BOOL WINAPI MoveFileExW_new(LPCWSTR existingfileW, LPCWSTR newfileW, DWORD flags)
{
	file_GetCP();
	file_ALLOC_WtoA(existingfile);
	file_ALLOC_WtoA(newfile);
	return MoveFileExA_new(existingfileA, newfileA, flags);
}

//MAKE_EXPORT MoveFileW_new=MoveFileW
BOOL WINAPI MoveFileW_new(LPCWSTR existingfileW, LPCWSTR newfileW)
{
	file_GetCP();
	file_ALLOC_WtoA(existingfile);
	file_ALLOC_WtoA(newfile);
	return MoveFileA(existingfileA, newfileA);
}

//MAKE_EXPORT MoveFileWithProgressW_new=MoveFileWithProgressW
BOOL WINAPI MoveFileWithProgressW_new(LPCWSTR existingfileW, LPCWSTR newfileW, LPPROGRESS_ROUTINE lpProgressRoutine, LPVOID lpData, DWORD dwFlags)
{
	file_GetCP();
	file_ALLOC_WtoA(existingfile);
	file_ALLOC_WtoA(newfile);
	return MoveFileWithProgressA_new(existingfileA, newfileA, lpProgressRoutine, lpData, dwFlags);
}

//MAKE_EXPORT OutputDebugStringW_new=OutputDebugStringW
void WINAPI OutputDebugStringW_new(LPCWSTR strW)
{
	ALLOC_WtoA(str);
	OutputDebugStringA(strA);
}

//MAKE_EXPORT Process32FirstW_new=Process32FirstW
BOOL WINAPI Process32FirstW_new(HANDLE hSnapshot, LPPROCESSENTRY32W lppeW)
{
	BOOL ret;
	char szExeFileA[MAX_PATH];
	WCHAR* szExeFileW = lppeW->szExeFile;
	
	if (lppeW->dwSize < sizeof(PROCESSENTRY32W))
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}
	
	lppeW->dwSize = sizeof(PROCESSENTRY32);
	
	if ((ret = Process32First(hSnapshot, (LPPROCESSENTRY32)lppeW)) != FALSE)
	{
		strcpy(szExeFileA, ((LPPROCESSENTRY32)lppeW)->szExeFile);
		AtoW(szExeFile, sizeof(lppeW->szExeFile) / sizeof(WCHAR));
	}
	lppeW->dwSize = sizeof(PROCESSENTRY32W);
	return ret;
}

//MAKE_EXPORT Process32NextW_new=Process32NextW
BOOL WINAPI Process32NextW_new(HANDLE hSnapshot, LPPROCESSENTRY32W lppeW)
{
	BOOL ret;
	char szExeFileA[MAX_PATH];
	WCHAR* szExeFileW = lppeW->szExeFile;
	
	if (lppeW->dwSize < sizeof(PROCESSENTRY32W))
	{
		SetLastError(ERROR_INVALID_DATA);
		return FALSE;
	}
	
	lppeW->dwSize = sizeof(PROCESSENTRY32);
	
	if ((ret = Process32Next(hSnapshot, (LPPROCESSENTRY32)lppeW)) != FALSE)
	{
		strcpy(szExeFileA, ((LPPROCESSENTRY32)lppeW)->szExeFile);
		AtoW(szExeFile, sizeof(lppeW->szExeFile) / sizeof(WCHAR));
	}
	lppeW->dwSize = sizeof(PROCESSENTRY32W);
	return ret;
}

//MAKE_EXPORT RemoveDirectoryW_new=RemoveDirectoryW
BOOL WINAPI RemoveDirectoryW_new(LPCWSTR lpPathNameW)
{
	file_GetCP();
	file_ALLOC_WtoA(lpPathName);
	return RemoveDirectoryA(lpPathNameA);
}

//MAKE_EXPORT SearchPathW_new=SearchPathW
DWORD WINAPI SearchPathW_new(LPCWSTR lpPathW, LPCWSTR lpFileNameW, LPCWSTR lpExtensionW, DWORD nBufferLength, LPWSTR lpBufferW, LPWSTR *lpFilePartW)
{
	DWORD ret;
	char lpBufferA[MAX_PATH];
	file_GetCP();
	file_ALLOC_WtoA(lpPath);
	file_ALLOC_WtoA(lpFileName);
	file_ALLOC_WtoA(lpExtension);
	ret = SearchPathA(lpPathA, lpFileNameA, lpExtensionA, MAX_PATH, lpBufferA, NULL);
	if (ret)
	{
		ret = file_AtoW(lpBuffer, 0);
		if (ret <= nBufferLength)
		{
			ret = file_AtoW(lpBuffer, nBufferLength);
			if (lpFilePartW)
			{
				DWORD i;
				for (i = ret ; i >= 0 ; i--)
				{
					if (lpBufferW[i] == '\\')
					{
						*lpFilePartW = &lpBufferW[i + 1];
						break;
					}
				}
			}
		}
	}
	return ret;
}

//MAKE_EXPORT SetConsoleTitleW_new=SetConsoleTitleW
BOOL WINAPI SetConsoleTitleW_new(LPCWSTR strW)
{
	ALLOC_WtoA(str);
	return SetConsoleTitleA(strA);
}

//MAKE_EXPORT SetCurrentDirectoryW_new=SetCurrentDirectoryW
BOOL WINAPI SetCurrentDirectoryW_new(LPCWSTR lpPathNameW)
{
	file_GetCP();
	file_ALLOC_WtoA(lpPathName);
	return SetCurrentDirectoryA(lpPathNameA);
}

//MAKE_EXPORT SetFileAttributesW_new=SetFileAttributesW
BOOL WINAPI SetFileAttributesW_new(LPCWSTR lpFileNameW, DWORD dwFileAttributes)
{
	file_GetCP();
	file_ALLOC_WtoA(lpFileName);
	return SetFileAttributesA(lpFileNameA, dwFileAttributes);
}

//MAKE_EXPORT VerLanguageNameW_new=VerLanguageNameW 
DWORD WINAPI VerLanguageNameW_new(DWORD wLang, LPWSTR szLangW, DWORD nSize)
{
	DWORD ret;
	DWORD buf_size = VerLanguageNameA(wLang, NULL, 1); //GPF if nSize==0

	if (!buf_size) return 0;
	buf_size++;

	ALLOC_A(szLang, buf_size);
	
	ret = VerLanguageNameA(wLang, szLangA, buf_size);
	if (ret)
	{
		DWORD last_err = GetLastError();
		ret = AtoW(szLang, nSize);
		if (!ret) 
		{
			szLangW[nSize - 1] = 0;
			ret = AtoW(szLang, 0);
		}
		if (ret) ret--;
		SetLastError(last_err);
	}
	return ret;
}

//MAKE_EXPORT lstrcpynW_new=lstrcpynW
LPWSTR WINAPI lstrcpynW_new(LPWSTR dst, LPCWSTR src, INT n)
{
	LPWSTR ret = dst;
	
	__try
	{
		while ((n > 1) && *src)
		{
			*dst++ = *src++;
			n--;
		}
		*dst = 0;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return NULL;
	}
	return ret;
}

//MAKE_EXPORT ReadConsoleInputW_new=ReadConsoleInputW
BOOL WINAPI ReadConsoleInputW_new( HANDLE hConsoleInput, PINPUT_RECORD lpBuffer,
								   DWORD nLength, LPDWORD lpNumberOfEventsRead)
{
	BOOL ret = ReadConsoleInputA(hConsoleInput,lpBuffer,nLength,lpNumberOfEventsRead);
	if ( ret && lpBuffer && lpNumberOfEventsRead )
	{
		DWORD i;
		for (i=0;i<*lpNumberOfEventsRead;i++)
		{
			if (lpBuffer->EventType == KEY_EVENT)
			{
				WCHAR uniChar;
				if ( MultiByteToWideChar(CP_OEMCP,0,&lpBuffer->Event.KeyEvent.uChar.AsciiChar,1,&uniChar,1) )
					lpBuffer->Event.KeyEvent.uChar.UnicodeChar = uniChar;
				else //fallback
					lpBuffer->Event.KeyEvent.uChar.UnicodeChar = lpBuffer->Event.KeyEvent.uChar.AsciiChar;
			}
			lpBuffer++;
		}
	}
	return ret;
}
