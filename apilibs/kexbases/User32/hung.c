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

#include <stdio.h>
#include "desktop.h"
#include "hung.h"
#include "../kernel32/_kernel32_apilist.h"
#include "_user32_apilist.h"

DWORD HangManagerThreadId = 0;

typedef struct _HUNGINFO
{
	DWORD dwProcessId;
	DWORD dwThreadId;
	PPDB98 Process;
	PTDB98 Thread;
	HWND hwndReplace;
	PWND pwndReplace;
	HWND hwndNew;
	HANDLE hEvent;
	int x;
	int y;
	int cx;
	int cy;
	DWORD dwStyle;
	PCHAR Title;
} HUNGINFO, *PHUNGINFO;

INT_PTR CALLBACK EndTaskDialogProc(HWND hwndDlg,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam
)
{
	HWND hWnd = NULL;
	CHAR buffer[255];
	CHAR Text[255];
	CHAR NewText[255];
	PCHAR Temp = NULL;
	DWORD dwVersion = 0;
	PCHAR pch = NULL;

	switch(uMsg)
	{
		case WM_INITDIALOG:
			hWnd = (HWND)lParam;

			GetWindowText(hWnd, buffer, sizeof(buffer));
			SetWindowText(hwndDlg, buffer);

			dwVersion = kexGetVersion();

			Temp = "Unknown";

			if(dwVersion == 0xc0000a04)
				Temp = "98";
			else if(dwVersion == 0xc0005a04)
				Temp = "ME";

			memset(Text, 0, sizeof(Text));
			memset(NewText, 0, sizeof(NewText));
			GetDlgItemText(hwndDlg, IDT_STATIC2, Text, sizeof(Text));
			wsprintf(NewText, Text, WaitToKillAppTimeout / 1000);
			SetDlgItemText(hwndDlg, IDT_STATIC2, NewText);

			/* The end task dialog should be the top window */
			SetWindowPos_fix(hwndDlg, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);

			return TRUE;

		case WM_SYSCOMMAND:
			switch(wParam)
			{
				case SC_CLOSE:
					wParam = IDCANCEL;
					break;
			}
			break;

		case WM_CLOSE:
			wParam = IDCANCEL;
		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case IDC_WAITTASK:
					EndDialog(hwndDlg, ET_WAIT);
					return TRUE;

				case IDC_ENDTASK:
					EndDialog(hwndDlg, ET_ENDTASK);
					return TRUE;

				case IDCANCEL:
					EndDialog(hwndDlg, ET_CANCEL);
					return TRUE;
			}
			break;
	}

	return FALSE;
}

ULONG FASTCALL ShowEndTaskDialog(HWND hwnd, BOOL fEndSession)
{
	ULONG result = FALSE;
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

	result = DialogBoxParam(hModule,
						fEndSession ? MAKEINTRESOURCE(IDD_ENDTASK) : MAKEINTRESOURCE(IDD_ENDTASK2),
						NULL,
						EndTaskDialogProc,
						(LPARAM)hwnd);

	if(fLoaded)
		FreeLibrary(hModule);

	return result;
}

INT_PTR CALLBACK GhostWindowProc(HWND hwnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam
)
{
	ULONG result;
	PHUNGINFO phi;
	DWORD dwSmto;
	DWORD dwMsgResult = 0;

	phi = (PHUNGINFO)GetWindowLong(hwnd, GWL_USERDATA);

	if(phi != NULL)
	{
		dwSmto = SendMessageTimeout(phi->hwndReplace, WM_NULL, 0, 0, SMTO_ABORTIFHUNG, 1000, &dwMsgResult);

		if(dwSmto)
		{
			SetWindowLong(hwnd, GWL_USERDATA, 0);
			ShowWindowAsync_fix(phi->hwndReplace, SW_SHOW);
			EndDialog(hwnd, 0);
			return 1;
		}
	}

	switch(uMsg)
	{
		case WM_INITDIALOG:
			phi = (PHUNGINFO)lParam;

			SetWindowLong(hwnd, GWL_USERDATA, (LONG)phi);

			phi->hwndNew = hwnd;

			phi->pwndReplace->style &= ~WS_VISIBLE;

			/* Because standard window API doesn't hide hung windows, we create a new region */
			IntCompleteRedrawWindow(phi->pwndReplace);

			SetEvent(phi->hEvent);

			SetCursor(LoadCursor(NULL, IDC_WAIT));

			SetWindowText(hwnd, phi->Title);

			SetWindowPos_fix(hwnd,
					NULL,
					phi->x, phi->y, phi->cx, phi->cy,
					SWP_NOZORDER | SWP_SHOWWINDOW);

			TRACE("Ghost window 0x%X created\n", hwnd);
			return TRUE;

		case WM_CLOSE:
			wParam = SC_CLOSE;
		case WM_SYSCOMMAND:
			switch(wParam)
			{
				case SC_CLOSE:
					result = ShowEndTaskDialog(hwnd, FALSE);

					if(result == ET_ENDTASK)
						ExitProcess(0);
					else
						return 0;

					return 1;

				case SC_MAXIMIZE:
					return 1;
			}
			break;
	}

	return 0;
}

