/*
 *  KernelEx
 *
 *  Copyright 1993 Alexandre Julliard
 *            1997 Alex Korobka
 *  Copyright 2002,2003 Shachar Shemesh 
 *  Copyright 2003 CodeWeavers Inc. (Ulrich Czekalla) 
 *
 *  Copyright (C) 2008, 2010, Tihiy
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

#include <windows.h>
#include <malloc.h>

#include "GdiObjects.h"

#ifdef _MSC_VER
#ifdef __cplusplus
extern "C"
#endif
__declspec(selectany) int _fltused=1;
#endif

#ifndef ETO_PDY
#define ETO_PDY 0x2000
#endif

/* MAKE_EXPORT GetFontUnicodeRanges_new=GetFontUnicodeRanges */
DWORD WINAPI GetFontUnicodeRanges_new(
  HDC hdc,
  LPGLYPHSET lpgs
)
{
	SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
	return 0;
}

/* On 9x fallback to system function */
/* MAKE_EXPORT SetGraphicsMode_NT=SetGraphicsMode */
int WINAPI SetGraphicsMode_NT(
	HDC hdc,    // handle to device context
	int iMode   // graphics mode
)
{
	return GM_COMPATIBLE;
}

/* MAKE_EXPORT SetWorldTransform_9x=SetWorldTransform */
BOOL WINAPI SetWorldTransform_9x(
	HDC hdc,               // handle to device context
	CONST XFORM *lpXform   // transformation data
)
{
	return FALSE;
}

/* MAKE_EXPORT GetRandomRgn_NT=GetRandomRgn */
int WINAPI GetRandomRgn_NT(
	HDC  hdc,    // handle to DC
	HRGN hrgn,   // handle to region
	INT  iNum    // must be SYSRGN
)
{
	int result = GetRandomRgn(hdc,hrgn,iNum);
	if (result)
	{
		POINT pt;
		GetDCOrgEx(hdc,&pt);
		OffsetRgn(hrgn,pt.x,pt.y);
	}
	return result;
}

void floattofrac( float f, int* m, int* d)
{
	float absf = f > 0 ? f : -f;
	if ( absf < 1 )
	{
		*m = 1;
		*d = (int) (1 / f);
	}
	else
	{
		*m = (int)f;
		*d = 1;
	}
}

#define almostzero(x) ( x > -0.0001 && x < 0.0001  )

/* MAKE_EXPORT SetWorldTransform_NT=SetWorldTransform */
BOOL WINAPI SetWorldTransform_NT(
	HDC hdc,               // handle to device context
	CONST XFORM *lpXform   // transformation data
)
{
	GrabWin16Lock();
	PDCOBJ dcobj = GetDCObj(hdc);
	WORD savemapmode;
	int wx;
	int wy;
	int vx;
	int vy;
	if ( !dcobj || !lpXform || !almostzero(lpXform->eM12) || !almostzero(lpXform->eM21)
		|| almostzero(lpXform->eM11) || almostzero(lpXform->eM22) )
	{
		ReleaseWin16Lock();
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE; //no rotating for you
	}
	floattofrac( lpXform->eM11, &vx, &wx );
	floattofrac( lpXform->eM22, &vy, &wy );
	//hack DC mode to anisotropic to make Set*ExtEx work
	savemapmode = dcobj->mapmode;
	dcobj->mapmode = MM_ANISOTROPIC;
	ReleaseWin16Lock();
	SetWindowExtEx(hdc,wx,wy,NULL);
	SetViewportExtEx(hdc,vx,vy,NULL);
	SetViewportOrgEx(hdc,(int)lpXform->eDx,(int)lpXform->eDy,NULL);
	//set it back
	GrabWin16Lock();
	dcobj = GetDCObj(hdc);
	dcobj->mapmode = savemapmode;
	ReleaseWin16Lock();
	return TRUE;
}

/************************************************************************/
/* Those hacks shouldn't hurt anybody.                                  */
/************************************************************************/

