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

#include "desktop.h"

/* If desktop and window stations are fully implemented, we could make Windows 98
   run multiples sessions */

/* NOTE: Desktops aren't input devices in KernelEx */
/* NOTE2: If KernelEx compatibility is disabled on an application,
   the application will always start on WinSta0\Default */
/* NOTE3: Windows directly created in other desktop than InputDesktop will
   sometimes have messed up scrollbars */
/* NOTE4: Hung threads will appear on any desktop (logically) */

/* Desktops */
PDESKTOP gpdeskInputDesktop = NULL;
PDESKTOP gpdeskScreenSaver = NULL;
PDESKTOP gpdeskWinlogon = NULL;

PTDB98 pDesktopThread = NULL;
DWORD dwDesktopThreadId = NULL;
BOOL fNewDesktop = FALSE;

VOID RepaintScreen(VOID)
{
	HINSTANCE hModule;
	DWORD RepaintScreen;

	hModule = (HINSTANCE)LoadLibrary16("USER.EXE");

	if((DWORD)hModule < 32)
		return;

	RepaintScreen = GetProcAddress16(hModule, "REPAINTSCREEN");

	if(RepaintScreen == NULL)
	{
		FreeLibrary16(hModule);
		return;
	}

	/* RepaintScreen has normally no parameters but we need to pass 4 parameters to not crash
	   the current application... */
	__asm	push 0
	__asm	push 0
	__asm	push 0
	__asm	push 0
	__asm	mov edx, [RepaintScreen]
	__asm	call ds:QT_Thunk
	__asm	add esp, 10h

	FreeLibrary16(hModule);

	return;
}

VOID DisableOEMLayer()
{
	HINSTANCE hModule;
	DWORD _DisableOEMLayer;

	hModule = (HINSTANCE)LoadLibrary16("USER.EXE");

	if((DWORD)hModule < 32)
		return;

	_DisableOEMLayer = GetProcAddress16(hModule, "DISABLEOEMLAYER");

	if(_DisableOEMLayer == NULL)
	{
		FreeLibrary16(hModule);
		return;
	}

	__asm	push 0
	__asm	push 0
	__asm	push 0
	__asm	push 0
	__asm	mov edx, [_DisableOEMLayer]
	__asm	call ds:QT_Thunk
	__asm	add esp, 10h

	FreeLibrary16(hModule);

	return;
}

VOID EnableOEMLayer()
{
	HINSTANCE hModule;
	DWORD _EnableOEMLayer;

	hModule = (HINSTANCE)LoadLibrary16("USER.EXE");

	if((DWORD)hModule < 32)
		return;

	_EnableOEMLayer = GetProcAddress16(hModule, "ENABLEOEMLAYER");

	if(_EnableOEMLayer == NULL)
	{
		FreeLibrary16(hModule);
		return;
	}

	__asm	push 0
	__asm	push 0
	__asm	push 0
	__asm	push 0
	__asm	mov edx, [_EnableOEMLayer]
	__asm	call ds:QT_Thunk
	__asm	add esp, 10h

	FreeLibrary16(hModule);

	return;
}

VOID APIENTRY RedrawDesktop()
{
	IntCompleteRedrawWindow(HWNDtoPWND(GetDesktopWindow()));
}

BOOL WINAPI CreateWindowStationAndDesktops()
{
	SECURITY_ATTRIBUTES sa;
	HDESK hDefault;
	HDESK hWinlogon;
	HDESK hScreenSaver;
	HWINSTA hWindowStation;

	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;

	InitializeListHead(&WindowStationList);
	hWindowStation = CreateWindowStationA_new("WinSta0", 0, WINSTA_ALL_ACCESS, &sa);

	if(hWindowStation == NULL)
		return FALSE;

	if(!SetProcessWindowStation_new(hWindowStation))
		return FALSE;

	hDefault = CreateDesktopA_new("Default", NULL, NULL, 0, GENERIC_ALL, &sa);
	hWinlogon = CreateDesktopA_new("Winlogon", NULL, NULL, 0, GENERIC_ALL, &sa);
	hScreenSaver = CreateDesktopA_new("Screen-Saver", NULL, NULL, 0, GENERIC_ALL, &sa);

	if(!hDefault || !hWinlogon || !hScreenSaver)
		return FALSE;

	if(!SetThreadDesktop_new(hDefault))
		return FALSE;

	if(!SwitchDesktop_new(hDefault))
		return FALSE;

	return TRUE;
}