DWORD WINAPI GhostWindowThread(PVOID lpParameter)
{
	PHUNGINFO phi = (PHUNGINFO)lpParameter;
	INT_PTR result;
	PCHAR Title;
	CHAR NewTitle[255];
	WINDOWINFO wi;
	RECT rc;
	DWORD dwPreviousPriority = GetPriorityClass(GetCurrentProcess());
	HGLOBAL hGlobal = GlobalAlloc(GMEM_ZEROINIT, sizeof(DLGTEMPLATE)*2);
	LPDLGTEMPLATE pDlg = (LPDLGTEMPLATE)GlobalLock(hGlobal);
	DWORD dwStyle = 0;

	TRACE_OUT("Creating ghost window\n");

	SetThreadDesktop(GetThreadDesktop(phi->dwThreadId));

	SetPriorityClass(GetCurrentProcess(), IDLE_PRIORITY_CLASS);

	Title = phi->Title;

	GetWindowInfo(phi->hwndReplace, &wi);

	if(Title == NULL || phi->pwndReplace->windowTextOffset == 0)
		Title = "<unknown>";

	wsprintf(NewTitle, "%s (Unresponsive)", Title);

	GetWindowRect(phi->hwndReplace, &rc);

	if(wi.dwStyle & WS_MINIMIZE)
		dwStyle |= WS_MINIMIZE;

	if(wi.dwStyle & WS_MAXIMIZE)
		dwStyle |= WS_MAXIMIZE;

	pDlg->cdit = 0;
	pDlg->cx = (SHORT)(rc.right - rc.left);
	pDlg->cy = (SHORT)(rc.bottom - rc.top);
	pDlg->dwExtendedStyle = 0;
	pDlg->style = DS_MODALFRAME | WS_POPUP | WS_SYSMENU | WS_VISIBLE | WS_CAPTION | WS_MINIMIZEBOX | dwStyle;
	pDlg->x = (SHORT)rc.left;
	pDlg->y = (SHORT)rc.top;

	phi->cx = rc.right - rc.left;
	phi->cy = rc.bottom - rc.top;
	phi->x = rc.left;
	phi->y = rc.top;
	phi->dwStyle = wi.dwStyle;
	phi->Title = NewTitle;

	GlobalUnlock(hGlobal);

	result = DialogBoxIndirectParam(GetModuleHandle(0),
						(LPDLGTEMPLATE)hGlobal,
						NULL,
						GhostWindowProc,
						(LPARAM)phi);

	GlobalFree(hGlobal);

	TRACE_OUT("Returning from ghost thread (result=%d)\n", result);

	SetPriorityClass(GetCurrentProcess(), dwPreviousPriority);

	if(phi->Thread->Win32Thread != NULL)
		phi->Thread->Win32Thread->fGhosted = FALSE;

	if(phi->pwndReplace != NULL && phi->pwndReplace->style & WS_INTERNAL_HUNGWASVISIBLE)
	{
		phi->pwndReplace->style &= ~WS_INTERNAL_HUNGWASVISIBLE;
		phi->pwndReplace->style |= WS_VISIBLE;
		IntCompleteRedrawWindow(phi->pwndReplace);
	}

	if(phi->Title != NULL)
		kexFreeObject(phi->Title);

	ExitThread(0);
	return 0;
}

