/*
 *  KernelEx
 *  Copyright (C) 2009, Tihiy
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
#include "desktop.h"
#include "resource.h"

HWND hwndStatusDlg = NULL;
HWND hwndStatusDlgText = NULL;

typedef struct _STATUSDLGSTRUCT
{
	HANDLE hEvent;
	LPSTR lpCaption;
	LPSTR lpText;
} STATUSDLGSTRUCT, *PSTATUSDLGSTRUCT;

BOOL FASTCALL CenterDialog(HWND hWnd)
{
    int screenWidth;
    int screenHeight;
    int x;
    int y;
    int Width;
    int Height;
    RECT rc;

	if(hWnd == NULL)
		return FALSE;

	if(!PostMessage(hWnd, WM_NULL, 0, 0))
		return FALSE;

	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	GetWindowRect(hWnd, &rc);

	Width = rc.right - rc.left;
	Height = rc.bottom - rc.top;
	x = (screenWidth - Width)/2;
	y = (screenHeight - Height)/3;

	SetWindowPos(hWnd,
				NULL,
				x, y, 0, 0,
				SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);

	return TRUE;
}

INT_PTR CALLBACK StatusDialogProc(HWND hwndDlg,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam
)
{
	PSTATUSDLGSTRUCT sDlg;

	switch(uMsg)
	{
		case WM_INITDIALOG:

			sDlg = (PSTATUSDLGSTRUCT)lParam;

			hwndStatusDlg = hwndDlg;
			hwndStatusDlgText = GetDlgItem(hwndDlg, IDT_TEXT);

			SetWindowText(hwndStatusDlg, sDlg->lpCaption);
			SetWindowText(hwndStatusDlgText, sDlg->lpText);

			CenterDialog(hwndDlg);
			break;

		case WM_PAINT:
			break;

		case WM_USER+20:
			PostQuitMessage(0);
			DestroyWindow(hwndDlg);
			return 1;
	}

	return 0;
}

DWORD WINAPI CreateStatusDlgThread(PVOID lParam)
{
	MSG msg;
	BOOL result;
	PSTATUSDLGSTRUCT sDlg = (PSTATUSDLGSTRUCT)lParam;
	CHAR Directory[255];
	CHAR Path[255];
	HMODULE hModule = NULL;
	BOOL fLoaded = FALSE;

	memset(Directory, 0, sizeof(Directory));
	memset(Path, 0, sizeof(Path));

	kexGetKernelExDirectory(Directory, sizeof(Directory));

	wsprintf(Path, "%sKEXBASES.DLL", Directory);

	hModule = GetModuleHandle(Path);

	if(hModule == NULL)
	{
		fLoaded = TRUE;
		hModule = LoadLibrary(Path);
	}

	hwndStatusDlg = CreateDialogParam(hModule,
						MAKEINTRESOURCE(IDD_STATUSDLG),
						NULL,
						StatusDialogProc,
						(LPARAM)sDlg);

	if(fLoaded)
		FreeLibrary(hModule);

	SetEvent(sDlg->hEvent);

	if(hwndStatusDlg == NULL)
		return 0;

	while((result = GetMessage( &msg, NULL, 0, 0 )) != 0)
	{
		if (result == -1)
		{
			return 0;
		}
		else
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return 0;
}

DWORD FASTCALL CreateStatusDialog(LPCSTR lpCaption, LPCSTR lpText)
{
	HANDLE hEvent;
	HANDLE hThread;
	PSTATUSDLGSTRUCT sDlg = (PSTATUSDLGSTRUCT)kexAllocObject(sizeof(sDlg));
	DWORD dwThreadId = 0;
	HANDLE hGlobalEvent;

	hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	hGlobalEvent = ConvertToGlobalHandle(hEvent);

	if(hEvent == NULL || hGlobalEvent == NULL)
		return 0;

	sDlg->hEvent = hGlobalEvent;
	sDlg->lpCaption = (LPSTR)kexAllocObject(strlen(lpCaption));
	sDlg->lpText = (LPSTR)kexAllocObject(strlen(lpText));

	strcpy(sDlg->lpCaption, lpCaption);
	strcpy(sDlg->lpText, lpText);

	hThread = CreateKernelThread(NULL,
						0,
						CreateStatusDlgThread,
						(LPVOID)sDlg,
						0,
						&dwThreadId);

	if(hThread != NULL)
		WaitForSingleObject(hGlobalEvent, INFINITE);

	CloseHandle(hEvent);
	CloseHandle(hGlobalEvent);
	CloseHandle(hThread);

	if(sDlg != NULL)
	{
		kexFreeObject(sDlg->lpCaption);
		kexFreeObject(sDlg->lpText);
		kexFreeObject(sDlg);
	}

	return dwThreadId;
}

VOID FASTCALL DestroyStatusDialog(void)
{
	if(hwndStatusDlg == NULL)
		return;

	SendMessage(hwndStatusDlg, WM_USER+20, 0, 0);

	hwndStatusDlg = NULL;
	hwndStatusDlgText = NULL;
}

VOID FASTCALL SetStatusDialogCaption(LPCSTR lpCaption)
{
	if(hwndStatusDlg == NULL)
		return;

	SetWindowText(hwndStatusDlg, lpCaption);
}

VOID FASTCALL SetStatusDialogText(LPCSTR lpText)
{
	if(hwndStatusDlg == NULL)
		return;

	SetWindowText(hwndStatusDlgText, lpText);
}

/* MAKE_EXPORT CreateDialogIndirectParamA_fix=CreateDialogIndirectParamA */
HWND WINAPI CreateDialogIndirectParamA_fix(HINSTANCE hInstance,
    LPCDLGTEMPLATE lpTemplate,
    HWND hWndParent,
    DLGPROC lpDialogFunc,
    LPARAM lParamInit
)
{
	PTHREADINFO pti = get_tdb()->Win32Thread;
	HWND hWnd = NULL;
	PWND pWnd = NULL;

	hWnd = CreateDialogIndirectParamA(hInstance,
								lpTemplate,
								hWndParent,
								lpDialogFunc,
								lParamInit);

	GrabWin16Lock();

	if(hWnd == NULL)
		goto _ret;

	pWnd = HWNDtoPWND(hWnd);

	if(pWnd == NULL)
		goto _ret;

	if(pti == NULL)
		goto _ret;

	if(pti->rpdesk != gpdeskInputDesktop && pWnd->style & WS_VISIBLE)
	{
		pWnd->style |= WS_INTERNAL_WASVISIBLE;
		ReleaseWin16Lock();
		ShowWindowAsync(hWnd, SW_HIDE);
		GrabWin16Lock();
	}

_ret:
	ReleaseWin16Lock();
	return hWnd;
}

