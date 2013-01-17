/*
 *  KernelEx
 *  Copyright (C) 2010, Xeno86, Tihiy
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

typedef struct 
{
	FONTENUMPROCW procW;
	LPARAM lParam;
	HKEY hKey;
	ENUMLOGFONTEXDVW elfW;
	ENUMTEXTMETRICW etmW;
} EnumHelperW_t;

static const char regfontsig[] = "SOFTWARE\\KernelEx\\FontSignature";
static const char regttfcachesize[] = "ttfCacheSize";

static inline void GetFontSignature(HKEY hkey, LOGFONTA* plfA, FONTSIGNATURE* sig)
{
	DWORD sz = sizeof(FONTSIGNATURE);
	if ( RegQueryValueEx(hkey,plfA->lfFaceName,0,NULL,(LPBYTE)sig,&sz) != ERROR_SUCCESS )
	{
		DBGPRINTF(("Querying signature from font %s",plfA->lfFaceName));
		HDC hdc = CreateCompatibleDC(0);
		HFONT fnt = CreateFontIndirect(plfA);
		HGDIOBJ old = SelectObject(hdc,fnt);
		GetTextCharsetInfo(hdc,sig,0);
		SelectObject(hdc,old);
		DeleteObject(fnt);
		DeleteDC(hdc);
		sz = sizeof(FONTSIGNATURE);
		RegSetValueEx(hkey,plfA->lfFaceName,0,REG_BINARY,(LPBYTE)sig,sz);
	}
}

static inline void ENUMLOGFONTEXAtoW(ENUMLOGFONTEXA* fontA, ENUMLOGFONTEXW* fontW)
{
	memcpy(&fontW->elfLogFont, &fontA->elfLogFont, FIELD_OFFSET(LOGFONT,lfFaceName));

	MultiByteToWideChar(CP_ACP, 0, (LPCSTR) fontA->elfLogFont.lfFaceName, -1, 
			fontW->elfLogFont.lfFaceName, LF_FACESIZE);
	fontW->elfLogFont.lfFaceName[LF_FACESIZE - 1] = 0;
	MultiByteToWideChar(CP_ACP, 0, (LPCSTR) fontA->elfFullName, -1,
			fontW->elfFullName, LF_FULLFACESIZE);
	fontW->elfFullName[LF_FULLFACESIZE - 1] = 0;
	MultiByteToWideChar(CP_ACP, 0, (LPCSTR) fontA->elfStyle, -1,
			fontW->elfStyle, LF_FACESIZE);
	fontW->elfStyle[LF_FACESIZE - 1] = 0;
	MultiByteToWideChar(CP_ACP, 0, (LPCSTR) fontA->elfScript, -1,
			fontW->elfScript, LF_FACESIZE);
	fontW->elfScript[LF_FACESIZE - 1] = 0;	
}

static inline void NEWTEXTMETRICAtoW(NEWTEXTMETRICA* tmA, NEWTEXTMETRICW* tmW)
{
	memcpy(tmW,tmA,FIELD_OFFSET(NEWTEXTMETRICA,tmFirstChar));
	
	MultiByteToWideChar(CP_ACP, 0, (LPCSTR) &tmA->tmFirstChar, 1,
			&tmW->tmFirstChar, 1);
	MultiByteToWideChar(CP_ACP, 0, (LPCSTR) &tmA->tmLastChar, 1, 
			&tmW->tmLastChar, 1);
	MultiByteToWideChar(CP_ACP, 0, (LPCSTR) &tmA->tmDefaultChar, 1, 
			&tmW->tmDefaultChar, 1);
	MultiByteToWideChar(CP_ACP, 0, (LPCSTR) &tmA->tmBreakChar, 1, 
			&tmW->tmBreakChar, 1);

	memcpy(&tmW->tmItalic,&tmA->tmItalic,sizeof(NEWTEXTMETRICA) - FIELD_OFFSET(NEWTEXTMETRICA,tmItalic));
}

int WINAPI EnumFontWHelper( CONST LOGFONTA *lfA, CONST TEXTMETRICA *tmA, DWORD FontType, LPARAM lParam )
{
	ENUMLOGFONTEXA* lpelfA = (ENUMLOGFONTEXA*)lfA;
	NEWTEXTMETRICA* lpntmA = (NEWTEXTMETRICA*)tmA;
	EnumHelperW_t* params = (EnumHelperW_t*)lParam;

	ENUMLOGFONTEXAtoW(lpelfA,&params->elfW.elfEnumLogfontEx);
	NEWTEXTMETRICAtoW(lpntmA,&params->etmW.etmNewTextMetricEx.ntmTm);
	if ( FontType == TRUETYPE_FONTTYPE )
		GetFontSignature(params->hKey,&lpelfA->elfLogFont,&params->etmW.etmNewTextMetricEx.ntmFontSig);
	else
		memset(&params->etmW.etmNewTextMetricEx.ntmFontSig,0,sizeof(FONTSIGNATURE));
	return params->procW((LOGFONTW*)&params->elfW,(TEXTMETRICW*)&params->etmW,FontType,params->lParam);
}

/* MAKE_EXPORT EnumFontFamiliesExA_new=EnumFontFamiliesExA */
int WINAPI EnumFontFamiliesExA_new(HDC hdc, LPLOGFONTA pLogfontA, 
		FONTENUMPROCA pEnumFontFamExProc, LPARAM lParam, DWORD dwFlags)
{
	LOGFONTA logfont;
	if (!pLogfontA) 
	{
		memset(&logfont, 0, sizeof(logfont));
		logfont.lfCharSet = DEFAULT_CHARSET;
		pLogfontA = &logfont;
	}
	return EnumFontFamiliesExA(hdc, pLogfontA, pEnumFontFamExProc, lParam, dwFlags);
}

