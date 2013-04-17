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

/* MAKE_EXPORT NtClose=NtClose */
/* MAKE_EXPORT NtClose=ZwClose */
NTSTATUS NTAPI NtClose(IN HANDLE Handle)
{
	BOOL result;

	if(Handle == NULL)
		return STATUS_INVALID_HANDLE;

	/* Try to close the handle */
	result = CloseHandle(Handle);

	if(!result)
	{
		/* The handle could be a system handle */
		result = CloseSystemHandle(Handle);
	}

	if(!result)
		return GetLastError();

	return STATUS_SUCCESS;
}

/* MAKE_EXPORT NtCreateEvent=NtCreateEvent */
/* MAKE_EXPORT NtCreateEvent=ZwCreateEvent */
NTSTATUS
NTAPI
NtCreateEvent(
	OUT PHANDLE EventHandle,
	IN ACCESS_MASK DesiredAccess,
	IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
	IN EVENT_TYPE EventType,
	IN BOOLEAN InitialState 
) 	
{
	DWORD Attributes;
	DWORD dwFlags;
	SECURITY_ATTRIBUTES sa;

	if(IsBadWritePtr(EventHandle, sizeof(DWORD)))
		return STATUS_INVALID_PARAMETER_1;

	if(ObjectAttributes != NULL)
		Attributes = ObjectAttributes->Attributes;

	if(Attributes & OBJ_INHERIT)
	{
		sa.nLength = sizeof(SECURITY_ATTRIBUTES);
		sa.lpSecurityDescriptor = NULL;
		sa.bInheritHandle = TRUE;

		dwFlags = HF_INHERIT;
	}

	*EventHandle = CreateEventW_new(&sa,
							FALSE,
							FALSE,
							ObjectAttributes != NULL ? ObjectAttributes->ObjectName->Buffer : NULL);

	if(*EventHandle == NULL)
		return STATUS_INVALID_PARAMETER;

	return STATUS_SUCCESS;
}

/* MAKE_EXPORT NtDuplicateObject=NtDuplicateObject */
/* MAKE_EXPORT NtDuplicateObject=ZwDuplicateObject */
NTSTATUS NTAPI NtDuplicateObject(IN HANDLE SourceProcessHandle,
		IN HANDLE		SourceHandle,
		IN HANDLE		TargetProcessHandle	OPTIONAL,
		OUT PHANDLE		TargetHandle		OPTIONAL,
		IN ACCESS_MASK	DesiredAccess,
		IN ULONG		HandleAttributes,
		IN ULONG		Options 
)
{
	BOOL result;
	HANDLE pHandle = NULL;

	if(SourceProcessHandle == NULL || SourceHandle == NULL)
		return STATUS_INVALID_HANDLE;

	if(TargetProcessHandle == NULL)
		TargetProcessHandle = GetCurrentProcess();

	if(TargetHandle == NULL)
		TargetHandle = &pHandle;

	result = DuplicateHandle(SourceProcessHandle,
						SourceHandle,
						TargetProcessHandle,
						TargetHandle,
						DesiredAccess,
						HandleAttributes,
						Options);

	if(!result)
		return GetLastError();

	return STATUS_SUCCESS;
}

/* MAKE_EXPORT NtSetEvent=NtSetEvent */
/* MAKE_EXPORT NtSetEvent=ZwSetEvent */
NTSTATUS
NTAPI
NtSetEvent(
	IN HANDLE EventHandle,
	OUT PLONG PreviousState OPTIONAL
)
{
	BOOL result = SetEvent(EventHandle);

	if(!result)
	{
		DWORD dwError = GetLastError();

		if(dwError == ERROR_ACCESS_DENIED)
			return STATUS_ACCESS_DENIED;
		else if(dwError == ERROR_NOT_ENOUGH_MEMORY)
			return STATUS_INSUFFICIENT_RESOURCES;
		else if(dwError == ERROR_INVALID_HANDLE)
			return STATUS_INVALID_HANDLE;
	}

	return STATUS_SUCCESS;
}

/* MAKE_EXPORT NtWaitForSingleObject=NtWaitForSingleObject */
/* MAKE_EXPORT NtWaitForSingleObject=ZwWaitForSingleObject */
NTSTATUS NTAPI NtWaitForSingleObject(IN HANDLE Handle,
	IN BOOLEAN Alertable,
	IN PLARGE_INTEGER Timeout
)
{
	return WaitForSingleObjectEx(Handle, (DWORD)Timeout->QuadPart, Alertable);
}