BOOL InitDesktops()
{
	PPDB98 Process = NULL;
	PPDB98 ParentProcess = NULL;
	PPROCESSINFO ppi = NULL;
	PTHREADINFO pti = NULL;
    LPSTARTUPINFOA psi = NULL;
    //CHAR Path[MAX_PATH];
	PCHAR DesktopPath = NULL;
	PCHAR pszWinSta = NULL;
	PCHAR pszDesktop = NULL;
    HDESK hDesktop = NULL;
	HWINSTA hWindowStation = NULL;
	PDESKTOP DesktopObject = NULL;
	PWINSTATION_OBJECT WindowStationObject = NULL;
	BOOL fNewPath = FALSE;

	Process = get_pdb();

	ppi = Process->Win32Process;
	pti = get_tdb()->Win32Thread;

	/* Don't assign desktops and window stations to process/threads
	   if there is not input desktop set yet */
    if(gpdeskInputDesktop == NULL)
	{
		TRACE_OUT("InputDesktop is NULL, don't allocate desktop handles yet\n");
        return TRUE;
	}

	/* Found a startup desktop, assign the current thread to it */
	if(ppi->hdeskStartup != NULL)
	{
		SetThreadDesktop_new(ppi->hdeskStartup);
		return TRUE;
	}

	__try
	{
		psi = Process->pEDB->pStartupInfo;

		DesktopPath = (PCHAR)psi->lpDesktop;

		ParentProcess = Process;
		/* Search for valid desktop path */
		while(DesktopPath == NULL)
		{
			ParentProcess = ParentProcess->ParentPDB;

			if(ParentProcess == Process || ParentProcess == NULL)
				break;

			psi = ParentProcess->pEDB->pStartupInfo;
			DesktopPath = (PCHAR)psi->lpDesktop;
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		DesktopPath = NULL;
	}

	if(DesktopPath != NULL && !IsBadStringPtr(DesktopPath, -1) && strlen(DesktopPath) > sizeof(CHAR))
	{
		char *pch;

		/* Copy the path to the current process's startup info */
		Process->pEDB->pStartupInfo->lpDesktop = DesktopPath;

		/* Allocate the path to a new buffer so that we won't
		   overwrite lpDesktop */
		DesktopPath = (PCHAR)malloc(strlen(DesktopPath));
		strcpy(DesktopPath, psi->lpDesktop);

		/* Look for window station names and desktop names */
		pch = strchr(DesktopPath, '\\');
		if(pch != NULL)
		{
			/* Desktop names and window station names */
			*pch = 0;
			pch++;
			pszDesktop = pch;
			pszWinSta = DesktopPath;
		}
		else
		{
			/* Only the desktop name will be used */
			pszWinSta = NULL;
			pszDesktop = DesktopPath;
		}
	}

	if(!kexFindObjectHandle(Process,
							NULL,
							K32OBJ_WINSTATION,
							(PHANDLE)&hWindowStation))
	{
		if(pszWinSta == NULL)
			pszWinSta = "WinSta0";
	}

	if(!kexFindObjectHandle(Process,
							NULL,
							K32OBJ_DESKTOP,
							(PHANDLE)&hDesktop))
	{
		if(pszDesktop == NULL)
			pszDesktop = "Default";
	}

	if(hWindowStation == NULL)
	{
		hWindowStation = OpenWindowStationA_new(pszWinSta, FALSE, WINSTA_ALL_ACCESS);

		if(hWindowStation == NULL)
		{
			TRACE("Failed to open window station %s\n", pszWinSta);
			goto error;
		}

		if(!IntValidateWindowStationHandle(hWindowStation, &WindowStationObject))
		{
			TRACE("Failed to validate window station %s (error %d)\n", pszWinSta, GetLastError());
			goto error;
		}

		/* Each process runing on Windows NT always have 2 window station handles refering
		   to the same window station */
		kexAllocHandle(Process, WindowStationObject, WINSTA_ALL_ACCESS);
	}

	if(!SetProcessWindowStation_new(hWindowStation))
	{
		TRACE("Failed to set process to window station 0x%X\n", hWindowStation);
		goto error;
	}

	if(hDesktop == NULL)
	{
		hDesktop = OpenDesktopA_new(pszDesktop, 0, FALSE, DESKTOP_ALL_ACCESS);

		if(hDesktop == NULL)
		{
			TRACE("Failed to open desktop %s (error %d)\n", pszDesktop, GetLastError());
			goto error;
		}

	}

	if(!IntValidateDesktopHandle(hDesktop, &DesktopObject))
	{
		TRACE("Failed to validate desktop 0x%X\n", hDesktop);
		goto error;
	}

	if(!SetThreadDesktop_new(hDesktop))
	{
		TRACE("Failed to set thread to desktop 0x%X\n", hDesktop);
		goto error;
	}

	ppi->rpdeskStartup = DesktopObject;
	ppi->hdeskStartup = hDesktop;

	free((PVOID)DesktopPath);
    return TRUE;
error:
	if(hDesktop != NULL)
		CloseHandle(hDesktop);
	if(hWindowStation != NULL)
		CloseHandle(hWindowStation);
	if(DesktopPath != NULL)
		free((PVOID)DesktopPath);
	return FALSE;
}

/* IntValidateDesktopHandle */
BOOL IntValidateDesktopHandle(HDESK hDesktop, PDESKTOP *DesktopObject)
{
	PDESKTOP Object;

	Object = (PDESKTOP)kexGetHandleObject(hDesktop, K32OBJ_DESKTOP, 0);

	if(Object == NULL)
		return FALSE;

	if(!IsBadWritePtr(DesktopObject, sizeof(DWORD)))
		*DesktopObject = Object;
	else
	{
		kexDereferenceObject(Object);
		return FALSE;
	}

	return TRUE;
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	PWND pwnd = NULL;
	DWORD dwThreadId = 0;
	DWORD dwProcessId = 0;
	PTDB98 Thread = NULL;
	PPDB98 Process = NULL;
	PTHREADINFO pti = NULL;
	PPROCESSINFO ppi = NULL;
	BOOL fHung = FALSE;

	dwThreadId = GetWindowThreadProcessId(hwnd, &dwProcessId);

	if(dwThreadId == 0)
		return TRUE;

	Thread = (PTDB98)kexGetThread(dwThreadId);
	Process = (PPDB98)kexGetProcess(dwProcessId);

	if(Thread == NULL || Process == NULL)
		return TRUE;

	if(Process == pKernelProcess)
		return TRUE;

	//if(Process->Flags & fWin16Process)
	//	return TRUE;

	pti = Thread->Win32Thread;
	ppi = Process->Win32Process;

	/* If the thread/process doesn't have a Win32Thread/Win32Process, then create it */
	if(ppi == NULL)
	{
		HWINSTA hWindowStation = NULL;
		HDESK hDesktop = NULL;
		PWINSTATION_OBJECT WindowStationObject = NULL;
		PDESKTOP DesktopObject = NULL;

		__try
		{
			WindowStationObject = Process->ParentPDB->Win32Process->rpwinsta;
			if(WindowStationObject == NULL)
			{
				WindowStationObject = InputWindowStation;
				DesktopObject = gpdeskInputDesktop;
				__leave;
			}

			DesktopObject = Process->ParentPDB->Win32Process->rpdeskStartup;
			if(DesktopObject == NULL)
			{
				WindowStationObject = InputWindowStation;
				DesktopObject = gpdeskInputDesktop;
			}
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			WindowStationObject = InputWindowStation;
			DesktopObject = gpdeskInputDesktop;
		}

		ppi_init(Process);
		ppi = Process->Win32Process;
		/* Allocate handles */
		hWindowStation = (HWINSTA)kexAllocHandle(Process, WindowStationObject, WINSTA_ALL_ACCESS);
		kexAllocHandle(Process, WindowStationObject, WINSTA_ALL_ACCESS);
		hDesktop = (HDESK)kexAllocHandle(Process, DesktopObject, DESKTOP_ALL_ACCESS);
		/* Set startup desktop and window station */
		ppi->rpwinsta = WindowStationObject;
		ppi->hwinsta = hWindowStation;
		ppi->rpdeskStartup = DesktopObject;
		ppi->hdeskStartup = hDesktop;

		TRACE("Created ppi %p for Process ", ppi);
		DBGPRINTF(("%p because it was NULL\n", Process));
	}

	if(pti == NULL)
	{
		pti_init(Thread);
		/* Set the thread's desktop to the process's startup desktop */
		pti = Thread->Win32Thread;
		pti->rpdesk = ppi->rpdeskStartup;
		pti->hdesk = ppi->hdeskStartup;
		TRACE("Created pti %p for Thread ", pti);
		DBGPRINTF(("%p because it was NULL\n", Thread));
	}

	if(IsBadReadPtr(pti, sizeof(THREADINFO)) || IsBadReadPtr(ppi, sizeof(PROCESSINFO)))
		return TRUE;

	pwnd = HWNDtoPWND(hwnd);

	if(!pwnd)
		return TRUE;

	/* Make sure the window isn't something like a button or something, could be buggy otherwise */
	if(pwnd->style & WS_CHILD)
		return TRUE;

	if(pti->rpdesk != gpdeskInputDesktop)
	{
		/* Window's thread isn't in the input desktop, hide it */
		if(pwnd->style & WS_VISIBLE)
		{
			if(!(pwnd->style & WS_INTERNAL_WASVISIBLE))
				pwnd->style |= WS_INTERNAL_WASVISIBLE;

			pwnd->style &= ~WS_VISIBLE;

			if(pwnd->ExStyle & WS_EX_TOPMOST)
			{
				SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_ASYNCWINDOWPOS | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);
				pwnd->ExStyle |= WS_EX_INTERNAL_WASTOPMOST;
			}

			/* Alot faster than SetWindowPos, and even redraw hung windows */
			IntCompleteRedrawWindow(pwnd);
		}
	}
	else
	{
		/* Window's thread is in the input desktop, show it and update it if necessary */
		if(pwnd->style & WS_INTERNAL_WASVISIBLE)
		{
			pwnd->style &= ~WS_INTERNAL_WASVISIBLE;

			pwnd->style |= WS_VISIBLE;

			if(pwnd->ExStyle & WS_EX_INTERNAL_WASTOPMOST)
			{
				SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_ASYNCWINDOWPOS | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);
				pwnd->ExStyle &= ~WS_EX_INTERNAL_WASTOPMOST;
			}

			IntCompleteRedrawWindow(pwnd);
		}
	}

	return TRUE;
}

