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

#include "security.h"
#include <Lmcons.h>
#include <shlwapi.h>

typedef LONG (WINAPI *REGREMAPPREDEFKEY)(HKEY hKey, HKEY hNewHKey);

DWORD FASTCALL GetProfileRegString(HKEY hKey,
	LPCSTR lpKeyName,
	LPCSTR lpValueName,
	LPCSTR lpDefault,
	LPSTR lpReturnedString,
	DWORD nSize
)
{
	HKEY hkResult = NULL;
	DWORD dwBufSize;
	LONG result = 0;

	result = RegOpenKeyEx(hKey, lpKeyName, NULL, KEY_ALL_ACCESS, &hkResult);

	if(result != ERROR_SUCCESS)
	{
		strcpy(lpReturnedString, lpDefault);
		return strlen(lpReturnedString);
	}

	dwBufSize = nSize;

	result = RegQueryValueEx(hkResult,
					lpValueName,
					NULL, NULL,
					(LPBYTE)lpReturnedString,
					&dwBufSize);

	RegCloseKey(hkResult);

	if(result != ERROR_SUCCESS)
	{
		strcpy(lpReturnedString, lpDefault);
		return strlen(lpReturnedString);
	}

	return dwBufSize;
}

LONG FASTCALL GetProfileRegInt(HKEY hKey, 
	LPCSTR lpKeyName,
	LPCSTR lpValueName,
	INT nDefault
)
{
	CHAR KeyValue[11];
	CHAR Default[11];
	int intKeyValue;

	wsprintf(Default, "%d", nDefault);

	GetProfileRegString(hKey, lpKeyName, lpValueName, Default, KeyValue, sizeof(KeyValue));

	intKeyValue = 0;

	__try
	{
		StrToIntEx(KeyValue, STIF_DEFAULT, &intKeyValue);
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		intKeyValue = nDefault;
	}

	return intKeyValue;
}

BOOL FASTCALL GetProfileRegStruct(HKEY hKey,
	LPCSTR lpKeyName,
	LPCSTR lpValueName,
	LPVOID lpStruct,
	UINT uSizeStruct
)
{
	HKEY hkResult = NULL;
	DWORD dwBufSize;
	LONG result = 0;

	result = RegOpenKeyEx(hKey, lpKeyName, NULL, KEY_ALL_ACCESS, &hkResult);

	if(result != ERROR_SUCCESS)
		return FALSE;

	dwBufSize = uSizeStruct;

	result = RegQueryValueEx(hkResult,
					lpValueName,
					NULL, NULL,
					(LPBYTE)lpStruct,
					&dwBufSize);

	RegCloseKey(hkResult);

	if(result != ERROR_SUCCESS)
		return FALSE;

	return dwBufSize;
}

BOOL FASTCALL GetNonClientMetrics(PNONCLIENTMETRICS pncm)
{
	PCHAR lpKeyPath = "Control Panel\\Desktop\\WindowMetrics";

	#define GETMETRICVALUE(name, defval) GetProfileRegInt(HKEY_CURRENT_USER, lpKeyPath, name, defval) / -15

	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), pncm, 0);

	pncm->iBorderWidth		= GETMETRICVALUE("BorderWidth", -15);
	pncm->iCaptionHeight	= GETMETRICVALUE("CaptionHeight", -270);
	pncm->iCaptionWidth		= GETMETRICVALUE("CaptionWidth", -270);
	pncm->iMenuHeight		= GETMETRICVALUE("MenuHeight", -270);
	pncm->iMenuWidth		= GETMETRICVALUE("MenuWidth", -270);
	pncm->iScrollHeight		= GETMETRICVALUE("ScrollHeight", -240);
	pncm->iScrollWidth		= GETMETRICVALUE("ScrollWidth", -240);
	pncm->iSmCaptionHeight	= GETMETRICVALUE("SmCaptionHeight", -255);
	pncm->iSmCaptionWidth	= GETMETRICVALUE("SmCaptionWidth", -195);

