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

#include "desktop.h"

DWORD dwKernelProcessId = 0;
PPDB98 pKernelProcess = NULL;
PWINSTATION_OBJECT InputWindowStation = NULL;
LIST_ENTRY WindowStationList;

BOOL InitProcessWindowStation(VOID)
{
	PPDB98 Process;

	kexGrabLocks();

	Process = get_pdb();

	if(Process->Win32Process->rpwinsta != NULL)
	{
		kexReleaseLocks();
		return TRUE;
	}

	kexReleaseLocks();
	return TRUE;
}

BOOL IntValidateWindowStationHandle(HWINSTA hWindowStation, PWINSTATION_OBJECT *WindowStationObject)
{
	PWINSTATION_OBJECT Object;

	Object = (PWINSTATION_OBJECT)kexGetHandleObject(hWindowStation, K32OBJ_WINSTATION, 0);

	if(Object == NULL)
		return FALSE;

	*WindowStationObject = Object;

	return TRUE;
}
 
/* MAKE_EXPORT CloseWindowStation_new=CloseWindowStation */
BOOL WINAPI CloseWindowStation_new(HWINSTA hWinSta)
{
	PWINSTATION_OBJECT WindowStationObject = NULL;
	PPROCESSINFO ppi = NULL;
	BOOL result = FALSE;

	// Make sure the object type is K32OBJ_WINSTATION
	if(!IntValidateWindowStationHandle(hWinSta, &WindowStationObject))
		return FALSE;

	ppi = get_pdb()->Win32Process;

	if(ppi == NULL)
	{
		SetLastError(ERROR_ACCESS_DENIED);
		return FALSE;
	}

	/* Fail if the process is using the window station */
	if(ppi->rpwinsta == WindowStationObject)
	{
		SetLastError(ERROR_ACCESS_DENIED);
		return FALSE;
	}

    result = CloseHandle(hWinSta);

	if(WindowStationObject->cReferences < 1)
	{
		if(RemoveEntryList(&WindowStationObject->ListEntry))
			kexFreeObject(WindowStationObject);
	}

	return result;
}

/* MAKE_EXPORT CreateWindowStationA_new=CreateWindowStationA */
HWINSTA WINAPI CreateWindowStationA_new(LPCSTR lpwinsta, DWORD dwFlags, ACCESS_MASK dwDesiredAccess, LPSECURITY_ATTRIBUTES lpsa)
{
    PCHAR WindowStationName = NULL;
    PWINSTATION_OBJECT WindowStationObject = NULL;
    HWINSTA WindowStation = NULL;
    PCHAR WindowStationPath;
    PPDB98 Process = NULL;
	DWORD flags;

	if(IsBadStringPtr(lpwinsta, -1))
		return NULL;

    kexGrabWin16Lock();
    Process = get_pdb();

	if(!IsBadReadPtr(lpsa, sizeof(SECURITY_ATTRIBUTES)) && lpsa->bInheritHandle == TRUE)
		flags |= HF_INHERIT;

	if(dwDesiredAccess & GENERIC_ALL)
		dwDesiredAccess = WINSTA_ALL_ACCESS;
	else if(dwDesiredAccess & GENERIC_EXECUTE)
		dwDesiredAccess = WINSTA_EXECUTE;
	else if(dwDesiredAccess & GENERIC_READ)
		dwDesiredAccess = WINSTA_READ;
	else if(dwDesiredAccess & GENERIC_WRITE)
		dwDesiredAccess = WINSTA_WRITE;

	/* Try to open an existing window station */
	if((WindowStation = (HWINSTA)OpenWindowStationA_new((LPTSTR)lpwinsta, flags & HF_INHERIT, dwDesiredAccess)) != NULL)
	{
		SetLastError(ERROR_ALREADY_EXISTS);
		kexReleaseWin16Lock();
		return WindowStation;
	}

	WindowStationObject = (PWINSTATION_OBJECT)kexAllocObject(sizeof(WINSTATION_OBJECT));

	if(WindowStationObject == NULL)
	{
		SetLastError(ERROR_NOT_ENOUGH_MEMORY);
		kexReleaseWin16Lock();
		return NULL;
	}

	WindowStationName = (PCHAR)lpwinsta;

	WindowStationPath = (PCHAR)kexAllocObject(strlen(WINSTA_ROOT_NAME) + strlen(WindowStationName) + 1);

	sprintf(WindowStationPath, "%s\\%s", WINSTA_ROOT_NAME, WindowStationName);

	InitializeListHead(&WindowStationObject->DesktopListHead);
	InitializeListHead(&WindowStationObject->ListEntry);

	InsertTailList(&WindowStationList, &WindowStationObject->ListEntry);

    WindowStationObject->Type = K32OBJ_WINSTATION;

    WindowStationObject->SystemMenuTemplate = (HANDLE)0;
    WindowStationObject->pName = kexAllocObjectName(WindowStationObject, WindowStationPath);
	WindowStationObject->lpName = (PCHAR)WindowStationName;

	if(InputWindowStation == NULL)
		InputWindowStation = WindowStationObject;

	WindowStation = (HWINSTA)kexAllocHandle(Process, WindowStationObject, dwDesiredAccess | flags);

	/* Create the desktop switch event */
	CreateEvent(NULL, FALSE, FALSE, "WinSta0_DesktopSwitch");

    kexReleaseWin16Lock();

    return WindowStation;
}