/* MAKE_EXPORT CreateDialogParamA_fix=CreateDialogParamA */
HWND WINAPI CreateDialogParamA_fix(HINSTANCE hInstance,
	LPCTSTR lpTemplateName,
	HWND hWndParent,
	DLGPROC lpDialogFunc,
	LPARAM dwInitParam
)
{
	PTHREADINFO pti = get_tdb()->Win32Thread;
	HWND hWnd = NULL;
	PWND pWnd = NULL;

	hWnd = CreateDialogParamA(hInstance,
							lpTemplateName,
							hWndParent,
							lpDialogFunc,
							dwInitParam);

	GrabWin16Lock();

	if(hWnd == NULL)
		goto _ret;

	pWnd = HWNDtoPWND(hWnd);

	if(pWnd == NULL)
		goto _ret;

	if(pti == NULL)
		goto _ret;

	if(pti->rpdesk != gpdeskInputDesktop && pWnd->style & WS_VISIBLE)
	{
		pWnd->style |= WS_INTERNAL_WASVISIBLE;
		ReleaseWin16Lock();
		ShowWindowAsync(hWnd, SW_HIDE);
		GrabWin16Lock();
	}

_ret:
	ReleaseWin16Lock();
	return hWnd;
}

/* Here we have fix for retarded situation.
 * 9x stucks trying to get next control in dialog to tab
 * when there is only one control on dialog, and it has tab-able
 *  child windows (WS_EX_CONTROLPARENT), and if it's disabled or hidden,
 * user.exe crashes with stack overflow, silently or soundly.
 * More proper fix would be to scan whole dialog and enable/show at least
 * one such window, since GetNextDlgTabItem also happens internally.
 * However, likely it won't be this function and our case works for
 * Winamp ML.
 */


/* MAKE_EXPORT GetNextDlgTabItem_fix=GetNextDlgTabItem */
HWND WINAPI GetNextDlgTabItem_fix(
	HWND hDlg,       // handle to dialog box
	HWND hCtl,       // handle to known control
	BOOL bPrevious   // direction flag
)
{
	HWND hTestCtl = hCtl;
	if (!hTestCtl) hTestCtl = GetWindow(hDlg,GW_CHILD);
	if ( hTestCtl && !GetWindow(hTestCtl,GW_HWNDNEXT)
		&& ( !IsWindowVisible(hTestCtl) || !IsWindowEnabled(hTestCtl) )
		&& ( GetWindowLong(hTestCtl,GWL_EXSTYLE) & WS_EX_CONTROLPARENT ) ) return hCtl;

	return GetNextDlgTabItem(hDlg,hCtl,bPrevious);
}
