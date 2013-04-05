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

BOOL FASTCALL GetNonClientMetrics(PNONCLIENTMETRICS pncm)
{
	CHAR chKeyValue[7];
	HKEY hKey = NULL;
	DWORD dwLength;

	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), pncm, 0);

	RegOpenKeyEx(HKEY_CURRENT_USER, "Control Panel\\Desktop\\WindowMetrics", 0, KEY_ALL_ACCESS, &hKey);

	if(hKey == NULL)
		return FALSE;

	dwLength = sizeof(chKeyValue);
	
	RegQueryValueEx(hKey, "BorderWidth", NULL, NULL, (LPBYTE)chKeyValue, &dwLength);
	pncm->iBorderWidth = atoi(chKeyValue);

	dwLength = sizeof(chKeyValue);

	RegQueryValueEx(hKey, "CaptionHeight", NULL, NULL, (LPBYTE)chKeyValue, &dwLength);
	pncm->iCaptionHeight = atoi(chKeyValue);

	dwLength = sizeof(chKeyValue);

	RegQueryValueEx(hKey, "CaptionWidth", NULL, NULL, (LPBYTE)chKeyValue, &dwLength);
	pncm->iCaptionWidth = atoi(chKeyValue);

	dwLength = sizeof(chKeyValue);

	RegQueryValueEx(hKey, "MenuHeight", NULL, NULL, (LPBYTE)chKeyValue, &dwLength);
	pncm->iMenuHeight = atoi(chKeyValue);

	dwLength = sizeof(chKeyValue);

	RegQueryValueEx(hKey, "MenuWidth", NULL, NULL, (LPBYTE)chKeyValue, &dwLength);
	pncm->iMenuWidth = atoi(chKeyValue);

	dwLength = sizeof(chKeyValue);

	RegQueryValueEx(hKey, "ScrollHeight", NULL, NULL, (LPBYTE)chKeyValue, &dwLength);
	pncm->iScrollHeight = atoi(chKeyValue);

	dwLength = sizeof(chKeyValue);

	RegQueryValueEx(hKey, "ScrollWidth", NULL, NULL, (LPBYTE)chKeyValue, &dwLength);
	pncm->iScrollWidth = atoi(chKeyValue);

	dwLength = sizeof(chKeyValue);

	RegQueryValueEx(hKey, "SmCaptionHeight", NULL, NULL, (LPBYTE)chKeyValue, &dwLength);
	pncm->iSmCaptionHeight = atoi(chKeyValue);

	dwLength = sizeof(chKeyValue);

	RegQueryValueEx(hKey, "SmCaptionWidth", NULL, NULL, (LPBYTE)chKeyValue, &dwLength);
	pncm->iSmCaptionWidth = atoi(chKeyValue);

	return TRUE;
}

int ReadColors(char *ColorType)
{
	char KeyName[255];
	char *pch;
	char *buff[2];
	int i;
	HKEY hKey;
	DWORD dwLength;
	int red, green, blue = 0;

	if(RegOpenKeyEx(HKEY_CURRENT_USER, "Control Panel\\Colors", 0, KEY_ALL_ACCESS, &hKey) != ERROR_SUCCESS)
		return 0;

	dwLength = sizeof(KeyName);

	if(RegQueryValueEx(hKey, ColorType, NULL, NULL, (LPBYTE)KeyName, &dwLength) != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		return 0;
	}

	pch = strtok(KeyName, " ");
	while(pch != NULL)
	{
		pch = strtok(NULL, " ");
		buff[i] = pch;
		i++;
	}

	StrToIntEx(buff[0], STIF_DEFAULT, &red);
	StrToIntEx(buff[1], STIF_DEFAULT, &green);
	StrToIntEx(buff[2], STIF_DEFAULT, &blue);

	return RGB(red, green, blue);
}

void SetNewColors()
{
	int i;
	int IndexArray[21];
	DWORD NewColors[20];

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
	if (NewColors[0] == 0) { NewColors[0] = RGB(192, 192, 192); }
	if (NewColors[1] == 0) { NewColors[1] = RGB(0, 128, 128); }
	if (NewColors[2] == 0) { NewColors[2] = RGB(0, 0, 128); }
	if (NewColors[3] == 0) { NewColors[3] = RGB(128, 128, 128); }
	if (NewColors[4] == 0) { NewColors[4] = RGB(192, 192, 192); }
	if (NewColors[5] == 0) { NewColors[5] = RGB(255, 255, 255); }
	if (NewColors[6] == 0) { NewColors[6] = RGB(0, 0, 0); }
	if (NewColors[7] == 0) { NewColors[7] = RGB(0, 0, 0); }
	if (NewColors[8] == 0) { NewColors[8] = RGB(0, 0, 0); }
	if (NewColors[9] == 0) { NewColors[9] = RGB(255, 255, 255); }
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

	for(i=0;i<=21;i++)
		IndexArray[i] = i;

	SetSysColors(21, IndexArray, NewColors);
}

