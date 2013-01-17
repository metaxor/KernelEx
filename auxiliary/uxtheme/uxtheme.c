/*
 *  KernelEx
 *  Copyright (C) 2008-2009, Xeno86
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

#define UNIMPL_FUNC(name,params,val) \
	unsigned long __declspec(naked) __stdcall name() \
	{ \
		__asm mov eax, val \
		__asm ret 4*params \
	}

#define _E_NOTIMPL 0x80004001

BOOL WINAPI DllMain (HINSTANCE hinstDLL, DWORD fdwReason, BOOL bLoadStatic)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		if (!bLoadStatic)
		{
			OSVERSIONINFO osv;
			osv.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
			GetVersionEx(&osv);
			if (osv.dwMajorVersion < 5)
				return FALSE;
		}
		DisableThreadLibraryCalls(hinstDLL);
	}
	return TRUE;
}

UNIMPL_FUNC(CloseThemeData, 1, _E_NOTIMPL);
UNIMPL_FUNC(DrawThemeBackground, 6, _E_NOTIMPL);
UNIMPL_FUNC(DrawThemeBackgroundEx, 6, _E_NOTIMPL);
UNIMPL_FUNC(DrawThemeEdge, 8, _E_NOTIMPL);
UNIMPL_FUNC(DrawThemeIcon, 7, _E_NOTIMPL);
UNIMPL_FUNC(DrawThemeParentBackground, 3, _E_NOTIMPL);
UNIMPL_FUNC(DrawThemeText, 9, _E_NOTIMPL);
UNIMPL_FUNC(EnableThemeDialogTexture, 2, _E_NOTIMPL);
UNIMPL_FUNC(EnableTheming, 1, _E_NOTIMPL);
UNIMPL_FUNC(GetCurrentThemeName, 6, _E_NOTIMPL);
UNIMPL_FUNC(GetThemeAppProperties, 0, 0);
UNIMPL_FUNC(GetThemeBackgroundContentRect, 6, _E_NOTIMPL);
UNIMPL_FUNC(GetThemeBackgroundExtent, 6, _E_NOTIMPL);
UNIMPL_FUNC(GetThemeBackgroundRegion, 6, _E_NOTIMPL);
UNIMPL_FUNC(GetThemeBool, 5, _E_NOTIMPL);
UNIMPL_FUNC(GetThemeColor, 5, _E_NOTIMPL);
UNIMPL_FUNC(GetThemeDocumentationProperty, 4, _E_NOTIMPL);
UNIMPL_FUNC(GetThemeEnumValue, 5, _E_NOTIMPL);
UNIMPL_FUNC(GetThemeFilename, 6, _E_NOTIMPL);
UNIMPL_FUNC(GetThemeFont, 6, _E_NOTIMPL);
UNIMPL_FUNC(GetThemeInt, 5, _E_NOTIMPL);
UNIMPL_FUNC(GetThemeIntList, 5, _E_NOTIMPL);
UNIMPL_FUNC(GetThemeMargins, 7, _E_NOTIMPL);
UNIMPL_FUNC(GetThemeMetric, 6, _E_NOTIMPL);
UNIMPL_FUNC(GetThemePartSize, 7, _E_NOTIMPL);
UNIMPL_FUNC(GetThemePosition, 5, _E_NOTIMPL);
UNIMPL_FUNC(GetThemePropertyOrigin, 5, _E_NOTIMPL);
UNIMPL_FUNC(GetThemeRect, 5, _E_NOTIMPL);
UNIMPL_FUNC(GetThemeString, 6, _E_NOTIMPL);
UNIMPL_FUNC(GetThemeTextExtent, 9, _E_NOTIMPL);
UNIMPL_FUNC(GetThemeTextMetrics, 5, _E_NOTIMPL);
UNIMPL_FUNC(GetWindowTheme, 1, 0);
UNIMPL_FUNC(HitTestThemeBackground, 10, _E_NOTIMPL);
UNIMPL_FUNC(IsAppThemed, 0, FALSE);
UNIMPL_FUNC(IsThemeActive, 0, FALSE);
UNIMPL_FUNC(IsThemeBackgroundPartiallyTransparent, 3, FALSE);
UNIMPL_FUNC(IsThemeDialogTextureEnabled, 1, FALSE);
UNIMPL_FUNC(IsThemePartDefined, 3, FALSE);
UNIMPL_FUNC(OpenThemeData, 2, 0);
UNIMPL_FUNC(SetThemeAppProperties, 1, 0);
UNIMPL_FUNC(SetWindowTheme, 3, _E_NOTIMPL);
