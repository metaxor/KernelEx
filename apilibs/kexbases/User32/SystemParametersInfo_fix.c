/*
 *  KernelEx
 *  Copyright (C) 2010, Tihiy, Xeno86
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

/* MAKE_EXPORT SystemParametersInfoA_fix=SystemParametersInfoA */
BOOL WINAPI SystemParametersInfoA_fix(
	UINT uiAction,
	UINT uiParam,
	PVOID pvParam,
	UINT fWinIni
)
{
	BOOL ret;
	
	if (uiAction == SPI_GETHIGHCONTRAST && uiParam == 0)
	{
		uiParam = sizeof(HIGHCONTRAST);
	}
	ret = SystemParametersInfoA(uiAction, uiParam, pvParam, fWinIni);
	if (!ret && uiAction == SPI_GETHIGHCONTRAST && pvParam)
	{
		LPHIGHCONTRAST hc = (LPHIGHCONTRAST) pvParam;
		hc->dwFlags = 0;
		hc->lpszDefaultScheme = NULL;
		ret = TRUE;
	}

	if (ret && uiAction == SPI_GETCARETWIDTH && pvParam)
	{
		//there's a bug in USER: default caret width is 0
		//(should be 1). although NT can be forced to set
		//caret width to 0, it's not sane so we fix this.
		DWORD* pCaretWidth = (DWORD*)pvParam;
		if (*pCaretWidth == 0)
			*pCaretWidth = 1;
	}

	return ret;
}
