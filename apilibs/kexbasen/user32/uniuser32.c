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

#include "_user32_apilist.h"

FORWARD_TO_UNICOWS(EnumDisplayDevicesW);
FORWARD_TO_UNICOWS(EnumDisplaySettingsExW);
FORWARD_TO_UNICOWS(EnumDisplaySettingsW);
FORWARD_TO_UNICOWS(EnumPropsExW);
FORWARD_TO_UNICOWS(EnumPropsW);

/* MAKE_EXPORT BroadcastSystemMessageW_new=BroadcastSystemMessageW */
long WINAPI BroadcastSystemMessageW_new(DWORD dwFlags, LPDWORD lpdwRecipients, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
	LPSTR lpwParam = NULL;
	LPSTR lplParam = NULL;

	if(!IsBadUnicodeStringPtr((LPCWSTR)wParam, -1))
		STACK_WtoA(wParam, lpwParam);

	if(!IsBadUnicodeStringPtr((LPCWSTR)lParam, -1))
		STACK_WtoA(lParam, lplParam);

	return BroadcastSystemMessageA(dwFlags,
								lpdwRecipients,
								uiMessage,
								lpwParam != NULL ? (WPARAM)lpwParam : wParam,
								lplParam != NULL ? (LPARAM)lplParam : lParam);
}

/* MAKE_EXPORT ChangeDisplaySettingsExW_new=ChangeDisplaySettingsExW */
LONG WINAPI ChangeDisplaySettingsExW_new(LPCWSTR lpszDeviceName, LPDEVMODEW lpDevMode, HWND hwnd, DWORD dwflags, LPVOID lParam)
{
	DEVMODEA DevModeA;
	LPCSTR lpszDeviceNameA = NULL;
	LPSTR lpFormName = NULL;
	LPSTR lpDeviceName = NULL;

	if(lpszDeviceName != NULL)
		STACK_WtoA(lpszDeviceName, lpszDeviceNameA);

	memcpy(&DevModeA, lpDevMode, sizeof(DEVMODEA));

	if(!IsBadUnicodeStringPtr(lpDevMode->dmDeviceName, -1))
		STACK_WtoA(lpDevMode->dmDeviceName, lpDeviceName);

	if(!IsBadUnicodeStringPtr(lpDevMode->dmFormName, -1))
		STACK_WtoA(lpDevMode->dmFormName, lpFormName);

	if(lpDeviceName != NULL)
		strcpy((char*)DevModeA.dmDeviceName, lpDeviceName);

	if(lpFormName != NULL)
		strcpy((char*)DevModeA.dmFormName, lpFormName);

	return ChangeDisplaySettingsExA(lpszDeviceNameA, &DevModeA, hwnd, dwflags, lParam);
}

/* MAKE_EXPORT ChangeDisplaySettingsExW_new=ChangeDisplaySettingsExW */
LONG WINAPI ChangeDisplaySettingsW_new(LPDEVMODEW lpDevMode, DWORD dwflags)
{
	return ChangeDisplaySettingsExW_new(NULL, lpDevMode, NULL, dwflags, NULL);
}

/* MAKE_EXPORT DdeCreateStringHandleW_new=DdeCreateStringHandleW */
HSZ WINAPI DdeCreateStringHandleW_new(DWORD idInst, LPWSTR pszW, int iCodePage)
{
	ALLOC_WtoA(psz);

	return DdeCreateStringHandleA(idInst, pszA, iCodePage);
}

/* MAKE_EXPORT DdeInitializeW_new=DdeInitializeW */
UINT WINAPI DdeInitializeW_new(LPDWORD pidInst, PFNCALLBACK pfnCallback, DWORD afCmd, DWORD ulRes)
{
	return DdeInitializeA(pidInst, pfnCallback, afCmd, ulRes);
}

/* MAKE_EXPORT DdeQueryStringW_new=DdeQueryStringW */
DWORD WINAPI DdeQueryStringW_new(DWORD idInst, HSZ hsz, LPWSTR pszW, DWORD cchMax, int iCodePage)
{
	ALLOC_WtoA(psz);

	return DdeQueryStringA(idInst, hsz, pszA, cchMax, iCodePage);
}

/* MAKE_EXPORT DlgDirListComboBoxW_new=DlgDirListComboBoxW */
int WINAPI DlgDirListComboBoxW_new(HWND hDlg, LPWSTR lpPathSpecW, int nIDComboBox, int nIDStaticPath, UINT uFiletype)
{
	int result;

	ALLOC_WtoA(lpPathSpec);

	result = DlgDirListComboBoxA(hDlg, lpPathSpecA, nIDComboBox, nIDStaticPath, uFiletype);

	if(!result)
		return 0;

	STACK_AtoW(lpPathSpecA, lpPathSpecW);

	return result;
}

