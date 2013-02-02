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

#include "../ntdll/_ntdll_apilist.h"
#include <stdlib.h>
#include <shlwapi.h>
#include "desktop.h"

BOOL fShutdown = FALSE;
BOOL fLoggingOff = FALSE;
BOOL fAborted = FALSE;
BOOL fForceShutdown = FALSE;
DWORD dwShutdownThreadId = 0;

int WaitToKillAppTimeout = 0;
int HungAppTimeout = 0;
int LogoffTimeout = 5 * 1000; // 5 minutes

HWND hwndGlobalText;

/* RegRemapPreDefKey - Remap a predefined key (e.g: HKEY_CURRENT_USER) to a new key
   HKEY hKey: Handle to a predefined key
   HKEY hNewHKey: Handle to an open key
   return: if the function succeeds, the return value is ERROR_SUCCESS, otherwise a nonzero error code defined in Winerror.h
   the differences between RegRemapPreDefKey and RegOverridePredefKey is that the first one is global and the second is only for the caller */
typedef LONG (WINAPI *REGREMAPPREDEFKEY)(HKEY hKey, HKEY hNewHKey);

/* ByeByeGDI - Destroy the first GDI heap
   DWORD Unknown: currently unknown, but seems to point to a module */
typedef VOID (WINAPI *BYEBYEGDI)(DWORD Unknown);

typedef struct _SHUTDOWNDATA
{
	UINT uFlags;
	PWINSTATION_OBJECT WindowStation;
	DWORD Result;
	DWORD ShellProcessId;
	DWORD dwProcessId;
	DWORD dwThreadId;
	BOOL fEndServices;
	BOOL fLoggingOff;
} SHUTDOWNDATA, *PSHUTDOWNDATA;

/* - In Win9x, the shutdown is processed in the context of the caller
   which makes this one freeze, i.e the shutdown dialog box which cover
   the entire screen freeze during shutdown which is annoying
   - The shutdown process seems to only look for existing windows and not for processes
   - if the EWX_FORCE flag is specified, explorer.exe will remains
   - Windows 9x team seemed too lazy to make a good working stable ExitWindowsEx,
   instead of enumerating processes, threads and windows, they used the simple
   BroadcastSystemMessage API...
*/

BOOL CALLBACK EnumThreadWndProc(HWND hwnd, LPARAM lParam)
{
	PSHUTDOWNDATA ShutdownData = (PSHUTDOWNDATA)lParam;
	DWORD dwResult = 0;
	DWORD smt = 0;
	BOOL fHung = FALSE;

	if(IS_SYSTEM_HWND(hwnd))
		return TRUE;

	fHung = IsHungThread_pfn(ShutdownData->dwThreadId);
	smt = SendMessageTimeout(hwnd,
							WM_QUERYENDSESSION,
							0,
							ShutdownData->uFlags & EWX_LOGOFF ? ENDSESSION_LOGOFF : 0,
							SMTO_NORMAL,
							fHung ? HungAppTimeout : WaitToKillAppTimeout,
							&dwResult);

	/* Abort if the message timeout or the thread doesn't want to quit
	   and EWX_FORCE flag is not specified/the application is not hung */
	if(!smt || !dwResult)
	{
		if((ShutdownData->uFlags & EWX_FORCE) || (ShutdownData->uFlags & EWX_FORCEIFHUNG) || (ShutdownData->fEndServices))
			goto _continue;

		ShutdownData->Result = 0;
		return FALSE;
	}

_continue:
	PostMessage(hwnd, WM_ENDSESSION, TRUE, ShutdownData->uFlags & EWX_LOGOFF ? ENDSESSION_LOGOFF : 0);

	ShutdownData->Result = 1;
	return TRUE;
}

BOOL CALLBACK EnumThreadsProc(DWORD dwThreadId, PSHUTDOWNDATA ShutdownData)
{
	/* Using result data because EnumThreadWindows can fail if the thread has no windows */
	ShutdownData->Result = 1;
	ShutdownData->dwThreadId = dwThreadId;

	SetLastError(0);

	/* FIXME: Should we post a message to threads that doesn't have windows ? */
	EnumThreadWindows_nothunk(dwThreadId, EnumThreadWndProc, (LPARAM)ShutdownData);

	if(ShutdownData->Result == 0)
		return FALSE;

	if(GetLastError() != 0)
		PostThreadMessage(dwThreadId, WM_ENDSESSION, TRUE, ShutdownData->uFlags & EWX_LOGOFF ? ENDSESSION_LOGOFF : 0);

	return TRUE;
}

