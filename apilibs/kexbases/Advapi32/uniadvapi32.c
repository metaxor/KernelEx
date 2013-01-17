/*
 *  KernelEx
 *  Copyright (C) 2008-2011, Xeno86
 *  Copyright (C) 2009, Tihiy
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
#include "_advapi32_apilist.h"

//MAKE_EXPORT RegCreateKeyW_new=RegCreateKeyW
LONG WINAPI RegCreateKeyW_new(
	HKEY hKey,
	LPCWSTR lpSubKeyW,
	PHKEY phkResult
)
{
	LPSTR lpSubKeyA;
	STACK_WtoA(lpSubKeyW, lpSubKeyA);
	return RegCreateKeyA(hKey, lpSubKeyA, phkResult);
}

//MAKE_EXPORT RegCreateKeyExW_new=RegCreateKeyExW
LONG WINAPI RegCreateKeyExW_new(
    HKEY hKey,
    LPCWSTR lpSubKeyW,
    DWORD Reserved,
    LPWSTR lpClassW,
    DWORD dwOptions,
    REGSAM samDesired,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    PHKEY phkResult,
    LPDWORD lpdwDisposition
)
{
	LPSTR lpSubKeyA;
	LPSTR lpClassA;
	STACK_WtoA(lpSubKeyW, lpSubKeyA);
	STACK_WtoA(lpClassW, lpClassA);
	return RegCreateKeyExA(hKey, lpSubKeyA, Reserved, lpClassA, dwOptions,
			samDesired, lpSecurityAttributes, phkResult, lpdwDisposition);
}

//MAKE_EXPORT RegDeleteKeyW_new=RegDeleteKeyW
LONG WINAPI RegDeleteKeyW_new(
	HKEY hKey,
	LPCWSTR lpSubKeyW
)
{
	LPSTR lpSubKeyA;
	STACK_WtoA(lpSubKeyW, lpSubKeyA);
	return RegDeleteKeyA(hKey, lpSubKeyA);
}

//MAKE_EXPORT RegOpenKeyW_new=RegOpenKeyW
LONG WINAPI RegOpenKeyW_new(
	HKEY hKey,
	LPCWSTR lpSubKeyW,
	PHKEY phkResult
)
{
	LPSTR lpSubKeyA;
	STACK_WtoA(lpSubKeyW, lpSubKeyA);
	return RegOpenKeyA(hKey, lpSubKeyA, phkResult);
}

//MAKE_EXPORT RegOpenKeyExW_new=RegOpenKeyExW
LONG WINAPI RegOpenKeyExW_new(
	HKEY hKey,
	LPCWSTR lpSubKeyW,
	DWORD ulOptions,
	REGSAM samDesired,
	PHKEY phkResult
)
{
	LPSTR lpSubKeyA;
	STACK_WtoA(lpSubKeyW, lpSubKeyA);
	return RegOpenKeyExA(hKey, lpSubKeyA, ulOptions, samDesired, phkResult);
}

//MAKE_EXPORT RegQueryValueExW_new=RegQueryValueExW
LONG WINAPI RegQueryValueExW_new(
	HKEY hKey,
	LPCWSTR lpValueNameW,
	LPDWORD lpReserved, 
	LPDWORD lpType,
	LPBYTE lpData,
	LPDWORD lpcbData
)
{
	LONG ret;
	DWORD type;
	BYTE stackbuf[256];
	BYTE* ptr = stackbuf;
	BYTE* heapbuf = NULL;
	DWORD bufsize = sizeof(stackbuf);
	DWORD strsize;
	
	if ((lpData && !lpcbData) || lpReserved) return ERROR_INVALID_PARAMETER;
	
	//try with stack buffer first
	ALLOC_WtoA(lpValueName);
	ret = RegQueryValueExA(hKey, lpValueNameA, lpReserved, &type, ptr, &bufsize);
	if (lpType) *lpType = type;

	//retry with dynamic buffer
	if (ret == ERROR_MORE_DATA)
	{
		ptr = heapbuf = (BYTE*) HeapAlloc(GetProcessHeap(), 0, bufsize);
		if (!heapbuf) 
		{
			return ERROR_NOT_ENOUGH_MEMORY;
		}
		ret = RegQueryValueExA(hKey, lpValueNameA, lpReserved, &type, ptr, &bufsize);
	}

	if (ret == ERROR_SUCCESS && lpcbData) //copy data
	{
		switch(type) {	
		case REG_SZ:
		case REG_EXPAND_SZ:
		case REG_MULTI_SZ:		
			strsize = MultiByteToWideChar(CP_ACP,0,(LPSTR)ptr,bufsize,NULL,NULL)*sizeof(WCHAR);
			if (lpData)
			{
				if (*lpcbData<strsize)
					ret = ERROR_MORE_DATA;
				else
					MultiByteToWideChar(CP_ACP,0,(LPSTR)ptr,bufsize,(LPWSTR)lpData,*lpcbData/sizeof(WCHAR));	
			}
			*lpcbData = strsize;
			break;
		default:
			if (lpData)
			{
				if (*lpcbData<bufsize)
					ret = ERROR_MORE_DATA;
				else
					memcpy(lpData, ptr, bufsize);
			}
			*lpcbData = bufsize;
		}
	}	

	if (heapbuf) HeapFree(GetProcessHeap(), 0, heapbuf);
	return ret;
}

//MAKE_EXPORT RegEnumValueW_new=RegEnumValueW
LONG WINAPI RegEnumValueW_new(
	HKEY hKey,             // handle to key to query
	DWORD dwIndex,         // index of value to query
	LPWSTR lpValueName,    // value buffer
	LPDWORD lpcValueName,  // size of value buffer
	LPDWORD lpReserved,    // reserved
	LPDWORD lpType,        // type buffer
	LPBYTE lpData,         // data buffer
	LPDWORD lpcbData       // size of data buffer
)
{
	LONG ret;
	DWORD type;
	CHAR valnamebuf[256];
	DWORD valsize = sizeof(valnamebuf);
	BYTE stackbuf[256];
	DWORD bufsize = sizeof(stackbuf);
	BYTE* ptr = stackbuf;
	BYTE* heapbuf = NULL;
	DWORD strsize;
	
	if ((lpData && !lpcbData) || !lpValueName || !lpcValueName) return ERROR_INVALID_PARAMETER;

	ret = RegEnumValueA(hKey, dwIndex, valnamebuf, &valsize, lpReserved, &type, ptr, &bufsize);
	if (lpType) *lpType = type;

	if (ret == ERROR_MORE_DATA) //retry with heap buffer
	{
		ptr = heapbuf = (BYTE*) HeapAlloc(GetProcessHeap(), 0, bufsize);
		if (!heapbuf) 
		{
			return ERROR_NOT_ENOUGH_MEMORY;
		}
		ret = RegEnumValueA(hKey, dwIndex, valnamebuf, &valsize, lpReserved, &type, ptr, &bufsize);
	}
	
	//check if value name buffer is big enough
	//note, it has to be with null while we have size without
	//also note, if buffer for value name is not long enough
	//RegEnumValue will not set *lpcValueName to right size
	//(9x also erroneously does (*lpcValueName)--;)

	valsize++;
	if (ret == ERROR_SUCCESS && *lpcValueName<valsize) ret = ERROR_MORE_DATA;

	if (ret == ERROR_SUCCESS && lpcbData) //copy data
	{
		switch(type) {	
		case REG_SZ:
		case REG_EXPAND_SZ:
		case REG_MULTI_SZ:		
			strsize = MultiByteToWideChar(CP_ACP,0,(LPSTR)ptr,bufsize,NULL,NULL)*sizeof(WCHAR);
			if (lpData)
			{
				if (*lpcbData<strsize)
					ret = ERROR_MORE_DATA;
				else
					MultiByteToWideChar(CP_ACP,0,(LPSTR)ptr,bufsize,(LPWSTR)lpData,*lpcbData/sizeof(WCHAR));	
			}
			*lpcbData = strsize;
			break;
		default:
			if (lpData)
			{
				if (*lpcbData<bufsize)
					ret = ERROR_MORE_DATA;
				else
					memcpy(lpData, ptr, bufsize);
			}
			*lpcbData = bufsize;
		}
	}

	if (ret == ERROR_SUCCESS) //copy valname
	{
		*lpcValueName = MultiByteToWideChar(CP_ACP,0,valnamebuf,valsize,lpValueName,*lpcValueName);
		(*lpcValueName)--;
	}

	if (heapbuf) HeapFree(GetProcessHeap(), 0, heapbuf);
	return ret;
}

/* MAKE_EXPORT RegSetValueExW_new=RegSetValueExW */
LONG WINAPI RegSetValueExW_new(
	HKEY hKey,
	LPCWSTR lpValueName,
	DWORD Reserved,
	DWORD dwType,
	CONST BYTE *lpData,
	DWORD cbData
)
{
	LPSTR strA;
	
	STACK_WtoA(lpValueName, strA);
	
	if (dwType == REG_SZ || dwType == REG_EXPAND_SZ || dwType == REG_MULTI_SZ)
	{
		if (HIWORD(lpData))
		{
			LPSTR lpDataA;
			int cbDataA;

			cbData = (cbData + 1) / 2;
			cbDataA = WideCharToMultiByte(CP_ACP, 0, (LPWSTR) lpData, cbData, NULL, 0, NULL, NULL);
			lpDataA = (LPSTR) alloca(cbDataA + 1);
			WideCharToMultiByte(CP_ACP, 0, (LPWSTR) lpData, cbData, lpDataA, cbDataA, NULL, NULL);
			lpDataA[cbDataA] = 0;
			lpData = (CONST BYTE*) lpDataA;
		}
	}
	
	return RegSetValueExA_fix(hKey, strA, Reserved, dwType, lpData, cbData); 
}
