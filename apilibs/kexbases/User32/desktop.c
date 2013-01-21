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

PDESKTOP gpdeskInputDesktop = NULL;
PTDB98 pDesktopThread = NULL;
DWORD dwDesktopThreadId = NULL;

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

BOOL WINAPI CreateWindowStationAndDesktops()
{
	SECURITY_ATTRIBUTES sa;
	HDESK hDesktop;
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

	hDesktop = CreateDesktopA_new("Default", NULL, NULL, 0, GENERIC_ALL, &sa);

	if(hDesktop == NULL)
		return FALSE;

	if(!SetThreadDesktop_new(hDesktop))
		return FALSE;

	if(!SwitchDesktop_new(hDesktop))
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

    //kexGrabLocks();

	Process = get_pdb();

	ppi = Process->Win32Process;
	pti = get_tdb()->Win32Thread;

	//MessageBox(NULL, "1", NULL, 0);

	/* Don't assign desktops and window stations to process/threads
	   if there is not input desktop set yet */
    if(gpdeskInputDesktop == NULL)
	{
		//kexReleaseLocks();
        return TRUE;
	}

	//MessageBox(NULL, "2", NULL, 0);

	/* Found a startup desktop, assign the current thread to it */
	if(ppi->hdeskStartup != NULL)
	{
		SetThreadDesktop_new(ppi->hdeskStartup);
		//kexReleaseLocks();
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

	if(DesktopPath != NULL && strlen(DesktopPath) > sizeof(CHAR))
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
			//pszWinSta = pch;
			//pch = strtok(NULL, "\\");
			//pszDesktop = pch;
		}
		else
		{
			/* Only the desktop name will be used */
			pszWinSta = NULL;
			pszDesktop = DesktopPath;
		}
	}

	//MessageBox(NULL, "4", NULL, 0);
	if(!kexFindObjectHandle(Process,
							NULL,
							K32OBJ_WINSTATION,
							(PHANDLE)&hWindowStation))
	{
		if(pszWinSta == NULL)
			pszWinSta = "WinSta0";
	}

	//MessageBox(NULL, "5", NULL, 0);
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
		//sprintf(Path, "%s\\%s", WINSTA_ROOT_NAME, pszWinSta);
		hWindowStation = OpenWindowStationA_new(pszWinSta, FALSE, WINSTA_ALL_ACCESS); //(HWINSTA)kexOpenObjectByName(Path, K32OBJ_WINSTATION, WINSTA_ALL_ACCESS);
		//MessageBox(NULL, pszWinSta, NULL, 0);

		if(hWindowStation == NULL)
			goto error;

		if(!IntValidateWindowStationHandle(hWindowStation, &WindowStationObject))
			goto error;

		/* Each process runing on Windows NT always have 2 window station handles refering
		   to the same window station */
		kexAllocHandle(Process, WindowStationObject, WINSTA_ALL_ACCESS);
	}

	if(!SetProcessWindowStation_new(hWindowStation))
		goto error;

	if(hDesktop == NULL)
	{
		//sprintf(Path, "\\%s", pszDesktop);
		hDesktop = OpenDesktopA_new(pszDesktop, 0, FALSE, DESKTOP_ALL_ACCESS); //(HDESK)kexOpenObjectByName(Path, K32OBJ_DESKTOP, DESKTOP_ALL_ACCESS);
		//MessageBox(NULL, pszDesktop, NULL, 0);

		if(hDesktop == NULL)
			goto error;

	}

	//MessageBox(NULL, "8", NULL, 0);
	if(!IntValidateDesktopHandle(hDesktop, &DesktopObject))
		goto error;

	//MessageBox(NULL, "9", NULL, 0);

	//MessageBox(NULL, "10", NULL, 0);
	if(!SetThreadDesktop_new(hDesktop))
		goto error;

	ppi->rpdeskStartup = DesktopObject;
	ppi->hdeskStartup = hDesktop;

	free((PVOID)DesktopPath);
	//MessageBox(NULL, "11", NULL, 0);
    //kexReleaseLocks();
    return TRUE;
error:
	//MessageBox(NULL, "ERR", NULL, 0);
	if(hDesktop != NULL)
		CloseHandle(hDesktop);
	if(hWindowStation != NULL)
		CloseHandle(hWindowStation);
	if(DesktopPath != NULL)
		free((PVOID)DesktopPath);
	//kexReleaseLocks();
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

	/* FIXME: Should we dereference the object's count after finished using it ? */

	return TRUE;
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	PWND pwnd;
	DWORD dwThreadId;
	DWORD dwProcessId;
	PTDB98 Thread;
	PPDB98 Process;
	PTHREADINFO pti;
	PPROCESSINFO ppi;

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
	}

	if(pti == NULL)
	{
		pti_init(Thread);
		/* Set the thread's desktop to the process's startup desktop */
		pti = Thread->Win32Thread;
		pti->rpdesk = ppi->rpdeskStartup;
		pti->hdesk = ppi->hdeskStartup;
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
			SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_ASYNCWINDOWPOS | SWP_HIDEWINDOW | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);

			/* In case the thread is hung, manually remove the WS_VISIBLE flag */
			if(IsHungThread_pfn(dwThreadId))
				pwnd->style &= ~WS_VISIBLE;
		}
	}
	else
	{
		/* Window's thread is in the input desktop, show it and update it if necessary */
		if(pwnd->style & WS_INTERNAL_WASVISIBLE)
		{
			pwnd->style &= ~WS_INTERNAL_WASVISIBLE;
			SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_ASYNCWINDOWPOS | SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);

			if(IsHungThread_pfn(dwThreadId))
				pwnd->style |= WS_VISIBLE;
		}
	}

	return TRUE;
}