/*
	GetProfileRegStruct(HKEY_CURRENT_USER, lpKeyPath, "CaptionFont", &pncm->lfCaptionFont, sizeof(LOGFONT));
	GetProfileRegStruct(HKEY_CURRENT_USER, lpKeyPath, "MenuFont", &pncm->lfMenuFont, sizeof(LOGFONT));
	GetProfileRegStruct(HKEY_CURRENT_USER, lpKeyPath, "MessageFont", &pncm->lfMessageFont, sizeof(LOGFONT));
	GetProfileRegStruct(HKEY_CURRENT_USER, lpKeyPath, "SmCaptionFont", &pncm->lfSmCaptionFont, sizeof(LOGFONT));
	GetProfileRegStruct(HKEY_CURRENT_USER, lpKeyPath, "StatusFont", &pncm->lfStatusFont, sizeof(LOGFONT));

	pncm->lfCaptionFont.lfHeight = pncm->lfCaptionFont.lfHeight > 0 ? -pncm->lfCaptionFont.lfHeight : pncm->lfCaptionFont.lfHeight;
	pncm->lfMenuFont.lfHeight = pncm->lfMenuFont.lfHeight > 0 ? -pncm->lfMenuFont.lfHeight : pncm->lfMenuFont.lfHeight;
	pncm->lfMessageFont.lfHeight = pncm->lfMessageFont.lfHeight > 0 ? -pncm->lfMessageFont.lfHeight : pncm->lfMessageFont.lfHeight;
	pncm->lfSmCaptionFont.lfHeight = pncm->lfSmCaptionFont.lfHeight > 0 ? -pncm->lfSmCaptionFont.lfHeight : pncm->lfSmCaptionFont.lfHeight;
	pncm->lfStatusFont.lfHeight = pncm->lfStatusFont.lfHeight > 0 ? -pncm->lfStatusFont.lfHeight : pncm->lfStatusFont.lfHeight;
*/

	return TRUE;
}

int ReadColors(char *ColorType)
{
	char KeyName[255];
	char *pch;
	char *chRed, *chGreen, *chBlue;
	HKEY hKey;
	DWORD dwLength;
	int red, green, blue;
	BYTE i;

	if(RegOpenKeyEx(HKEY_CURRENT_USER, "Control Panel\\Colors", 0, KEY_ALL_ACCESS, &hKey) != ERROR_SUCCESS)
		return 0;

	dwLength = sizeof(KeyName);

	if(RegQueryValueEx(hKey, ColorType, NULL, NULL, (LPBYTE)KeyName, &dwLength) != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		return 0;
	}

	i = 0;

	pch = strtok(KeyName, " ");
	while(pch != NULL)
	{
		if(i == 0)
			chRed = pch;
		else if(i == 1)
			chGreen = pch;
		else if(i == 2)
			chBlue = pch;

		pch = strtok(NULL, " ");
		i++;
	}

	if(chRed == NULL || chGreen == NULL || chBlue == NULL)
		return 0;

	red = atoi(chRed);
	green = atoi(chGreen);
	blue = atoi(chBlue);

	return RGB(red, green, blue);
}

