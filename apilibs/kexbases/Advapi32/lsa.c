/*
 *  KernelEx
 *  Copyright (C) 2010 Tihiy
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

#include <ntstatus.h>
#define WIN32_NO_STATUS
#include <windows.h>
#include "k32ord.h"

typedef LONG NTSTATUS;

/* MAKE_EXPORT LsaClose_new=LsaClose */
DWORD WINAPI LsaClose_new(IN HANDLE ObjectHandle)
{
	return STATUS_SUCCESS;
}


/* MAKE_EXPORT LsaNtStatusToWinError_new=LsaNtStatusToWinError */
ULONG WINAPI LsaNtStatusToWinError_new(LONG Status)
{
	return K32_RtlNtStatusToDosError(Status);
}

/* MAKE_EXPORT LsaOpenPolicy_new=LsaOpenPolicy */
DWORD WINAPI LsaOpenPolicy_new(
	IN PVOID SystemName,
	IN PVOID ObjectAttributes,
	IN DWORD DesiredAccess,
	IN OUT PHANDLE PolicyHandle)
{
	if (PolicyHandle)
		*PolicyHandle = (HANDLE)0xCAFE;
	return STATUS_SUCCESS;
}


/* MAKE_EXPORT LsaRetrievePrivateData_new=LsaRetrievePrivateData */
DWORD WINAPI LsaRetrievePrivateData_new(
	IN HANDLE PolicyHandle,
	IN PVOID KeyName,
	OUT HANDLE* PrivateData)
{
	return STATUS_OBJECT_NAME_NOT_FOUND;
}

/* MAKE_EXPORT LsaStorePrivateData_new=LsaStorePrivateData */
DWORD WINAPI LsaStorePrivateData_new(
	IN HANDLE PolicyHandle,
	IN PVOID KeyName,
	IN PVOID PrivateData)
{
	return STATUS_OBJECT_NAME_NOT_FOUND;
}

