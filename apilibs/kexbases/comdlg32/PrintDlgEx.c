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

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <ole2.h>
#include <commdlg.h>

/* MAKE_EXPORT PrintDlgEx_new=PrintDlgExA
 * MAKE_EXPORT PrintDlgEx_new=PrintDlgExW 
 */
HRESULT WINAPI PrintDlgEx_new(LPPRINTDLGEX lppd)
{
	PRINTDLG pdlg;

	if(lppd == NULL || IsBadReadPtr(lppd, sizeof(PRINTDLGEX)) || IsBadWritePtr(lppd, sizeof(PRINTDLGEX)))
		return E_POINTER;

	if(lppd->lStructSize != sizeof(PRINTDLGEX))
		return E_FAIL;

	pdlg.lStructSize = sizeof(PRINTDLG);

	pdlg.Flags = lppd->Flags;
	pdlg.hDC = lppd->hDC;
	pdlg.hDevMode = lppd->hDevMode;
	pdlg.hDevNames = lppd->hDevNames;
	pdlg.hInstance = lppd->hInstance;
	pdlg.hwndOwner = lppd->hwndOwner;
	pdlg.lpPrintTemplateName = lppd->lpPrintTemplateName;
	pdlg.nCopies = (SHORT)lppd->nCopies;
	pdlg.nFromPage = (SHORT)lppd->nStartPage;
	pdlg.nMaxPage = (SHORT)lppd->nMaxPage;
	pdlg.nMinPage = (SHORT)lppd->nMinPage;
	pdlg.nToPage = (SHORT)lppd->nStartPage + (SHORT)lppd->nCopies;

	return PrintDlg(&pdlg);
}
