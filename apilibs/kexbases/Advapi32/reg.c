/*
 *  KernelEx
 *  Copyright (C) 2008, Tihiy
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

/* MAKE_EXPORT RegDisablePredefinedCache_new=RegDisablePredefinedCache */
BOOL WINAPI RegDisablePredefinedCache_new()
{
	return ERROR_SUCCESS;
}

/* MAKE_EXPORT RegOpenCurrentUser_new=RegOpenCurrentUser */
LONG WINAPI RegOpenCurrentUser_new(REGSAM access, PHKEY retkey)
{
	return RegOpenKeyExA(HKEY_CURRENT_USER, NULL, 0, access, retkey);
}

/* MAKE_EXPORT RegSetValueExA_fix=RegSetValueExA */
LONG WINAPI RegSetValueExA_fix(
	HKEY hKey,
	LPCSTR lpValueName,
	DWORD Reserved,
	DWORD dwType,
	CONST BYTE *lpData,
	DWORD cbData
)
{
	if (!lpData && cbData)
		return ERROR_INVALID_PARAMETER;

	if (dwType == REG_SZ || dwType == REG_EXPAND_SZ || dwType == REG_MULTI_SZ)
	{
		if (!lpData)
			lpData = (CONST BYTE*) "";
	}
	
	return RegSetValueExA(hKey, lpValueName, Reserved, dwType, lpData, cbData);
}
