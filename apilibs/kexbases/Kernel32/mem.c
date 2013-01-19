/*
 *  KernelEx
 *  Copyright (C) 2008, Xeno86
 *  Copyright (C) 2013, Ley0k
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

#include "common.h"
#include "_kernel32_apilist.h"

#ifndef VA_SHARED
#define VA_SHARED	0x8000000
#endif

#define ISPOINTER(h)         (((ULONG_PTR)(h)&2)==0)

static BOOL IsValidHeap(HANDLE hHeap)
{
	WORD *sig;
	if ( IsBadReadPtr(hHeap,0x74) ) return FALSE;
	sig = (WORD*)((DWORD)hHeap+0x72);
	if ( *sig != 0x4948 ) return FALSE;
	return TRUE;
}

/* MAKE_EXPORT GlobalLock_fix=GlobalLock */
LPVOID WINAPI GlobalLock_fix(
	HGLOBAL hMem    // address of the global memory object 
)
{
	if (ISPOINTER(hMem))
		return IsBadReadPtr(hMem, 1) ? NULL : hMem;
	return GlobalLock(hMem);
}

/* MAKE_EXPORT GlobalMemoryStatusEx_new=GlobalMemoryStatusEx */
BOOL WINAPI GlobalMemoryStatusEx_new(LPMEMORYSTATUSEX lpmemex)
{
	MEMORYSTATUS mem;

	if (lpmemex->dwLength != sizeof(*lpmemex))
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	mem.dwLength = sizeof(mem);
	GlobalMemoryStatus(&mem);
	lpmemex->dwMemoryLoad = mem.dwMemoryLoad;
	lpmemex->ullTotalPhys = mem.dwTotalPhys;
	lpmemex->ullAvailPhys = mem.dwAvailPhys;
	lpmemex->ullTotalPageFile = mem.dwTotalPageFile;
	lpmemex->ullAvailPageFile = mem.dwAvailPageFile;
	lpmemex->ullTotalVirtual = mem.dwTotalVirtual;
	lpmemex->ullAvailVirtual = mem.dwAvailVirtual;
	lpmemex->ullAvailExtendedVirtual = 0;

	return TRUE;
}

/* MAKE_EXPORT GlobalUnlock_fix=GlobalUnlock */
BOOL WINAPI GlobalUnlock_fix(
	HGLOBAL hMem    // handle to the global memory object 
)
{
	if (ISPOINTER(hMem)) 
		return TRUE;
	return GlobalUnlock(hMem);
}

/* MAKE_EXPORT HeapLock_new=HeapLock */
BOOL WINAPI HeapLock_new(
	HANDLE hHeap
)
{
	if ( !IsValidHeap(hHeap) ) return FALSE;
	EnterCriticalSection((CRITICAL_SECTION*)((DWORD)hHeap+0x50));
	return TRUE;
}

/* MAKE_EXPORT HeapQueryInformation_new=HeapQueryInformation */
BOOL WINAPI HeapQueryInformation_new(HANDLE HeapHandle,
	HEAP_INFORMATION_CLASS HeapInformationClass,
	PVOID HeapInformation,
	SIZE_T HeapInformationLength,
	PSIZE_T ReturnLength
)
{
	if(!IsValidHeap(HeapHandle))
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	if(IsBadWritePtr(HeapInformation, HeapInformationLength))
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	memcpy(HeapInformation, NULL, HeapInformationLength);

	if(!IsBadWritePtr(ReturnLength, sizeof(SIZE_T)))
		*ReturnLength = 0;

	return TRUE;
}

/* MAKE_EXPORT HeapSetInformation_new=HeapSetInformation */
BOOL WINAPI HeapSetInformation_new(HANDLE HeapHandle,
	HEAP_INFORMATION_CLASS HeapInformationClass,
	PVOID HeapInformation,
	SIZE_T HeapInformationLength
)
{
	if(!IsValidHeap(HeapHandle))
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	if(HeapInformationClass > HeapCompatibilityInformation)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	if(IsBadReadPtr(HeapInformation, HeapInformationLength))
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	return TRUE;
}

/* MAKE_EXPORT HeapUnlock_new=HeapUnlock */
BOOL WINAPI HeapUnlock_new(
	HANDLE hHeap
)
{
	if ( !IsValidHeap(hHeap) ) return FALSE;
	LeaveCriticalSection((CRITICAL_SECTION*)((DWORD)hHeap+0x50));
	return TRUE;
}

/* MAKE_EXPORT HeapValidate_new=HeapValidate */
BOOL WINAPI HeapValidate_new(HANDLE hHeap, DWORD dwFlags, LPCVOID lpMem)
{
	SIZE_T BlockSize = 0;

	if(!IsValidHeap(hHeap))
		return FALSE;

	/* Get the block size */
	BlockSize = HeapSize(hHeap, dwFlags, lpMem);

	if(BlockSize == -1)
		return FALSE;

	/* FIXME: if lpMem is NULL, validate the entire heap */

	/* Is this a valid memory block ? */
	if(IsBadReadPtr(lpMem, BlockSize))
		return FALSE;

	/* Is the block greater or equal than the heap and 
	   is the block lower than the maximum size ? */
	if((DWORD)lpMem <= (DWORD)hHeap || (DWORD)lpMem >= ((DWORD)lpMem + BlockSize))
		return FALSE;

	return TRUE;
}