DWORD WINAPI DesktopThread(PVOID lParam)
{
	MSG msg;

	kexGrabLocks();
	pDesktopThread = get_tdb();
	pKernelProcess = get_pdb();
	dwDesktopThreadId = GetCurrentThreadId();
	dwKernelProcessId = GetCurrentProcessId();

	/* Prevent the kernel process from being terminated by adding the terminating flag */
	pKernelProcess->Flags |= fTerminating;

	kexReleaseLocks();

	while(1)
	{
		Sleep(1);
		PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE);
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		GrabWin16Lock(); // Don't get interrupted while we are switching desktop/hiding some windows

		/* We don't want our desktop thread crash, so we safely use
		exception handling, crash happen when there is no free memory */
		__try
		{
			EnumWindows_nothunk(EnumWindowsProc, 0);
			EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, gpdeskInputDesktop->pdev);
			if(InterlockedExchange((volatile long *)&fNewDesktop, FALSE) == TRUE)
			{
				TRACE_OUT("Input desktop has changed, redrawing screen... ");
				RepaintScreen();
				RedrawDesktop();
				DBGPRINTF(("successful\n"));
			}
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
		}

		ReleaseWin16Lock();

		/* FIXME: Wait for Win16Lock to be grabbed/released without wasting system resources */
		/*while(pWin16Mutex == NULL || pWin16Mutex->LockCount >= 0)
		{
			PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE);
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			Sleep(1);
			continue;
		}*/
	}

	return 0;
}

