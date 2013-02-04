/*
 *  KernelEx
 *
 *  Copyright (C) 2010, Xeno86
 *  Copyright (C) 2008, Tihiy
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

#include "common.h"
#include "_kernel32_apilist.h"

DWORD WINAPI AllocConsole_caller(PVOID lParam)
{
	DWORD ret;

	ret = AllocConsole();
	*(DWORD*)lParam = GetLastError();

	return ret;
}

/* MAKE_EXPORT AttachConsole_new=AttachConsole */
BOOL WINAPI AttachConsole_new(DWORD dwProcessId)
{
	PPDB98 Process = NULL;
	HANDLE hProcess = NULL;
	HANDLE hThread = NULL;
	DWORD Obfuscator = 0;
	DWORD dwExitCode = 0;
	DWORD dwLastError = 0;

	if(dwProcessId != ATTACH_PARENT_PROCESS)
		Process = (PPDB98)kexGetProcess(dwProcessId);
	else
	{
		/* If dwProcessId is ATTACH_PARENT_PROCESS, get the parent PID and PDB */
		Process = get_pdb()->ParentPDB;
		Obfuscator = (DWORD)get_pdb() ^ GetCurrentProcessId();
		dwProcessId = (DWORD)Process ^ Obfuscator;
	}

	if(Process == NULL)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	if(Process->Flags & INVALID_FLAGS)
	{
		TRACE("Error, trying to create a console on a terminating/terminated process (%p) !\n", Process);
		SetLastError(ERROR_BUSY); // Which error to set in this way?
		return FALSE;
	}

	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);

	if(hProcess == NULL)
		return FALSE;

	/* Now create a remote thread that will allocate a console for this process */
	hThread = CreateRemoteThread_new(hProcess,
							NULL,
							0,
							AllocConsole_caller,
							(LPVOID)&dwLastError,
							0,
							0);

	if(hThread == NULL)
		return FALSE;

	WaitForSingleObject(hThread, INFINITE);
	GetExitCodeThread(hThread, &dwExitCode);

	SetLastError(dwLastError);

	CloseHandle(hThread);

	return dwExitCode;
}