/* MAKE_EXPORT HeapWalk_new=HeapWalk */
BOOL WINAPI HeapWalk_new(HANDLE hHeap,
						 LPPROCESS_HEAP_ENTRY lpEntry
)
{
	HEAPENTRY32 he32;
	HEAPLIST32 hl32;
	HANDLE hSnapshot = NULL;

	if(!IsValidHeap(hHeap))
		return FALSE;

	memset(&he32, 0, sizeof(HEAPENTRY32));
	memset(&hl32, 0, sizeof(HEAPLIST32));

	he32.dwSize = sizeof(HEAPENTRY32);
	hl32.dwSize = sizeof(HEAPLIST32);

	/* FIXME: Get the process id */
	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPHEAPLIST, 0);

	CloseHandle(hSnapshot);
	SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
	return FALSE;
}

typedef struct _VMEMPARAMS
{
	LPVOID lpAddress;
	DWORD dwSize;
	DWORD flAllocationType;
	DWORD flProtect;
	DWORD dwFreeType;
} VMEMPARAMS, *PVMEMPARAMS;

DWORD WINAPI VirtualAllocEx_thread(PVOID lParam)
{
	PVMEMPARAMS pvmp = (PVMEMPARAMS)lParam;

	return (DWORD)VirtualAlloc(pvmp->lpAddress, pvmp->dwSize, pvmp->flAllocationType, pvmp->flProtect);
}

DWORD WINAPI VirtualFreeEx_thread(PVOID lParam)
{
	PVMEMPARAMS pvmp = (PVMEMPARAMS)lParam;

	return VirtualFree(pvmp->lpAddress, pvmp->dwSize, pvmp->dwFreeType);
}

/* MAKE_EXPORT VirtualAllocEx_new=VirtualAllocEx */
LPVOID WINAPI VirtualAllocEx_new(HANDLE hProcess, LPVOID lpAddress, DWORD dwSize, DWORD flAllocationType, DWORD flProtect)
{
	PVMEMPARAMS vmp;
	DWORD dwProcessId = 0;
	HANDLE hThread = NULL;
	LPVOID result = NULL;

	if(hProcess == GetCurrentProcess())
		return VirtualAlloc(lpAddress, dwSize, flAllocationType, flProtect);

	dwProcessId = GetProcessId_new(hProcess);

	if(dwProcessId == 0)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return NULL;
	}

	if(dwProcessId == GetCurrentProcessId())
		return VirtualAlloc(lpAddress, dwSize, flAllocationType, flProtect);

	vmp = (PVMEMPARAMS)kexAllocObject(sizeof(VMEMPARAMS));

	vmp->lpAddress = lpAddress;
	vmp->dwSize = dwSize;
	vmp->flAllocationType = flAllocationType;
	vmp->flProtect = flProtect;

	//if (hProcess != GetCurrentProcess())
	//	flAllocationType |= VA_SHARED;

	/* Create the remote thread that will allocate memory */
	hThread = CreateRemoteThread_new(hProcess,
								NULL,
								0,
								VirtualAllocEx_thread,
								(LPVOID)vmp,
								0, NULL);

	if(hThread == NULL)
		return NULL;

	WaitForSingleObject(hThread, INFINITE);
	GetExitCodeThread(hThread, (LPDWORD)&result);
	CloseHandle(hThread);
	kexFreeObject(vmp);

	return result;
}

/* MAKE_EXPORT VirtualFreeEx_new=VirtualFreeEx */
BOOL WINAPI VirtualFreeEx_new(HANDLE hProcess, LPVOID lpAddress, DWORD dwSize, DWORD dwFreeType)
{
	PVMEMPARAMS vmp;
	DWORD dwProcessId = 0;
	HANDLE hThread = NULL;
	BOOL result = NULL;

	if(hProcess == GetCurrentProcess())
		return VirtualFree(lpAddress, dwSize, dwFreeType);

	dwProcessId = GetProcessId_new(hProcess);

	if(dwProcessId == 0)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return NULL;
	}

	if(dwProcessId == GetCurrentProcessId())
		return VirtualFree(lpAddress, dwSize, dwFreeType);

	vmp = (PVMEMPARAMS)kexAllocObject(sizeof(VMEMPARAMS));

	vmp->lpAddress = lpAddress;
	vmp->dwSize = dwSize;
	vmp->dwFreeType = dwFreeType;

	hThread = CreateRemoteThread_new(hProcess,
								NULL,
								0,
								VirtualFreeEx_thread,
								(LPVOID)vmp,
								0, NULL);

	if(hThread == NULL)
		return NULL;

	WaitForSingleObject(hThread, INFINITE);
	GetExitCodeThread(hThread, (LPDWORD)&result);
	CloseHandle(hThread);
	kexFreeObject(vmp);

	return result;
}
