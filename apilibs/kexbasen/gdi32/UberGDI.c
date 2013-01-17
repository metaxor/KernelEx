/*
 *  KernelEx
 *
 *  Copyright (C) 2008, 2010, Tihiy
 *  This file is part of KernelEx source code.
 *
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

#include <malloc.h>
#include <windows.h>
#include <usp10.h>
#include "ScriptCache.h"
#include "k32ord.h"
#include "../../kexbases/Gdi32/GdiObjects.h"

static DWORD g_GdiBase;
BOOL InitUberGDI(void)
{
	g_GdiBase = MapSL( LoadLibrary16("gdi") << 16 );
	return (BOOL)g_GdiBase;
}

//grab GDI object number to distinguish fonts
//don't do strict validation or locking since
//dc font can't move or vanish when selected
FONTUID GetHDCFontUID(HDC hdc)
{
	WORD wFont = (WORD)GetCurrentObject(hdc,OBJ_FONT);
	if (wFont < 0x80) return 0;
	DWORD* high = (DWORD*)(g_GdiBase + GDIHEAP32BASE + wFont);
	PGDIOBJ16 fntobj = (PGDIOBJ16)(g_GdiBase + *high);
	return fntobj->dwNumber;
}

/* MAKE_EXPORT GetGlyphIndicesW_new=GetGlyphIndicesW */
int WINAPI GetGlyphIndicesW_new(
	HDC hdc,       // handle to DC
	LPWSTR lpstr,  // string to convert
	int c,         // number of characters in string
	LPWORD pgi,    // array of glyph indices
	DWORD fl       // glyph options
)
{
	HRESULT result;
	if (!hdc || !pgi || (UINT)lpstr<0xFFFFu || c<=0) return GDI_ERROR;
	ScriptCache::instance.Lock();
	FONTUID hFont = GetHDCFontUID(hdc);
	SCRIPT_CACHE cache = ScriptCache::instance.GetCache(hFont);
	result = ScriptGetCMap(hdc,&cache,lpstr,c,0,pgi);
	if ( !( result == S_OK || result == S_FALSE ) ) 
	{
		ScriptCache::instance.Unlock();
		return GDI_ERROR;
	}
	if ( fl && result == S_FALSE)
	{		
		WORD* checkglyph = pgi;
		int i;
		SCRIPT_FONTPROPERTIES fpr;
		fpr.cBytes = sizeof(SCRIPT_FONTPROPERTIES);
		if (FAILED(ScriptGetFontProperties(hdc,&cache,&fpr)))
		{
			ScriptCache::instance.Unlock();
			return GDI_ERROR;
		}
		for (i = 0; i < c; i++)
		{
			if (*checkglyph == fpr.wgDefault) *checkglyph = 0xFFFF;
			checkglyph++;
		}
	}
	ScriptCache::instance.SetCache(hFont,cache); \
	ScriptCache::instance.Unlock();
	return c;
}

static int WINAPI GdiGetCodePage( HDC hdc )
{
	int charset = GetTextCharset(hdc);

	switch(charset) {
	case DEFAULT_CHARSET:
		return GetACP();
	case OEM_CHARSET:
		return GetOEMCP();
	default:
		{
			CHARSETINFO csi;
			if(TranslateCharsetInfo((DWORD*)charset, &csi, TCI_SRCCHARSET))
				return csi.ciACP;
			else
				return CP_ACP;
		}
	}
}

/* MAKE_EXPORT GetGlyphIndicesA_new=GetGlyphIndicesA */
int WINAPI GetGlyphIndicesA_new(
	HDC hdc,       // handle to DC
	LPCSTR lpstr,  // string to convert
	int c,         // number of characters in string
	LPWORD pgi,    // array of glyph indices
	DWORD fl       // glyph options
)
{
	LPWSTR lpstrwide;
	if (!hdc || !pgi || (UINT)lpstr<0xFFFF || c<=0) return GDI_ERROR;	
	lpstrwide = (LPWSTR)alloca(c*sizeof(WCHAR));
	c = MultiByteToWideChar(GdiGetCodePage(hdc),0,lpstr,c,lpstrwide,c);
	if (!c)
		return GDI_ERROR;

	return GetGlyphIndicesW_new(hdc,lpstrwide,c,pgi,fl);
}