/**
 *
 *
 *
 * Implementations start from here
 *
 *
 *
 */
 
/* MAKE_EXPORT CloseDesktop_new=CloseDesktop */
BOOL WINAPI CloseDesktop_new(HDESK hDesktop)
{
	PDESKTOP DesktopObject = NULL;
	DWORD pThread[256];
	DWORD cbThreads = 0;
	PTHREADINFO pti = NULL;
	PPROCESSINFO ppi = NULL;
	PTDB98 Thread = NULL;
	UINT index = 0;
	BOOL result = FALSE;

	GrabWin16Lock();

	// Make sure the object type is K32OBJ_DESKTOP
	if(!IntValidateDesktopHandle(hDesktop, &DesktopObject))
	{
		TRACE_OUT("hDesktop is invalid!\n");
		SetLastError(ERROR_ACCESS_DENIED);
		ReleaseWin16Lock();
		return FALSE;
	}

	pti = get_tdb()->Win32Thread;
	ppi = get_pdb()->Win32Process;

	if(pti == NULL || ppi == NULL)
	{
		ReleaseWin16Lock();
		return FALSE;
	}

	/* Fail if the desktop is the startup desktop */
	if(ppi->rpdeskStartup == DesktopObject)
	{
		TRACE_OUT("Can't close the startup desktop ! \n");
		kexDereferenceObject(DesktopObject);
		SetLastError(ERROR_ACCESS_DENIED);
		ReleaseWin16Lock();
		return FALSE;
	}

	kexEnumThreads(GetCurrentProcessId(), pThread, sizeof(pThread), &cbThreads);

	cbThreads /= sizeof(DWORD);

	/* Now check if any threads from the current process is using the specified desktop handle */
	for(index=0;index<=cbThreads;index++)
	{
		Thread = (PTDB98)kexGetThread(pThread[index]);

		if(Thread == NULL)
			continue;

		pti = Thread->Win32Thread;

		/* Found one using the desktop object */
		if(pti->rpdesk == DesktopObject)
		{
			TRACE("Can't close desktop 0x%X because thread 0x%X is using it !!!\n", DesktopObject, Thread);
			kexDereferenceObject(DesktopObject);
			SetLastError(ERROR_ACCESS_DENIED);
			ReleaseWin16Lock();
			return FALSE;
		}
	}

	/* Dereference the desktop 2 times
	   one because we used IntValidateDesktopHandle and
	   another for decrementing the reference count */
	kexDereferenceObject(DesktopObject);
	result = kexDereferenceObject(DesktopObject);

	TRACE("Object 0x%X dereferenced\n", DesktopObject);

	if(DesktopObject->cReferences < 1)
	{
		TRACE("Removing object 0x%X from the system\n", DesktopObject);
		if(RemoveEntryList(&DesktopObject->ListEntry))
		{
			kexFreeObject(DesktopObject->lpName);
			kexFreeObject(DesktopObject->pName);
			kexFreeObject(DesktopObject->pdev);
			kexFreeObject(DesktopObject);
		}
	}

	return result;
}