DWORD WINAPI SessionSwitchThread(LPVOID lParam)
{
	ANIMATIONINFO ai;
	NONCLIENTMETRICS ncm;
	int border = 0;
	BOOL fDragFullWindows = 0;
	BOOL fMenuDropAlignment = 0;
	BOOL fScreenSaverEnabled = 0;
	int ScreenSaverTimeout = 0;
	BOOL fSmoothFont = 0;
	RECT rect;
	CHAR Wallpaper[255];
	CHAR chDragFullWindows[7], chFontSmoothing[7], chScreenSaveActive[7], chScreenSaveTimeout[11];
	HKEY hKey = NULL;
	DWORD dwLength;

	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_IDLE); // Could it crash the CPU?

	memset(&ai, 0, sizeof(ANIMATIONINFO));
	memset(&ncm, 0, sizeof(NONCLIENTMETRICS));

	strcpy(chDragFullWindows, "0");
	strcpy(chFontSmoothing, "0");
	strcpy(chScreenSaveActive, "0");
	strcpy(chScreenSaveTimeout, "0");

	ai.cbSize = sizeof(ai);
	ncm.cbSize = sizeof(NONCLIENTMETRICS);

	rect.left = 15;
	rect.top = GetSystemMetrics(SM_CYBORDER);
	rect.right = 15;
	rect.bottom = GetSystemMetrics(SM_CXBORDER);

	// Set up cursors

	//SystemParametersInfo(SPI_SETCURSORS, 0, NULL, SPIF_SENDCHANGE);
	SystemParametersInfo(SPI_SETMOUSEBUTTONSWAP, GetSystemMetrics(SM_SWAPBUTTON), 0, SPIF_SENDCHANGE);

	// Retrieves settings in HKEY_CURRENT_USER

	RegOpenKeyEx(HKEY_CURRENT_USER, "Control Panel\\Desktop", 0, KEY_ALL_ACCESS, &hKey);

	if(hKey != NULL)
	{
		dwLength = sizeof(Wallpaper);
		RegQueryValueEx(hKey, "Wallpaper", NULL, NULL, (LPBYTE)Wallpaper, &dwLength);

		dwLength = 7 * sizeof(CHAR);
		RegQueryValueEx(hKey, "DragFullWindows", NULL, NULL, (LPBYTE)chDragFullWindows, &dwLength);

		RegQueryValueEx(hKey, "FontSmoothing", NULL, NULL, (LPBYTE)chFontSmoothing, &dwLength);

		RegQueryValueEx(hKey, "ScreenSaveActive", NULL, NULL, (LPBYTE)chScreenSaveActive, &dwLength);

		dwLength = 11 * sizeof(CHAR);
		RegQueryValueEx(hKey, "ScreenSaveTimeout", NULL, NULL, (LPBYTE)chScreenSaveTimeout, &dwLength);
	}

	SystemParametersInfo(SPI_GETANIMATION, 0, &ai, 0);

	fDragFullWindows = atoi(chDragFullWindows);
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0);

	//GetNonClientMetrics(&ncm);

	border = ncm.iBorderWidth;

	SystemParametersInfo(SPI_GETMENUDROPALIGNMENT, 0, &fMenuDropAlignment, 0);

	fScreenSaverEnabled = atoi(chScreenSaveActive);
	ScreenSaverTimeout = atoi(chScreenSaveTimeout);
	fSmoothFont = atoi(chFontSmoothing);

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
	HKEY hKey;
	HANDLE hThread;
	LONG result;
	REGREMAPPREDEFKEY RegRemapPreDefKey = (REGREMAPPREDEFKEY)kexGetProcAddress(GetModuleHandle("ADVAPI32.DLL"), "RegRemapPreDefKey");
	CHAR CurrentUser[UNLEN + 1];
	DWORD dwUserSize = sizeof(CurrentUser);

	if((result = GetUserName(CurrentUser, &dwUserSize)))
		RegUnLoadKey(HKEY_USERS, CurrentUser);

	/* Don't switch if the user name is the current user name */
	if(strcmp(CurrentUser, lpszUsername) == 0 || (!result && strcmp(lpszUsername, ".DEFAULT") == 0))
		return TRUE;

	result = RegOpenKey(HKEY_USERS, lpszUsername, &hKey);

	if(result != ERROR_SUCCESS)
	{
		SetLastError(result);
		return FALSE;
	}

	/* Change HKEY_CURRENT_USER to a new key  */
	result = RegRemapPreDefKey(hKey, HKEY_CURRENT_USER);

	RegCloseKey(hKey);

	if(result != ERROR_SUCCESS)
	{
		SetLastError(result);
		return FALSE;
	}

	RegCloseKey(hKey);

	/* This should be a kernel thread, to avoid corruptions */
	hThread = CreateKernelThread(NULL, 0, SessionSwitchThread, NULL, 0, &dwThreadId);

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