BOOL CALLBACK EnumProcessesProc(DWORD dwProcessId, PSHUTDOWNDATA ShutdownData)
{
	HANDLE hProcess = NULL;
	DWORD pThread[1024];
	DWORD Threads = 0;
	ULONG i = 0;
	PPDB98 Process = NULL;

	Process = (PPDB98)kexGetProcess(dwProcessId);

	if(Process == NULL)
		return TRUE;

	/* Skip services processes */
	if((Process->Flags & fServiceProcess || Process == Msg32Process) && !ShutdownData->fEndServices)
		return TRUE;

	if(!(Process->Flags & fServiceProcess) && Process != Msg32Process && ShutdownData->fEndServices)
		return TRUE;

	if(Process == MprProcess)
		return TRUE;

	ShutdownData->ShellProcessId = GetWindowProcessId(GetShellWindow_new());

	/* Don't terminate the shell now because the user could abort the shutdown process */
	if(dwProcessId == ShutdownData->ShellProcessId)
		return TRUE;

	kexEnumThreads(dwProcessId, pThread, sizeof(pThread), &Threads);

	Threads /= sizeof(DWORD);

	ShutdownData->dwProcessId = dwProcessId;

	for(i=0;i<=Threads;i++)
	{
		if(!EnumThreadsProc(pThread[i], ShutdownData))
			return FALSE;
	}

	/* The kernel process doesn't have the fServiceProcess flag */
	if(Process == pKernelProcess)
		return TRUE;

	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);

	if(!hProcess)
		return TRUE;

	TerminateProcess(hProcess, 0);
	CloseHandle(hProcess);

	return TRUE;
}

ULONG GetUserProcessesCount(PSHUTDOWNDATA sa)
{
	DWORD pProcess[1024];
	DWORD cbProcesses = 0;
	ULONG ProcessCount = 0;
	ULONG index = 0;
	PPDB98 Process = NULL;

	kexEnumProcesses(pProcess, sizeof(pProcess), &cbProcesses);

	cbProcesses /= sizeof(DWORD);

	for(index=0;index<=cbProcesses;index++)
	{
		Process = (PPDB98)kexGetProcess(pProcess[index]);

		if(Process == NULL)
			continue;

		if((Process->Flags & fServiceProcess || Process == Msg32Process) && !sa->fEndServices)
			continue;

		if(!(Process->Flags & fServiceProcess) && Process != Msg32Process && sa->fEndServices)
			continue;

		if(Process == pKernelProcess || Process == MprProcess)
			continue;

		/* Skip the shell process because he must be the last process running
		   in the session */
		if(pProcess[index] == sa->ShellProcessId)
			continue;

		ProcessCount++;
	}

	return ProcessCount;
}