/* MAKE_EXPORT DlgDirListW_new=DlgDirListW */
int WINAPI DlgDirListW_new(HWND hDlg, LPWSTR lpPathSpecW, int nIDListBox, int nIDStaticPath, UINT uFileType)
{
	int result;

	ALLOC_WtoA(lpPathSpec);

	result = DlgDirListA(hDlg, lpPathSpecA, nIDListBox, nIDStaticPath, uFileType);

	if(!result)
		return 0;

	STACK_AtoW(lpPathSpecA, lpPathSpecW);

	return result;
}

/* MAKE_EXPORT DlgDirSelectComboBoxExW_new=DlgDirSelectComboBoxExW */
BOOL WINAPI DlgDirSelectComboBoxExW_new(HWND hDlg, LPWSTR lpString, int nCount, int nIDComboBox)
{
	BOOL result;
	LPSTR lpStringA = (LPSTR)malloc(nCount);

	if(lpStringA == NULL)
		return FALSE;

	result = DlgDirSelectComboBoxExA(hDlg, lpStringA, nCount, nIDComboBox);

	if(!result)
	{
		free(lpStringA);
		return FALSE;
	}

	STACK_AtoW(lpStringA, lpString);

	free(lpStringA);

	return TRUE;
}

/* MAKE_EXPORT DlgDirSelectExW_new=DlgDirSelectExW */
BOOL WINAPI DlgDirSelectExW_new(HWND hDlg, LPWSTR lpString, int nCount, int nIDListBox)
{
	BOOL result;
	LPSTR lpStringA = (LPSTR)malloc(nCount);

	if(lpStringA == NULL)
		return FALSE;

	result = DlgDirSelectExA(hDlg, lpStringA, nCount, nIDListBox);

	if(!result)
	{
		free(lpStringA);
		return FALSE;
	}

	STACK_AtoW(lpStringA, lpString);

	free(lpStringA);

	return TRUE;
}

/* MAKE_EXPORT DrawStateW_new=DrawStateW */
BOOL WINAPI DrawStateW_new(HDC hdc, HBRUSH hbr, DRAWSTATEPROC lpOutputFunc, LPARAM lData, WPARAM wData, int x, int y, int cx, int cy, UINT fuFlags)
{
	return DrawStateA(hdc, hbr, lpOutputFunc, lData, wData, x, y, cx, cy, fuFlags);
}

/* MAKE_EXPORT DrawTextExW_new=DrawTextExW */
int WINAPI DrawTextExW_new(HDC hdc, LPWSTR lpchTextW, int cchText, LPRECT lprc, UINT dwDTFormat, LPDRAWTEXTPARAMS lpDTParams)
{
	int result;

	ALLOC_WtoA(lpchText);

	result = DrawTextExA(hdc, lpchTextA, cchText, lprc, dwDTFormat, lpDTParams);

	if(!result)
		return 0;

	STACK_AtoW(lpchTextA, lpchTextW);

	return result;
}

/* MAKE_EXPORT DrawTextW_new=DrawTextW */
int WINAPI DrawTextW_new(HDC hdc, LPWSTR lpchText, int cchText, LPRECT lprc, UINT dwDTFormat)
{
	return DrawTextExW_new(hdc, lpchText, cchText, lprc, dwDTFormat, NULL);
}

/* MAKE_EXPORT FindWindowExW_new=FindWindowExW */
HWND WINAPI FindWindowExW_new(HWND hwndParent, HWND hwndChildAfter, LPCWSTR lpszClassW, LPCWSTR lpszWindowW)
{
	ALLOC_WtoA(lpszClass);
	ALLOC_WtoA(lpszWindow);

	return FindWindowExA(hwndParent, hwndChildAfter, lpszClassA, lpszWindowA);
}

/* MAKE_EXPORT FindWindowW_new=FindWindowW */
HWND WINAPI FindWindowW_new(LPCWSTR lpszClass, LPCWSTR lpszWindow)
{
	return FindWindowExW_new(NULL, NULL, lpszClass, lpszWindow);
}

