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

#include "_ntdll_apilist.h"

NTSTATUS NTAPI NtSuspendResumeProcess(IN HANDLE ProcessHandle, DWORD dwOperation)
{
	DWORD pThread[1024];
	DWORD cThreads = 0;
	DWORD dwProcessId = 0;
	ULONG index = 0;
	HANDLE hThread = NULL;

	if(dwOperation > 1)
		return STATUS_INVALID_PARAMETER;

	dwProcessId = GetProcessId_new(ProcessHandle);

	if(dwProcessId == 0)
		return STATUS_INVALID_PARAMETER;

	kexEnumThreads(dwProcessId, pThread, sizeof(pThread), &cThreads);

	cThreads /= sizeof(DWORD);

	for(index=0;index<=cThreads;index++)
	{
		hThread = OpenThread_new(THREAD_SUSPEND_RESUME, FALSE, pThread[index]);

		if(dwOperation == 0)
		{
			if(!ResumeThread(hThread))
				goto _fail;
		}
		else if(dwOperation == 1)
		{
			if(!SuspendThread(hThread))
				goto _fail;
		}

		CloseHandle(hThread);
	}

	return STATUS_SUCCESS;
_fail:
	CloseHandle(hThread);
	return STATUS_INVALID_PARAMETER;
}

/* MAKE_EXPORT NtOpenProcess=NtOpenProcess */
/* MAKE_EXPORT NtOpenProcess=ZwOpenProcess */
NTSTATUS NTAPI NtOpenProcess(OUT PHANDLE ProcessHandle,
	IN ACCESS_MASK          AccessMask,
	IN POBJECT_ATTRIBUTES   ObjectAttributes,
	IN PCLIENT_ID           ClientId
)
{
	__try
	{
		*ProcessHandle = OpenProcess(AccessMask, ObjectAttributes->Attributes, ClientId->UniqueProcessId);
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		return STATUS_INVALID_PARAMETER;
	}

	return STATUS_SUCCESS;
}

/* MAKE_EXPORT NtResumeProcess=NtResumeProcess */
/* MAKE_EXPORT NtResumeProcess=ZwResumeProcess */
NTSTATUS NTAPI NtResumeProcess(IN HANDLE ProcessHandle)
{
	return NtSuspendResumeProcess(ProcessHandle, 0);
}

/* MAKE_EXPORT NtSuspendProcess=NtSuspendProcess */
/* MAKE_EXPORT NtSuspendProcess=ZwSuspendProcess */
NTSTATUS NTAPI NtSuspendProcess(IN HANDLE ProcessHandle)
{
	return NtSuspendResumeProcess(ProcessHandle, 1);
}

/* MAKE_EXPORT NtTerminateProcess=NtTerminateProcess */
/* MAKE_EXPORT NtTerminateProcess=ZwTerminateProcess */
NTSTATUS NTAPI NtTerminateProcess(IN HANDLE ProcessHandle OPTIONAL,
	IN NTSTATUS ExitStatus
)
{
	BOOL result;

	if(ProcessHandle == NULL)
		ExitProcess(ExitStatus);

	result = TerminateProcess(ProcessHandle, ExitStatus);

	if(!result)
		return STATUS_INVALID_PARAMETER;

	return STATUS_SUCCESS;
}
