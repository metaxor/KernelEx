/*
 *  KernelEx
 *  Copyright (C) 2009, Xeno86
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

#include <shlobj.h>
#include "folderfix.h"
#include "common.h"

/*	Win95 shell doesn't support SHGetSpecialFolderPathA/W so we don't use it;
 *	it has SHGetSpecialFolderPath (@175) however which we link to.
 */
#ifdef SHGetSpecialFolderPath
#undef SHGetSpecialFolderPath
#endif
SHSTDAPI_(BOOL) SHGetSpecialFolderPath(HWND hwndOwner, LPSTR lpszPath, int nFolder, BOOL fCreate);


/* MAKE_EXPORT SHGetSpecialFolderLocation_fix=SHGetSpecialFolderLocation */
HRESULT WINAPI SHGetSpecialFolderLocation_fix(HWND hwndOwner, int nFolder, LPVOID *_ppidl)
{
	LPITEMIDLIST *ppidl = (LPITEMIDLIST*) _ppidl;
	// explorer shouldn't receive this because of start menu bug which displays entries twice
	if (GetModuleHandle("EXPLORER.EXE") == NULL)
		nFolder = folder_fix(nFolder);
	return SHGetSpecialFolderLocation(hwndOwner, nFolder, ppidl);
}

/* MAKE_EXPORT SHGetSpecialFolderPathA_new=SHGetSpecialFolderPathA */
BOOL WINAPI SHGetSpecialFolderPathA_new(HWND hwndOwner, LPSTR lpszPath, int nFolder, BOOL fCreate)
{
	nFolder = folder_fix(nFolder);
	return SHGetSpecialFolderPath(hwndOwner, lpszPath, nFolder, fCreate);
}

/* MAKE_EXPORT SHGetSpecialFolderPathW_new=SHGetSpecialFolderPathW */
BOOL WINAPI SHGetSpecialFolderPathW_new(HWND hwndOwner, LPWSTR lpszPathW, int nFolder, BOOL fCreate)
{
	char pathA[MAX_PATH];
	LPSTR lpszPathA = pathA;
	if (!lpszPathW)
		return E_INVALIDARG;
	*lpszPathW = L'\0';
	BOOL ret = SHGetSpecialFolderPathA_new(hwndOwner,lpszPathA,nFolder,fCreate);
	if (ret)
		AtoW(lpszPath,MAX_PATH);
	return ret;
}
