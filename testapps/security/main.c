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

#include "main.h"

HDESK hSecurity = NULL;
HDESK hDefault = NULL;
BOOL fWinME;
const char *InvalidVersion = TEXT("This application must be run in Windows 98/Millenium with KernelEx !");
const char *InvalidPassword = TEXT("The password you entered is incorrect. Reenter your password in case-sensitive. Letters in passwords must be typed using the correct case.");
const char *ComputerLocked = TEXT("Computer Locked");

BOOL WINAPI LockWorkStation(void);

BOOL CenterWindow(HWND hWnd)
{
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int x;
    int y;
    int Width;
    int Height;
    RECT rc;

    GetWindowRect(hWnd, &rc);

    Width = rc.right - rc.left;
    Height = rc.bottom - rc.top;
    x = (screenWidth - Width)/2;
    y = (screenHeight - Height)/3;

    return SetWindowPos(hWnd,
                        NULL,
                        x, y, 0, 0,
                        SWP_NOSIZE | SWP_NOZORDER);
}

INT_PTR CALLBACK LockProc(HWND hwndDlg,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam
)
{
	TCHAR UserName[MAX_PATH];
	TCHAR Password[MAX_PATH];
	DWORD Length = 0;
	BOOL fMatch = FALSE;
	HANDLE hToken = NULL;
	TCHAR Text[MAX_PATH];
	TCHAR NewText[MAX_PATH];
	TCHAR ComputerName[MAX_PATH];
	DWORD nSize = sizeof(ComputerName);

	switch(uMsg)
	{
	case WM_INITDIALOG:
		CenterWindow(hwndDlg);
		Length = sizeof(UserName);

		GetComputerName(ComputerName, &nSize);
		WNetGetUser(NULL, UserName, &Length);

		GetDlgItemText(hwndDlg, IDC_TEXT2, Text, sizeof(Text));
		wsprintf(NewText, Text, ComputerName, UserName);
		SetDlgItemText(hwndDlg, IDC_TEXT2, NewText);

		SetDlgItemText(hwndDlg, IDC_USER, UserName);
		EnableWindow(GetDlgItem(hwndDlg, IDCANCEL), FALSE);
		ShowWindow(GetDlgItem(hwndDlg, IDC_DOMAIN), SW_HIDE);
		ShowWindow(GetDlgItem(hwndDlg, IDC_TDOMAIN), SW_HIDE);
		SetFocus(GetDlgItem(hwndDlg, IDC_PASSWORD));
		break;

	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			GetDlgItemText(hwndDlg, IDC_USER, UserName, sizeof(UserName));
			GetDlgItemText(hwndDlg, IDC_PASSWORD, Password, sizeof(Password));
			if(!LogonUser(UserName, NULL, Password, LOGON32_LOGON_INTERACTIVE, 0, &hToken))
				MessageBox(hwndDlg, InvalidPassword, ComputerLocked, MB_ICONERROR);
			else
			{
				CloseHandle(hToken);
				EndDialog(hwndDlg, 0);
				return 0;
			}
			SetFocus(GetDlgItem(hwndDlg, IDC_PASSWORD));
			SetDlgItemText(hwndDlg, IDC_PASSWORD, TEXT(""));
			break;

		case IDCANCEL:
			break;
		}
		break;
	}

	return 0;
}