/* MAKE_EXPORT CreateDesktopA_new=CreateDesktopA */
HDESK WINAPI CreateDesktopA_new(LPCSTR lpszDesktop, LPCSTR lpszDevice, LPDEVMODE pDevmode, DWORD dwFlags, ACCESS_MASK dwDesiredAccess, LPSECURITY_ATTRIBUTES lpsa)
{
	PCHAR DesktopName = NULL;
	PDESKTOP DesktopObject = NULL;
	PWINSTATION_OBJECT WindowStationObject = NULL;
	HDESK hDesktop = NULL;
	PCHAR DesktopPath;
	PPDB98 Process = get_pdb();
	PPROCESSINFO ppi = Process->Win32Process;
	DWORD flags = 0;
	PDEVMODE pdev = pDevmode;

	GrabWin16Lock();

	if(pDevmode != NULL && IsBadReadPtr(pDevmode, sizeof(DEVMODE)))
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		ReleaseWin16Lock();
		return NULL;
	}

	if(ppi == NULL)
	{
		SetLastError(ERROR_ACCESS_DENIED);
		ReleaseWin16Lock();
		return NULL;
	}

	WindowStationObject = ppi->rpwinsta;

	if(WindowStationObject == NULL)
	{
		SetLastError(ERROR_ACCESS_DENIED);
		ReleaseWin16Lock();
		return NULL;
	}

	if(!IsBadReadPtr(lpsa, sizeof(SECURITY_ATTRIBUTES)) && lpsa->bInheritHandle == TRUE)
		flags |= HF_INHERIT;

	/* SYNCHRONIZE access right for desktops is not supported */
	if(dwDesiredAccess & SYNCHRONIZE)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		ReleaseWin16Lock();
		return NULL;
	}

	/* Convert generic access to a compatible access*/
	if(dwDesiredAccess & GENERIC_ALL)
		dwDesiredAccess = DESKTOP_ALL_ACCESS;
	else if(dwDesiredAccess & GENERIC_EXECUTE)
		dwDesiredAccess = DESKTOP_EXECUTE;
	else if(dwDesiredAccess & GENERIC_READ)
		dwDesiredAccess = DESKTOP_READ;
	else if(dwDesiredAccess & GENERIC_WRITE)
		dwDesiredAccess = DESKTOP_WRITE;

	/* Check if another desktop exists */
	if((hDesktop = OpenDesktopA_new((LPTSTR)lpszDesktop, dwFlags, flags & HF_INHERIT, dwDesiredAccess)) != NULL)
	{
		/* Nothing to do */
		SetLastError(ERROR_ALREADY_EXISTS);
		ReleaseWin16Lock();
		return hDesktop;
	}

	DesktopObject = (PDESKTOP)kexAllocObject(sizeof(DESKTOP));

	if(DesktopObject == NULL)
	{
		SetLastError(ERROR_NOT_ENOUGH_MEMORY);
		ReleaseWin16Lock();
		return NULL;
	}

	if(pdev == NULL)
	{
		/* Allocate a new devmode for the desktop */
		pdev = (PDEVMODE)kexAllocObject(sizeof(DEVMODEA));

		if(pdev == NULL)
		{
			SetLastError(ERROR_NOT_ENOUGH_MEMORY);
			ReleaseWin16Lock();
			return NULL;
		}

		pdev->dmSize = sizeof(DEVMODE);
		/* Obtain the current display settings for the new devmode */
		if(!EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, pdev))
		{
			ReleaseWin16Lock();
			return NULL;
		}
	}

	/* Copy the desktop name to a shared memory string */
	DesktopName = (PCHAR)kexAllocObject(strlen(lpszDesktop));
	strcpy(DesktopName, lpszDesktop);

	/* Add \Path to the desktop name */
	DesktopPath = (PCHAR)kexAllocObject(strlen(DesktopName) + 1);

	sprintf(DesktopPath, "\\%s", DesktopName);

	InitializeListHead(&DesktopObject->ListEntry);
	InsertTailList(&WindowStationObject->DesktopListHead, &DesktopObject->ListEntry);

	DesktopObject->Type = K32OBJ_DESKTOP;
	DesktopObject->cReferences = 0;
	DesktopObject->pName = kexAllocObjectName(DesktopObject, DesktopPath);
	DesktopObject->lpName = (PCHAR)DesktopName;
	DesktopObject->rpwinstaParent = Process->Win32Process->rpwinsta;
	DesktopObject->pdev = pdev;
	DesktopObject->DesktopWindow = GetDesktopWindow();

	DesktopObject->pheapDesktop = g_UserBase; // Using USER32's Heap for each desktop
	DesktopObject->ulHeapSize = 4194304; // USER32's Heap size

	hDesktop = (HDESK)kexAllocHandle(Process, DesktopObject, dwDesiredAccess | flags);

	if(strcmp(lpszDesktop, "Winlogon") == 0 && gpdeskWinlogon == NULL)
		gpdeskWinlogon = DesktopObject;
	else if(strcmp(lpszDesktop, "Screen-Saver") == 0 && gpdeskScreenSaver == NULL)
		gpdeskScreenSaver = DesktopObject;

	ReleaseWin16Lock();
    return hDesktop;
}

/* MAKE_EXPORT CreateDesktopExA_new=CreateDesktopExA */
HDESK WINAPI CreateDesktopExA_new(LPCSTR lpszDesktop, LPCSTR lpszDevice, LPDEVMODEA pDevmode, DWORD dwFlags, ACCESS_MASK dwDesiredAccess, LPSECURITY_ATTRIBUTES lpsa, ULONG ulHeapSize, PVOID pvoid)
{
	return CreateDesktopA_new(lpszDesktop, lpszDevice, pDevmode, dwFlags, dwDesiredAccess, lpsa);
}

