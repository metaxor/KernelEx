/*
 * Win32 5.1 Theme metrics
 *
 * Copyright (C) 2003 Kevin Koltzau
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

#define UNICODE
#define _WIN32_WINNT 0x0501
#include <windows.h>
#include <uxtheme.h>
#include <tmschema.h>

#ifndef TMT_FIRSTCOLOR
#define TMT_FIRSTCOLOR  TMT_SCROLLBAR
#endif

#pragma warning(disable:4273)

/***********************************************************************
 *      GetThemeSysBool                                     (UXTHEME.@)
 * MAKE_EXPORT GetThemeSysBool=GetThemeSysBool
 */
BOOL WINAPI GetThemeSysBool(HTHEME hTheme, int iBoolID)
{
	SetLastError(0);
	if(iBoolID == TMT_FLATMENUS) {
		return FALSE;
	}
	else {
		SetLastError(STG_E_INVALIDPARAMETER);
	}
	return FALSE;
}

/***********************************************************************
 *      GetThemeSysColor                                    (UXTHEME.@)
 * MAKE_EXPORT GetThemeSysColor=GetThemeSysColor
 */
COLORREF WINAPI GetThemeSysColor(HTHEME hTheme, int iColorID)
{
	SetLastError(0);
	return GetSysColor(iColorID - TMT_FIRSTCOLOR);
}

/***********************************************************************
 *      GetThemeSysColorBrush                               (UXTHEME.@)
 * MAKE_EXPORT GetThemeSysColorBrush=GetThemeSysColorBrush
 */
HBRUSH WINAPI GetThemeSysColorBrush(HTHEME hTheme, int iColorID)
{
	return CreateSolidBrush(GetThemeSysColor(hTheme, iColorID));
}

/***********************************************************************
 *      GetThemeSysFont                                     (UXTHEME.@)
 * MAKE_EXPORT GetThemeSysFont=GetThemeSysFont
 */
HRESULT WINAPI GetThemeSysFont(HTHEME hTheme, int iFontID, LOGFONTW *plf)
{
	HRESULT hr = S_OK;

	if(iFontID == TMT_ICONTITLEFONT) {
		if(!SystemParametersInfoW(SPI_GETICONTITLELOGFONT, sizeof(LOGFONTW), &plf, 0))
			return HRESULT_FROM_WIN32(GetLastError());
	}
	else {
		NONCLIENTMETRICSW ncm;
		LOGFONTW *font = NULL;
		ncm.cbSize = sizeof(NONCLIENTMETRICSW);
		if(!SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICSW), &ncm, 0))
			return HRESULT_FROM_WIN32(GetLastError());
		switch(iFontID) {
			case TMT_CAPTIONFONT: font = &ncm.lfCaptionFont; break;
			case TMT_SMALLCAPTIONFONT: font = &ncm.lfSmCaptionFont; break;
			case TMT_MENUFONT: font = &ncm.lfMenuFont; break;
			case TMT_STATUSFONT: font = &ncm.lfStatusFont; break;
			case TMT_MSGBOXFONT: font = &ncm.lfMessageFont; break;
		}
		if(font) *plf = *font;
		else     hr = STG_E_INVALIDPARAMETER;
	}
	return hr;
}

/***********************************************************************
 *      GetThemeSysInt                                      (UXTHEME.@)
 * MAKE_EXPORT GetThemeSysInt=GetThemeSysInt
 */
HRESULT WINAPI GetThemeSysInt(HTHEME hTheme, int iIntID, int *piValue)
{
	return E_HANDLE;
}

/***********************************************************************
 *      GetThemeSysSize                                     (UXTHEME.@)
 * MAKE_EXPORT GetThemeSysSize=GetThemeSysSize
 */
int WINAPI GetThemeSysSize(HTHEME hTheme, int iSizeID)
{
	return GetSystemMetrics(iSizeID);
}

/***********************************************************************
 *      GetThemeSysString                                   (UXTHEME.@)
 * MAKE_EXPORT GetThemeSysString=GetThemeSysString
 */
HRESULT WINAPI GetThemeSysString(HTHEME hTheme, int iStringID,
                                 LPWSTR pszStringBuff, int cchMaxStringChars)
{
	return E_HANDLE;
}
