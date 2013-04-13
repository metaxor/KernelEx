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

NTSTATUS NTAPI NtSuspendResumeThread(
		IN HANDLE ThreadHandle,
		OUT PULONG PreviousSuspendCount OPTIONAL,
		IN DWORD dwOperation
)
{
	BOOL result;

	if(dwOperation > 1)
		return STATUS_INVALID_PARAMETER;

	if(ThreadHandle == NULL)
		return STATUS_INVALID_PARAMETER;

	if(dwOperation == 0)
		result = ResumeThread(ThreadHandle);
	else if(dwOperation == 1)
		result = SuspendThread(ThreadHandle);

	if(result == -1)
		return STATUS_INVALID_PARAMETER;

	__try
	{
		*PreviousSuspendCount = result;
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
	}

	return STATUS_SUCCESS;
}


/* MAKE_EXPORT NtCreateThread=NtCreateThread */
/* MAKE_EXPORT NtCreateThread=ZwCreateThread */
NTSTATUS NTAPI NtCreateThread(OUT PHANDLE ThreadHandle,
	IN ACCESS_MASK DesiredAccess,
	IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
	IN HANDLE ProcessHandle,
	OUT PCLIENT_ID ClientId,
	IN PCONTEXT ThreadContext,
	IN PINITIAL_TEB InitialTeb,
	IN BOOLEAN CreateSuspended
)
{
	*ThreadHandle = CreateRemoteThread(ProcessHandle,
							NULL,
							(DWORD)InitialTeb->StackCommitMax,
							(LPTHREAD_START_ROUTINE)ThreadContext->Eip,
							(LPVOID)*(DWORD*)ThreadContext->Ebp,
							CreateSuspended ? CREATE_SUSPENDED : 0,
							&ClientId->UniqueThreadId);

	if(*ThreadHandle == NULL)
		return STATUS_INVALID_PARAMETER;

	return STATUS_SUCCESS;
}

/* MAKE_EXPORT NtGetContextThread=NtGetContextThread */
/* MAKE_EXPORT NtGetContextThread=ZwGetContextThread */
NTSTATUS NTAPI NtGetContextThread(IN HANDLE ThreadHandle,
	OUT PCONTEXT pContext
)
{
	BOOL result;

	if(ThreadHandle == NULL || pContext == NULL)
		return STATUS_INVALID_PARAMETER;

	__try
	{
		result = GetThreadContext(ThreadHandle, pContext);
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		return STATUS_INVALID_PARAMETER;
	}

	if(!result)
		return STATUS_INVALID_PARAMETER;

	return STATUS_SUCCESS;
}

/* MAKE_EXPORT NtSetContextThread=NtSetContextThread */
/* MAKE_EXPORT NtSetContextThread=ZwSetContextThread */
NTSTATUS NTAPI NtSetContextThread(IN HANDLE ThreadHandle,
	IN PCONTEXT Context
)
{
	BOOL result;

	if(ThreadHandle == NULL || Context == NULL)
		return STATUS_INVALID_PARAMETER;

	__try
	{
		result = SetThreadContext(ThreadHandle, Context);
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		return STATUS_INVALID_PARAMETER;
	}

	if(!result)
		return STATUS_INVALID_PARAMETER;

	return STATUS_SUCCESS;
}

/* MAKE_EXPORT NtOpenThread=NtOpenThread */
/* MAKE_EXPORT NtOpenThread=ZwOpenThread */
NTSTATUS NTAPI NtOpenThread(OUT PHANDLE ThreadHandle,
	IN ACCESS_MASK          AccessMask,
	IN POBJECT_ATTRIBUTES   ObjectAttributes,
	IN PCLIENT_ID           ClientId
)
{
	__try
	{
		*ThreadHandle = OpenThread_new(AccessMask, ObjectAttributes->Attributes, ClientId->UniqueProcessId);
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		return STATUS_INVALID_PARAMETER;
	}

	return STATUS_SUCCESS;
}

/* MAKE_EXPORT NtResumeThread=NtResumeThread */
/* MAKE_EXPORT NtResumeThread=ZwResumeThread */
NTSTATUS NTAPI NtResumeThread(
		IN HANDLE ThreadHandle,
		OUT PULONG SuspendCount OPTIONAL
)
{
	return NtSuspendResumeThread(ThreadHandle, SuspendCount, 0);
}

/* MAKE_EXPORT NtSuspendThread=NtSuspendThread */
/* MAKE_EXPORT NtSuspendThread=ZwSuspendThread */
NTSTATUS NTAPI NtSuspendThread(
		IN HANDLE ThreadHandle,
		OUT PULONG PreviousSuspendCount OPTIONAL
)
{
	return NtSuspendResumeThread(ThreadHandle, PreviousSuspendCount, 1);
}

/* MAKE_EXPORT NtTerminateThread=NtTerminateThread */
/* MAKE_EXPORT NtTerminateThread=ZwTerminateThread */
NTSTATUS NTAPI NtTerminateThread(IN HANDLE ThreadHandle,
	IN NTSTATUS ExitStatus
)
{
	BOOL result;

	result = TerminateThread(ThreadHandle, ExitStatus);

	if(!result)
		return STATUS_INVALID_PARAMETER;

	return STATUS_SUCCESS;
}