DWORD WINAPI DesktopThread(PVOID lParam)
{
	MSG msg;

	pDesktopThread = get_tdb();
	pKernelProcess = get_pdb();
	dwDesktopThreadId = GetCurrentThreadId();
	dwKernelProcessId = GetCurrentProcessId();

	/* Prevent the kernel process from being terminated by adding the terminating flag */
	pKernelProcess->Flags |= fTerminating;

	while(1)
	{
		PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE);
		//GetMessage(&msg, NULL, 0, 0);
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		kexGrabLocks(); // Don't get interrupted while we are switching desktop/hiding some windows

		/* We don't want our desktop thread crash, so we safely use
		exception handling, crash happen when there is no free memory */
		__try
		{
			/* By using the nothunk version of EnumWindows, it saves the CPU consumption up to 10% */
			EnumWindows_nothunk(EnumWindowsProc, 0);
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
		}

		kexReleaseLocks();

		Sleep(1);
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

	// Make sure the object type is K32OBJ_DESKTOP
	if(!IntValidateDesktopHandle(hDesktop, &DesktopObject))
	{
		SetLastError(ERROR_ACCESS_DENIED);
		return FALSE;
	}

	pti = get_tdb()->Win32Thread;
	ppi = get_pdb()->Win32Process;

	if(pti == NULL || ppi == NULL)
		return FALSE;

	/* Fail if the desktop is the startup desktop */
	if(ppi->rpdeskStartup == DesktopObject)
	{
		SetLastError(ERROR_ACCESS_DENIED);
		return FALSE;
	}

	kexEnumThreads(GetCurrentProcessId(), pThread, sizeof(pThread), &cbThreads);

	cbThreads /= sizeof(DWORD);

	/* Now check if any threads is using the specified desktop handle */
	for(index=0;index<=cbThreads;index++)
	{
		Thread = (PTDB98)kexGetThread(pThread[index]);

		if(Thread == NULL)
			continue;

		pti = Thread->Win32Thread;

		if(pti->rpdesk == DesktopObject)
		{
			SetLastError(ERROR_ACCESS_DENIED);
			return FALSE;
		}
	}

	result = CloseHandle(hDesktop);

	if(DesktopObject->cReferences < 1)
	{
		if(RemoveEntryList(&DesktopObject->ListEntry))
			kexFreeObject(DesktopObject);
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
	DesktopObject->pName = kexAllocObjectName(DesktopObject, DesktopPath);
	DesktopObject->lpName = (PCHAR)DesktopName;
	DesktopObject->rpwinstaParent = Process->Win32Process->rpwinsta;
	DesktopObject->pdev = pdev;

	hDesktop = (HDESK)kexAllocHandle(Process, DesktopObject, dwDesiredAccess | flags);

	ReleaseWin16Lock();
    return hDesktop;
}

/* MAKE_EXPORT CreateDesktopExA_new=CreateDesktopExA*/
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
		return FALSE;
	}

	for(DesktopList = WindowStationObject->DesktopListHead.Flink; DesktopList != &WindowStationObject->DesktopListHead; DesktopList = DesktopList->Flink)
	{
		// FIXME: Check DesktopObject's access right for DESKTOP_ENUMERATE
		DesktopObject = CONTAINING_RECORD(DesktopList, DESKTOP, ListEntry);

		if(!(*lpEnumFunc)(DesktopObject->lpName, lParam))
			return FALSE;
	}

    return TRUE;
}

