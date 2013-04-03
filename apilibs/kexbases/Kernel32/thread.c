/*
 *  KernelEx
 *  Copyright (C) 2009, Xeno86
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

#include <windows.h>
#include "common.h"
#include "_kernel32_apilist.h"

/* MAKE_EXPORT CreateFiberEx_new=CreateFiberEx */
LPVOID WINAPI CreateFiberEx_new(
  SIZE_T dwStackCommitSize,
  SIZE_T dwStackReserveSize,
  DWORD dwFlags,
  LPFIBER_START_ROUTINE lpStartAddress,
  LPVOID lpParameter
)
{
	return CreateFiber( dwStackCommitSize ? dwStackCommitSize : dwStackReserveSize, lpStartAddress, lpParameter );
}

/* MAKE_EXPORT CreateRemoteThread_new=CreateRemoteThread*/
HANDLE WINAPI CreateRemoteThread_new(HANDLE hProcess,
	LPSECURITY_ATTRIBUTES lpThreadAttributes,
	SIZE_T dwStackSize,
	LPTHREAD_START_ROUTINE lpStartAddress,
	LPVOID lpParameter,
	DWORD dwCreationFlags,
	LPDWORD lpThreadId
)
{
    PPDB98  Process;
    PTDB98  Thread = NULL;
    DWORD   dwThreadId, dwProcessId;
    HANDLE  hThread = NULL;
    BOOL    bInheritHandle = FALSE;
    DWORD   fFlags = fGrowableStack;
    DWORD   StackSize = 0;
	DWORD	dwObsfucator = (DWORD)get_tdb() ^ GetCurrentThreadId();

    if (lpThreadAttributes != NULL)
    {
        if (lpThreadAttributes->nLength != sizeof(SECURITY_ATTRIBUTES))
            return NULL;

        bInheritHandle = lpThreadAttributes->bInheritHandle;
    }

    if (!(dwProcessId = GetProcessId_new(hProcess)))
        return NULL;

    if (!(Process = (PPDB98)kexGetProcess(dwProcessId)))
        return NULL;

	if(Process == get_pdb())
		return CreateThread_fix(lpThreadAttributes, dwStackSize, lpStartAddress, lpParameter, dwCreationFlags, lpThreadId);

	/* Make sure the current process isn't terminating */
    if (Process->Flags & INVALID_FLAGS)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return NULL;
    }

	if (dwStackSize == 0)
		StackSize = -(int)0x3000;
	else
		StackSize = -(int)dwStackSize;

	fFlags |= fCreateSuspended;

	if (dwCreationFlags & CREATE_SILENT)
		fFlags |= 0x10; // Suppress DLL_THREAD_ATTACH notification if the process is not initialized

	kexGrabLocks();

	Thread = kexCreateRemoteThread(Process,
								StackSize,
								lpStartAddress,
								lpParameter,
								fFlags);

	kexReleaseLocks();

	dwThreadId = (DWORD)Thread ^ dwObsfucator;

	if (!IsBadWritePtr(lpThreadId, sizeof(DWORD)))
		*lpThreadId = dwThreadId;

	hThread = OpenThread_new(THREAD_ALL_ACCESS, bInheritHandle, dwThreadId);

	if (!(dwCreationFlags & CREATE_SUSPENDED))
		ResumeThread(hThread);

    return hThread;
}


/* MAKE_EXPORT CreateThread_fix=CreateThread */
HANDLE WINAPI CreateThread_fix(
	LPSECURITY_ATTRIBUTES lpThreadAttributes, // SD
	SIZE_T dwStackSize,                       // initial stack size
	LPTHREAD_START_ROUTINE lpStartAddress,    // thread function
	LPVOID lpParameter,                       // thread argument
	DWORD dwCreationFlags,                    // creation option
	LPDWORD lpThreadId                        // thread identifier
)
{
	DWORD dummy;

	if (IsBadWritePtr(lpThreadId, sizeof(DWORD)))
		lpThreadId = &dummy;

	return CreateThread(lpThreadAttributes, dwStackSize, lpStartAddress, lpParameter, dwCreationFlags, lpThreadId);
}

/* MAKE_EXPORT GetProcessIdOfThread_new=GetProcessIdOfThread */
DWORD WINAPI GetProcessIdOfThread_new(HANDLE hThread)
{
	PTDB98 Thread = (PTDB98)kexGetHandleObject(hThread, WIN98_K32OBJ_THREAD, 0);
	PPDB98 Process = NULL;
	DWORD Obfuscator = (DWORD)get_pdb() ^ GetCurrentProcessId();

	if(Thread == NULL)
		return 0;

	Process = (PPDB98)Thread->tib.pProcess;

	if(Process == NULL)
		return 0; /* a thread without a process ?!?!*/

	return (DWORD)Process ^ Obfuscator;
}

/* MAKE_EXPORT GetThreadId_new=GetThreadId */
DWORD WINAPI GetThreadId_new(HANDLE hThread)
{
	PTDB98 Thread = (PTDB98)kexGetHandleObject(hThread, WIN98_K32OBJ_THREAD, 0);
	DWORD Obfuscator = (DWORD)get_tdb() ^ GetCurrentThreadId();

	if(Thread == NULL)
		return 0;

	return (DWORD)Thread ^ Obfuscator;
}

/* MAKE_EXPORT GetThreadTimes_new=GetThreadTimes */
BOOL WINAPI GetThreadTimes_new(
	HANDLE hThread,
	LPFILETIME lpCreationTime,
	LPFILETIME lpExitTime,
	LPFILETIME lpKernelTime,
	LPFILETIME lpUserTime
)
{
	PTDB98 Thread = (PTDB98)kexGetHandleObject(hThread, WIN98_K32OBJ_THREAD, 0);

	if(Thread == NULL)
	{
		SetLastError(ERROR_INVALID_HANDLE);
		return FALSE;
	}

	if(Thread->Win32Thread == NULL)
	{
		SetLastError(ERROR_BUSY);
		return FALSE;
	}

	if(IsBadWritePtr(lpCreationTime, sizeof(FILETIME)) || IsBadWritePtr(lpExitTime, sizeof(FILETIME))
		|| IsBadWritePtr(lpKernelTime, sizeof(FILETIME)) || IsBadWritePtr(lpUserTime, sizeof(FILETIME)))
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	*lpCreationTime = Thread->Win32Thread->CreationTime;
	*lpExitTime = Thread->Win32Thread->ExitTime;
	*lpKernelTime = Thread->Win32Thread->KernelTime;
	*lpUserTime = Thread->Win32Thread->UserTime;

	return TRUE;
}

/* MAKE_EXPORT OpenThread_new=OpenThread */
HANDLE WINAPI OpenThread_new(DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwThreadId)
{
	return kexOpenThread(dwDesiredAccess, bInheritHandle, dwThreadId);
}
