/*
 *  KernelEx
 *  Copyright (C) 2009, Xeno86
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

#include <stdlib.h>
#include "desktop.h"

BOOL fShutdown = FALSE;
DWORD dwShutdownThreadId = 0;

DWORD WaitToKillAppTimeout = 0;
DWORD HungAppTimeout = 0;

typedef struct _SHUTDOWNDATA
{
	UINT uFlags;
	PWINSTATION_OBJECT WindowStation;
	DWORD Result;
	DWORD ShellProcessId;
	DWORD dwProcessId;
	DWORD dwThreadId;
} SHUTDOWNDATA, *PSHUTDOWNDATA;

/* - In Win9x, the shutdown is processed by the process that call ExitWindowsEx
   which makes the process freeze, i.e the shutdown dialog box which cover
   the entire screen freeze during shutdown which is annoying
   - The shutdown process only look for existing windows and not for processes
   - if the EWX_FORCE flag is specified, explorer.exe remains */

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
							SMTO_ABORTIFHUNG,
							fHung ? HungAppTimeout : WaitToKillAppTimeout,
							&dwResult);

	/* Abort if the message timeout or the thread doesn't want to quit
	   and EWX_FORCE flag is not specified/the application is not hung */
	if((!smt || !dwResult) && (!(ShutdownData->uFlags & EWX_FORCE) || !fHung))
	{
		ShutdownData->Result = 0;
		return FALSE;
	}

	PostMessage(hwnd, WM_ENDSESSION, TRUE, ShutdownData->uFlags & EWX_LOGOFF ? ENDSESSION_LOGOFF : 0);

	ShutdownData->Result = 1;
	return TRUE;
}

BOOL CALLBACK EnumThreadsProc(DWORD dwThreadId, PSHUTDOWNDATA ShutdownData)
{
	/* Using result data to make sure it's not EnumThreadWindows that fail */
	ShutdownData->Result = 1;
	ShutdownData->dwThreadId = dwThreadId;

	/* FIXME: Should we post a message to threads that doesn't have windows ? */
	while(GetLastError() == 0 && ShutdownData->dwProcessId == dwKernelProcessId)
		EnumThreadWindows_nothunk(dwThreadId, EnumThreadWndProc, (LPARAM)ShutdownData);

	if(ShutdownData->Result == 0)
		return FALSE;

	return TRUE;
}

BOOL CALLBACK EnumProcessesProc(DWORD dwProcessId, PSHUTDOWNDATA ShutdownData)
{
	HANDLE hProcess = NULL;
	DWORD pThread[1024];
	DWORD Threads = 0;
	ULONG i = 0;
	PPDB98 Process = NULL;
	PPROCESSINFO ppi = NULL;

	Process = (PPDB98)kexGetProcess(dwProcessId);

	if(Process == NULL)
		return TRUE;

	/* Skip services processes */
	if(Process->Flags & fServiceProcess || Process == Msg32Process)
		return TRUE;

	ppi = Process->Win32Process;

	if(ppi != NULL && ppi->rpwinsta != ShutdownData->WindowStation && Process != pKernelProcess)
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

DWORD WINAPI ShutdownThread(PVOID lParam)
{
	MSG msg;
	DWORD pProcess[1024];
	DWORD Processes = 0;
	BOOL fAborted = FALSE;
	ULONG i = 0;
	SHUTDOWNDATA sa;
	HANDLE hProcess = NULL;
	HKEY hKey = NULL;

	dwShutdownThreadId = GetCurrentThreadId();

	while(1)
	{
		GetMessage(&msg, NULL, 0, 0);

		/* Make sure the message is WM_QUERYENDSESSION */
		if(msg.message != WM_QUERYENDSESSION)
			continue;

		fShutdown = TRUE;

		RegOpenKeyEx(HKEY_CURRENT_USER,  "Control Panel\\Desktop", 0, KEY_ALL_ACCESS, &hKey);

		if(hKey != NULL)
		{
			CHAR KeyValue[7];
			LONG Result;
			DWORD KeySize;

			/* Look for WaitToKillAppTimeout */
			Result = RegQueryValueEx(hKey,
									"WaitToKillAppTimeout",
									NULL, NULL,
									(LPBYTE)KeyValue,
									&KeySize);

			if(Result) /* Found */
				WaitToKillAppTimeout = atoi(KeyValue);

			/* Now look for HungAppTimeout */
			Result = RegQueryValueEx(hKey,
									"HungAppTimeout",
									NULL, NULL,
									(LPBYTE)KeyValue,
									&KeySize);

			if(Result)
				HungAppTimeout = atoi(KeyValue);

			CloseHandle(hKey);
		}

		if(WaitToKillAppTimeout == 0)
			WaitToKillAppTimeout = 20000;

		if(HungAppTimeout == 0)
			HungAppTimeout = 5000;

		kexEnumProcesses(pProcess, sizeof(pProcess), &Processes);

		Processes /= sizeof(DWORD);

		sa.uFlags = msg.wParam;
		sa.WindowStation = (PWINSTATION_OBJECT)msg.lParam;
		sa.Result = 1;
		sa.dwProcessId = 0;
		sa.dwThreadId = 0;

		__try
		{
			for(i=0;i<=Processes;i++)
			{
				Sleep(100);
				if(!EnumProcessesProc(pProcess[i], &sa))
				{
					fAborted = TRUE;
					break;
				}
			}
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
		}

		if(fAborted)
			goto finished;

		if(sa.ShellProcessId != 0)
		{
			/* Now terminate the shell process */
			hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, sa.ShellProcessId);

			if(hProcess != NULL)
			{
				TerminateProcess(hProcess, 0);
				CloseHandle(hProcess);
			}
		}

		/* FIXME: Instead of calling ExitWindowsEx, send a logoff message to MPREXE */
		ExitWindowsEx(sa.uFlags, 0);

finished:
		fAborted = FALSE;
		fShutdown = FALSE;

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

/* MAKE_EXPORT ExitWindowsEx_fix=ExitWindowsEx */
BOOL WINAPI ExitWindowsEx_fix(UINT uFlags, DWORD dwReserved)
{
	/* dwReserved will be used for window stations */

	PPROCESSINFO ppi = NULL;
	HWINSTA hWinSta = (HWINSTA)dwReserved;
	PWINSTATION_OBJECT WindowStation = NULL;

	ppi = get_pdb()->Win32Process;

	if(ppi != NULL)
	{
		/* Check for the WINSTA_EXITWINDOWS access right
		   this also prevent KERNEL32/Services process from shutting down the system */
		if(!(kexGetHandleAccess(ppi->hwinsta) & WINSTA_EXITWINDOWS))
			return FALSE;
	}

	if(fShutdown)
		return TRUE;

	if(hWinSta != NULL && !IntValidateWindowStationHandle(hWinSta, &WindowStation))
		return FALSE;

	if(hWinSta == NULL)
		WindowStation = ppi->rpwinsta;

	return PostThreadMessage(dwShutdownThreadId, WM_QUERYENDSESSION, uFlags, (LPARAM)WindowStation);
}