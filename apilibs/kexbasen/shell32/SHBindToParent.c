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

/* MAKE_EXPORT SHBindToParent_new=SHBindToParent */
HRESULT WINAPI SHBindToParent_new(LPCITEMIDLIST pidl, REFIID riid, VOID **ppv, LPCITEMIDLIST *ppidlLast)
{
	HRESULT hres;
	LPITEMIDLIST pidlParent = ILClone(pidl);
	if (pidlParent)
	{
		ILRemoveLastID(pidlParent);
		IShellFolder *psf;
		hres = SHGetDesktopFolder(&psf);
		if (SUCCEEDED(hres))
		{
			if (pidl->mkid.cb == 0) //empty i.e. desktop
				hres = psf->lpVtbl->QueryInterface(psf,riid,ppv);
			else
				hres = psf->lpVtbl->BindToObject(psf,pidlParent,NULL,riid,ppv);
			psf->lpVtbl->Release(psf);
		}	
		ILFree(pidlParent);

		if (SUCCEEDED(hres) && (*ppv == NULL)) //no interface provided?
			hres = E_FAIL;			
	}
	else
		hres = E_OUTOFMEMORY;

	if (ppidlLast)
		*ppidlLast = ILFindLastID(pidl);
	
	return hres;
}
