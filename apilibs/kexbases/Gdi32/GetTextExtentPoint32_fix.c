/*
 *  KernelEx
 *  Copyright (C) 2010, Xeno86
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

/* MAKE_EXPORT GetTextExtentPoint32A_fix=GetTextExtentPoint32A */
BOOL WINAPI GetTextExtentPoint32A_fix(HDC hdc, LPCSTR str, int count, LPSIZE size)
{
	if (count < 0)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}
	return GetTextExtentPoint32A(hdc, str, count, size);
}

/* MAKE_EXPORT GetTextExtentPoint32W_fix=GetTextExtentPoint32W */
BOOL WINAPI GetTextExtentPoint32W_fix(HDC hdc, LPCWSTR str, int count, LPSIZE size)
{
	if (count < 0)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}
	return GetTextExtentPoint32W(hdc, str, count, size);
}
