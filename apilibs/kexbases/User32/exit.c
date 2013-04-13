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
#include "resource.h"
#include "hung.h"
#include "../advapi32/security.h"

BOOL fShutdown = FALSE;
BOOL fLoggingOff = FALSE;
BOOL fAborted = FALSE;
BOOL fForceShutdown = FALSE;
DWORD dwShutdownThreadId = 0;
PTDB98 pShutdownThread = NULL;
DWORD ShutdownThreadWndId = 0;

int WaitToKillAppTimeout = 20000;
int HungAppTimeout = 5000;
int TimeBetweenTermination = 250;
ULONG LogoffTimeout = 2 * 1250 * 60; // 2 minutes 30 seconds - shut down doesn't take more than 2 minutes

HWND hwndGlobalText = NULL;
HWND hwndShutdownDlg = NULL;

/* RegRemapPreDefKey - Remap a predefined key (e.g: HKEY_CURRENT_USER) to a new key
   HKEY hKey: Handle to a predefined key
   HKEY hNewHKey: Handle to an open key
   return: if the function succeeds, the return value is ERROR_SUCCESS, otherwise a nonzero error code defined in Winerror.h
   the differences between RegRemapPreDefKey and RegOverridePredefKey is that the first one is global and the second is only for the caller */
typedef LONG (WINAPI *REGREMAPPREDEFKEY)(HKEY hKey, HKEY hNewHKey);

/* ByeByeGDI - Destroy the first GDI heap
   DWORD Unknown: currently unknown, but seems to point to a module */
typedef VOID (WINAPI *BYEBYEGDI)(DWORD Unknown);

typedef BOOLEAN (WINAPI *ISPWRSHUTDOWNALLOWED)(void);

typedef struct _SHUTDOWNDATA
{
	UINT uFlags;
	DWORD SessionId;
	PWINSTATION_OBJECT WindowStation;
	DWORD Result;
	DWORD ShellProcessId;
	DWORD dwProcessId;
	DWORD dwThreadId;
	DWORD StartShutdownTickCount;
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

VOID TerminateAllProcesses()
{
	DWORD pProcess[255];
	DWORD dwProcesses = 0;
	ULONG i;

	kexEnumProcesses(pProcess, sizeof(pProcess), &dwProcesses);

	dwProcesses /= 4;

	for(i=0;i<=dwProcesses;i++)
	{
		HANDLE hProcess;
		PPDB98 Process;

		Process = (PPDB98)kexGetProcess(pProcess[i]);

		if(Process == pKernelProcess || Process == Msg32Process || Process == MprProcess)
			continue;

		hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pProcess[i]);

		if(hProcess == NULL)
			continue;

		TerminateProcess(hProcess, 0);
		CloseHandle(hProcess);
	}
}

