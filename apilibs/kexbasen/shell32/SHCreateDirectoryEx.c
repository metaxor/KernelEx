/*
 *  KernelEx
 *  Copyright (C) 2010, Tihiy
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

#if defined(_WIN32_IE) && _WIN32_IE != 0x0601
#undef _WIN32_IE
#endif
#define _WIN32_IE 0x0601
#include <shlobj.h>
#include "common.h"

/*  SHCreateDirectory exists since 95 and ofc ANSI;
 *	SHCreateDirectoryEx is in ME shell but no security anyway.
 */

static inline int SHCreateDirectoryA(HWND hwnd, LPCSTR pszPath)
{
	return SHCreateDirectory(hwnd, (LPCWSTR)pszPath);
}

/* MAKE_EXPORT SHCreateDirectoryExA_new=SHCreateDirectoryExA */
int WINAPI SHCreateDirectoryExA_new(HWND hwnd, LPCSTR pszPathA, SECURITY_ATTRIBUTES *psa)
{
	return SHCreateDirectoryA(hwnd, pszPathA);
}


/* MAKE_EXPORT SHCreateDirectoryExW_new=SHCreateDirectoryExW */
int WINAPI SHCreateDirectoryExW_new(HWND hwnd, LPCWSTR pszPathW, SECURITY_ATTRIBUTES *psa)
{
	ALLOC_WtoA(pszPath);
	return SHCreateDirectoryA(hwnd, pszPathA);
}