/* MAKE_EXPORT EnumDesktopsA_new=EnumDesktopsA */
BOOL WINAPI EnumDesktopsA_new(HWINSTA hwinsta, DESKTOPENUMPROCA lpEnumFunc, LPARAM lParam)
{
	PWINSTATION_OBJECT WindowStationObject;
	PDESKTOP DesktopObject;
	PLIST_ENTRY DesktopList;

	if(IsBadCodePtr((FARPROC)lpEnumFunc))
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	if(!IntValidateWindowStationHandle(hwinsta, &WindowStationObject))
	{
		SetLastError(ERROR_INVALID_HANDLE);
		return FALSE;
	}

	/* Check if hwinsta has the WINSTA_ENUMDESKTOPS access right */
	if(!(kexGetHandleAccess(hwinsta) & WINSTA_ENUMDESKTOPS))
	{
		SetLastError(ERROR_ACCESS_DENIED);
		kexDereferenceObject(WindowStationObject);
		return FALSE;
	}

	for(DesktopList = WindowStationObject->DesktopListHead.Flink; DesktopList != &WindowStationObject->DesktopListHead; DesktopList = DesktopList->Flink)
	{
		// FIXME: Check DesktopObject's access right for DESKTOP_ENUMERATE
		DesktopObject = CONTAINING_RECORD(DesktopList, DESKTOP, ListEntry);

		if(!(*lpEnumFunc)(DesktopObject->lpName, lParam))
		{
			kexDereferenceObject(WindowStationObject);
			return FALSE;
		}
	}

	kexDereferenceObject(WindowStationObject);
    return TRUE;
}

BOOL CALLBACK EnumDesktopWindowsProc(HWND hWnd, LPARAM lParam)
{
	__try
	{
		DWORD *ParamArray = (DWORD*)lParam;
		PTDB98 Thread = NULL;
		PTHREADINFO pti = NULL;
		PDESKTOP DesktopObject = (PDESKTOP)ParamArray[0];
		WNDENUMPROC lpfn = (WNDENUMPROC)ParamArray[1];
		LPARAM lpParam = ParamArray[2];
		PWND pWnd = HWNDtoPWND(hWnd);
		PMSGQUEUE pQueue = GetWindowQueue(pWnd);
		DWORD dwThreadId = 0;

		if(pQueue == NULL)
			return TRUE;

		dwThreadId = pQueue->threadId;

		Thread = (PTDB98)kexGetThread(dwThreadId);

		if(Thread == NULL)
			return TRUE;

		pti = Thread->Win32Thread;

		if(pti->rpdesk == NULL)
			return TRUE;

		if(pti->rpdesk == DesktopObject)
			return (*lpfn)(hWnd, lpParam);
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
	}

	return TRUE;
}

/* MAKE_EXPORT EnumDesktopWindows_new=EnumDesktopWindows */
BOOL WINAPI EnumDesktopWindows_new(HDESK hDesktop, WNDENUMPROC lpfn, LPARAM lParam)
{
	PDESKTOP DesktopObject = NULL;
	BOOL result = FALSE;
	DWORD ParamArray[2];

	if(IsBadCodePtr((FARPROC)lpfn))
		return FALSE;

	ParamArray[0] = (DWORD)DesktopObject;
	ParamArray[1] = (DWORD)lpfn;
	ParamArray[2] = lParam;

	if(!IntValidateDesktopHandle(hDesktop, &DesktopObject))
		return FALSE;

	result = EnumWindows(EnumDesktopWindowsProc, (LPARAM)ParamArray);

	kexDereferenceObject(DesktopObject);
	return result;
}

/* MAKE_EXPORT GetInputDesktop_new=GetInputDesktop */
HDESK WINAPI GetInputDesktop_new(VOID)
{
	/* This function is provided for compatibility */
	/* It seems to always return an handle of the input desktop found in the process */
	/* return NULL if there is no handle to the current desktop in the process */
	PPDB98 Process = get_pdb();
	HDESK hDesktop;

	if(kexFindObjectHandle((PVOID)Process, gpdeskInputDesktop, K32OBJ_DESKTOP, (PHANDLE)&hDesktop))
		return hDesktop;

	return NULL;
}

/* MAKE_EXPORT GetThreadDesktop_new=GetThreadDesktop */
HDESK WINAPI GetThreadDesktop_new(DWORD dwThreadId)
{
	PTDB98 Thread = NULL;
	PTHREADINFO pti = NULL;

	GrabWin16Lock();

	Thread = (PTDB98)kexGetThread(dwThreadId);

	if(Thread == NULL)
	{
		ReleaseWin16Lock();
		return NULL;
	}

	pti = Thread->Win32Thread;

	if(pti == NULL)
	{
		ReleaseWin16Lock();
		return NULL;
	}

	ReleaseWin16Lock();
	return pti->hdesk;
}

/* MAKE_EXPORT OpenDesktopA_new=OpenDesktopA */
HDESK WINAPI OpenDesktopA_new(LPSTR lpszDesktop, DWORD dwFlags, BOOL fInherit, ACCESS_MASK dwDesiredAccess)
{
	HDESK hDesktop = NULL;
	PDESKTOP Desktop = NULL;
	PWINSTATION_OBJECT WindowStation = NULL;
	CHAR DesktopPath[MAX_PATH];
	PCHAR DesktopName = NULL;
	DWORD flags = 0;
	PPDB98 Process = get_pdb();
	PPROCESSINFO ppi = Process->Win32Process;
	PLIST_ENTRY DesktopList;

	if(IsBadStringPtr(lpszDesktop, -1))
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return NULL;
	}

	if(ppi == NULL)
		goto globalsearch;

	if(fInherit)
		flags |= HF_INHERIT;

	DesktopName = lpszDesktop;

	WindowStation = ppi->rpwinsta;

	if(WindowStation == NULL)
		goto globalsearch;

	for(DesktopList = WindowStation->DesktopListHead.Flink; DesktopList != &WindowStation->DesktopListHead; DesktopList = DesktopList->Flink)
	{
		// FIXME: Check DesktopObject's access right for DESKTOP_ENUMERATE
		Desktop = CONTAINING_RECORD(DesktopList, DESKTOP, ListEntry);

		if(strcmpi(Desktop->lpName, DesktopName) == 0)
		{
			hDesktop = (HDESK)kexAllocHandle(Process, Desktop, dwDesiredAccess | flags);
			goto validatedesktop;
		}
	}

	goto validatedesktop;

