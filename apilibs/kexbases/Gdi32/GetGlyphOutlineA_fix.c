/*
 *  KernelEx
 *
 *  Copyright (C) 2008, Tihiy
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

/* MAKE_EXPORT GetGlyphOutlineA_fix=GetGlyphOutlineA */
DWORD WINAPI GetGlyphOutlineA_fix(
	HDC hdc,             // handle to DC
	UINT uChar,          // character to query
	UINT uFormat,        // data format
	LPGLYPHMETRICS lpgm, // glyph metrics
	DWORD cbBuffer,      // size of data buffer
	LPVOID lpvBuffer,    // data buffer
	CONST MAT2 *lpmat2   // transformation matrix
)
{
	//last parameter must point to writeable memory
	MAT2 matr;
	if (lpmat2)
	{
		memcpy( &matr, lpmat2, sizeof(MAT2) );
		lpmat2 = &matr;
	}
	return GetGlyphOutlineA( hdc, uChar, uFormat, lpgm, cbBuffer, lpvBuffer, lpmat2 );
}
