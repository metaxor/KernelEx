/*
 *  KernelEx
 *  Copyright (C) 2008, Xeno86
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
#include "common.h"
#include "_user32_apilist.h"
#include <commctrl.h>

/* MAKE_EXPORT CreateDesktopExW_new=CreateDesktopExW */
HDESK WINAPI CreateDesktopExW_new(LPWSTR lpszDesktop, LPWSTR lpszDevice, LPDEVMODEW pDevmode, DWORD dwFlags, ACCESS_MASK dwDesiredAccess, LPSECURITY_ATTRIBUTES lpsa, ULONG ulHeapSize, PVOID pvoid)
{
	return CreateDesktopW_new(lpszDesktop, lpszDevice, pDevmode, dwFlags, dwDesiredAccess, lpsa);
}

/* MAKE_EXPORT CreateDesktopW_new=CreateDesktopW */
HDESK WINAPI CreateDesktopW_new(LPWSTR lpszDesktopW, LPWSTR lpszDeviceW, LPDEVMODEW pDevmode, DWORD dwFlags, ACCESS_MASK dwDesiredAccess, LPSECURITY_ATTRIBUTES lpsa)
{
	WCHAR *dmDeviceNameW;
	WCHAR *dmFormNameW;
	DEVMODEA Devmode;

	ALLOC_WtoA(lpszDesktop);
	ALLOC_WtoA(lpszDevice);

	if(!IsBadReadPtr(pDevmode, sizeof(DEVMODEW)))
	{
		memmove(&Devmode, pDevmode, sizeof(*pDevmode));

		memset(Devmode.dmDeviceName, 0, sizeof(Devmode.dmDeviceName));
		memset(Devmode.dmFormName, 0, sizeof(Devmode.dmFormName));

		dmDeviceNameW = pDevmode->dmDeviceName;
		dmFormNameW = pDevmode->dmFormName;

		ALLOC_WtoA(dmDeviceName);
		ALLOC_WtoA(dmFormName);

		strcpy((char*)Devmode.dmDeviceName, dmDeviceNameA);
		strcpy((char*)Devmode.dmFormName, dmFormNameA);
	}

	return CreateDesktopA_new(lpszDesktopA, lpszDeviceA, &Devmode, dwFlags, dwDesiredAccess, lpsa);
}

/* MAKE_EXPORT CreateWindowStationW_new=CreateWindowStationW */
HWINSTA WINAPI CreateWindowStationW_new(LPWSTR lpwinstaW, DWORD dwFlags, ACCESS_MASK dwDesiredAccess, LPSECURITY_ATTRIBUTES lpsa)
{
	ALLOC_WtoA(lpwinsta);

	return CreateWindowStationA_new(lpwinstaA, dwFlags, dwDesiredAccess, lpsa);
}

/* MAKE_EXPORT DrawCaptionTempW_new=DrawCaptionTempW */
BOOL WINAPI DrawCaptionTempW_new(HWND hwnd, HDC hdc, const RECT *rect, HFONT hFont, HICON hIcon, LPCWSTR strW, UINT uFlags)
{
	if (!(uFlags & DC_TEXT) || !strW)
		return DrawCaptionTempA_pfn(hwnd, hdc, rect, hFont, hIcon, (LPCSTR)strW, uFlags);
	else
	{
		ALLOC_WtoA(str);
		return DrawCaptionTempA_pfn(hwnd, hdc, rect, hFont, hIcon, strA, uFlags);
	}
}

/* MAKE_EXPORT EnumDesktopsW_new=EnumDesktopsW */
BOOL WINAPI EnumDesktopsW_new(HWINSTA hwinsta, DESKTOPENUMPROCW lpEnumFunc, LPARAM lParam)
{
	SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
	return FALSE;
}

/* MAKE_EXPORT EnumWindowStationsW_new=EnumWindowStationsW */
BOOL WINAPI EnumWindowStationsW_new(WINSTAENUMPROCW lpEnumFunc, LPARAM lParam)
{
	SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
	return FALSE;
}