/* MAKE_EXPORT SetMapMode_NT=SetMapMode */
int WINAPI SetMapMode_NT( 
	HDC hdc,        // handle to device context
	int fnMapMode   // new mapping mode
)
{
	
	if ( fnMapMode == MM_TEXT && GetMapMode(hdc) == MM_TEXT ) return MM_TEXT;
	return SetMapMode( hdc, fnMapMode );
}

/* MAKE_EXPORT GetTextMetricsA_NT=GetTextMetricsA */
BOOL WINAPI GetTextMetricsA_NT(
	HDC hdc,            // handle to DC
	LPTEXTMETRIC lptm   // text metrics
)
{
	GrabWin16Lock();
	PDCOBJ dcobj = GetDCObj(hdc);
	int saved = 0;
	BOOL retval;
	if ( !dcobj )
	{
		ReleaseWin16Lock();
		return FALSE;
	}
	if ( dcobj->ViewportExtX != 1 || dcobj->ViewportExtY != 1 || dcobj->WindowExtX != 1 || dcobj->WindowExtY != 1 )
	{
		ReleaseWin16Lock();
		saved = SaveDC(hdc);
		ResetMapMode(hdc);
	}
	else
		ReleaseWin16Lock();	
	retval = GetTextMetricsA(hdc,lptm);
	if ( saved )
		RestoreDC(hdc,-1);	
	return retval;
}

/* MAKE_EXPORT GetWorldTransform_NT=GetWorldTransform */
BOOL WINAPI GetWorldTransform_NT(
	HDC hdc,         // handle to device context
	LPXFORM lpXform  // transformation
)
{
	SIZE v;
	SIZE w;
	POINT org;
	if ( !lpXform ) return FALSE;
	GetWindowExtEx(hdc,&w);
	GetViewportExtEx(hdc,&v);
	GetViewportOrgEx(hdc,&org);
	lpXform->eM11 = (float)v.cx/w.cx;
	lpXform->eM12 = 0;
	lpXform->eM21 = 0;
	lpXform->eM22 = (float)v.cy/w.cy;
	lpXform->eDx = (float)org.x;
	lpXform->eDy = (float)org.y;
	return TRUE;
}

/* MAKE_EXPORT ModifyWorldTransform_NT=ModifyWorldTransform */
BOOL WINAPI ModifyWorldTransform_NT(
	HDC hdc,               // handle to device context
	CONST XFORM *lpXform,  // transformation data
	DWORD iMode            // modification mode
)
{
	//we accept only 'reset world' scenario
	XFORM x;
	if ( iMode != MWT_IDENTITY ) return FALSE;
	x.eDx = 0;
	x.eDy = 0;
	x.eM11 = 1;
	x.eM12 = 0;
	x.eM21 = 0;
	x.eM22 = 1;
	return SetWorldTransform_NT(hdc,&x);
}

/************************************************************************/
/* Text out API fixes                                                   */
/************************************************************************/


static void WINAPI MakeDxFromDxDy(const int* lpDx, int* newlpDx, UINT cbCount)
{
	UINT oneDx;
	for (oneDx = 0; oneDx < cbCount; oneDx++)
	{
		*newlpDx = *lpDx;
		newlpDx++;
		lpDx++;
		lpDx++;
	}
	newlpDx--;
	if (*newlpDx == 0) *newlpDx = 16; //TERRIBLE HACK but needed on some fonts/glyphs/etc!
}

/* MAKE_EXPORT ExtTextOutA_new=ExtTextOutA */
BOOL WINAPI ExtTextOutA_new(
	HDC hdc,          // handle to DC
	int X,            // x-coordinate of reference point
	int Y,            // y-coordinate of reference point
	UINT fuOptions,   // text-output options
	CONST RECT* lprc, // optional dimensions
	LPCSTR lpString, // string
	UINT cbCount,     // number of characters in string
	CONST INT* lpDx   // array of spacing values
)
{
	BOOL result;
	int* buffer = NULL;
	
	if ( HIWORD(lpString) )
	{
		if (cbCount>8192) cbCount = 8192;
		if (fuOptions & ETO_PDY)	//win9x can't understand it and messes up other flags
		{
			fuOptions = fuOptions & ~ETO_PDY; 
			if (lpDx)				//we have to make array which contains dx values only
			{	
				if ( cbCount>128 )
					buffer = (int*)HeapAlloc(GetProcessHeap(),0,cbCount*sizeof(int));
				else
					buffer = (int*)alloca(cbCount*sizeof(int));
				
				if (!buffer) 
					return FALSE;
				MakeDxFromDxDy(lpDx,buffer, cbCount);
				lpDx = buffer;
			}
		}
	}
	result = ExtTextOutA(hdc,X,Y,fuOptions,lprc,lpString,cbCount,lpDx);
	if ( buffer && cbCount>128 )
		HeapFree(GetProcessHeap(),0,buffer);
	return result;
}


