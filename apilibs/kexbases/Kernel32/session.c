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

#include "../user32/desktop.h"
#include "../advapi32/security.h"

PPERPROCESSDATA gpdcs = NULL;

LIST_ENTRY SessionList;

/* Internal functions */

BOOL FASTCALL WTSInitializeSession(void)
{
	if(gpdcs != NULL)
		return FALSE;

	InitializeListHead(&SessionList);

	return WTSCreateSession(&gpdcs);
}

BOOL WINAPI WTSCreateSession(PPERPROCESSDATA *Session)
{
	PPERPROCESSDATA Object;
	CHAR UserName[255];
	DWORD dwSize;
	BOOL result;

	if(IsBadWritePtr(Session, sizeof(DWORD)))
		return FALSE;

	Object = (PPERPROCESSDATA)kexAllocObject(sizeof(PERPROCESSDATA));

	if(Object == NULL)
		return FALSE;

	InitializeListHead(&Object->ListEntry);
	InsertTailList(&SessionList, &Object->ListEntry);

	dwSize = sizeof(UserName);

	result = GetUserName(UserName, &dwSize);

	if(result)
	{
		Object->lpSessionName = (LPSTR)kexAllocObject(strlen(UserName));
		strcpy(Object->lpSessionName, UserName);
	}

	*Session = Object;

	return TRUE;
}

BOOL WINAPI WTSGetSessionById(DWORD SessionId, PPERPROCESSDATA *Session)
{
	if(IsBadReadPtr(Session, sizeof(DWORD)))
		return FALSE;

	return TRUE;
}

BOOL WINAPI WTSDeleteSession(PPERPROCESSDATA Session)
{
	if(IsBadReadPtr(Session, sizeof(PERPROCESSDATA)))
		return FALSE;

	RemoveEntryList(&Session->ListEntry);

	kexFreeObject(Session);

	return TRUE;
}

BOOL WINAPI WTSSetProcessSessionId(DWORD dwProcessId, DWORD SessionId)
{
	PPERPROCESSDATA Object;
	PPDB98 Process;
	PPROCESSINFO ppi;

	if(dwProcessId != 0)
	{
		Process = (PPDB98)kexGetProcess(dwProcessId);

		if(Process == NULL || Process->Win32Process == NULL)
			return FALSE;
	}
	else
		Process = get_pdb();

	ppi = Process->Win32Process;

	if(!WTSGetSessionById(SessionId, &Object))
		return FALSE;

	ppi->SessionId = SessionId;

	return TRUE;
}

BOOL WINAPI WTSSwitchToSession(PPERPROCESSDATA Session)
{
	if(IsBadReadPtr(Session, sizeof(PERPROCESSDATA)))
		return FALSE;

	DisableOEMLayer();

	Session->fNewDesktop = TRUE;
	gpdcs = Session;

	EnableOEMLayer();

	IntSwitchUser(Session->lpSessionName, FALSE, 0, FALSE);

	SetCursorPos_nothunk(GetSystemMetrics(SM_CXSCREEN)/2, GetSystemMetrics(SM_CYSCREEN)/2);

	return TRUE;
}

/* Exports */

/* MAKE_EXPORT WTSGetActiveConsoleSessionId_new=WTSGetActiveConsoleSessionId */
DWORD WINAPI WTSGetActiveConsoleSessionId_new(void)
{
	/* local session */
	return 0;
}