/* MAKE_EXPORT GetUserObjectInformationW_new=GetUserObjectInformationW */
BOOL WINAPI GetUserObjectInformationW_new(HANDLE hObj, int nIndex, PVOID pvInfo, DWORD nLength, LPDWORD lpnLengthNeeded)
{
	PVOID pvInfoA = NULL;
	PVOID pvInfoW = NULL;
	BOOL gui = FALSE;
	DWORD nSize = 0;

	pvInfoA = malloc(nLength);

	if(pvInfoA == NULL)
		return FALSE;

	gui = GetUserObjectInformationA_new(hObj, nIndex, pvInfoA, nLength, &nSize);

	if(!gui)
		return FALSE;

	if(nIndex == UOI_NAME || nIndex == UOI_TYPE)
		AtoW(pvInfo, nLength);
	else
		memcpy(pvInfoA, pvInfo, nSize);

	if(!IsBadReadPtr(lpnLengthNeeded, sizeof(DWORD)))
		*lpnLengthNeeded = nSize;

	return TRUE;
}

/* MAKE_EXPORT LoadStringW_new=LoadStringW */
int WINAPI LoadStringW_new(HINSTANCE hInstance, UINT uID, LPWSTR lpBuffer, int nBufferMax)
{
	int len;
	LPWSTR wstr;

	len = 0;
	if (lpBuffer)
	{
		// find stringtable resource
		HRSRC res = FindResourceA(hInstance, MAKEINTRESOURCE((uID >> 4) + 1), RT_STRING);
		if (res)
		{
			// load stringtable resource
			HGLOBAL data = LoadResource(hInstance, res);
			if (data)
			{
				// find string in stringtable
				uID &= 0xf;
				do
				{
					len = *(WORD*)data;
					wstr = (LPWSTR)data + 1;
					data = (HGLOBAL)(wstr + len);
				}
				while (uID--);
				// if nBufferMax is zero, then lpBuffer receives pointer to the resource itself
				if (!nBufferMax)
				{
					*(LPWSTR*)lpBuffer = wstr;
					return len;
				}
				if (len > nBufferMax - 1)
				{
					len = nBufferMax - 1;
				}
				memcpy(lpBuffer, wstr, len * sizeof(WCHAR));
			}
		}
		lpBuffer[len] = 0;
	}
	return len;
}

/* MAKE_EXPORT OpenDesktopW_new=OpenDesktopW */
HDESK WINAPI OpenDesktopW_new(LPWSTR lpszDesktopW, DWORD dwFlags, BOOL fInherit, ACCESS_MASK dwDesiredAccess)
{
	ALLOC_WtoA(lpszDesktop);

	return OpenDesktopA_new(lpszDesktopA, dwFlags, fInherit, dwDesiredAccess);
}

/* MAKE_EXPORT OpenWindowStationW_new=OpenWindowStationW */
HWINSTA WINAPI OpenWindowStationW_new(LPWSTR lpszWinStaW, BOOL fInherit, ACCESS_MASK dwDesiredAccess)
{
	ALLOC_WtoA(lpszWinSta);

	return OpenWindowStationA_new(lpszWinStaA, fInherit, dwDesiredAccess);
}

/* MAKE_EXPORT RealGetWindowClassW_new=RealGetWindowClassW */
UINT WINAPI RealGetWindowClassW_new(HWND  hwnd, LPWSTR pszTypeW, UINT cchType)
{
	UINT ret;
	ALLOC_A(pszType, cchType * acp_mcs);
	ret = RealGetWindowClass(hwnd, pszTypeA, cchType * acp_mcs);
	if (ret)
		ret = ABUFtoW(pszType, ret, cchType);
	return ret;
}

/* MAKE_EXPORT RegisterWindowMessageW_new=RegisterClipboardFormatW */
/* MAKE_EXPORT RegisterWindowMessageW_new=RegisterWindowMessageW */
UINT WINAPI RegisterWindowMessageW_new(LPCWSTR lpStringW)
{
	ALLOC_WtoA(lpString);

	return RegisterWindowMessageA_nothunk(lpStringA);
}
