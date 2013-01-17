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

#include "common.h"
#include <winspool.h>

/* MAKE_EXPORT GetDefaultPrinterA_new=GetDefaultPrinterA */
BOOL WINAPI GetDefaultPrinterA_new(LPSTR buf, LPDWORD size)
{
	DWORD needed, returned;
	BOOL ret;
	DWORD len;

	EnumPrinters(PRINTER_ENUM_DEFAULT, NULL, 1, NULL, 0, &needed, &returned);
	if (needed == 0)
	{
		SetLastError(ERROR_FILE_NOT_FOUND);
		return FALSE;
	}
	PRINTER_INFO_1* ppi = (PRINTER_INFO_1*) alloca(needed);
	ret = EnumPrinters(PRINTER_ENUM_DEFAULT, NULL, 1, (BYTE*) ppi, needed, &needed, &returned);
	if (!ret)
	{
		SetLastError(ERROR_FILE_NOT_FOUND);
		return FALSE;
	}
	len = strlen(ppi->pName);
	if (len >= *size)
	{
		*size = len + 1;
		SetLastError(ERROR_INSUFFICIENT_BUFFER);
		return FALSE;
	}
	strcpy(buf, ppi->pName);
	*size = len + 1;
	return TRUE;
}

/* MAKE_EXPORT GetDefaultPrinterW_new=GetDefaultPrinterW */
BOOL WINAPI GetDefaultPrinterW_new(LPWSTR bufW, LPDWORD sizeW)
{
	BOOL ret;
	DWORD sizeA = *sizeW;
	ALLOC_A(buf, sizeA);
	ret = GetDefaultPrinterA(bufA, &sizeA);
	if (ret)
	{
		*sizeW = ABUFtoW(buf, sizeA, *sizeW);
		if (*sizeW)
			return TRUE;
		*sizeW = ABUFtoW(buf, sizeA, 0);
	}
	return FALSE;
}
