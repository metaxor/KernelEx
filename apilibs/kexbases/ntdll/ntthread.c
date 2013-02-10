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

NTSTATUS NTAPI ZwSuspendResumeThread(
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


/* MAKE_EXPORT ZwCreateThread=NtCreateThread */
/* MAKE_EXPORT ZwCreateThread=ZwCreateThread */
NTSTATUS NTAPI ZwCreateThread(OUT PHANDLE ThreadHandle,
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

/* MAKE_EXPORT ZwGetContextThread=NtGetContextThread */
/* MAKE_EXPORT ZwGetContextThread=ZwGetContextThread */
NTSTATUS NTAPI ZwGetContextThread(IN HANDLE ThreadHandle,
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

/* MAKE_EXPORT ZwSetContextThread=NtSetContextThread */
/* MAKE_EXPORT ZwSetContextThread=ZwSetContextThread */
NTSTATUS NTAPI ZwSetContextThread(IN HANDLE ThreadHandle,
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

/* MAKE_EXPORT ZwOpenThread=NtOpenThread */
/* MAKE_EXPORT ZwOpenThread=ZwOpenThread */
NTSTATUS NTAPI ZwOpenThread(OUT PHANDLE ThreadHandle,
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

/* MAKE_EXPORT ZwResumeThread=NtResumeThread */
/* MAKE_EXPORT ZwResumeThread=ZwResumeThread */
NTSTATUS NTAPI ZwResumeThread(
		IN HANDLE ThreadHandle,
		OUT PULONG SuspendCount OPTIONAL
)
{
	return ZwSuspendResumeThread(ThreadHandle, SuspendCount, 0);
}

/* MAKE_EXPORT ZwSuspendThread=NtSuspendThread */
/* MAKE_EXPORT ZwSuspendThread=ZwSuspendThread */
NTSTATUS NTAPI ZwSuspendThread(
		IN HANDLE ThreadHandle,
		OUT PULONG PreviousSuspendCount OPTIONAL
)
{
	return ZwSuspendResumeThread(ThreadHandle, PreviousSuspendCount, 1);
}

/* MAKE_EXPORT ZwTerminateThread=NtTerminateThread */
/* MAKE_EXPORT ZwTerminateThread=ZwTerminateThread */
NTSTATUS NTAPI ZwTerminateThread(IN HANDLE ThreadHandle,
	IN NTSTATUS ExitStatus
)
{
	BOOL result;

	result = TerminateThread(ThreadHandle, ExitStatus);

	if(!result)
		return STATUS_INVALID_PARAMETER;

	return STATUS_SUCCESS;
}