INT_PTR CALLBACK DialogProc(HWND hwndDlg,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam
)
{
    int screenWidth;
    int screenHeight;
    int x;
    int y;
    int Width;
    int Height;
    RECT rc;
	HFONT hFont;

	switch(uMsg)
	{
	case WM_INITDIALOG:
		SetWindowText(hwndDlg, "Shutdown in Progress");

		hFont = CreateFont(8,
						0,
						0,
						0,
						FW_DONTCARE,
						FALSE,
						FALSE,
						FALSE,
						ANSI_CHARSET, 
						OUT_TT_PRECIS,
						CLIP_DEFAULT_PRECIS,
						DEFAULT_QUALITY, 
						DEFAULT_PITCH | FF_DONTCARE, "MS Shell Dlg");

		hwndGlobalText = CreateWindowEx(WS_EX_NOPARENTNOTIFY,
					"STATIC",
					"Please wait while the system writes unsaved data to the disk.",
					SS_LEFT | WS_CHILD | WS_VISIBLE | WS_GROUP,
					11*2,
					10*2,
					112*3/2,
					18*2,
					hwndDlg,
					NULL,
					GetModuleHandle(0),
					NULL);

		SendMessage(hwndGlobalText, WM_SETFONT, (WPARAM)hFont, TRUE);

		screenWidth = GetSystemMetrics(SM_CXSCREEN);
		screenHeight = GetSystemMetrics(SM_CYSCREEN);

		GetWindowRect(hwndDlg, &rc);

		Width = rc.right - rc.left;
		Height = rc.bottom - rc.top;
		x = (screenWidth - Width)/2;
		y = (screenHeight - Height)/3;

		SetWindowPos(hwndDlg,
					NULL,
					x, y, 0, 0,
					SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
		break;

	case WM_PAINT:
		break;

	}

	return 0;
}

DWORD WINAPI CreateShutdownWindow(PVOID lParam)
{
	HGLOBAL hGlobal = GlobalAlloc(GMEM_ZEROINIT, sizeof(DLGTEMPLATE)*2);
	LPDLGTEMPLATE pDlg = (LPDLGTEMPLATE)GlobalLock(hGlobal);
	MSG msg;
	BOOL result;
	HANDLE hEvent = lParam;

	pDlg->cdit = 0;
	pDlg->cx = 105;
	pDlg->cy = 34;
	pDlg->dwExtendedStyle = 0;
	pDlg->style = DS_MODALFRAME | WS_POPUP | WS_VISIBLE | WS_CAPTION;
	pDlg->x = 0;
	pDlg->y = 0;

	GlobalUnlock(hGlobal);

	CreateDialogIndirect(GetModuleHandle(0),
						(LPDLGTEMPLATE)hGlobal,
						NULL,
						DialogProc);

	GlobalFree(hGlobal);

	SetEvent(hEvent);

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

/* Sort processes by their shutdown level or ID */
int ProcessCompare(const void *elem1, const void *elem2)
{
	DWORD ProcessId1 = *(DWORD*)elem1;
	DWORD ProcessId2 = *(DWORD*)elem2;

	PPROCESSINFO ppi1 = ((PPDB98)kexGetProcess(ProcessId1))->Win32Process;
	PPROCESSINFO ppi2 = ((PPDB98)kexGetProcess(ProcessId2))->Win32Process;

	if(ppi1 == NULL || ppi2 == NULL)
		return 0;

	if(ppi1->ShutdownLevel < ppi2->ShutdownLevel)
		return +1;

	if(ppi1->ShutdownLevel > ppi2->ShutdownLevel)
		return -1;

	if(ppi1->UniqueProcessId < ppi2->UniqueProcessId)
		return +1;

	if(ppi1->UniqueProcessId > ppi2->UniqueProcessId)
		return -1;

	return 0;
}

VOID __fastcall LogoffCurrentUser(PSHUTDOWNDATA ShutdownData)
{
	DWORD pProcess[1024];
	DWORD Processes = 0;
	ULONG i;
	DWORD StartTickCount = GetTickCount();

	__try
	{
		/* Loop until there are no processes in the current user and all applications
		   accepted to terminate */
		do
		{
			memset(&pProcess, 0, sizeof(pProcess));

			kexEnumProcesses(pProcess, sizeof(pProcess), &Processes);

			Processes /= sizeof(DWORD);

			qsort(pProcess, Processes, sizeof(DWORD), ProcessCompare);

			for(i=0;i<=Processes;i++)
			{
				if(GetTickCount() - StartTickCount > LogoffTimeout)
				{
					/* Force processes to terminate if the logoff time has raised the maximum logoff timeout */

					if(ShutdownData->fLoggingOff)
					{
						if(!(ShutdownData->uFlags & EWX_FORCE))
							ShutdownData->uFlags |= EWX_FORCE;
					}
					else
					{
						fForceShutdown = TRUE;
						return;
					}
				}

				Sleep(100);

				if(pProcess[i] == 0)
					continue;

				if(!EnumProcessesProc(pProcess[i], ShutdownData))
				{
					fAborted = TRUE;
					break;
				}
				pProcess[i] = 0;
			}

			Processes = 0;
		} while(GetUserProcessesCount(ShutdownData) != 0 && !fAborted);
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
	}
}

DWORD WINAPI ShutdownThread(PVOID lParam)
{
	MSG msg;
	ULONG i = 0;
	SHUTDOWNDATA sa;
	HANDLE hProcess = NULL;
	HKEY hKey = NULL;
	DWORD tid = 0;
	HANDLE hThread = NULL;
	HANDLE hEvent = NULL;
	REGREMAPPREDEFKEY RegRemapPreDefKey = (REGREMAPPREDEFKEY)GetProcAddress(GetModuleHandle("ADVAPI32.DLL"), "RegRemapPreDefKey");
	BYEBYEGDI ByeByeGDI = (BYEBYEGDI)GetProcAddress(GetModuleHandle("GDI32.DLL"), "ByeByeGDI");

	dwShutdownThreadId = GetCurrentThreadId();

	while(1)
	{
		GetMessage(&msg, NULL, 0, 0);

		/* Make sure the message is WM_QUERYENDSESSION */
		if(msg.message != WM_QUERYENDSESSION)
			continue;

		if(IsBadReadPtr(MprProcess, sizeof(PDB98)) || MprProcess->Flags & INVALID_FLAGS)
		{
			TRACE_OUT("Cannot logoff/shutdown because MPREXE process is terminated !!\n");
			continue;
		}

		fShutdown = TRUE;

		RegOpenKeyEx(HKEY_CURRENT_USER,  "Control Panel\\Desktop", 0, KEY_ALL_ACCESS, &hKey);

		if(hKey != NULL)
		{
			CHAR KeyValue[7];
			LONG Result;
			DWORD KeySize;

			__try
			{
				/* Look for WaitToKillAppTimeout */
				Result = RegQueryValueEx(hKey,
										"WaitToKillAppTimeout",
										NULL, NULL,
										(LPBYTE)KeyValue,
										&KeySize);

				if(Result) /* Found */
					WaitToKillAppTimeout = StrToInt(KeyValue);

				/* Now look for HungAppTimeout */
				Result = RegQueryValueEx(hKey,
										"HungAppTimeout",
										NULL, NULL,
										(LPBYTE)KeyValue,
										&KeySize);

				if(Result)
					HungAppTimeout = StrToInt(KeyValue);

				CloseHandle(hKey);
			}
			__except(EXCEPTION_EXECUTE_HANDLER)
			{
				WaitToKillAppTimeout = 0;
				HungAppTimeout = 0;
			}
		}

		if(WaitToKillAppTimeout == 0)
			WaitToKillAppTimeout = 20000;

		if(HungAppTimeout == 0)
			HungAppTimeout = 5000;

		sa.uFlags = msg.wParam;
		sa.WindowStation = NULL;
		sa.Result = 1;
		sa.dwProcessId = 0;
		sa.dwThreadId = 0;
		sa.fEndServices = FALSE;

		fLoggingOff = (sa.uFlags == EWX_LOGOFF || sa.uFlags == EWX_FORCEIFHUNG || sa.uFlags == EWX_FORCE);

		sa.fLoggingOff = fLoggingOff;

		LogoffCurrentUser(&sa);

		if(fAborted)
			goto finished;

		if(sa.ShellProcessId != 0 && !fForceShutdown)
		{
			/* Now terminate the shell process */
			hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, sa.ShellProcessId);

			if(hProcess != NULL)
			{
				TerminateProcess(hProcess, 0);
				CloseHandle(hProcess);
			}
		}

		Sleep(750);

		if(!fLoggingOff)
		{
			HKEY hKey = NULL;

			SystemParametersInfo(SPI_SETSCREENSAVERRUNNING, TRUE, 0, 0);

			hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
			hThread = CreateThread(NULL, 0, CreateShutdownWindow, hEvent, 0, &tid);
			WaitForSingleObject(hEvent, INFINITE);
			SetWindowText(hwndGlobalText, "Please wait while the system is logging off.");
			CloseHandle(hEvent);
			CloseHandle(hThread);

			Sleep(50);

			RegOpenKey(HKEY_USERS, ".DEFAULT", &hKey);
			if(hKey != NULL)
			{
				RegRemapPreDefKey(HKEY_CURRENT_USER, hKey);
				CloseHandle(hKey);
			}
			else
			{
				DWORD LastErr = GetLastError();
				char buffer[255];
				LPVOID lpString = NULL;

				kexErrorCodeToString(LastErr, (LPSTR)&lpString);
				wsprintf(buffer, "%s (Error %d)", lpString, LastErr);
				Win32RaiseHardError(buffer, "Shut down", MB_OK, TRUE);
			}

			RegOpenKey(HKEY_LOCAL_MACHINE, "System\\CurrentControlSet\\Control", &hKey);

			if(hKey != NULL)
			{
				RegDeleteValue(hKey, "Current User");
				CloseHandle(hKey);
			}

			SetWindowText(hwndGlobalText, "Please wait while the system is terminating services processes.");
			sa.fEndServices = TRUE;
			LogoffCurrentUser(&sa);

			if(HardErrorThreadId != 0)
			{
				HANDLE hThread;

				hThread = OpenThread_new(THREAD_ALL_ACCESS, FALSE, HardErrorThreadId);

				if(hThread != NULL)
				{
					TerminateThread(hThread, 0);
					CloseHandle(hThread);
				}
			}
		}

		if(hwndGlobalText != NULL)
			SetWindowText(hwndGlobalText, "Please wait while the system is flushing the registry.");

		RegFlushKey(HKEY_CLASSES_ROOT);
		RegFlushKey(HKEY_CURRENT_CONFIG);
		RegFlushKey(HKEY_CURRENT_USER);
		RegFlushKey(HKEY_LOCAL_MACHINE);
		RegFlushKey(HKEY_PERFORMANCE_DATA);
		RegFlushKey(HKEY_USERS);
		RegFlushKey(HKEY_DYN_DATA);

		if(hwndGlobalText != NULL)
			SetWindowText(hwndGlobalText, "Please wait while the system is shutting down.");

		Sleep(50);

		if(sa.uFlags & EWX_REBOOT)
		{
			ByeByeGDI(0);
			ZwShutdownSystem(ShutdownReboot);
			ExitWindowsEx(EWX_REBOOT | EWX_FORCE, 0);
		}
		else if(sa.uFlags & EWX_SHUTDOWN)
		{
			ZwShutdownSystem(ShutdownPowerOff);
			ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE, 0);
		}
		else if(sa.uFlags & EWX_POWEROFF)
		{
			ZwShutdownSystem(ShutdownPowerOff);
			ExitWindowsEx(EWX_POWEROFF | EWX_FORCE, 0);
		}


		/* FIXME: Instead of calling ExitWindowsEx, send a logoff message to MPREXE */
		if(fLoggingOff)
			ExitWindowsEx(EWX_LOGOFF | EWX_FORCE, 0);

finished:
		fAborted = FALSE;
		fShutdown = FALSE;
		fLoggingOff = FALSE;
		fForceShutdown = FALSE;
		hwndGlobalText = NULL;

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

/* MAKE_EXPORT ExitWindowsEx_fix=ExitWindowsEx */
BOOL WINAPI ExitWindowsEx_fix(UINT uFlags, DWORD dwReserved)
{
	PPROCESSINFO ppi = NULL;
	PWINSTATION_OBJECT WindowStation = NULL;

	if(fShutdown)
		return TRUE;

	/* Check if the flags does not exceed the maximum shutdown type */
	if(uFlags & ~(EWX_LOGOFF | EWX_SHUTDOWN | EWX_REBOOT | EWX_FORCE | EWX_POWEROFF | EWX_FORCEIFHUNG))
		return FALSE;

	ppi = get_pdb()->Win32Process;

	if(ppi != NULL)
	{
		/* Check for the WINSTA_EXITWINDOWS access right
		   this can also prevent services process from shutting down the system */
		if(!(kexGetHandleAccess(ppi->hwinsta) & WINSTA_EXITWINDOWS))
			return FALSE;
	}

	return PostThreadMessage(dwShutdownThreadId, WM_QUERYENDSESSION, uFlags, 0);
}