/* MAKE_EXPORT EnumWindowStationsA_new=EnumWindowStationsA */
BOOL WINAPI EnumWindowStationsA_new(WINSTAENUMPROCA lpEnumFunc, LPARAM lParam)
{
	PWINSTATION_OBJECT WindowStationObject;
	PLIST_ENTRY WinStaList;

	if(IsBadCodePtr((FARPROC)lpEnumFunc))
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	for(WinStaList = WindowStationList.Flink; WinStaList != &WindowStationList; WinStaList = WinStaList->Flink)
	{
		// FIXME: Check WindowStationObject's access right for WINSTA_ENUMERATE
		WindowStationObject = CONTAINING_RECORD(WinStaList, WINSTATION_OBJECT, ListEntry);

		if(!(*lpEnumFunc)(WindowStationObject->lpName, lParam))
			return FALSE;
	}

	return TRUE;
}

/* MAKE_EXPORT GetProcessWindowStation_new=GetProcessWindowStation */
HWINSTA WINAPI GetProcessWindowStation_new(void)
{
	PPDB98 Process;

	Process = get_pdb();

	return Process->Win32Process->hwinsta;
}

/* MAKE_EXPORT GetUserObjectInformationA_new=GetUserObjectInformationA */
BOOL WINAPI GetUserObjectInformationA_new(HANDLE hObj, int nIndex, PVOID pvInfo, DWORD nLength, LPDWORD lpnLengthNeeded)
{
	PDESKTOP Desktop = NULL;
	PWINSTATION_OBJECT WindowStation = NULL;
	PVOID pvData = NULL;
	SIZE_T nSize = 0;
	BOOL result = FALSE;
	USEROBJECTFLAGS uof;

	if(!IntValidateWindowStationHandle((HWINSTA)hObj, &WindowStation))
	{
		if(!IntValidateDesktopHandle((HDESK)hObj, &Desktop))
			return FALSE;
	}

	switch(nIndex)
	{
		case UOI_FLAGS:
			nSize = sizeof(USEROBJECTFLAGS);
			if(IsBadReadPtr(pvInfo, sizeof(USEROBJECTFLAGS)))
			{
				result = FALSE;
				SetLastError(ERROR_INVALID_PARAMETER);
				goto Cont;
			}
			memset(&uof, 0, sizeof(USEROBJECTFLAGS));
			uof.fInherit = (kexGetHandleAccess(hObj) & HF_INHERIT) ? TRUE : FALSE;
			uof.fReserved = FALSE;
			if(WindowStation != NULL)
			{
				/* Currently, WindowStation are always visible */
				uof.dwFlags = WSF_VISIBLE;
			}
			else
			{
				/* DF_ALLOWOTHERACCOUNTHOOK is (currently) not supported */
				uof.dwFlags = 0;
			}
			break;

		case UOI_HEAPSIZE:
			/* Should we return USER32's heap size ? */
			SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
			result = FALSE;
			break;

		case UOI_NAME:
			if(WindowStation != NULL)
			{
				pvData = WindowStation->lpName;
				/* Include the null-terminated string */
				nSize = strlen(WindowStation->lpName) + sizeof(CHAR);
				result = TRUE;
			}
			else
			{
				pvData = Desktop->lpName;
				nSize = strlen(Desktop->lpName) + sizeof(CHAR);
				result = TRUE;
			}
			break;

		case UOI_TYPE:
			if (WindowStation != NULL)
			{
				pvData = "WindowStation";
				nSize = strlen((const char*)pvData);
				result = TRUE;
			}
			else
			{
				pvData = "Desktop";
				nSize = strlen((const char*)pvData);
				result = TRUE;
			}
			break;

		case UOI_USER_SID:
			break;

		default:
			SetLastError(ERROR_INVALID_PARAMETER);
			return FALSE;
			break;
	}

Cont:
	if(result)
	{
		/* Copy the data to pvInfo */
		if(nLength > nSize)
			memcpy(pvInfo, pvData, nSize);
		else
		{
			SetLastError(ERROR_INSUFFICIENT_BUFFER);
			result = FALSE;
		}
	}

	if(!IsBadReadPtr(lpnLengthNeeded, sizeof(DWORD)))
		*lpnLengthNeeded = nSize;

	return result;
}