/* Perform a global search if the process has no window station */
globalsearch:

	sprintf(DesktopPath, "\\%s", DesktopName);

	Sleep(1);

	hDesktop = (HDESK)kexOpenObjectByName(DesktopPath, K32OBJ_DESKTOP, dwDesiredAccess | flags);

validatedesktop:
	if(hDesktop == NULL)
	{
		SetLastError(ERROR_INVALID_HANDLE);
		return NULL;
	}

	return hDesktop;
}

/* MAKE_EXPORT OpenInputDesktop_new=OpenInputDesktop */
HDESK WINAPI OpenInputDesktop_new(DWORD dwFlags, BOOL fInherit, ACCESS_MASK dwDesiredAccess)
{
	HDESK hDesktop;
	PPDB98 Process;
	DWORD flags;

	GrabWin16Lock();

	Process = get_pdb();

	if(fInherit)
		flags |= HF_INHERIT;

	/* OpenInputDesktop allocate an handle of the input desktop */
	hDesktop = (HDESK)kexAllocHandle(Process, gpdeskInputDesktop, dwDesiredAccess | flags);

	ReleaseWin16Lock();

	return hDesktop;
}

/* MAKE_EXPORT SetThreadDesktop_new=SetThreadDesktop */
BOOL WINAPI SetThreadDesktop_new(HDESK hDesktop)
{
	PTHREADINFO pti;
	PPROCESSINFO ppi;
	PDESKTOP DesktopObject;
	GUITHREADINFO gti;

	GrabWin16Lock();

	pti = get_tdb()->Win32Thread;
	ppi = get_pdb()->Win32Process;

	if(pti == NULL)
	{
		SetLastError(ERROR_ACCESS_DENIED);
		ReleaseWin16Lock();
		return FALSE;
	}

	if(pti->hdesk == hDesktop)
	{
		ReleaseWin16Lock();
		return TRUE;
	}

	if(!IntValidateDesktopHandle(hDesktop, &DesktopObject))
	{
		SetLastError(ERROR_INVALID_HANDLE);
		ReleaseWin16Lock();
		return FALSE;
	}

	if(pti->rpdesk == DesktopObject)
	{
		kexDereferenceObject(DesktopObject);
		ReleaseWin16Lock();
		return TRUE;
	}

	/* Skip windows check if the thread has no desktop */
	if(pti->rpdesk == NULL)
		goto _skipwndcheck;

	memset(&gti, 0, sizeof(GUITHREADINFO));
	gti.cbSize = sizeof(GUITHREADINFO);

	GetGUIThreadInfo(GetCurrentThreadId(), &gti);

	/* Fail if the calling thread has any windows on its current desktop */
	if(gti.hwndActive != NULL || gti.hwndCapture != NULL || gti.hwndCaret != NULL
		|| gti.hwndFocus != NULL || gti.hwndMenuOwner != NULL || gti.hwndMoveSize != NULL)
	{
		TRACE("Cannot set thread 0x%X to desktop handle 0x%X (object 0x%X) because it has windows !\n", GetCurrentThreadId(), hDesktop, DesktopObject);
		kexDereferenceObject(DesktopObject);
		SetLastError(ERROR_BUSY);
		ReleaseWin16Lock();
		return FALSE;
	}

_skipwndcheck:

	pti->rpdesk = DesktopObject;
	pti->hdesk = hDesktop;

	if(ppi != NULL && ppi->rpdeskStartup == NULL)
	{
		ppi->rpdeskStartup = DesktopObject;
		ppi->hdeskStartup = hDesktop;
	}

	kexDereferenceObject(DesktopObject);
	ReleaseWin16Lock();
	return TRUE;
}