/* MAKE_EXPORT ExtTextOutW_new=ExtTextOutW */
BOOL WINAPI ExtTextOutW_new(
	HDC hdc,          // handle to DC
	int X,            // x-coordinate of reference point
	int Y,            // y-coordinate of reference point
	UINT fuOptions,   // text-output options
	CONST RECT* lprc, // optional dimensions
	LPCWSTR lpString, // string
	UINT cbCount,     // number of characters in string
	CONST INT* lpDx   // array of spacing values
)
{
	BOOL result;
	int* buffer = NULL;
	WORD savemapmode = 0;
	
	if ( HIWORD(lpString) )
	{
		if (cbCount>8192) cbCount = 8192;
		if (fuOptions & ETO_PDY)	//win9x can't understand it and messes up other flags
		{
			fuOptions = fuOptions & ~ETO_PDY; 
			if (lpDx)				//we have to make array which contains dx values only
			{	
				if ( cbCount>128 )
					buffer = (int*)HeapAlloc(GetProcessHeap(),0,cbCount*sizeof(int));
				else
					buffer = (int*)alloca(cbCount*sizeof(int));
				
				if (!buffer) 
					return FALSE;
				MakeDxFromDxDy(lpDx,buffer,cbCount);
				lpDx = buffer;
			}
		}
	}

	GrabWin16Lock();
	PDCOBJ dcobj = GetDCObj( hdc );
	if ( dcobj && dcobj->mapmode == MM_TEXT && 
		( dcobj->ViewportExtX!=1 || dcobj->ViewportExtY!=1
		|| dcobj->WindowExtX!=1 || dcobj->WindowExtY!=1) )
	{
		savemapmode = dcobj->mapmode;
		dcobj->mapmode = MM_ANISOTROPIC;
	}
	ReleaseWin16Lock();
	result = ExtTextOutW(hdc,X,Y,fuOptions,lprc,lpString,cbCount,lpDx);
	if ( savemapmode )
	{
		GrabWin16Lock();
		dcobj = GetDCObj( hdc );
		dcobj->mapmode = savemapmode;
		ReleaseWin16Lock();
	}
	if ( buffer && cbCount>128 )
		HeapFree(GetProcessHeap(),0,buffer);
	return result;
}

/* MAKE_EXPORT PolyTextOutA_new=PolyTextOutA */
BOOL WINAPI PolyTextOutA_new( HDC hdc, const POLYTEXTA *pptxt, INT cStrings )
{
	if (!hdc || !cStrings || !pptxt)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}	
	for (; cStrings>0; cStrings--, pptxt++)
		if (!ExtTextOutA_new (hdc, pptxt->x, pptxt->y, pptxt->uiFlags, &pptxt->rcl, pptxt->lpstr, pptxt->n, pptxt->pdx ))
			return FALSE;
	return TRUE;
}

/* MAKE_EXPORT PolyTextOutW_new=PolyTextOutW */
BOOL WINAPI PolyTextOutW_new( HDC hdc, const POLYTEXTW *pptxt, INT cStrings )
{
	if (!hdc || !cStrings || !pptxt)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}
	for (; cStrings>0; cStrings--, pptxt++)
		if (!ExtTextOutW_new (hdc, pptxt->x, pptxt->y, pptxt->uiFlags, &pptxt->rcl, pptxt->lpstr, pptxt->n, pptxt->pdx ))
			return FALSE;
	return TRUE;
}
