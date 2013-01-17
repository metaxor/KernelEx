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

#define CINTERFACE
#include <shlobj.h>
#include <shlwapi.h>

/* MAKE_EXPORT SHParseDisplayName_new=SHParseDisplayName */
HRESULT WINAPI SHParseDisplayName_new(PCWSTR pszName, IBindCtx *pbc, LPITEMIDLIST *ppidl, SFGAOF sfgaoIn, SFGAOF *psfgaoOut)
{
	IShellFolder *psf;
	HRESULT ret = SHGetDesktopFolder(&psf);
	if (SUCCEEDED(ret))
	{
		ULONG attrs = sfgaoIn;
		LPOLESTR pszNameCopyW = StrDupW(pszName);
		ret = psf->lpVtbl->ParseDisplayName(psf,NULL,pbc,pszNameCopyW,NULL,ppidl,&attrs);
		if (psfgaoOut) *psfgaoOut = attrs;
		psf->lpVtbl->Release(psf);
		LocalFree(pszNameCopyW);
	}
	return ret;
}
