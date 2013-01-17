/*
 *  KernelEx
 *  Copyright (C) 2008, Xeno86
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

static const char signature[] = "KxRC";

/* MAKE_EXPORT GetFileVersionInfoSizeW_new=GetFileVersionInfoSizeW */
DWORD WINAPI GetFileVersionInfoSizeW_new(LPWSTR filenameW, LPDWORD handle)
{
	DWORD size;

	file_GetCP();
	file_ALLOC_WtoA(filename);
	size = GetFileVersionInfoSizeA(filenameA, handle);
	if (size == 0)
		return size;
	return size * (1 + sizeof(WCHAR)) + 4;
}

/* MAKE_EXPORT GetFileVersionInfoW_new=GetFileVersionInfoW */
BOOL WINAPI GetFileVersionInfoW_new(LPWSTR filenameW, DWORD handle, DWORD len, LPVOID data)
{
	DWORD ret;

	file_GetCP();
	file_ALLOC_WtoA(filename);
	ret = GetFileVersionInfoA(filenameA, handle, len, data);
	if (ret)
	{
		if (len < (DWORD)(*(WORD*)data * (1 + sizeof(WCHAR)) + 4))
		{
			SetLastError(ERROR_INSUFFICIENT_BUFFER);
			return FALSE;
		}
		
		memcpy(((char*)data) + *(WORD*)data, signature, 4);
		memset(((char*)data) + *(WORD*)data + 4, 0, *(WORD*)data * sizeof(WCHAR));
	}
	return ret;
}

/* MAKE_EXPORT VerFindFileW_new=VerFindFileW */
DWORD WINAPI VerFindFileW_new(DWORD dwFlags, LPWSTR szFileNameW, LPWSTR szWinDirW, LPWSTR szAppDirW, LPWSTR szCurDirW, PUINT lpuCurDirLenW, LPWSTR szDestDirW, PUINT lpuDestDirLenW)
{
	DWORD ret;
	DWORD last_error = GetLastError();
	DWORD result;
	UINT CurDirLenA = MAX_PATH;
	UINT DestDirLenA = MAX_PATH;
	char szCurDirA[MAX_PATH];
	char szDestDirA[MAX_PATH];
	
	file_GetCP();
	file_ALLOC_WtoA(szFileName);
	file_ALLOC_WtoA(szWinDir);
	file_ALLOC_WtoA(szAppDir);

	*szCurDirA = '\0';
	*szDestDirA = '\0';
	
	ret = VerFindFileA(dwFlags, szFileNameA, szWinDirA, szAppDirA, szCurDirA, &CurDirLenA, szDestDirA, &DestDirLenA);

	result = file_AtoW(szCurDir, *lpuCurDirLenW);
	if (!result)
	{
		szCurDirW[*lpuCurDirLenW - 1] = 0;
		result = file_AtoW(szCurDir, 0);
		ret |= VFF_BUFFTOOSMALL;
	}
	*lpuCurDirLenW = result;
	result = file_AtoW(szDestDir, *lpuDestDirLenW);
	if (!result)
	{
		szDestDirW[*lpuDestDirLenW - 1] = 0;
		result = file_AtoW(szDestDir, 0);
		ret |= VFF_BUFFTOOSMALL;
	}
	*lpuDestDirLenW = result;

	SetLastError(last_error);
	return ret;
}

/* MAKE_EXPORT VerInstallFileW_new=VerInstallFileW */
DWORD WINAPI VerInstallFileW_new(DWORD uFlags, LPWSTR szSrcFileNameW, LPWSTR szDestFileNameW, LPWSTR szSrcDirW, LPWSTR szDestDirW, LPWSTR szCurDirW, LPWSTR szTmpFileW, PUINT lpuTmpFileLenW)
{
	DWORD ret;
	DWORD last_error = GetLastError();
	DWORD result;
	UINT TmpFileLenA = MAX_PATH;
	char szTmpFileA[MAX_PATH];

	file_GetCP();
	file_ALLOC_WtoA(szSrcFileName);
	file_ALLOC_WtoA(szDestFileName);
	file_ALLOC_WtoA(szSrcDir);
	file_ALLOC_WtoA(szDestDir);
	file_ALLOC_WtoA(szCurDir);
	
	*szTmpFileA = '\0';

	ret = VerInstallFileA(uFlags, szSrcFileNameA, szDestFileNameA, szSrcDirA, szDestDirA, szCurDirA, szTmpFileA, &TmpFileLenA);

	result = file_AtoW(szTmpFile, *lpuTmpFileLenW);
	if (!result)
	{
		szTmpFileW[*lpuTmpFileLenW - 1] = 0;
		result = file_AtoW(szTmpFile, 0);
		ret |= VIF_BUFFTOOSMALL;
	}
	*lpuTmpFileLenW = result;

	SetLastError(last_error);
	return ret;
}

/* MAKE_EXPORT VerLanguageNameW_new=VerLanguageNameW */
DWORD WINAPI VerLanguageNameW_new(DWORD wLang, LPWSTR szLangW, DWORD nSize); /* -> implementation is in kernel32 folder */

/* MAKE_EXPORT VerQueryValueW_new=VerQueryValueW */
BOOL WINAPI VerQueryValueW_new(const LPVOID pBlock, LPWSTR lpSubBlockW, LPVOID *lplpBuffer, PUINT puLen)
{
	BOOL ret;

	if (!pBlock || memcmp((char*)pBlock + *(WORD*)pBlock, signature, 4))
		return FALSE;

	ALLOC_WtoA(lpSubBlock);
	ret = VerQueryValueA(pBlock, lpSubBlockA, lplpBuffer, puLen);
	if (ret && strcmpi(lpSubBlockA, "\\") && strcmpi(lpSubBlockA, "\\VarFileInfo\\Translation"))
	{
		WCHAR* lpBlockW = (WCHAR*)((char*)pBlock + *(WORD*)pBlock + 4);
		DWORD pos = (char*)*lplpBuffer - (char*)pBlock;
		WCHAR* p = lpBlockW + pos;
		UINT len = 0;
		if (!*p)
			len = MultiByteToWideChar(CP_ACP, 0, (char*)*lplpBuffer, -1, p, *(WORD*)pBlock - pos);
		else
			do len++; while (*p++);
		*lplpBuffer = lpBlockW + pos;
		*puLen = len;
	}
	return ret;
}