void ReloadMPRServices()
{    
    char regValue[255];
    char regName[255];
    HKEY hKey;
    DWORD dwSizeName = sizeof(regName);
	DWORD dwSizeValue = sizeof(regValue);
    DWORD dwIndex = 0;
    
    if (RegOpenKey(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\RunServices", &hKey) != ERROR_SUCCESS)
        return;

	regName[0] = '\0';

    while (RegEnumValue(hKey, dwIndex, regName, &dwSizeName, NULL, NULL, (LPBYTE)regValue, &dwSizeValue) == ERROR_SUCCESS)
    {
		STARTUPINFO si;
		PROCESS_INFORMATION pi;

		dwSizeName = sizeof(regName);
		dwSizeValue = sizeof(regValue);

		memset(&si, 0, sizeof(STARTUPINFO));
		memset(&pi, 0, sizeof(PROCESS_INFORMATION));

		si.cb = sizeof(STARTUPINFO);

		CreateProcess(NULL,
					regValue,
					NULL,
					NULL,
					FALSE,
					0,
					NULL,
					NULL,
					&si,
					&pi);

		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);

        dwIndex++;

		regName[0] = '\0';
    }

    RegCloseKey(hKey);
}

DWORD WINAPI MprReinit(PVOID lParam)
{
	SendMessage(hwndShutdownDlg, WM_INITDIALOG, 0, 0);
	SetWindowText(hwndGlobalText, "Windows is starting up...");

	IntSwitchUser(".DEFAULT", TRUE, INFINITE, TRUE);

	SetCursorPos(GetSystemMetrics(SM_CXSCREEN)/2, GetSystemMetrics(SM_CYSCREEN)/2);

	ExitWindowsEx(EWX_LOGOFF | EWX_FORCE, 0);

	ReloadMPRServices();

	return 0;
}

VOID IntReinitialize()
{
	HANDLE hProcess, hThread;
	MSG msg;

	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, gpidMpr);

	//TerminateAllProcesses();

	hThread = CreateRemoteThread_new(hProcess, NULL, 0, MprReinit, NULL, 0, NULL);

	if(hThread == NULL)
	{
		CloseHandle(hProcess);
		EnableOEMLayer();
		SendMessage(hwndShutdownDlg, WM_USER+20, 0, 0);
		return;
	}

	WaitForSingleObject(hThread, INFINITE);

	CloseHandle(hThread);
	CloseHandle(hProcess);

	while(GetForegroundWindow() == hwndShutdownDlg)
	{
		Sleep(1);
		PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE);
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	SendMessage(hwndShutdownDlg, WM_USER+20, 0, 0);

	return;
}

BOOL CALLBACK EnumThreadWndProc(HWND hwnd, LPARAM lParam)
{
	PSHUTDOWNDATA ShutdownData = (PSHUTDOWNDATA)lParam;
	DWORD dwResult = 0;
	DWORD smt = 0;
	BOOL fHung = FALSE;
	PPDB98 Process = (PPDB98)kexGetProcess(ShutdownData->dwProcessId);
	PPROCESSINFO ppi = Process->Win32Process;
	int nLength = 0;
	PWND pWnd = HWNDtoPWND(hwnd);

	if(IS_SYSTEM_HWND(hwnd))
		return TRUE;

begining:
	fHung = IsHungThread_pfn(ShutdownData->dwThreadId);

	smt = SendMessageTimeout(hwnd,
							WM_QUERYENDSESSION,
							0,
							ShutdownData->uFlags & EWX_LOGOFF ? ENDSESSION_LOGOFF : 0,
							SMTO_NORMAL | SMTO_ABORTIFHUNG,
							fHung ? HungAppTimeout : WaitToKillAppTimeout,
							&dwResult);

	/* Abort if the message timeout or the thread doesn't want to quit
	   and EWX_FORCE flag is not specified/the application is not hung */
	if(!smt || !dwResult)
	{
		DWORD RetVal = 0;

		if((ShutdownData->uFlags & EWX_FORCE) || (ShutdownData->uFlags & EWX_FORCEIFHUNG)
			|| (ShutdownData->fEndServices) || fHung || (ppi != NULL && ppi->ShutdownFlags == SHUTDOWN_NORETRY))
			goto _continue;

		RetVal = ShowEndTaskDialog(hwnd, TRUE);

		if(RetVal == ET_WAIT)
			goto begining; // Using goto to prevent the stack to overflow
		else if(RetVal == ET_ENDTASK)
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

	/* FIXME: Should we post a message to threads that doesn't have windows ? */

	SetLastError(0);

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
	char ProcessName[255];

	Process = (PPDB98)kexGetProcess(dwProcessId);

	if(Process == NULL)
		return TRUE;

	kexGetProcessName(dwProcessId, ProcessName);

	/* Skip services processes */
	if((Process->Flags & fServiceProcess || !strcmp(ProcessName, "SYSTRAY.EXE")) && !ShutdownData->fEndServices)
		return TRUE;

	if(!(Process->Flags & fServiceProcess) && ShutdownData->fEndServices)
		return TRUE;

	/* Fail if the process is the MPR process or the process is the system tray */
	if(Process == MprProcess || Process == Msg32Process)
		return TRUE;

	if(Process->Win32Process != NULL && Process->Win32Process->SessionId != ShutdownData->SessionId)
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

	if(sa->fEndServices == 1)
		ProcessCount = -1; // Windows ME has one hidden unkillable process

	kexEnumProcesses(pProcess, sizeof(pProcess), &cbProcesses);

	cbProcesses /= sizeof(DWORD);

	for(index=0;index<=cbProcesses;index++)
	{
		char ProcessName[255];

		Process = (PPDB98)kexGetProcess(pProcess[index]);

		if(Process == NULL)
			continue;

		kexGetProcessName(pProcess[index], ProcessName);

		if((Process->Flags & fServiceProcess || !strcmp(ProcessName, "SYSTRAY.EXE")) && !sa->fEndServices)
			continue;

		if(!(Process->Flags & fServiceProcess) && sa->fEndServices)
			continue;

		if(Process == pKernelProcess || Process == MprProcess || Process == Msg32Process)
			continue;

		if(Process->Win32Process != NULL && Process->Win32Process->SessionId != sa->SessionId)
			continue;

		/* Skip the shell process because it must be the last process running
		   in the session */
		if(pProcess[index] == sa->ShellProcessId)
			continue;

		ProcessCount++;
	}

	if(ProcessCount == -1)
		ProcessCount = 0;

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

	switch(uMsg)
	{
		case WM_INITDIALOG:

			hwndGlobalText = GetDlgItem(hwndDlg, IDT_TEXT);
			hwndShutdownDlg = hwndDlg;

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

		case WM_USER+20:
			PostQuitMessage(0);
			DestroyWindow(hwndDlg);
			return 1;

		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case IDC_RESTART:
					EnableWindow_fix(GetDlgItem(hwndDlg, IDC_RESTART), FALSE);
					SetDlgItemText(hwndDlg, IDC_RESTART, "Restarting...");
					NtShutdownSystem(ShutdownReboot);
					return 1;
			}

	}

	return 0;
}

