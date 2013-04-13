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

/* MAKE_EXPORT NtOpenFile=NtOpenFile */
NTSTATUS NTAPI NtOpenFile(
  OUT  PHANDLE FileHandle,
  IN   ACCESS_MASK DesiredAccess,
  IN   POBJECT_ATTRIBUTES ObjectAttributes,
  OUT  PIO_STATUS_BLOCK IoStatusBlock,
  IN   ULONG ShareAccess,
  IN   ULONG OpenOptions
)
{
	PCHAR lpFileName;
	SECURITY_ATTRIBUTES sa;

	if(ObjectAttributes->ObjectName == NULL)
		return STATUS_INVALID_PARAMETER;

	STACK_WtoA(ObjectAttributes->ObjectName->Buffer, lpFileName);

	if(ObjectAttributes->Attributes & OBJ_INHERIT)
	{
		sa.nLength = sizeof(SECURITY_ATTRIBUTES);
		sa.bInheritHandle = TRUE;
		sa.lpSecurityDescriptor = ObjectAttributes->SecurityDescriptor;
	}

	SetLastError(0);

	*FileHandle = CreateFileA_fix(lpFileName,
								DesiredAccess,
								ShareAccess,
								ObjectAttributes->Attributes & OBJ_INHERIT ? &sa : NULL,
								0,
								OpenOptions,
								NULL);

	if(*FileHandle == INVALID_HANDLE_VALUE)
	{
		DWORD dwErr = GetLastError();

		if(dwErr == ERROR_FILE_NOT_FOUND)
			return STATUS_NO_SUCH_FILE;
		else if(dwErr == ERROR_ACCESS_DENIED)
			return STATUS_ACCESS_DENIED;
		else if(dwErr == ERROR_SHARING_VIOLATION)
			return STATUS_SHARING_VIOLATION;
		else
			return STATUS_INVALID_PARAMETER;
	}

	if(ObjectAttributes->Attributes & OBJ_KERNEL_HANDLE)
	{
		HANDLE hObject = ConvertToGlobalHandle(*FileHandle);

		if(hObject != NULL)
			*FileHandle = hObject;
		else
		{
			NtClose(*FileHandle);
			return STATUS_ACCESS_DENIED; // what to return in case of this fail ?
		}
	}

	return STATUS_SUCCESS;
}