/* MAKE_EXPORT EnumFontFamiliesExW_new=EnumFontFamiliesExW */
int WINAPI EnumFontFamiliesExW_new(HDC hdc, LPLOGFONTW pLogfontW, 
		FONTENUMPROCW pEnumFontFamExProc, LPARAM lParam, DWORD dwFlags)
{
	LOGFONTA logfont;
	if (pLogfontW)
	{		
		memcpy(&logfont, pLogfontW, FIELD_OFFSET(LOGFONT,lfFaceName));
		WideCharToMultiByte(CP_ACP, 0, pLogfontW->lfFaceName, -1, logfont.lfFaceName, 
				LF_FACESIZE, NULL, NULL);
		logfont.lfFaceName[LF_FACESIZE - 1] = '\0';
	}
	else
	{
		memset(&logfont, 0, sizeof(logfont));
		logfont.lfCharSet = DEFAULT_CHARSET;
	}	
	EnumHelperW_t params = {0};	
	params.procW = pEnumFontFamExProc;
	params.lParam = lParam;
	RegCreateKey(HKEY_LOCAL_MACHINE,regfontsig,&params.hKey);
	int ret = EnumFontFamiliesExA(hdc,&logfont,EnumFontWHelper,(LPARAM)&params,dwFlags);
	RegCloseKey(params.hKey);
	return ret;
}

/* MAKE_EXPORT EnumFontFamiliesW_new=EnumFontFamiliesW */
int WINAPI EnumFontFamiliesW_new( HDC hdc, LPCWSTR lpszFamilyW,
		FONTENUMPROCW lpEnumFontFamProc, LPARAM lParam)
{
	LPSTR lpszFamilyA;
	STACK_WtoA(lpszFamilyW,lpszFamilyA);
	EnumHelperW_t params = {0};	
	params.procW = lpEnumFontFamProc;
	params.lParam = lParam;
	RegCreateKey(HKEY_LOCAL_MACHINE,regfontsig,&params.hKey);
	int ret = EnumFontFamiliesA(hdc,lpszFamilyA,EnumFontWHelper,(LPARAM)&params);
	RegCloseKey(params.hKey);
	return ret;
}

/* MAKE_EXPORT EnumFontsW_new=EnumFontsW */
int WINAPI EnumFontsW_new( HDC hdc, LPCWSTR lpFaceNameW,
		FONTENUMPROCW lpFontFunc, LPARAM lParam)
{
	LPSTR lpFaceNameA;
	STACK_WtoA(lpFaceNameW,lpFaceNameA);
	EnumHelperW_t params = {0};	
	params.procW = lpFontFunc;
	params.lParam = lParam;
	RegCreateKey(HKEY_LOCAL_MACHINE,regfontsig,&params.hKey);
	int ret = EnumFontsA(hdc,lpFaceNameA,EnumFontWHelper,(LPARAM)&params);
	RegCloseKey(params.hKey);
	return ret;
}
