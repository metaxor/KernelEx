/*
 *  KernelEx
 *  Copyright (C) 2009, Xeno86
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

#include <windows.h>

#ifndef CP_THREAD_ACP
#define CP_THREAD_ACP 3
#endif

#ifndef WC_ERR_INVALID_CHARS
#define WC_ERR_INVALID_CHARS 0x0080
#endif


/* MAKE_EXPORT WideCharToMultiByte_new=WideCharToMultiByte */
int WINAPI WideCharToMultiByte_new(
	UINT CodePage,
	DWORD dwFlags,
	LPCWSTR lpWideCharStr,
	int cchWideChar,
	LPSTR lpMultiByteStr,
	int cchMultiByte,
	LPCSTR lpDefaultChar,
	LPBOOL lpUsedDefaultChar
	)
{
	if (CodePage == CP_THREAD_ACP)
		CodePage = CP_ACP;
	else if (CodePage == CP_UTF8 || CodePage == CP_UTF7)
		dwFlags &= ~WC_ERR_INVALID_CHARS;
	return WideCharToMultiByte(CodePage, dwFlags, lpWideCharStr, cchWideChar, 
		lpMultiByteStr, cchMultiByte, lpDefaultChar, lpUsedDefaultChar);
}

/* MAKE_EXPORT MultiByteToWideChar_new=MultiByteToWideChar */
int WINAPI MultiByteToWideChar_new(
	UINT CodePage,
	DWORD dwFlags,
	LPCSTR lpMultiByteStr,
	int cchMultiByte,
	LPWSTR lpWideCharStr,
	int cchWideChar
	)
{
	if (CodePage == CP_THREAD_ACP)
		CodePage = CP_ACP;
	else if (CodePage == CP_UTF8 || CodePage == CP_UTF7)
		dwFlags &= ~MB_ERR_INVALID_CHARS;
	return MultiByteToWideChar(CodePage, dwFlags, lpMultiByteStr, cchMultiByte,
		lpWideCharStr, cchWideChar);
}