/* KEXBASEN can't find USER32!GetAltTabInfoA */
#if 0
BOOL WINAPI GetAltTabInfoW_new(HWND hwnd, int iItem, PALTTABINFO pati, LPWSTR pszItemText, UINT cchItemText)
{
	LPSTR lpBuffer = (LPSTR)malloc(cchItemText);
	BOOL result;

	if(lpBuffer == NULL)
		return FALSE;

	result = GetAltTabInfoA(hwnd, iItem, pati, lpBuffer, cchItemText);

	if(!result)
	{
		free(lpBuffer);
		return FALSE;
	}

	STACK_AtoW(lpBuffer, pszItemText);

	free(lpBuffer);

	return TRUE;
}
#endif

/* MAKE_EXPORT GetClassNameW_new=GetClassNameW */
BOOL WINAPI GetClassNameW_new(HWND hWnd, LPWSTR lpClassName, int nMaxCount)
{
	LPSTR lpBuffer = (LPSTR)malloc(nMaxCount);
	BOOL result;

	if(lpBuffer == NULL)
		return FALSE;

	result = GetClassNameA(hWnd, lpBuffer, nMaxCount);

	if(!result)
	{
		free(lpBuffer);
		return FALSE;
	}

	STACK_AtoW(lpBuffer, lpClassName);

	free(lpBuffer);

	return TRUE;
}

/* MAKE_EXPORT GetClipboardFormatNameW_new=GetClipboardFormatNameW */
BOOL WINAPI GetClipboardFormatNameW_new(UINT format, LPWSTR lpszFormatName, int cchMaxCount)
{
	LPSTR lpBuffer = (LPSTR)malloc(cchMaxCount);
	BOOL result;

	if(lpBuffer == NULL)
		return FALSE;

	result = GetClipboardFormatNameA(format, lpBuffer, cchMaxCount);

	if(!result)
	{
		free(lpBuffer);
		return FALSE;
	}

	STACK_AtoW(lpBuffer, lpszFormatName);

	free(lpBuffer);

	return TRUE;
}

/* MAKE_EXPORT GetKeyboardLayoutNameW_new=GetKeyboardLayoutNameW */
BOOL WINAPI GetKeyboardLayoutNameW_new(LPWSTR pwszKLID)
{
	LPSTR lpBuffer = (LPSTR)malloc(KL_NAMELENGTH);
	BOOL result;

	if(lpBuffer == NULL)
		return FALSE;

	result = GetKeyboardLayoutNameA(lpBuffer);

	if(!result)
	{
		free(lpBuffer);
		return FALSE;
	}

	STACK_AtoW(lpBuffer, pwszKLID);

	free(lpBuffer);

	return TRUE;
}

/* MAKE_EXPORT GetKeyNameTextW_new=GetKeyNameTextW */
int WINAPI GetKeyNameTextW_new(LONG lParam, LPWSTR lpString, int nSize)
{
	LPSTR lpBuffer = (LPSTR)malloc(nSize);
	BOOL result;

	if(lpBuffer == NULL)
		return FALSE;

	result = GetKeyNameTextA(lParam, lpBuffer, nSize);

	if(!result)
	{
		free(lpBuffer);
		return FALSE;
	}

	STACK_AtoW(lpBuffer, lpString);

	free(lpBuffer);

	return TRUE;
}

/* MAKE_EXPORT GetMenuItemInfoW_new=GetMenuItemInfoW */
BOOL WINAPI GetMenuItemInfoW_new(HMENU hMenu, UINT uItem, BOOL fByPosition, LPMENUITEMINFOW lpmii)
{
	MENUITEMINFOA mii;
	LPWSTR lpTypeData;
	BOOL result;

	if(IsBadWritePtr(lpmii, sizeof(MENUITEMINFOW)))
		return FALSE;

	memcpy(&mii, lpmii, sizeof(MENUITEMINFOA));

	result = GetMenuItemInfoA(hMenu, uItem, fByPosition, &mii);

	if(!result)
		return FALSE;

	STACK_AtoW(mii.dwTypeData, lpTypeData);

	memcpy(lpmii, &mii, sizeof(MENUITEMINFOW));

	lpmii->dwTypeData = lpTypeData;

	return result;
}

/* MAKE_EXPORT GetMenuStringW_new=GetMenuStringW */
int WINAPI GetMenuStringW_new(HMENU hMenu, UINT uIDItem, LPWSTR lpString, int nMaxCount, UINT uFlag)
{
	LPSTR lpBuffer = (LPSTR)malloc(nMaxCount);
	int result;

	if(lpBuffer == NULL)
		return FALSE;

	result = GetMenuStringA(hMenu, uIDItem, lpBuffer, nMaxCount, uFlag);

	if(!result)
	{
		free(lpBuffer);
		return FALSE;
	}

	STACK_AtoW(lpBuffer, lpString);

	free(lpBuffer);

	return TRUE;
}