void SetNewColors()
{
	int i;
	int IndexArray[28];
	DWORD NewColors[28];

	NewColors[0] = ReadColors("ScrollBar");
	NewColors[1] = ReadColors("Background");
	NewColors[2] = ReadColors("ActiveTitle");
	NewColors[3] = ReadColors("InactiveTitle");
	NewColors[4] = ReadColors("Menu");
	NewColors[5] = ReadColors("Window");
	NewColors[6] = ReadColors("WindowFrame");
	NewColors[7] = ReadColors("MenuText");
	NewColors[8] = ReadColors("WindowText");
	NewColors[9] = ReadColors("TitleText");
	NewColors[10] = ReadColors("ActiveBorder");
	NewColors[11] = ReadColors("InactiveBorder");
	NewColors[12] = ReadColors("AppWorkspace");
	NewColors[13] = ReadColors("Highlight");
	NewColors[14] = ReadColors("HighlightText");
	NewColors[15] = ReadColors("ButtonFace");
	NewColors[16] = ReadColors("ButtonShadow");
	NewColors[17] = ReadColors("GrayText");
	NewColors[18] = ReadColors("ButtonText");
	NewColors[19] = ReadColors("InactiveTitleText");
	NewColors[20] = ReadColors("ButtonHighlight");
	NewColors[21] = ReadColors("ButtonDkShadow");
	NewColors[22] = ReadColors("ButtonLight");
	NewColors[23] = ReadColors("InfoText");
	NewColors[24] = ReadColors("InfoWindow");
	NewColors[25] = 0;
	NewColors[26] = ReadColors("HotTrackingColor");
	NewColors[27] = ReadColors("GradientActiveTitle");
	NewColors[28] = ReadColors("GradientInactiveTitle");

	if (NewColors[0]  == 0) { NewColors[0] = RGB(192, 192, 192); }
	if (NewColors[1]  == 0) { NewColors[1] = RGB(0, 128, 128); }
	if (NewColors[2]  == 0) { NewColors[2] = RGB(0, 0, 128); }
	if (NewColors[3]  == 0) { NewColors[3] = RGB(128, 128, 128); }
	if (NewColors[4]  == 0) { NewColors[4] = RGB(192, 192, 192); }
	if (NewColors[5]  == 0) { NewColors[5] = RGB(255, 255, 255); }
	if (NewColors[6]  == 0) { NewColors[6] = RGB(0, 0, 0); }
	if (NewColors[7]  == 0) { NewColors[7] = RGB(0, 0, 0); }
	if (NewColors[8]  == 0) { NewColors[8] = RGB(0, 0, 0); }
	if (NewColors[9]  == 0) { NewColors[9] = RGB(255, 255, 255); }
	if (NewColors[10] == 0) { NewColors[10] = RGB(192, 192, 192); }
	if (NewColors[11] == 0) { NewColors[11] = RGB(192, 192, 192); }
	if (NewColors[12] == 0) { NewColors[12] = RGB(128, 128, 128); }
	if (NewColors[13] == 0) { NewColors[13] = RGB(0, 0, 128); }
	if (NewColors[14] == 0) { NewColors[14] = RGB(255, 255, 255); }
	if (NewColors[15] == 0) { NewColors[15] = RGB(192, 192, 192); }
	if (NewColors[16] == 0) { NewColors[16] = RGB(128, 128, 128); }
	if (NewColors[17] == 0) { NewColors[17] = RGB(128, 128, 128); }
	if (NewColors[18] == 0) { NewColors[18] = RGB(0, 0, 0); }
	if (NewColors[19] == 0) { NewColors[19] = RGB(192, 192, 192); }
	if (NewColors[20] == 0) { NewColors[20] = RGB(255, 255, 255); }
	if (NewColors[21] == 0) { NewColors[21] = RGB(0, 0, 0); }
	if (NewColors[22] == 0) { NewColors[22] = RGB(223, 223, 223); }
	if (NewColors[23] == 0) { NewColors[23] = RGB(0, 0, 0); }
	if (NewColors[24] == 0) { NewColors[24] = RGB(255, 255, 225); }
	if (NewColors[26] == 0) { NewColors[26] = RGB(0, 0, 255); }
	if (NewColors[27] == 0) { NewColors[27] = RGB(0, 0, 128); }
	if (NewColors[28] == 0) { NewColors[28] = RGB(128, 128, 128); }

	for(i=0;i<=28;i++)
	{
		if(i == 25)
			continue;

		IndexArray[i] = i;
	}

	SetSysColors(28, IndexArray, NewColors);
}

