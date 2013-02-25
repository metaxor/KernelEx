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

/* MAKE_EXPORT ZwAdjustPrivilegesToken=NtAdjustPrivilegesToken */
/* MAKE_EXPORT ZwAdjustPrivilegesToken=ZwAdjustPrivilegesToken */
NTSTATUS
NTAPI
ZwAdjustPrivilegesToken(
	IN HANDLE TokenHandle,
	IN BOOLEAN DisableAllPrivileges,
	IN PTOKEN_PRIVILEGES TokenPrivileges,
	IN ULONG PreviousPrivilegesLength,
	OUT PTOKEN_PRIVILEGES PreviousPrivileges OPTIONAL,
	OUT PULONG RequiredLength OPTIONAL
)
{
	FIXME("Stub %p %d %p %d %p %p", TokenHandle, DisableAllPrivileges, TokenPrivileges,
							PreviousPrivilegesLength, PreviousPrivileges, RequiredLength);
	return STATUS_SUCCESS;
}

/* MAKE_EXPORT ZwOpenProcessToken=NtOpenProcessToken */
/* MAKE_EXPORT ZwOpenProcessToken=ZwOpenProcessToken */
NTSTATUS
NTAPI
ZwOpenProcessToken(
	IN HANDLE ProcessHandle,
	IN ACCESS_MASK DesiredAccess,
	OUT PHANDLE TokenHandle
)
{
	FIXME("Stub %p %d %p", ProcessHandle, DesiredAccess, TokenHandle);
	*(int*)TokenHandle = 0xcafe;
	return STATUS_SUCCESS;
}