/* MAKE_EXPORT GetTextExtentExPointI_new=GetTextExtentExPointI */
BOOL WINAPI GetTextExtentExPointI_new(
	HDC hdc,         // handle to DC
	LPWORD pgiIn,    // array of glyph indices
	int cgi,         // number of glyphs in array
	int nMaxExtent,  // maximum width of formatted string
	LPINT lpnFit,    // maximum number of characters
	LPINT alpDx,     // array of partial string widths
	LPSIZE lpSize    // string dimensions
)
{
	int i;
	int sum = 0;	
	int glyphwidth;	
	int charextra = GetTextCharacterExtra(hdc);
	ABC abc;
	BOOL unfit = FALSE;
	
	if ( !hdc || !pgiIn || cgi<=0 || !lpSize)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}
	
	ScriptCache::instance.Lock();
	FONTUID hFont = GetHDCFontUID(hdc);
	SCRIPT_CACHE cache = ScriptCache::instance.GetCache(hFont);

	if (lpnFit) *lpnFit = cgi;
	for (i = 0; i < cgi; i++)
	{
		if ( ScriptGetGlyphABCWidth(hdc,&cache,*pgiIn,&abc) != S_OK ) break;
		glyphwidth = abc.abcA + abc.abcB + abc.abcC + charextra;
		sum += glyphwidth;
		if ( !unfit )
		{
			unfit = ( sum > nMaxExtent );
			if (unfit)
			{
				if ( lpnFit ) *lpnFit = i;
			}
			else
			{
				if ( alpDx ) *alpDx++ = sum;
			}			
		}
		pgiIn++;
	}
	lpSize->cx = sum;	

	ScriptCacheGetHeight(hdc,&cache,&lpSize->cy);
	ScriptCache::instance.SetCache(hFont,cache);
	ScriptCache::instance.Unlock();

	return TRUE;
}

/* MAKE_EXPORT GetTextExtentPointI_new=GetTextExtentPointI */
BOOL WINAPI GetTextExtentPointI_new(
	HDC hdc,       // handle to DC
	LPWORD pgiIn,  // glyph indices
	int cgi,       // number of indices in array
	LPSIZE lpSize  // string size  
)
{
	return GetTextExtentExPointI_new(hdc,pgiIn,cgi,0,0,0,lpSize);
}

/* MAKE_EXPORT GetCharWidthI_new=GetCharWidthI */
BOOL WINAPI GetCharWidthI_new(
	HDC hdc,       // handle to DC
	UINT giFirst,  // first glyph index in range
	UINT cgi,      // number of glyph indices in range
	WORD* pgi,     // array of glyph indices
	INT* lpBuffer  // buffer for widths
)
{
	ABC abc;
	WORD glyph;
	
	if ( !hdc || !lpBuffer || cgi<=0)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	ScriptCache::instance.Lock();
	FONTUID hFont = GetHDCFontUID(hdc);
	SCRIPT_CACHE cache = ScriptCache::instance.GetCache(hFont);

	if ( !pgi ) //cgi glyphs starting giFirst
	{
		for ( glyph = giFirst; glyph < giFirst+cgi; glyph++)
		{
			ScriptGetGlyphABCWidth(hdc,&cache,glyph,&abc);
			*lpBuffer = abc.abcA + abc.abcB + abc.abcC;
			lpBuffer++;
		}
	}
	else
	{
		for ( glyph = 0; glyph < cgi; glyph++)
		{
			ScriptGetGlyphABCWidth(hdc,&cache,*pgi,&abc);
			*lpBuffer = abc.abcA + abc.abcB + abc.abcC;
			pgi++;
			lpBuffer++;
		}		
	}

	ScriptCache::instance.SetCache(hFont,cache); \
	ScriptCache::instance.Unlock();
	return TRUE;
}

//NOTE: usp10 is probing for that function, don't forget to exclude it

