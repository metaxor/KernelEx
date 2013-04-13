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

/* MAKE_EXPORT NtAllocateVirtualMemory=NtAllocateVirtualMemory */
/* MAKE_EXPORT NtAllocateVirtualMemory=ZwAllocateVirtualMemory */
NTSTATUS NtAllocateVirtualMemory(IN HANDLE ProcessHandle,
	IN OUT PVOID *BaseAddress,
	IN ULONG_PTR ZeroBits,
	IN OUT PSIZE_T RegionSize,
	IN ULONG AllocationType,
	IN ULONG Protect
)
{
	PVOID NewAddress = NULL;

	NewAddress = VirtualAllocEx_new(ProcessHandle,
							BaseAddress != NULL ? *BaseAddress : NULL,
							*RegionSize,
							AllocationType,
							Protect);

	if(NewAddress == NULL)
	{
		DWORD dwLastErr = GetLastError();

		if(dwLastErr == ERROR_ACCESS_DENIED)
			return STATUS_ACCESS_DENIED;
		else if(dwLastErr == ERROR_INVALID_HANDLE)
			return STATUS_INVALID_HANDLE;
		else if(dwLastErr == ERROR_NOT_ENOUGH_MEMORY)
			return STATUS_NO_MEMORY;
		else
			return STATUS_INVALID_PARAMETER;
	}

	return STATUS_SUCCESS;
}

/* MAKE_EXPORT NtFreeVirtualMemory=NtFreeVirtualMemory */
/* MAKE_EXPORT NtFreeVirtualMemory=ZwFreeVirtualMemory */
NTSTATUS NtFreeVirtualMemory(IN HANDLE ProcessHandle,
	IN OUT PVOID *BaseAddress,
	IN OUT PSIZE_T RegionSize,
	IN ULONG FreeType
)
{
	BOOL Result = 0;

	Result = VirtualFreeEx_new(ProcessHandle,
							*BaseAddress,
							*RegionSize,
							FreeType);

	if(Result == FALSE)
	{
		DWORD dwLastErr = GetLastError();

		if(dwLastErr == ERROR_ACCESS_DENIED)
			return STATUS_ACCESS_DENIED;
		else if(dwLastErr == ERROR_INVALID_HANDLE)
			return STATUS_INVALID_HANDLE;
		else if(dwLastErr == ERROR_NOT_ENOUGH_MEMORY)
			return STATUS_NO_MEMORY;
		else
			return STATUS_INVALID_PARAMETER;
	}

	return STATUS_SUCCESS;
}