INT_PTR CALLBACK ShutdownProc(HWND hwndDlg,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam
)
{
	TCHAR Text[255];
	TCHAR NewText[255];

	switch(uMsg)
	{

	case WM_INITDIALOG:

		CenterWindow(hwndDlg);
		GetDlgItemText(hwndDlg, IDC_SHUTDOWNTEXT, Text, sizeof(Text));
		wsprintf(NewText, Text, fWinME ? TEXT("ME") : TEXT("98"));
		SetDlgItemText(hwndDlg, IDC_SHUTDOWNTEXT, NewText);
		ShowWindow(GetDlgItem(hwndDlg, IDOK), SW_HIDE);
		ShowWindow(GetDlgItem(hwndDlg, IDCANCEL), SW_HIDE);
		SendDlgItemMessage(hwndDlg, IDOK, BM_SETSTYLE, BS_DEFPUSHBUTTON, TRUE);
		SendDlgItemMessage(hwndDlg, IDC_CSHUTDOWN, BM_SETCHECK, BST_CHECKED, 0);
		break;

	case WM_CLOSE:

		EndDialog(hwndDlg, 0);
		return 1;
		break;

	case WM_COMMAND:

		switch(LOWORD(wParam))
		{

		case IDOK:
		case IDC_POK:

			

			if(SendDlgItemMessage(hwndDlg, IDC_CSHUTDOWN, BM_GETCHECK, 0, 0) == BST_CHECKED)
				EndDialog(hwndDlg, EWX_SHUTDOWN);
			else if(SendDlgItemMessage(hwndDlg, IDC_CRESTART, BM_GETCHECK, 0, 0) == BST_CHECKED)
				EndDialog(hwndDlg, EWX_REBOOT);
			else if(SendDlgItemMessage(hwndDlg, IDC_CPOWEROFF, BM_GETCHECK, 0, 0) == BST_CHECKED)
				EndDialog(hwndDlg, EWX_POWEROFF);

			return 1;
			break;

		case IDCANCEL:
		case IDC_PCANCEL:

			EndDialog(hwndDlg, -2);
			break;

		}

		break;
	}

	return 0;
}

INT_PTR CALLBACK LogoffProc(HWND hwndDlg,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam
)
{
	TCHAR Text[255];
	TCHAR NewText[255];

	switch(uMsg)
	{

	case WM_INITDIALOG:

		CenterWindow(hwndDlg);

		GetDlgItemText(hwndDlg, IDC_ENDTEXT, Text, sizeof(Text));
		wsprintf(NewText, Text, fWinME ? TEXT("ME") : TEXT("98"));
		SetDlgItemText(hwndDlg, IDC_ENDTEXT, NewText);
		break;

	case WM_CLOSE:

		EndDialog(hwndDlg, 0);
		return 1;
		break;

	case WM_COMMAND:

		switch(LOWORD(wParam))
		{

		case IDOK:

			EndDialog(hwndDlg, EWX_LOGOFF);
			break;

		case IDCANCEL:

			EndDialog(hwndDlg, -2);
			break;

		}

		break;
	}

	return 0;
}

INT_PTR CALLBACK SecurityProc(HWND hwndDlg,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam
)
{
	typedef int (WINAPI *PWDCHANGEPASSWORDA)(int,int,HWND,DWORD);
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	FILE *file = NULL;
	HMODULE hLib = NULL;
	PWDCHANGEPASSWORDA PwdChangePasswordA = NULL;
	int result;
	TCHAR Text[255];
	TCHAR NewText[255];
	TCHAR UserName[UNLEN+1];
	TCHAR ComputerName[MAX_COMPUTERNAME_LENGTH+1];
	DWORD nSize;

	switch(uMsg)
	{

	case WM_INITDIALOG:

		CenterWindow(hwndDlg);

		nSize = sizeof(UserName);
		GetUserName(UserName, &nSize);
		nSize = sizeof(ComputerName);
		GetComputerName(ComputerName, &nSize);

		GetDlgItemText(hwndDlg, IDC_LOGONNAME, Text, sizeof(Text));
		wsprintf(NewText, Text, ComputerName, UserName);
		SetDlgItemText(hwndDlg, IDC_LOGONNAME, NewText);

		GetDlgItemText(hwndDlg, IDC_LOGONDATE, Text, sizeof(Text));
		wsprintf(NewText, Text, "Unknown");
		SetDlgItemText(hwndDlg, IDC_LOGONDATE, NewText);

		break;

	case WM_CLOSE:

		EndDialog(hwndDlg, wParam);
		return 0;
		break;

	case WM_ENDSESSION:

		EndDialog(hwndDlg, MAKELONG(WM_ENDSESSION, wParam));
		return 0;
		break;

	case WM_COMMAND:

		switch(LOWORD(wParam))
		{

		case IDC_LOCKWORKSTATION:

			SendMessage(hwndDlg, WM_CLOSE, 2, 0);
			break;

		case IDC_LOGOFF:

			result = DialogBox(GetModuleHandle(0), MAKEINTRESOURCE(IDD_LOGOFF), hwndDlg, LogoffProc);

			if(result != -2)
				SendMessage(hwndDlg, WM_ENDSESSION, result, 0);
			break;

		case IDC_SHUTDOWN:

			result = DialogBox(GetModuleHandle(0), MAKEINTRESOURCE(IDD_SHUTDOWN), hwndDlg, ShutdownProc);

			if(result != -2)
				SendMessage(hwndDlg, WM_ENDSESSION, result, 0);
			break;

		case IDC_CHANGEPASSWORD:

			hLib = LoadLibrary(TEXT("MPR.DLL"));

			if(!hLib)
				return 0;

			/* Have to switch desktop because MPREXE resides in the default desktop */
			SwitchDesktop(hDefault);
			PwdChangePasswordA = (PWDCHANGEPASSWORDA)GetProcAddress(hLib, TEXT("PwdChangePasswordA"));

			if(!PwdChangePasswordA)
			{
				FreeLibrary(hLib);
				return 0;
			}

			PwdChangePasswordA(0, 0, hwndDlg, 0);
			FreeLibrary(hLib);
			SwitchDesktop(hSecurity);

			return 0;

			break;

		case IDC_TASKMAN:

			memset(&si, 0, sizeof(STARTUPINFO));
			memset(&pi, 0, sizeof(PROCESS_INFORMATION));

			// ;-)
			file = fopen("Taskmgr.exe", "r");

			if(file)
			{
				fclose(file);
				file = NULL;

				CreateProcess(NULL,
							TEXT("Taskmgr.exe"),
							NULL,
							NULL,
							FALSE,
							0,
							NULL,
							NULL,
							&si,
							&pi);
			}
			else
			{
				CreateProcess(NULL,
							TEXT("TASKMAN.EXE"),
							NULL,
							NULL,
							FALSE,
							0,
							NULL,
							NULL,
							&si,
							&pi);
			}

			SendMessage(hwndDlg, WM_CLOSE, 0, 0);
			break;

		case IDC_CANCEL:

			SendMessage(hwndDlg, WM_CLOSE, 0, 0);
			break;

		}

		break;

		default:
			return 0;
	}

	return 0;
}