/* MAKE_EXPORT GetUserObjectSecurity_new=GetUserObjectSecurity */
BOOL WINAPI GetUserObjectSecurity_new(HANDLE hObj, PSECURITY_INFORMATION pSIRequested, PSECURITY_DESCRIPTOR pSD, DWORD nLength,  LPDWORD lpnLengthNeeded)
{
	SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
	return FALSE;
}

/* MAKE_EXPORT LockWindowStation_new=LockWindowStation */
BOOL WINAPI LockWindowStation_new(HWINSTA hWinSta)
{
	/* always return FALSE because there is no logon process */
	SetLastError(ERROR_ACCESS_DENIED);
	return FALSE;
}

/* MAKE_EXPORT OpenWindowStationA_new=OpenWindowStationA */
HWINSTA WINAPI OpenWindowStationA_new(LPSTR lpszWinSta, BOOL fInherit, ACCESS_MASK dwDesiredAccess)
{
	HWINSTA WindowStation = NULL;
	CHAR WindowStationPath[MAX_PATH];
	PCHAR WindowStationName = NULL;
	DWORD flags = 0;

	if(IsBadStringPtr(lpszWinSta, -1))
		return NULL;

	if(fInherit)
		flags |= HF_INHERIT;

	WindowStationName = lpszWinSta;

	sprintf(WindowStationPath, "%s\\%s", WINSTA_ROOT_NAME, WindowStationName);

	Sleep(1); // Sounds useless, but without this, this function become buggy...

	WindowStation = (HWINSTA)kexOpenObjectByName(WindowStationPath, K32OBJ_WINSTATION, dwDesiredAccess | flags);

	if(WindowStation == NULL)
	{
		SetLastError(ERROR_INVALID_HANDLE);
		return NULL;
	}

	return WindowStation;
}

/* MAKE_EXPORT SetProcessWindowStation_new=SetProcessWindowStation */
BOOL WINAPI SetProcessWindowStation_new(HWINSTA hWinSta)
{
	PPDB98 Process;
	PWINSTATION_OBJECT WindowStationObject;

	Process = get_pdb();

	if(!IntValidateWindowStationHandle(hWinSta, &WindowStationObject))
	{
		SetLastError(ERROR_INVALID_HANDLE);
		return FALSE;
	}

    Process->Win32Process->rpwinsta = WindowStationObject;
    Process->Win32Process->hwinsta = hWinSta;

	return TRUE;
}

/* MAKE_EXPORT SetUserObjectInformationA_new=SetUserObjectInformationA */
BOOL WINAPI SetUserObjectInformationA_new(HANDLE hObj, int nIndex, PVOID pvInfo, DWORD nLength)
{
	return FALSE;
}

/* MAKE_EXPORT SetUserObjectSecurity_new=SetUserObjectSecurity */
BOOL WINAPI SetUserObjectSecurity_new(HANDLE hObj, PSECURITY_INFORMATION pSIRequested, PSECURITY_DESCRIPTOR pSD)
{
	return FALSE;
}

/* MAKE_EXPORT UnlockWindowStation_new=UnlockWindowStation */
BOOL WINAPI UnlockWindowStation_new(HWINSTA hWinSta)
{
	SetLastError(ERROR_ACCESS_DENIED);
	return FALSE;
}