/*******************************************************************************************
* Description : Create a ghost window (exclusive for hungs app) that will create a window
* over the specified hwnd
*
* Parameters : - dwProcessId : Process ID for which it will create the window
*              - hwndReplace : Window to replace
*
* return value : Return the ghost window that replaced hwndReplace
********************************************************************************************/
HWND FASTCALL CreateGhostWindow(DWORD dwProcessId, DWORD dwThreadId, HWND hwndReplace)
{
	HANDLE hProcess = NULL;
	PWND pwndReplace = NULL;
	PPDB98 Process = (PPDB98)kexGetProcess(dwProcessId);
	PTDB98 Thread = (PTDB98)kexGetThread(dwThreadId);
	PHUNGINFO phunginf = (PHUNGINFO)kexAllocObject(sizeof(HUNGINFO));
	HWND hwndNew = NULL;
	HANDLE hEvent = NULL;
	PPROCESSINFO ppi = Process->Win32Process;
	PTHREADINFO pti = Thread->Win32Thread;
	DWORD wndTextLength = 0;

	TRACE_OUT("CreateGhostWindow\n");

	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);

	pwndReplace = HWNDtoPWND(hwndReplace);

	if(hProcess == NULL || Process == NULL || Thread == NULL || pwndReplace == NULL || phunginf == NULL ||
		Process == ppdbKernelProcess || ppi == NULL || pti == NULL || Process == ppdbKernelProcess || phunginf == NULL)
	{
		TRACE_OUT("CreateGhostWindow failed\n");
		return NULL;
	}

	if(pwndReplace->style & WS_CHILD || !(pwndReplace->style & WS_VISIBLE))
	{
		TRACE_OUT("Cannot ghost a child window !\n");
		return NULL;
	}

	if(ppi->WindowsGhosting == FALSE)
	{
		TRACE("Cannot create the ghost window because process %p disabled it !\n", dwProcessId);
		return NULL;
	}

	if(pti->fGhosted)
	{
		TRACE("Cannot create the ghost window because thread %p is already ghosted !\n", dwThreadId);
		return NULL;
	}

	pti->fGhosted = TRUE;

	phunginf->hwndReplace = hwndReplace;
	phunginf->pwndReplace = pwndReplace;
	phunginf->dwProcessId = dwProcessId;
	phunginf->dwThreadId = dwThreadId;
	phunginf->Process = Process;
	phunginf->Thread = Thread;

	wndTextLength = GetWindowTextLength(hwndReplace);

	if(wndTextLength != 0)
	{
		wndTextLength += 2;

		phunginf->Title = (PCHAR)kexAllocObject(wndTextLength);
		GetWindowText(hwndReplace, phunginf->Title, wndTextLength);
	}

	hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	DuplicateHandle(GetCurrentProcess(),
				hEvent,
				hProcess,
				&phunginf->hEvent,
				0,
				FALSE,
				DUPLICATE_SAME_ACCESS);

	if(!CreateRemoteThread_new(hProcess, NULL, 0, GhostWindowThread, (PVOID)phunginf, 0, NULL))
	{
		TRACE_OUT("Failed to create the ghost thread for process %p", dwProcessId);
		pti->fGhosted = FALSE;
		return NULL;
	}

	/* Wait for the thread to create and return the new hwnd */
	WaitForSingleObject(phunginf->hEvent, 2000);

	return phunginf->hwndNew;
}

BOOL CALLBACK EnumHungWindowsProc(HWND hwnd, LPARAM lParam)
{
	HANDLE hThread;
	DWORD dwProcessId = 0;
	DWORD dwThreadId = GetWindowThreadProcessId(hwnd, &dwProcessId);
	PWND pwnd = HWNDtoPWND(hwnd);
	//DWORD dwMsgResult = 0;
	//DWORD dwSmto = 0;

	if(pwnd == NULL)
		return TRUE;

	if(dwProcessId == gpidKernelProcess)
		return TRUE;

	if(!IsWindowVisible_fix(hwnd) || (pwnd->style & WS_CHILD))
		return TRUE;

	hThread = OpenThread_new(THREAD_ALL_ACCESS, FALSE, dwThreadId);

	//dwSmto = SendMessageTimeout(hwnd, WM_NULL, 0, 0, SMTO_ABORTIFHUNG, 2000, &dwMsgResult);

	/* Check if the thread is hung or suspended (WFSO returns WAIT_ABANDONNED if suspended) */
	if(IsHungThread_pfn(dwThreadId) || WaitForSingleObject(hThread, 0) == WAIT_ABANDONED)
		CreateGhostWindow(dwProcessId, dwThreadId, hwnd);

	CloseHandle(hThread);

	return TRUE;
}

DWORD WINAPI HangManagerThread(PVOID lpParameter)
{
	MSG msg;

	HangManagerThreadId = GetCurrentThreadId();

	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_LOWEST);

	if(hwndStatusDlg != NULL)
	{
		while(GetForegroundWindow() == hwndStatusDlg)
		{
			Sleep(1);
			PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE);
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		DestroyStatusDialog();
	}

	while(1)
	{
		Sleep(100);
		PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE);
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		EnumWindows(EnumHungWindowsProc, 0);
	}
}