/* MAKE_EXPORT GetMonitorInfoW_new=GetMonitorInfoW */
BOOL WINAPI GetMonitorInfoW_new(HMONITOR hMonitor, LPMONITORINFO lpmi)
{
	LPWSTR lpDevice = NULL;
	BOOL result;
	MONITORINFOEX miex;

	if(IsBadWritePtr(lpmi, sizeof(MONITORINFO)))
		return FALSE;

	if(lpmi->cbSize == sizeof(MONITORINFO))
		return GetMonitorInfoA(hMonitor, lpmi);

	if(lpmi->cbSize != sizeof(MONITORINFOEXW))
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	miex.cbSize = sizeof(MONITORINFOEXA);

	result = GetMonitorInfoA(hMonitor, &miex);

	if(!result)
		return FALSE;

	STACK_AtoW(miex.szDevice, lpDevice);

	//wcscpy(((LPMONITORINFOEXW)lpmi)->szDevice, lpDevice);

	return result;
}

/* MAKE_EXPORT GetPropW_new=GetPropW */
HANDLE WINAPI GetPropW_new(HWND hWnd, LPCWSTR lpStringW)
{
	ALLOC_WtoA(lpString);

	return GetPropA(hWnd, lpStringA);
}

/* MAKE_EXPORT GetTabbedTextExtentW_new=GetTabbedTextExtentW */
DWORD WINAPI GetTabbedTextExtentW_new(HDC hDC, LPCWSTR lpStringW, int nCount, int nTabPositions, CONST LPINT lpnTabStopPositions)
{
	ALLOC_WtoA(lpString);

	return GetTabbedTextExtentA(hDC, lpStringA, nCount, nTabPositions, lpnTabStopPositions);
}

/* MAKE_EXPORT GrayStringW_new=GrayStringW */
BOOL WINAPI GrayStringW_new(HDC hDC, HBRUSH hBrush, GRAYSTRINGPROC lpOutputFunc, LPARAM lpData, int nCount, int X, int Y, int nWidth, int nHeight)
{
	return GrayStringA(hDC, hBrush, lpOutputFunc, lpData, nCount, X, Y, nWidth, nHeight);
}

/* MAKE_EXPORT InsertMenuItemW_new=InsertMenuItemW */
BOOL WINAPI InsertMenuItemW_new(HMENU hMenu, UINT uItem, BOOL fByPosition, LPCMENUITEMINFO lpmii)
{
	MENUITEMINFOA mii;
	BOOL result;
	LPSTR lpTypeData;

	if(IsBadReadPtr(lpmii, sizeof(MENUITEMINFOW)))
		return FALSE;

	memcpy(&mii, lpmii, sizeof(MENUITEMINFOA));

	lpTypeData = NULL;

	STACK_WtoA(lpmii->dwTypeData, lpTypeData);

	mii.dwTypeData = lpTypeData;

	result = InsertMenuItemA(hMenu, uItem, fByPosition, lpmii);

	if(!result)
		return FALSE;

	return TRUE;
}

/* MAKE_EXPORT LoadAcceleratorsW_new=LoadAcceleratorsW_new */
HACCEL WINAPI LoadAcceleratorsW_new(HINSTANCE hInstance, LPCWSTR lpTableNameW)
{
	if(IS_INTRESOURCE(lpTableNameW))
		return LoadAcceleratorsA(hInstance, (LPCSTR)lpTableNameW);

	ALLOC_WtoA(lpTableName);

	return LoadAcceleratorsA(hInstance, lpTableNameA);
}

/* MAKE_EXPORT LoadBitmapW_new=LoadBitmapW */
HBITMAP WINAPI LoadBitmapW_new(HINSTANCE hInstance, LPCWSTR lpBitmapNameW)
{
	if(IS_INTRESOURCE(lpBitmapNameW))
		return LoadBitmapA(hInstance, (LPCSTR)lpBitmapNameW);

	ALLOC_WtoA(lpBitmapName);

	return LoadBitmapA(hInstance, lpBitmapNameA);
}

/* MAKE_EXPORT LoadCursorFromFileW_new=LoadCursorFromFileW */
HCURSOR WINAPI LoadCursorFromFileW_new(LPCWSTR lpFileNameW)
{
	ALLOC_WtoA(lpFileName);

	return LoadCursorFromFileA(lpFileNameA);
}

