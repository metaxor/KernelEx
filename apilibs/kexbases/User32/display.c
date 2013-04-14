/*
 *  KernelEx
 *  Copyright (C) 2013, Ley0k
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

#include "desktop.h"

/* MAKE_EXPORT ChangeDisplaySettingsExA_fix=ChangeDisplaySettingsExA */
LONG WINAPI ChangeDisplaySettingsExA_fix(
	LPCSTR lpszDeviceName,
	LPDEVMODE lpDevMode,
	HWND hwnd,
	DWORD dwflags,
	LPVOID lParam
)
{
	DEVMODE DevMode;
	PDESKTOP Desktop = NULL;
	LONG result;

	if(gpdeskInputDesktop == NULL)
		return ChangeDisplaySettingsEx(lpszDeviceName, lpDevMode, hwnd, dwflags, lParam);

	if(lpDevMode != NULL && IsBadReadPtr(lpDevMode, sizeof(DEVMODE)))
		return DISP_CHANGE_FAILED;

	if(lpDevMode == NULL)
	{
		memset(&DevMode, 0, sizeof(DEVMODE));
		DevMode.dmSize = sizeof(DEVMODE);
		EnumDisplaySettings(NULL, ENUM_REGISTRY_SETTINGS, &DevMode);
	}
	else
		memcpy(&DevMode, lpDevMode, sizeof(DEVMODE));

	__try
	{
		Desktop = get_tdb()->Win32Thread->rpdesk;

		if(Desktop == NULL)
			Desktop = gpdeskInputDesktop;
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		Desktop = gpdeskInputDesktop;
	}

	result = ChangeDisplaySettingsExA(lpszDeviceName, lpDevMode, hwnd, dwflags, lParam);

	if(result != DISP_CHANGE_SUCCESSFUL)
		return result;

	GrabWin16Lock();

	/* Change the thread's desktop pdev flags */
	if(DevMode.dmFields & DM_BITSPERPEL)
		Desktop->pdev->dmBitsPerPel = DevMode.dmBitsPerPel;

	if(DevMode.dmFields & DM_PELSWIDTH)
		Desktop->pdev->dmPelsWidth = DevMode.dmPelsWidth;

	if(DevMode.dmFields & DM_PELSHEIGHT)
		Desktop->pdev->dmPelsHeight = DevMode.dmPelsHeight;

	if(DevMode.dmFields & DM_DISPLAYFLAGS)
		Desktop->pdev->dmDisplayFlags = DevMode.dmDisplayFlags;

	if(DevMode.dmFields & DM_DISPLAYFREQUENCY)
		Desktop->pdev->dmDisplayFrequency = DevMode.dmDisplayFrequency;

	if(DevMode.dmFields & DM_POSITION)
		Desktop->pdev->dmPosition = DevMode.dmPosition;

	ReleaseWin16Lock();

	return result;
}

/* MAKE_EXPORT ChangeDisplaySettingsA_fix=ChangeDisplaySettingsA */
LONG WINAPI ChangeDisplaySettingsA_fix(LPDEVMODE lpDevMode,
	DWORD dwflags
)
{
	return ChangeDisplaySettingsExA_fix(NULL, lpDevMode, NULL, dwflags, NULL);
}

/* MAKE_EXPORT EnumDisplaySettingsA_fix=EnumDisplaySettingsA */
BOOL WINAPI EnumDisplaySettingsA_fix(LPCSTR lpszDeviceName,
	DWORD iModeNum,
	LPDEVMODE lpDevMode)
{
	return EnumDisplaySettingsExA_fix(lpszDeviceName, iModeNum, lpDevMode, 0);
}

/* MAKE_EXPORT EnumDisplaySettingsExA_fix=EnumDisplaySettingsExA */
BOOL WINAPI EnumDisplaySettingsExA_fix(LPCSTR lpszDeviceName,
	DWORD iModeNum,
	LPDEVMODE lpDevMode,
	DWORD dwFlags)
{
	LPDEVMODE pcurdev = NULL;
	PTHREADINFO pti = NULL;

	if(IsBadWritePtr(lpDevMode, sizeof(DEVMODE)))
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	if(iModeNum == ENUM_REGISTRY_SETTINGS || lpszDeviceName != NULL)
		return EnumDisplaySettingsEx(lpszDeviceName, iModeNum, lpDevMode, dwFlags);

	if(pti == NULL || pti->rpdesk == NULL)
		return FALSE;

	if(iModeNum == ENUM_CURRENT_SETTINGS)
		memcpy(lpDevMode, pti->rpdesk->pdev, sizeof(*lpDevMode));

	return TRUE;
}