DWORD WINAPI SessionSwitchThread(LPVOID lParam)
{
	PCHAR lpDeskPath = "Control Panel\\Desktop";
	PCHAR lpMousePath = "Control Panel\\Mouse";
	PCHAR lpKeyboardPath = "Control Panel\\Keyboard";
	ANIMATIONINFO ai;
	NONCLIENTMETRICS ncm;
	BOOL fDragFullWindows, fMenuDropAlignment, fScreenSaverEnabled, fSmoothFont, fSwapMouseButtons = FALSE;
	int ScreenSaverTimeout = 0;
	RECT rect;
	CHAR Wallpaper[255];
	HKEY hKey = NULL;
	LPCSTR lpUserName = (LPCSTR)lParam;
	LONG result;
	int DoubleClickTime, DoubleClickHeight, DoubleClickWidth, MouseSpeed = 0;
	int KeyboardDelay, KeyboardSpeed = 0;

	#define GETPROFILEVALUE(path, val, def) GetProfileRegInt(HKEY_CURRENT_USER, path, val, def)

	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_IDLE);

	result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "System\\CurrentControlSet\\Control", NULL, KEY_ALL_ACCESS, &hKey);

	if(result == ERROR_SUCCESS)
	{
		RegSetValueEx(hKey, "Current User", 0, REG_SZ, (BYTE*)lpUserName, strlen(lpUserName));
		RegCloseKey(hKey);
	}

	memset(&ai, 0, sizeof(ANIMATIONINFO));
	memset(&ncm, 0, sizeof(NONCLIENTMETRICS));

	ai.cbSize = sizeof(ai);
	ncm.cbSize = sizeof(NONCLIENTMETRICS);

	rect.left = 15;
	rect.top = GetSystemMetrics(SM_CYBORDER);
	rect.right = 15;
	rect.bottom = GetSystemMetrics(SM_CXBORDER);

	// Retrieves settings in HKEY_CURRENT_USER

	GetProfileRegString(HKEY_CURRENT_USER, lpDeskPath, "Wallpaper", "", Wallpaper, sizeof(Wallpaper));

	fDragFullWindows		= GETPROFILEVALUE(lpDeskPath, "DragFullWindows", 0);
	fSmoothFont				= GETPROFILEVALUE(lpDeskPath, "FontSmoothing", 0);

	fScreenSaverEnabled		= GETPROFILEVALUE(lpDeskPath, "ScreenSaveActive", 0);
	ScreenSaverTimeout		= GETPROFILEVALUE(lpDeskPath, "ScreenSaveTimeout", 840);

	fSwapMouseButtons		= GETPROFILEVALUE(lpMousePath, "SwapMouseButtons", 0);
	DoubleClickTime			= GETPROFILEVALUE(lpMousePath, "DoubleClickSpeed", 500);
	DoubleClickHeight		= GETPROFILEVALUE(lpMousePath, "DoubleClickHeight", 4);
	DoubleClickWidth		= GETPROFILEVALUE(lpMousePath, "DoubleClickWidth", 4);
	MouseSpeed				= GETPROFILEVALUE(lpMousePath, "MouseSpeed", 1);

	KeyboardDelay			= GETPROFILEVALUE(lpKeyboardPath, "KeyboardDelay", 1);
	KeyboardSpeed			= GETPROFILEVALUE(lpKeyboardPath, "KeyboardSpeed", 31);

	SystemParametersInfo(SPI_GETANIMATION, 0, &ai, 0);

	GetNonClientMetrics(&ncm);

	SystemParametersInfo(SPI_GETMENUDROPALIGNMENT, 0, &fMenuDropAlignment, 0);

	// Set up mouse

	SystemParametersInfo(SPI_SETMOUSEBUTTONSWAP, fSwapMouseButtons, 0, SPIF_SENDCHANGE);
	SystemParametersInfo(SPI_SETDOUBLECLICKTIME, DoubleClickTime, 0, SPIF_SENDCHANGE);
	SystemParametersInfo(SPI_SETDOUBLECLKHEIGHT, DoubleClickHeight, 0, SPIF_SENDCHANGE);
	SystemParametersInfo(SPI_SETDOUBLECLKWIDTH, DoubleClickWidth, 0, SPIF_SENDCHANGE);
	SystemParametersInfo(SPI_SETMOUSESPEED, MouseSpeed, 0, SPIF_SENDCHANGE);

	// Set up keyboard

	SystemParametersInfo(SPI_SETKEYBOARDDELAY, KeyboardDelay, 0, SPIF_SENDCHANGE);
	SystemParametersInfo(SPI_SETKEYBOARDSPEED, KeyboardSpeed, 0, SPIF_SENDCHANGE);

	// Set up the wallpaper, pattern, screen-saver, and other options

	SystemParametersInfo(SPI_SETANIMATION, 0, &ai, SPIF_SENDCHANGE);

	SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, Wallpaper, SPIF_SENDCHANGE);
	SystemParametersInfo(SPI_SETDESKPATTERN, 0, 0, SPIF_SENDCHANGE);

	SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, fScreenSaverEnabled, 0, SPIF_SENDCHANGE);
	SystemParametersInfo(SPI_SETSCREENSAVETIMEOUT, ScreenSaverTimeout, 0, SPIF_SENDCHANGE);

	SystemParametersInfo(SPI_SETDRAGFULLWINDOWS, fDragFullWindows, 0, SPIF_SENDCHANGE);
	SystemParametersInfo(SPI_SETDRAGHEIGHT, GetSystemMetrics(SM_CYDRAG), 0, SPIF_SENDCHANGE);
	SystemParametersInfo(SPI_SETDRAGWIDTH, GetSystemMetrics(SM_CXDRAG), 0, SPIF_SENDCHANGE);
	SystemParametersInfo(SPI_SETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, SPIF_SENDCHANGE);

	SystemParametersInfo(SPI_SETMENUDROPALIGNMENT, fMenuDropAlignment, 0, SPIF_SENDCHANGE);
	SystemParametersInfo(SPI_SETFONTSMOOTHING, fSmoothFont, 0, SPIF_SENDCHANGE);
	SystemParametersInfo(SPI_SETWORKAREA, 0, &rect, SPIF_SENDCHANGE);

	SetNewColors();

	ExitThread(0);

	return 0;
}