DWORD WINAPI CreateShutdownWindow(PVOID lParam)
{
	MSG msg;
	BOOL result;
	HWND hWnd;
	HANDLE hEvent = (HANDLE)LOWORD(lParam);
	BOOL fShuttingDown = HIWORD(lParam);
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

	hWnd = CreateDialog(hModule,
						fShuttingDown ? MAKEINTRESOURCE(IDD_SHUTTINGDOWN) : MAKEINTRESOURCE(IDD_SHUTDOWN_SAFE),
						NULL,
						DialogProc);

	if(fLoaded)
		FreeLibrary(hModule);

	SetEvent(hEvent);

	if(hWnd == NULL)
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

VOID CreateShutdownDialog(BOOL fShuttingDown)
{
	HANDLE hEvent;
	HANDLE hThread;

	hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	hThread = CreateThread(NULL,
						0,
						CreateShutdownWindow,
						(LPVOID)MAKELONG((WORD)hEvent, fShuttingDown),
						0,
						&ShutdownThreadWndId);

	if(hThread != NULL)
		WaitForSingleObject(hEvent, INFINITE);

	CloseHandle(hEvent);
	CloseHandle(hThread);
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

VOID FASTCALL DestroyKernelWnd(PSHUTDOWNDATA ShutdownData, BOOL fCloseAll)
{
	int count = 0;

	while(count <= 65536)
	{
		DWORD wndThread = 0;
		DWORD wndProcess = 0;
		HWND hWnd = FindWindow("#32770", NULL);

		count++;

		if(fForceShutdown == TRUE)
			return;

		if(hWnd == NULL)
			break;

		wndThread = GetWindowThreadProcessId(hWnd, &wndProcess);

		if(wndProcess == dwKernelProcessId && wndThread != dwShutdownThreadId && wndThread != ShutdownThreadWndId)
		{
			Sleep(TimeBetweenTermination);

			if(GetTickCount() - ShutdownData->StartShutdownTickCount >= LogoffTimeout && !ShutdownData->fLoggingOff)
			{
				fForceShutdown = TRUE;
				return;
			}

			SendMessage(hWnd, WM_COMMAND, IDOK, 0);
			__try
			{
				EndDialog(hWnd, IDOK);
			}
			__except(EXCEPTION_EXECUTE_HANDLER)
			{
			}
		}

		if(!fCloseAll)
			return;
	}
}

VOID FASTCALL CleanHardErrorQueue(VOID)
{
	int i;

	for(i=0;i<=MAX_HARD_ERRORS;i++)
	{
		__try
		{
			kexFreeObject((PVOID)pHardErrorData[i]->lpHardErrorMessage);
			kexFreeObject((PVOID)pHardErrorData[i]->lpHardErrorTitle);
			kexFreeObject(pHardErrorData[i]);
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
		}
		pHardErrorData[i] = NULL;
	}
}

VOID FASTCALL LogoffCurrentUser(PSHUTDOWNDATA ShutdownData)
{
	DWORD pProcess[1024];
	DWORD Processes = 0;
	ULONG i;

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
				if(GetTickCount() - ShutdownData->StartShutdownTickCount > LogoffTimeout && fForceShutdown == FALSE)
				{
					/* Force processes to terminate if the logoff time has raised the maximum logoff timeout */

					if(!ShutdownData->fLoggingOff)
						TimeBetweenTermination /= 4;

					fForceShutdown = TRUE;
					return;
				}
				else
					Sleep(TimeBetweenTermination);

				if(pProcess[i] == 0)
					continue;

				if(!EnumProcessesProc(pProcess[i], ShutdownData))
				{
					fAborted = TRUE;
					break;
				}
				pProcess[i] = 0;

				DestroyKernelWnd(ShutdownData, FALSE);
			}

			Processes = 0;
		} while(GetUserProcessesCount(ShutdownData) > 0 && !fAborted);
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
	HANDLE hThread = NULL;
	HANDLE hEvent = NULL;
	HMODULE hPowrprof = LoadLibrary("POWRPROF.DLL");
	REGREMAPPREDEFKEY RegRemapPreDefKey = (REGREMAPPREDEFKEY)kexGetProcAddress(GetModuleHandle("ADVAPI32.DLL"), "RegRemapPreDefKey");
	BYEBYEGDI ByeByeGDI = (BYEBYEGDI)kexGetProcAddress(GetModuleHandle("GDI32.DLL"), "ByeByeGDI");
	ISPWRSHUTDOWNALLOWED IsPwrShutdownAllowed = (ISPWRSHUTDOWNALLOWED)kexGetProcAddress(hPowrprof, "IsPwrShutdownAllowed");

	pShutdownThread = get_tdb();

	dwShutdownThreadId = GetCurrentThreadId();

	while(1)
	{
		GetMessage(&msg, NULL, 0, 0);

		/* Make sure the message is WM_QUERYENDSESSION */
		if(msg.message != WM_QUERYENDSESSION)
		{
			ERR("The message isn't WM_QUERYENDESSION ! (msg=0x%X)", msg.message);
			goto finished;
		}

		if(IsBadReadPtr(MprProcess, sizeof(PDB98)) || MprProcess->Flags & INVALID_FLAGS)
		{
			ERR_OUT("Cannot logoff/shutdown because MPREXE process is terminated !!\n");
			goto finished;
		}

		fShutdown = TRUE;

		//SystemParametersInfo(SPI_SETSCREENSAVERRUNNING, TRUE, 0, 0);

		RegOpenKeyEx(HKEY_CURRENT_USER, "Control Panel\\Desktop", 0, KEY_ALL_ACCESS, &hKey);

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
		sa.SessionId = msg.lParam;
		sa.WindowStation = NULL;
		sa.Result = 1;
		sa.dwProcessId = 0;
		sa.dwThreadId = 0;
		sa.fEndServices = FALSE;
		sa.StartShutdownTickCount = GetTickCount();

		fLoggingOff = (sa.uFlags == EWX_LOGOFF || sa.uFlags == EWX_FORCEIFHUNG || sa.uFlags == EWX_FORCE);

		sa.fLoggingOff = fLoggingOff;

		LogoffCurrentUser(&sa);

		if(fAborted)
			goto finished;

		if(fForceShutdown)
			WARN_OUT("Logoff timed out, shutdown is now forced\n");

		DestroyKernelWnd(&sa, TRUE);

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

			CreateShutdownDialog(TRUE);

			if(Msg32Process->Flags & INVALID_FLAGS)
			{
				if((sa.uFlags & EWX_SHUTDOWN) || (sa.uFlags & EWX_POWEROFF))
				{
					SetWindowText(hwndGlobalText, "32-bit message VxD server terminated. Shutting down...");
					NtShutdownSystem(ShutdownPowerOff);
				}
				else if((sa.uFlags & EWX_REBOOT) || (sa.uFlags & EWX_FASTRESTART))
				{
					SetWindowText(hwndGlobalText, "32-bit message VxD server terminated. Rebooting...");
					NtShutdownSystem(ShutdownReboot);
				}
				else
				{
					SetWindowText(hwndGlobalText, "msgsrv32 terminated. You can now turn of your computer.");
					NtShutdownSystem(ShutdownNoReboot);
				}

				while(1){}
			}

			SetWindowText(hwndGlobalText, "Please wait while the system is logging off.");

			Sleep(50);

			if(!IntSwitchUser(".DEFAULT", TRUE, INFINITE, FALSE))
			{
				DWORD LastErr = GetLastError();
				char buffer[255];
				LPVOID lpString = NULL;

				kexErrorCodeToString(LastErr, (LPSTR)&lpString);
				wsprintf(buffer, "Switching to the default user failed with error %d : %s", LastErr, lpString);
				Win32RaiseHardError(buffer, "Shut down", MB_OK, TRUE);
			}

			SetWindowText(hwndGlobalText, "Please wait while the system is terminating services processes.");
			sa.fEndServices = TRUE;
			LogoffCurrentUser(&sa);

			if(HardErrorThreadId != 0)
			{
				HANDLE hThread;
				int count = 0;

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

	
		if(IsPwrShutdownAllowed != NULL)
		{
			if(!IsPwrShutdownAllowed() && (sa.uFlags & EWX_SHUTDOWN || sa.uFlags & EWX_POWEROFF))
			{
				SendMessage(hwndShutdownDlg, WM_USER+20, 0, 0);
				CreateShutdownDialog(FALSE);
				goto finished;
			}
		}

		Sleep(50);

		if(sa.uFlags & EWX_FASTRESTART)
		{
			SetWindowText(hwndGlobalText, "Windows is restarting...");
			Sleep(1000);
			IntReinitialize();
			goto finished;
		}
		else if(sa.uFlags & EWX_REBOOT)
		{
			ByeByeGDI(0);
			NtShutdownSystem(ShutdownReboot);
			ExitWindowsEx(EWX_REBOOT | EWX_FORCE, 0);
		}
		else if(sa.uFlags & EWX_SHUTDOWN)
		{
			NtShutdownSystem(ShutdownPowerOff);
			ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE, 0);
		}
		else if(sa.uFlags & EWX_POWEROFF)
		{
			NtShutdownSystem(ShutdownPowerOff);
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
		//SystemParametersInfo(SPI_SETSCREENSAVERRUNNING, FALSE, 0, 0);

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
	if(uFlags & ~(EWX_LOGOFF | EWX_SHUTDOWN | EWX_REBOOT | EWX_FORCE | EWX_POWEROFF | EWX_FORCEIFHUNG | EWX_FASTRESTART))
		return FALSE;

	ppi = get_pdb()->Win32Process;

	if(ppi != NULL)
	{
		/* Check for the WINSTA_EXITWINDOWS access right
		   this can also prevent services process from shutting down the system */
		if(!(kexGetHandleAccess(ppi->hwinsta) & WINSTA_EXITWINDOWS))
			return FALSE;
	}

	return PostThreadMessage(dwShutdownThreadId, WM_QUERYENDSESSION, uFlags, ppi->SessionId);
}