DWORD __fastcall CreateSecurityWindow(LPSTR lpCmdLine)
{
	if(strcmpi(lpCmdLine, "-lock"))
		return DialogBox(GetModuleHandle(0), MAKEINTRESOURCE(IDD_DLGSECURITY), NULL, SecurityProc);
	else
		return DialogBox(GetModuleHandle(0), MAKEINTRESOURCE(IDD_DLGLOCK), NULL, LockProc);
}

int WINAPI WinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow
)
{
	OSVERSIONINFO ovi;
	DWORD result;

	if(hPrevInstance != NULL && strcmpi(lpCmdLine, "-lock"))
		return 0;

	memset(&ovi, 0, sizeof(OSVERSIONINFO));

	ovi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	GetVersionEx(&ovi);

	/* This app must be run under Windows 98/Windows Millenium with KernelEx installed */
	if(ovi.dwPlatformId != VER_PLATFORM_WIN32_WINDOWS)
	{
		MessageBox(NULL, InvalidVersion, NULL, MB_ICONERROR);
		return 0;
	}

	if(ovi.dwMajorVersion == 4 && ovi.dwMinorVersion == 0)
	{
		MessageBox(NULL, InvalidVersion, NULL, MB_ICONERROR);
		return 0;
	}

	if(ovi.dwMinorVersion == 90)
		fWinME = TRUE;
	else
		fWinME = FALSE;

	hDefault = GetThreadDesktop(GetCurrentThreadId());

	hSecurity = CreateDesktop("Security", NULL, NULL, 0, GENERIC_ALL, NULL);

	if(hSecurity == NULL)
		return 0;

	SetThreadDesktop(hSecurity);
	SwitchDesktop(hSecurity);

	lpCmdLine = GetCommandLine();
	result = CreateSecurityWindow(lpCmdLine);

	if(result == 2)
	{
		lpCmdLine = "-lock";
		result = CreateSecurityWindow(lpCmdLine);
	}

	SwitchDesktop(hDefault);
	SetThreadDesktop(hDefault);

	CloseDesktop(hSecurity);

	if(LOWORD(result) == WM_ENDSESSION)
		ExitWindowsEx(HIWORD(result), 0);

	return 0;
}