/* IntSwitchUser
 *
 * Switch to another user from HKEY_USERS
 * Sets the system parameters and colors
 *
 * Parameters:
 * @lpszUserName : User name, it must be loaded with the same name in HKEY_USERS
 * @fWait : If TRUE, wait for the user to be fully loaded
 *
 * Return value:
 * If the function succeeds, the return value is TRUE.
 * If the function fails, the return value is FALSE. To get extended error information, call GetLastError.
 *
 *
 */
BOOL WINAPI IntSwitchUser(LPCSTR lpszUsername, BOOL fWait)
{
	DWORD dwThreadId = 0;
	HKEY hKey = NULL;
	HANDLE hThread;
	LONG result = 0;
	REGREMAPPREDEFKEY RegRemapPreDefKey = (REGREMAPPREDEFKEY)kexGetProcAddress(GetModuleHandle("ADVAPI32.DLL"), "RegRemapPreDefKey");
	CHAR CurrentUser[UNLEN + 1];
	DWORD dwUserSize = sizeof(CurrentUser);

	result = GetUserName(CurrentUser, &dwUserSize);

	/* Don't switch if the user name is the current user name */
	if(strcmp(CurrentUser, lpszUsername) == 0 || (!result && strcmp(lpszUsername, ".DEFAULT") == 0))
		return TRUE;

	result = RegOpenKeyEx(HKEY_USERS, lpszUsername, NULL, KEY_ALL_ACCESS, &hKey);

	if(result != ERROR_SUCCESS)
	{
		SetLastError(result);
		return FALSE;
	}

	RegFlushKey(HKEY_CURRENT_USER);

	/* Change HKEY_CURRENT_USER to a new key  */
	result = RegRemapPreDefKey(hKey, HKEY_CURRENT_USER);

	RegCloseKey(hKey);

	if(result != ERROR_SUCCESS)
	{
		SetLastError(result);
		return FALSE;
	}

	if(result)
		RegUnLoadKey(HKEY_USERS, CurrentUser);

	/* This should be a kernel thread, to avoid corruptions */
	hThread = CreateKernelThread(NULL, 0, SessionSwitchThread, (LPVOID)lpszUsername, 0, &dwThreadId);

	if(hThread == NULL)
		return FALSE;

	if(fWait)
		WaitForSingleObject(hThread, INFINITE);

	CloseHandle(hThread);

	return TRUE;
}