/* MAKE_EXPORT LoadCursorW_new=LoadCursorW */
HCURSOR WINAPI LoadCursorW_new(HINSTANCE hInstance, LPCWSTR lpCursorNameW)
{
	if(IS_INTRESOURCE(lpCursorNameW))
		return LoadCursorA(hInstance, (LPCSTR)lpCursorNameW);

	ALLOC_WtoA(lpCursorName);

	return LoadCursorA(hInstance, lpCursorNameA);
}

/* MAKE_EXPORT LoadIconW_new=LoadIconW */
HICON WINAPI LoadIconW_new(HINSTANCE hInstance, LPCWSTR lpIconNameW)
{
	if(IS_INTRESOURCE(lpIconNameW))
		return LoadIconA(hInstance, (LPCSTR)lpIconNameW);

	ALLOC_WtoA(lpIconName);

	return LoadIconA(hInstance, lpIconNameA);
}

/* MAKE_EXPORT LoadImageW_new=LoadImageW */
HANDLE WINAPI LoadImageW_new(HINSTANCE hinst, LPCWSTR lpszNameW, UINT uType, int cxDesired, int cyDesired, UINT fuLoad)
{
	if(IS_INTRESOURCE(lpszNameW))
		return LoadImageA(hinst, (LPCSTR)lpszNameW, uType, cxDesired, cyDesired, fuLoad);

	ALLOC_WtoA(lpszName);

	return LoadImageA(hinst, lpszNameA, uType, cxDesired, cyDesired, fuLoad);
}

/* MAKE_EXPORT LoadKeyboardLayoutW_new=LoadKeyboardLayoutW */
HKL WINAPI LoadKeyboardLayoutW_new(LPCWSTR pwszKLIDW, UINT Flags)
{
	ALLOC_WtoA(pwszKLID);

	return LoadKeyboardLayoutA(pwszKLIDA, Flags);
}

/* MAKE_EXPORT MapVirtualKeyExW_new=MapVirtualKeyExW */
UINT WINAPI MapVirtualKeyExW_new(UINT uCode, UINT uMapType, HKL dwhkl)
{
	return MapVirtualKeyExA(uCode, uMapType, dwhkl);
}

/* MAKE_EXPORT MapVirtualKeyW_new=MapVirtualKeyW */
UINT WINAPI MapVirtualKeyW_new(UINT uCode, UINT uMapType)
{
	return MapVirtualKeyA(uCode, uMapType);
}

/* MAKE_EXPORT MessageBoxIndirectW_new=MessageBoxIndirectW */
int WINAPI MessageBoxIndirectW_new(const LPMSGBOXPARAMSW lpMsgBoxParams)
{
	LPSTR lpszText = NULL;
	LPSTR lpszCaption = NULL;
	LPSTR lpszIcon = NULL;
	MSGBOXPARAMSA mbp;

	if(IsBadReadPtr(lpMsgBoxParams, sizeof(MSGBOXPARAMSW)))
		return 0;

	memcpy(&mbp, lpMsgBoxParams, sizeof(MSGBOXPARAMSA));

	STACK_WtoA(lpMsgBoxParams->lpszText, lpszText);
	STACK_WtoA(lpMsgBoxParams->lpszCaption, lpszCaption);
	STACK_WtoA(lpMsgBoxParams->lpszIcon, lpszIcon);

	mbp.lpszText = lpszText;
	mbp.lpszCaption = lpszCaption;
	mbp.lpszIcon = lpszIcon;

	return MessageBoxIndirectA(&mbp);
}

/* MAKE_EXPORT RegisterClipboardFormatW_new=RegisterClipboardFormatW */
UINT WINAPI RegisterClipboardFormatW_new(LPCWSTR lpszFormatW)
{
	ALLOC_WtoA(lpszFormat);

	return RegisterClipboardFormatA(lpszFormatA);
}

/* MAKE_EXPORT RegisterDeviceNotificationW_new=RegisterDeviceNotificationW */
HDEVNOTIFY WINAPI RegisterDeviceNotificationW_new(HANDLE hRecipient, LPVOID NotificationFilter, DWORD Flags)
{
	return RegisterDeviceNotificationA(hRecipient, NotificationFilter, Flags);
}

/* MAKE_EXPORT RegisterWindowMessageW_new=RegisterWindowMessageW_new */
UINT WINAPI RegisterWindowMessageW_new(LPCWSTR lpStringW)
{
	ALLOC_WtoA(lpString);

	return RegisterWindowMessageA(lpStringA);
}