/* MAKE_EXPORT GetCharABCWidthsI_new=GetCharABCWidthsI */
BOOL WINAPI GetCharABCWidthsI_new(
	HDC hdc,       // handle to DC
	UINT giFirst,  // first glyph index in range
	UINT cgi,      // count of glyph indices in range
	LPWORD pgi,    // array of glyph indices
	LPABC lpabc    // array of character widths
)
{
	WORD glyph;	
	if ( !hdc || !lpabc || cgi<=0)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	ScriptCache::instance.Lock();
	FONTUID hFont = GetHDCFontUID(hdc);
	SCRIPT_CACHE cache = ScriptCache::instance.GetCache(hFont);

	if ( !pgi ) //cgi glyphs starting giFirst
	{
		for ( glyph = giFirst; glyph < giFirst+cgi; glyph++)
		{
			ScriptGetGlyphABCWidth(hdc,&cache,glyph,lpabc);
			lpabc++;
		}
	}
	else
	{
		for ( glyph = 0; glyph < cgi; glyph++)
		{
			ScriptGetGlyphABCWidth(hdc,&cache,*pgi,lpabc);
			pgi++;
			lpabc++;
		}		
	}
	ScriptCache::instance.SetCache(hFont,cache); \
	ScriptCache::instance.Unlock();
	return TRUE;
}

/* MAKE_EXPORT GetCharABCWidthsW_new=GetCharABCWidthsW */
BOOL WINAPI GetCharABCWidthsW_new(
	HDC hdc,         // handle to DC
	UINT uFirstChar, // first character in range
	UINT uLastChar,  // last character in range
	LPABC lpabc      // array of character widths
)
{
	if ( !hdc || !lpabc || uFirstChar>uLastChar )
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}
	UINT c = uLastChar-uFirstChar+1;
	LPWORD glyphs = (LPWORD)alloca(c*sizeof(WORD)); 
	LPWSTR chrW = (LPWSTR)alloca(c*sizeof(WCHAR));
	LPWSTR strW = chrW;
	for (int i = uFirstChar; i<=uLastChar; i++)
	{
		*chrW=(WCHAR)i;
		chrW++;
	}
	if ( GetGlyphIndicesW_new(hdc,strW,c,glyphs,0) == GDI_ERROR )
		return FALSE;
	return GetCharABCWidthsI_new(hdc,0,c,glyphs,lpabc);
}

/* MAKE_EXPORT GetGlyphOutlineW_new=GetGlyphOutlineW */
DWORD WINAPI GetGlyphOutlineW_new(
	HDC hdc,             // handle to DC
	UINT uChar,          // character to query
	UINT uFormat,        // data format
	LPGLYPHMETRICS lpgm, // glyph metrics
	DWORD cbBuffer,      // size of data buffer
	LPVOID lpvBuffer,    // data buffer
	CONST MAT2 *lpmat2   // transformation matrix
)
{
	UINT glyph = 0;
	//GetGlyphOutlineW must receive same fix GetGlyphOutlineA received since apilibs interlinking changed
	MAT2 matr;
	if ( lpmat2 ) 
	{
		memcpy( &matr, lpmat2, sizeof(MAT2) );
		lpmat2 = &matr;
	}


	if ( uFormat & GGO_GLYPH_INDEX )
		return GetGlyphOutlineA( hdc, uChar, uFormat, lpgm, cbBuffer, lpvBuffer, lpmat2 );
	else
	{
		DWORD ret = GetGlyphIndicesW_new( hdc, (LPWSTR)&uChar, 1, (LPWORD)&glyph, 0 );
		if (ret != GDI_ERROR)
			ret = GetGlyphOutlineA( hdc, glyph, uFormat | GGO_GLYPH_INDEX, lpgm, cbBuffer, lpvBuffer, lpmat2 );
		
		//someone assumed MS Shell Dlg is TrueType?
		if (ret == GDI_ERROR && uFormat == GGO_METRICS && lpmat2 && lpgm)
		{
			char fontface[100];
			GetTextFaceA(hdc,100,fontface);
			if (strcmp(fontface,"MS Sans Serif") == 0)
			{
				//well let's fake it then
				SIZE sz;
				if (GetTextExtentPointW(hdc,(LPWSTR)&uChar,1,&sz))
				{
					lpgm->gmBlackBoxX = sz.cx;
					lpgm->gmBlackBoxY = sz.cy;
					lpgm->gmptGlyphOrigin.x = 0;
					lpgm->gmptGlyphOrigin.y = sz.cy;
					lpgm->gmCellIncX = (short) sz.cx;
					lpgm->gmCellIncY = 0;
					ret = 1;
				}
			}
		}
		
		return ret;
	}
}
