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

#include <stdlib.h>
#include <shlwapi.h>
#include "desktop.h"
#include "../ntdll/_ntdll_apilist.h"

BOOL fShutdown = FALSE;
DWORD dwShutdownThreadId = 0;

int WaitToKillAppTimeout = 0;
int HungAppTimeout = 0;

typedef struct _SHUTDOWNDATA
{
	UINT uFlags;
	PWINSTATION_OBJECT WindowStation;
	DWORD Result;
	DWORD ShellProcessId;
	DWORD dwProcessId;
	DWORD dwThreadId;
} SHUTDOWNDATA, *PSHUTDOWNDATA;

/* - In Win9x, the shutdown is processed in the context of the caller
   which makes the caller freeze, i.e the shutdown dialog box which cover
   the entire screen freeze during shutdown which is annoying
   - The shutdown process only look for existing windows and not for processes
   - if the EWX_FORCE flag is specified, explorer.exe remains
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
		if((ShutdownData->uFlags & EWX_FORCE) || (ShutdownData->uFlags & EWX_FORCEIFHUNG))
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
	if(Process->Flags & fServiceProcess || Process == Msg32Process)
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

		if(Process->Flags & fServiceProcess || Process == Msg32Process)
			continue;

		if(Process == pKernelProcess)
			continue;

		/* Skip the shell process because he must be the last process running
		   in the session */
		if(pProcess[index] == sa->ShellProcessId)
			continue;

		ProcessCount++;
	}

	return ProcessCount;
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

		__try
		{
			/* Loop until there are no processes in the current user and no application
			   refused to terminate */
			do
			{
				kexEnumProcesses(pProcess, sizeof(pProcess), &Processes);

				Processes /= sizeof(DWORD);

				for(i=0;i<=Processes;i++)
				{
					Sleep(100);
					if(!EnumProcessesProc(pProcess[i], &sa))
					{
						fAborted = TRUE;
						break;
					}
				}

			} while(GetUserProcessesCount(&sa) != 0 && !fAborted);
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

		RegFlushKey(HKEY_CLASSES_ROOT);
		RegFlushKey(HKEY_CURRENT_CONFIG);
		RegFlushKey(HKEY_CURRENT_USER);
		RegFlushKey(HKEY_LOCAL_MACHINE);
		RegFlushKey(HKEY_PERFORMANCE_DATA);
		RegFlushKey(HKEY_USERS);
		RegFlushKey(HKEY_DYN_DATA);

		/* FIXME: Instead of calling ExitWindowsEx, send a logoff message to MPREXE */

		ExitWindowsEx(EWX_LOGOFF, 0);

		if(sa.uFlags & EWX_REBOOT)
		{
			ZwShutdownSystem(ShutdownReboot);
			ExitWindowsEx(EWX_REBOOT | EWX_FORCE, 0);
		}
		else if(sa.uFlags & EWX_SHUTDOWN)
		{
			ZwShutdownSystem(ShutdownNoReboot);
			ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE, 0);
		}
		else if(sa.uFlags & EWX_POWEROFF)
		{
			ZwShutdownSystem(ShutdownPowerOff);
			ExitWindowsEx(EWX_POWEROFF | EWX_FORCE, 0);
		}

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