/* MAKE_EXPORT LogonUserA_new=LogonUserA */
BOOL WINAPI LogonUserA_new(LPSTR lpszUsername,
	LPSTR lpszDomain,
	LPSTR lpszPassword,
	DWORD dwLogonType,
	DWORD dwLogonProvider,
	PHANDLE phToken)
{
	CHAR lpszCurrentUser[50];
	BOOL fMatch = FALSE;
	ULONG nLength = 50;
	DWORD result = 0;

	/* dwLogonProvider and dwLogonType must be supported */
	if(dwLogonProvider == LOGON32_PROVIDER_WINNT50 || dwLogonType == LOGON32_LOGON_NEW_CREDENTIALS
		|| dwLogonType == LOGON32_LOGON_NETWORK_CLEARTEXT)
	{
		TRACE("%s %s %s %d %d %p not supported\n", lpszUsername, lpszDomain, dwLogonType,
						dwLogonProvider, phToken);
		SetLastError(ERROR_NOT_SUPPORTED);
		return FALSE;
	}

	if(dwLogonProvider > LOGON32_PROVIDER_WINNT50 || dwLogonType > LOGON32_LOGON_UNLOCK
		|| dwLogonType < LOGON32_LOGON_INTERACTIVE)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	/* if the domain is not null, it must be a "." in Win9x (compatibility reason) */
	if(!IsBadStringPtr(lpszDomain, -1) && strcmpi(lpszDomain, "."))
	{
		TRACE("%s %s %s %d %d %p not supported\n", lpszUsername, lpszDomain, dwLogonType,
						dwLogonProvider, phToken);
		SetLastError(ERROR_NOT_SUPPORTED);
		return FALSE;
	}

	if(IsBadWritePtr(phToken, sizeof(DWORD)))
	{
		TRACE_OUT("phToken is NULL !\n");
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	/* Get the current user */
	result = GetUserName(lpszCurrentUser, &nLength);

	if(!result)
	{
		TRACE("retrieving current user failed with error %d\n", GetLastError());
		SetLastError(result);
		return FALSE;
	}

	/* lpszUsername must be current user in Windows 98 */
	if(strcmpi(lpszCurrentUser, lpszUsername))
	{
		TRACE("lpszCurrentUser %s does not match lpszUsername %s\n", lpszCurrentUser, lpszUsername);
		SetLastError(ERROR_CANNOT_OPEN_PROFILE);
		return FALSE;
	}

	/* Now check if the password match*/
	result = WNetVerifyPassword(lpszPassword, &fMatch);

	if(result != NO_ERROR)
	{
		TRACE("WNetVerifyPassword failed with error %d\n", GetLastError());
		return FALSE;
	}

	if(fMatch == FALSE)
	{
		TRACE_OUT("lpszPassword does not match !\n");
		return FALSE;
	}

	*(int*)phToken = 0xCAFE;

	return TRUE;
}