/*
 *  KernelEx
 *  Copyright (C) 2009, Xeno86, Tihiy
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
#include <shlobj.h>
#include "common.h"
#include "_shell32_apilist.h"

typedef HRESULT (WINAPI *SHGetFolderLocation_t)(HWND, int, HANDLE, DWORD, LPVOID*);

HRESULT WINAPI SHGetFolderLocation_impl(
	HWND hwndOwner,
	int nFolder,
	HANDLE hToken,
	DWORD dwReserved,
	LPVOID *ppidl
)
{
	if (!ppidl) 
		return E_INVALIDARG;
	if (dwReserved) 
		return E_INVALIDARG;

	HRESULT hr = SHGetSpecialFolderLocation_fix(hwndOwner, nFolder, ppidl);
	if (!SUCCEEDED(hr))
	{
		char szPath[MAX_PATH];

		hr = SHGetFolderPathA_new(hwndOwner, nFolder, hToken, SHGFP_TYPE_CURRENT, szPath);
		if (SUCCEEDED(hr))
		{
			DWORD attributes = 0;
			hr = SHILCreateFromPath((LPWSTR)szPath, (LPITEMIDLIST*)ppidl, &attributes);
		}
		else if (hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
		{
			/* unlike SHGetFolderPath, SHGetFolderLocation in shell32
			 * version 6.0 returns E_FAIL for nonexistent paths
			 */
			hr = E_FAIL;
		}
	}
	if (*ppidl)
		hr = NOERROR;
	return hr;
}

static SHGetFolderLocation_t dld()
{
	HMODULE h = GetModuleHandle("SHELL32");
	SHGetFolderLocation_t pfn = (SHGetFolderLocation_t) 
			kexGetProcAddress(h, "SHGetFolderLocation");
	if (!pfn)
		pfn = SHGetFolderLocation_impl;
	return pfn;
}

/* MAKE_EXPORT SHGetFolderLocation_new=SHGetFolderLocation */
HRESULT WINAPI SHGetFolderLocation_new(
	HWND hwndOwner,
	int nFolder,
	HANDLE hToken,
	DWORD dwReserved,
	LPVOID *ppidl
)
{
	static SHGetFolderLocation_t SHGetFolderLocation_pfn;
	if (!SHGetFolderLocation_pfn) 
		SHGetFolderLocation_pfn = dld();
	return SHGetFolderLocation_pfn(hwndOwner, nFolder, hToken, dwReserved, ppidl);
}
