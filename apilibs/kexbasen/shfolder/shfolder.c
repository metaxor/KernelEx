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
#include "common.h"
#include "../shell32/_shell32_apilist.h"

/* MAKE_EXPORT SHFOLDER_SHGetFolderPathA_new=SHGetFolderPathA */
HRESULT WINAPI SHFOLDER_SHGetFolderPathA_new(HWND hwndOwner, int nFolder, HANDLE hToken, DWORD dwFlags, LPSTR pszPath)
{
	return SHGetFolderPathA_new(hwndOwner, nFolder, hToken, dwFlags, pszPath);
}

/* MAKE_EXPORT SHFOLDER_SHGetFolderPathW_new=SHGetFolderPathW */
HRESULT WINAPI SHFOLDER_SHGetFolderPathW_new(HWND hwndOwner, int nFolder, HANDLE hToken, DWORD dwFlags, LPWSTR pszPath)
{
	return SHGetFolderPathW_new(hwndOwner, nFolder, hToken, dwFlags, pszPath);
}
