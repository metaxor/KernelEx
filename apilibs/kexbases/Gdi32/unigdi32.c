/*
 *  KernelEx
 *  Copyright (C) 2009-2010, Xeno86
 *  Copyright (C) 2009, Tihiy
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
#include "_gdi32_apilist.h"

//MAKE_EXPORT AddFontResourceExW_new=AddFontResourceExW
INT WINAPI AddFontResourceExW_new(LPCWSTR strW, DWORD fl, PVOID pdv)
{
	LPSTR strA;
	STACK_WtoA(strW,strA)
	return AddFontResourceExA_new(strA, fl, pdv);
}

//MAKE_EXPORT AddFontResourceW_new=AddFontResourceW
INT WINAPI AddFontResourceW_new(LPCWSTR strW)
{
	LPSTR strA;
	STACK_WtoA(strW,strA)
	return AddFontResourceA(strA);
}

//MAKE_EXPORT RemoveFontResourceExW_new=RemoveFontResourceExW
BOOL WINAPI RemoveFontResourceExW_new(LPCWSTR strW, DWORD fl, PVOID pdv)
{
	LPSTR strA;
	STACK_WtoA(strW,strA)
	return RemoveFontResourceExA_new(strA, fl, pdv);
}

//MAKE_EXPORT RemoveFontResourceW_new=RemoveFontResourceW
BOOL WINAPI RemoveFontResourceW_new(LPCWSTR strW)
{
	LPSTR strA;
	STACK_WtoA(strW,strA)
	return RemoveFontResourceA(strA);
}

/* MAKE_EXPORT ExtCreatePen_fix=ExtCreatePen */
HPEN WINAPI ExtCreatePen_fix(
	DWORD dwPenStyle,      // pen style
	DWORD dwWidth,         // pen width
	CONST LOGBRUSH *lplb,  // brush attributes
	DWORD dwStyleCount,    // length of custom style array
	CONST DWORD *lpStyle   // custom style array
)
{
	dwPenStyle &= ~PS_USERSTYLE;
	return ExtCreatePen(dwPenStyle,dwWidth,lplb,dwStyleCount,lpStyle);
}

/* MAKE_EXPORT GetObjectW_new=GetObjectW */
int WINAPI GetObjectW_new(
	HGDIOBJ hgdiobj,  // handle to graphics object
	int cbBuffer,     // size of buffer for object information
	LPVOID lpvObject  // buffer for object information
)
{
	int type = GetObjectType_fix(hgdiobj);
	if (type != OBJ_FONT) return GetObjectA(hgdiobj,cbBuffer,lpvObject);
	if (!lpvObject) return sizeof(LOGFONTW);
	LOGFONTA fontA = {0};
	LOGFONTW fontW = {0};
	if (!GetObjectA(hgdiobj,sizeof(LOGFONTA),&fontA)) return 0; //err not font
	memcpy(&fontW,&fontA,FIELD_OFFSET(LOGFONTA,lfFaceName));
	if (hgdiobj == GetStockObject(DEFAULT_GUI_FONT)) //unicode apps get Tahoma
		lstrcpyW(fontW.lfFaceName,L"Tahoma");
	else
		MultiByteToWideChar(CP_ACP,0,fontA.lfFaceName,-1,fontW.lfFaceName,LF_FACESIZE);	
	memcpy(lpvObject,&fontW,cbBuffer);
	return cbBuffer;
}