/* MAKE_EXPORT EnumDesktopWindows_new=EnumDesktopWindows */
BOOL WINAPI EnumDesktopWindows_new(HDESK hDesktop, WNDENUMPROC lpfn, LPARAM lParam)
{
	PDESKTOP DesktopObject;

	if(IsBadCodePtr((FARPROC)lpfn))
		return FALSE;

	if(!IntValidateDesktopHandle(hDesktop, &DesktopObject))
		return FALSE;

	return EnumWindowsEx(0, lpfn, lParam, FALSE, hDesktop, TRUE);

#if 0
	__try
	{
		for(; pWnd != NULL; pWnd = (PWND)REBASEUSER(pWnd->spwndNext))
		{
			pQueue = GetWindowQueue(pWnd);

			if(pQueue == NULL)
				continue;

			Thread = (PTDB98)kexGetThread(pQueue->threadId);
			Process = (PPDB98)kexGetProcess(pQueue->npProcess);


			if(Thread == NULL || Process == NULL)
				continue;

			/* Skip the kernel process because the application can maliciously terminate it */
			if(Process == pKernelProcess)
				continue;

			pti = Thread->Win32Thread;

			if(pti == NULL)
				continue;

			if(pti->rpdesk != DesktopObject)
				continue;

			if(!(*lpfn)((HWND)pWnd->hWnd16, lParam))
				return FALSE;
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		return FALSE;
	}

	return TRUE;
#endif
}

/* MAKE_EXPORT GetInputDesktop_new=GetInputDesktop */
HDESK WINAPI GetInputDesktop_new(VOID)
{
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
	PTDB98 Thread;

	Thread = (PTDB98)kexGetThread(dwThreadId);

	if(Thread == NULL)
		return NULL;

	return Thread->Win32Thread->hdesk;
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
		return NULL;

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

	Process = get_pdb();

	if(fInherit)
		flags |= HF_INHERIT;

	/* OpenInputDesktop allocate an handle of the input desktop */
	hDesktop = (HDESK)kexAllocHandle(Process, gpdeskInputDesktop, dwDesiredAccess | flags);

	return NULL;
}

/* MAKE_EXPORT SetThreadDesktop_new=SetThreadDesktop */
BOOL WINAPI SetThreadDesktop_new(HDESK hDesktop)
{
	PTHREADINFO pti;
	PPROCESSINFO ppi;
	PDESKTOP DesktopObject;

	pti = get_tdb()->Win32Thread;
	ppi = get_pdb()->Win32Process;

	if(pti == NULL)
	{
		SetLastError(ERROR_ACCESS_DENIED);
		return FALSE;
	}

	if(pti->hdesk == hDesktop)
		return TRUE;

	if(!IntValidateDesktopHandle(hDesktop, &DesktopObject))
	{
		SetLastError(ERROR_INVALID_HANDLE);
		return FALSE;
	}

	pti->rpdesk = DesktopObject;
	pti->hdesk = hDesktop;

	if(ppi->rpdeskStartup == NULL)
	{
		ppi->rpdeskStartup = DesktopObject;
		ppi->hdeskStartup = hDesktop;
	}

	return TRUE;
}

/* MAKE_EXPORT SwitchDesktop_new=SwitchDesktop */
BOOL WINAPI SwitchDesktop_new(HDESK hDesktop)
{
	// FIXME: There should be a thread looping on every windows and check if the window's thread
	// match the current desktop, if not, hide the window, so 1) should we use a normal thread
	// (in MPREXE) or 2) a KERNEL thread (an endless thread in KERNEL process, there is an API called
	// CreateKernelThread in KERNEL32.DLL), a kernel thread would be better
	// because if MPREXE terminates, everything will blow up

	// FIXME2: We could even use CreateDC API, but where is stored the DC in the _WND structure?

    PDESKTOP DesktopObject = NULL;
    PWINSTATION_OBJECT WindowStationObject = NULL;
    HWINSTA hWinSta = GetProcessWindowStation_new();
	BOOL FirstSwitch = (gpdeskInputDesktop == NULL);
	HANDLE hEvent = NULL;
	BOOL fFirstSwitch = (gpdeskInputDesktop == NULL);

    GrabWin16Lock();

    if(!IntValidateDesktopHandle(hDesktop, &DesktopObject))
    {
		SetLastError(ERROR_INVALID_HANDLE);
        ReleaseWin16Lock();
        return FALSE;
    }

	if(DesktopObject == gpdeskInputDesktop)
	{
		/* Nothing to do */
		ReleaseWin16Lock();
		return TRUE;
	}

	if(!(kexGetHandleAccess(hDesktop) & DESKTOP_SWITCHDESKTOP))
	{
		SetLastError(ERROR_ACCESS_DENIED);
		ReleaseWin16Lock();
		return FALSE;
	}

    if(!IntValidateWindowStationHandle(hWinSta, &WindowStationObject))
    {
		SetLastError(ERROR_INVALID_HANDLE);
        ReleaseWin16Lock();
        return FALSE;
    }

    if(InputWindowStation != NULL)
    {
		// FIXME: MPREXE must be allowed to switch desktop even if the
		// input window station is locked
        if(InputWindowStation->Flags & WSS_LOCKED)
        {
			SetLastError(ERROR_ACCESS_DENIED);
            ReleaseWin16Lock();
            return FALSE;
        }

		InputWindowStation->ActiveDesktop = NULL;
    }

	gpdeskInputDesktop = DesktopObject;

	InputWindowStation = gpdeskInputDesktop->rpwinstaParent;

	InputWindowStation->ActiveDesktop = gpdeskInputDesktop;

	ChangeDisplaySettings(DesktopObject->pdev, CDS_UPDATEREGISTRY);
	RepaintScreen();

	ReleaseWin16Lock();

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