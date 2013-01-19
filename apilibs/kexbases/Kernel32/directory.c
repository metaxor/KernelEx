/*
 *  KernelEx
 *  Copyright (C) 2011, Xeno86
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

/* MAKE_EXPORT SetDllDirectoryA_new=SetDllDirectoryA */
BOOL WINAPI SetDllDirectoryA_new(LPCSTR lpPathName)
{
	DWORD i, j, len, lasterr;
	char* buf;
	BOOL ret;
	
	lasterr = GetLastError();
	
	i = GetEnvironmentVariable("PATH", NULL, 0);
	j = strlen(lpPathName);
	
	len = i + j + 2;
	buf = (char*) HeapAlloc(GetProcessHeap(), 0, len);
	GetEnvironmentVariable("PATH", buf, i);
	if (i)
		strcpy(buf + i - 1, ";");
	strcpy(buf + i, lpPathName);
	
	SetLastError(lasterr);
	
	ret = SetEnvironmentVariable("PATH", buf);
	
	HeapFree(GetProcessHeap(), 0, buf);
	
	return ret;
}

/* MAKE_EXPORT SetDllDirectoryW_new=SetDllDirectoryW */
BOOL WINAPI SetDllDirectoryW_new(LPCWSTR lpPathNameW)
{
	ALLOC_WtoA(lpPathName);
	return SetDllDirectoryA_new(lpPathNameA);
}