/* MAKE_EXPORT SwitchDesktop_new=SwitchDesktop */
BOOL WINAPI SwitchDesktop_new(HDESK hDesktop)
{
    PDESKTOP DesktopObject = NULL;
    PWINSTATION_OBJECT WindowStationObject = NULL;
    HWINSTA hWinSta = GetProcessWindowStation_new();
	BOOL FirstSwitch = (gpdeskInputDesktop == NULL);
	HANDLE hEvent = NULL;
	BOOL fFirstSwitch = (gpdeskInputDesktop == NULL);
	BOOL fParent = FALSE;
	PDEVMODE pdev = NULL;
	PDEVMODE polddev = NULL;
	PVOID Object = NULL;
	PPDB98 Process = get_pdb();

    GrabWin16Lock();

	TRACE_OUT("About to switch desktop\n");

    if(!IntValidateDesktopHandle(hDesktop, &DesktopObject))
    {
		TRACE_OUT("hDesktop is INVALID !\n");
		SetLastError(ERROR_INVALID_HANDLE);
        ReleaseWin16Lock();
        return FALSE;
    }

	if(DesktopObject == gpdeskInputDesktop)
	{
		/* Nothing to do */
		TRACE("hDesktop 0x%X is already the current desktop !\n", hDesktop);
		kexDereferenceObject(DesktopObject);
		ReleaseWin16Lock();
		return TRUE;
	}

	if(!(kexGetHandleAccess(hDesktop) & DESKTOP_SWITCHDESKTOP))
	{
		ERR("hDesktop 0x%X doesn't have the DESKTOP_SWITCHDESKTOP access right !\n", hDesktop);
		kexDereferenceObject(DesktopObject);
		SetLastError(ERROR_ACCESS_DENIED);
		ReleaseWin16Lock();
		return FALSE;
	}

    if(!IntValidateWindowStationHandle(hWinSta, &WindowStationObject))
    {
		ERR_OUT("Window station is invalid ! Maybe the process doesn't have one ?\n");
		SetLastError(ERROR_INVALID_HANDLE);
		kexDereferenceObject(DesktopObject);
        ReleaseWin16Lock();
        return FALSE;
    }

	/* Check if the process is associated with a secured desktop (Winlogon or Screen-Saver) */
	if((kexFindObjectHandle(Process, gpdeskWinlogon, K32OBJ_DESKTOP, &Object) ||
		kexFindObjectHandle(Process, gpdeskScreenSaver, K32OBJ_DESKTOP, &Object)) && Process != MprProcess)
	{
		ERR("Process 0x%X is associated with a secured desktop !!\n", Process);
		kexDereferenceObject(DesktopObject);
		ReleaseWin16Lock();
		return FALSE;
	}

    if(InputWindowStation != NULL)
    {
		/* Don't allow desktop switch if the input window station is locked and the process
		   isn't mpr process */
        if(InputWindowStation->Flags & WSS_LOCKED && GetCurrentProcessId() != gpidMpr)
        {
			SetLastError(ERROR_ACCESS_DENIED);
			ERR("Switching to desktop 0x%X denied because the current window station is locked !\n", hDesktop);
			kexDereferenceObject(DesktopObject);
			kexDereferenceObject(WindowStationObject);
            ReleaseWin16Lock();
            return FALSE;
        }

		InputWindowStation->ActiveDesktop = NULL; /* Should it be NULL ? */
    }

	/* FIXME: Disable DirectDraw while switching desktop */
	/* FIXME: Should we use input device ? */

	TRACE("Switching to desktop object 0x%X\n", DesktopObject);
	fParent = (DesktopObject->rpwinstaParent == InputWindowStation);

	if(fParent == FALSE)
	{
		TRACE_OUT("new desktop's parent window station is different than the current one, Disabling OEM layer...\n");
		DisableOEMLayer(); /* FIXME: We should have a reason of calling this */
	}

	pdev = DesktopObject->pdev;
	polddev = gpdeskInputDesktop == NULL ? DesktopObject->pdev : gpdeskInputDesktop->pdev;

	/* Set the global state */
	gpdeskInputDesktop = DesktopObject;

	/* Set the current window station to the desktop's parent */
	InputWindowStation = gpdeskInputDesktop->rpwinstaParent;

	/* Set the window station's active desktop */
	InputWindowStation->ActiveDesktop = gpdeskInputDesktop;

	if(pdev->dmBitsPerPel != polddev->dmBitsPerPel || pdev->dmPelsWidth != polddev->dmPelsWidth
		|| pdev->dmPelsHeight != polddev->dmPelsHeight || pdev->dmDisplayFlags != polddev->dmDisplayFlags
		|| pdev->dmDisplayFrequency != polddev->dmDisplayFrequency || pdev->dmPosition.x != polddev->dmPosition.x
		|| pdev->dmPosition.y != polddev->dmPosition.y)
	{
		/* Set the new display setting if the new desktop's display setting is different than the current one */
		TRACE("DesktopObject 0x%X has different display setting than the input desktop, changing display settings...\n", DesktopObject);
		ChangeDisplaySettings(DesktopObject->pdev, CDS_UPDATEREGISTRY);
	}

	if(fParent == FALSE)
	{
		TRACE_OUT("Re-enabling OEM layer...\n");
		EnableOEMLayer();
		SetCursorPos_nothunk(GetSystemMetrics(SM_CXSCREEN)/2, GetSystemMetrics(SM_CYSCREEN)/2);
	}

	TRACE("Switching to desktop 0x%X successful\n", hDesktop);

	kexDereferenceObject(DesktopObject);
	kexDereferenceObject(WindowStationObject);

	ReleaseWin16Lock();

	InterlockedExchange((volatile long *)&fNewDesktop, TRUE);

	/* Signal the desktop switch event */
	hEvent = OpenEvent(EVENT_MODIFY_STATE, FALSE, "WinSta0_DesktopSwitch");

	if(hEvent != NULL)
	{
		SetEvent(hEvent);
		ResetEvent(hEvent);
		CloseHandle(hEvent);
	}

	return TRUE;
}