/* MAKE_EXPORT RemovePropW_new=RemovePropW */
HANDLE WINAPI RemovePropW_new(HWND hWnd, LPCWSTR lpStringW)
{
	ALLOC_WtoA(lpString);

	return RemovePropA(hWnd, lpStringA);
}

/* MAKE_EXPORT SetMenuItemInfoW_new=SetMenuItemInfoW */
BOOL WINAPI SetMenuItemInfoW_new(HMENU hMenu, UINT uItem, BOOL fByPosition, LPCMENUITEMINFO lpmii)
{
	MENUITEMINFOA mii;
	BOOL result;
	LPSTR lpTypeData;

	if(IsBadReadPtr(lpmii, sizeof(MENUITEMINFOW)))
		return FALSE;

	memcpy(&mii, lpmii, sizeof(MENUITEMINFOA));

	lpTypeData = NULL;

	STACK_WtoA(lpmii->dwTypeData, lpTypeData);

	mii.dwTypeData = lpTypeData;

	result = SetMenuItemInfoA(hMenu, uItem, fByPosition, lpmii);

	if(!result)
		return FALSE;

	return TRUE;
}

/* MAKE_EXPORT SetPropW_new=SetPropW */
BOOL WINAPI SetPropW_new(HWND hWnd, LPCWSTR lpStringW, HANDLE hData)
{
	ALLOC_WtoA(lpString);

	return SetPropA(hWnd, lpStringA, hData);
}

/* MAKE_EXPORT SystemParametersInfoW_new=SystemParametersInfoW */
BOOL WINAPI SystemParametersInfoW_new(UINT uiAction, UINT uiParam, PVOID pvParam, UINT fWinIni)
{
	LPSTR lpString = NULL;

	if(!IsBadUnicodeStringPtr((LPCWSTR)pvParam, -1))
		STACK_WtoA(pvParam, lpString);

	return SystemParametersInfoA(uiAction,
								uiParam,
								lpString != NULL ? lpString : pvParam,
								fWinIni);
}

/* MAKE_EXPORT TabbedTextOutW_new=TabbedTextOutW */
LONG WINAPI TabbedTextOutW_new(HDC hDC, int X, int Y, LPCWSTR lpStringW, int nCount, int nTabPositions, CONST LPINT lpnTabStopPositions, int nTabOrigin)
{
	ALLOC_WtoA(lpString);

	return TabbedTextOutA(hDC, X, Y, lpStringA, nCount, nTabPositions, lpnTabStopPositions, nTabOrigin);
}

/* MAKE_EXPORT VkKeyScanExW_new=VkKeyScanExW */
SHORT WINAPI VkKeyScanExW_new(WCHAR ch, HKL dwhkl)
{
	LPSTR lpChar = NULL;
	CHAR character;

	STACK_WtoA(ch, lpChar);

	character = lpChar[0];

	return VkKeyScanExA(character, dwhkl);
}

/* MAKE_EXPORT VkKeyScanW_new=VkKeyScanW */
SHORT WINAPI VkKeyScanW_new(WCHAR ch)
{
	LPSTR lpChar = NULL;
	CHAR character;

	STACK_WtoA(ch, lpChar);

	character = lpChar[0];

	return VkKeyScanA(character);
}

/* MAKE_EXPORT WinHelpW_new=WinHelpW */
BOOL WINAPI WinHelpW_new(HWND hWndMain, LPCWSTR lpszHelpW, UINT uCommand, ULONG_PTR dwData)
{
	ALLOC_WtoA(lpszHelp);

	return WinHelpA(hWndMain, lpszHelpA, uCommand, dwData);
}

/* MAKE_EXPORT wsprintfW_new=wsprintfW */
int wsprintfW_new(LPWSTR lpOut, LPCWSTR lpFmt, ...)
{
	int count = 0;
	va_list args;

	__try
	{
		va_start(args, lpFmt);
		count = wvsprintfW_new(lpOut, lpFmt, args);
		va_end(args);
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		return 0;
	}

	return count;
}

/* MAKE_EXPORT wvsprintfW_new=wvsprintfW */
int wvsprintfW_new(LPWSTR lpOutput, LPCWSTR lpFmt, va_list arglist)
{
	int count = 0;

	__try
	{
		count = vswprintf(lpOutput, lpFmt, arglist);
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		return 0;
	}

	return count;
}

