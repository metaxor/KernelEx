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
#include "../advapi32/_advapi32_apilist.h"

/* MAKE_EXPORT NtOpenKey=NtOpenKey */
/* MAKE_EXPORT NtOpenKey=ZwOpenKey */
NTSTATUS NTAPI NtOpenKey(
	OUT		PHANDLE KeyHandle,
	IN		ACCESS_MASK DesiredAccess,
	IN		POBJECT_ATTRIBUTES ObjectAttributes
)
{
	ULONG result;
	DWORD dwFlags;

	if(IsBadReadPtr(ObjectAttributes, sizeof(LSA_OBJECT_ATTRIBUTES)))
		return STATUS_INVALID_PARAMETER;

	if(IsBadWritePtr(KeyHandle, sizeof(DWORD)))
		return STATUS_INVALID_PARAMETER;

	if(ObjectAttributes->Attributes & OBJ_INHERIT)
		dwFlags = HF_INHERIT;

	result = RegOpenKeyExW_new((HKEY)ObjectAttributes->RootDirectory,
						ObjectAttributes->ObjectName->Buffer,
						0,
						DesiredAccess | dwFlags,
						(PHKEY)KeyHandle);

	if(result != ERROR_SUCCESS)
		return STATUS_INVALID_PARAMETER;

	if(ObjectAttributes->Attributes & OBJ_KERNEL_HANDLE)
		*KeyHandle = ConvertToGlobalHandle(*KeyHandle);

	return STATUS_SUCCESS;
}

/* MAKE_EXPORT NtQueryValueKey=NtQueryValueKey */
/* MAKE_EXPORT NtQueryValueKey=ZwQueryValueKey */
NTSTATUS NTAPI NtQueryValueKey(
	IN		HANDLE KeyHandle,
	IN		PUNICODE_STRING ValueName,
	IN		KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
	OUT		PVOID KeyValueInformation OPTIONAL,
	IN		ULONG Length,
	OUT		PULONG ResultLength
)
{
	PKEY_VALUE_BASIC_INFORMATION pkvbi;
	PKEY_VALUE_FULL_INFORMATION pkvfi;
	PKEY_VALUE_PARTIAL_INFORMATION pkvpi;
	LPDWORD dwData = NULL;
	DWORD dwLength = 0;
	ULONG result;

	if(KeyValueInformationClass > MaxKeyValueInfoClass)
		return STATUS_INVALID_PARAMETER;

	if(IsBadReadPtr(ValueName, sizeof(UNICODE_STRING)))
		return STATUS_INVALID_PARAMETER;

	pkvbi = (PKEY_VALUE_BASIC_INFORMATION)KeyValueInformation;
	pkvfi = (PKEY_VALUE_FULL_INFORMATION)KeyValueInformation;
	pkvpi = (PKEY_VALUE_PARTIAL_INFORMATION)KeyValueInformation;

	result = RegQueryValueExW_new((HKEY)KeyHandle,
								ValueName->Buffer,
								NULL,
								&pkvbi->Type,
								(LPBYTE)&dwData,
								&dwLength);

	if(result != ERROR_SUCCESS)
		return result; // FIXME : Return other errors

	if(KeyValueInformationClass == KeyValueBasicInformation &&
		KeyValueInformationClass == KeyValuePartialInformation)
	{
		pkvbi->NameLength = dwLength;
		pkvbi->Name[0] = (WCHAR)dwData[0];
		pkvbi->Name[1] = (WCHAR)dwData[1];
	}
	else
	{
		pkvfi->DataOffset = (ULONG)dwData;
		pkvfi->DataLength = dwLength;
		pkvfi->NameLength = dwLength;
		pkvfi->Name[0] = (WCHAR)dwData[0];
		pkvfi->Name[1] = (WCHAR)dwData[1];
	}

	return STATUS_SUCCESS;
}
