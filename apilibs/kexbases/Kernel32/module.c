/*
 *  KernelEx
 *  Copyright (C) 2009 Tihiy
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

#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0600

#include "common.h"
#include "_kernel32_apilist.h"
#include <Tlhelp32.h>

/* MAKE_EXPORT GetModuleFileNameExA_new=GetModuleFileNameExA */
DWORD WINAPI GetModuleFileNameExA_new(
	HANDLE hProcess,
	HMODULE hModule,
	LPSTR lpFilename,
	DWORD nSize
)
{
	HANDLE hSnap = INVALID_HANDLE_VALUE;
	DWORD dwProcessId = GetProcessId_new(hProcess);
	MODULEENTRY32 me32;
	DWORD strSize = 0;

	if(dwProcessId == 0 || IsBadStringPtr(lpFilename, nSize))
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return 0;
	}

	hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetProcessId_new(hProcess));

	if(hSnap == INVALID_HANDLE_VALUE)
		return 0;

	memset(&me32, 0, sizeof(MODULEENTRY32));

	me32.dwSize = sizeof(MODULEENTRY32);
	me32.th32ProcessID = dwProcessId;

	Module32First(hSnap, &me32);

	do
	{
		if(me32.hModule == hModule)
		{
			strncpy(lpFilename, me32.szModule, nSize);
			strSize = strlen(lpFilename);
			break;
		}

	} while(Module32Next(hSnap, &me32));


	CloseHandle(hSnap);
	return strSize;
}

/* MAKE_EXPORT GetModuleFileNameExW_new=GetModuleFileNameExW */
DWORD WINAPI GetModuleFileNameExW_new(
	HANDLE hProcess,
	HMODULE hModule,
	LPWSTR lpFilename,
	DWORD nSize
)
{
	PCHAR lpBuffer = (PCHAR)malloc(nSize);
	LPWSTR lpBufferW = NULL;
	DWORD dwRet = 0;
	DWORD i;

	memset(lpBuffer, 0, nSize);
	dwRet = GetModuleFileNameExA_new(hProcess, hModule, lpBuffer, nSize);

	if(dwRet == 0)
	{
		free(lpBuffer);
		return 0;
	}

	STACK_AtoW(lpBuffer, lpBufferW);

	for(i=0;i<=nSize;i++)
		lpFilename[i] = lpBufferW[i];

	free(lpBuffer);
	return dwRet;
}

/* MAKE_EXPORT GetModuleHandleExA_new=GetModuleHandleExA */
BOOL WINAPI GetModuleHandleExA_new(
	DWORD     dwFlags,
	LPCSTR   lpModuleName,
	HMODULE*  phModule
)
{
	char buf[MAX_PATH];
	if (!phModule)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}
	*phModule = NULL;
	if (dwFlags & GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS)
	{
		MEMORY_BASIC_INFORMATION mbi;
		if (!VirtualQuery(lpModuleName,&mbi,sizeof(mbi))) return FALSE;
		*phModule = (HMODULE)mbi.AllocationBase;
	}
	else
		*phModule = GetModuleHandleA(lpModuleName);
	if (*phModule == NULL || !GetModuleFileNameA(*phModule,buf,MAX_PATH))
	{
		*phModule = NULL;
		return FALSE;
	}
	if (!(dwFlags & GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT))
		LoadLibraryA(buf);
	
	return TRUE;
}

/* MAKE_EXPORT GetModuleHandleExW_new=GetModuleHandleExW */
BOOL WINAPI GetModuleHandleExW_new(
	DWORD     dwFlags,
	LPCWSTR   lpModuleNameW,
	HMODULE*  phModule
)
{
	if (dwFlags & GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS)
		return GetModuleHandleExA_new(dwFlags,(LPSTR)lpModuleNameW,phModule);
	ALLOC_WtoA(lpModuleName);
	return GetModuleHandleExA_new(dwFlags,lpModuleNameA,phModule);
}

