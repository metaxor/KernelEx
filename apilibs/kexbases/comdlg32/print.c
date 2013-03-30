/*
 *  KernelEx
 *  Copyright (C) 2008, Xeno86
 *	Copyright (C) 2013, Ley0k
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
#include "common.h"
#include <ole2.h>
#include <commdlg.h>

/* MAKE_EXPORT PrintDlgExA_new=PrintDlgExA */
HRESULT WINAPI PrintDlgExA_new(LPPRINTDLGEXA lppd)
{
	PRINTDLGA pdlg;

	if(lppd == NULL || IsBadReadPtr(lppd, sizeof(PRINTDLGEXA)) || IsBadWritePtr(lppd, sizeof(PRINTDLGEXA)))
		return E_POINTER;

	if(lppd->lStructSize != sizeof(PRINTDLGEXA))
		return E_FAIL;

	pdlg.lStructSize			= sizeof(PRINTDLGA);

	pdlg.hwndOwner				= lppd->hwndOwner;
	pdlg.hDevMode				= lppd->hDevMode;
	pdlg.hDevNames				= lppd->hDevNames;
	pdlg.hDC					= lppd->hDC;
	pdlg.Flags					= !(lppd->Flags & PD_USEDEVMODECOPIESANDCOLLATE) ? PD_USEDEVMODECOPIESANDCOLLATE : 0
									| !(lppd->Flags & PD_RETURNDC) ? PD_RETURNDC : 0 | lppd->Flags;
	pdlg.nCopies				= 1;
	pdlg.nFromPage				= 0xFFFF; 
	pdlg.nToPage				= 0xFFFF; 
	pdlg.nMinPage				= (SHORT)lppd->nMinPage;
	pdlg.nMaxPage				= (SHORT)lppd->nMaxPage;
	pdlg.nCopies				= (SHORT)lppd->nCopies;
	pdlg.hInstance				= lppd->hInstance;
	pdlg.lCustData				= 0;
	pdlg.lpfnPrintHook			= NULL;
	pdlg.lpfnSetupHook			= NULL;
	pdlg.lpPrintTemplateName	= lppd->lpPrintTemplateName;
	pdlg.lpSetupTemplateName	= NULL;
	pdlg.hPrintTemplate			= NULL;
	pdlg.hSetupTemplate			= NULL;


	if(PrintDlg(&pdlg))
	{
		lppd->dwResultAction	= PD_RESULT_PRINT;
		lppd->hDevMode			= pdlg.hDevMode;
		lppd->hDevNames			= pdlg.hDevNames;
		lppd->hDC				= pdlg.hDC;
		lppd->nPageRanges		= 0;
		lppd->nMaxPageRanges	= 0;
		lppd->lpPageRanges		= NULL;
		lppd->nMinPage			= pdlg.nMinPage;
		lppd->nMaxPage			= pdlg.nMaxPage;
		lppd->nCopies			= pdlg.nCopies;
	}
	else
		lppd->dwResultAction	= PD_RESULT_CANCEL;

	return S_OK;
}

/* MAKE_EXPORT PrintDlgExW_new=PrintDlgExW */
HRESULT WINAPI PrintDlgExW_new(LPPRINTDLGEXW lppd)
{
	PRINTDLGEXA pdlg;
	PSTR lpPrintTemplateNameA = NULL;

	if(lppd == NULL || IsBadReadPtr(lppd, sizeof(PRINTDLGEXW)) || IsBadWritePtr(lppd, sizeof(PRINTDLGEXW)))
		return E_POINTER;

	if(lppd->lStructSize != sizeof(PRINTDLGEXW))
		return E_FAIL;

	memcpy(&pdlg, lppd, sizeof(PRINTDLGEXA));

	STACK_WtoA(pdlg.lpPrintTemplateName, lpPrintTemplateNameA);

	pdlg.lpPrintTemplateName = lpPrintTemplateNameA;

	return PrintDlgExA_new(&pdlg);
}