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
#include <limits.h>

/* MAKE_EXPORT RtlDeleteCriticalSection=RtlDeleteCriticalSection */
NTSTATUS NTAPI RtlDeleteCriticalSection(
	RTL_CRITICAL_SECTION *crit
)
{
	__try
	{
		DeleteCriticalSection_fix(crit);
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		return STATUS_INVALID_ADDRESS;
	}

	return STATUS_SUCCESS;
}

/* MAKE_EXPORT RtlEnterCriticalSection=RtlEnterCriticalSection */
NTSTATUS NTAPI RtlEnterCriticalSection(
	RTL_CRITICAL_SECTION *crit
)
{
	__try
	{
		EnterCriticalSection_fix(crit);
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		return STATUS_INVALID_ADDRESS;
	}

	return STATUS_SUCCESS;
}

/* MAKE_EXPORT RtlInitAnsiString=RtlInitAnsiString */
VOID NTAPI RtlInitAnsiString(
  IN OUT	PANSI_STRING DestinationString,
  IN		PCSZ SourceString
)
{
    SIZE_T Size;

	if (SourceString)
	{
		Size = strlen(SourceString);
		if (Size > (USHRT_MAX - sizeof(CHAR)))
			Size = USHRT_MAX - sizeof(CHAR);

		DestinationString->Length = (USHORT)Size;
		DestinationString->MaximumLength = (USHORT)Size + sizeof(CHAR);
	}
	else
	{
		DestinationString->Length = 0;
		DestinationString->MaximumLength = 0;
	}

	DestinationString->Buffer = (PCHAR)SourceString;
}

/* MAKE_EXPORT RtlInitAnsiStringEx=RtlInitAnsiStringEx */
NTSTATUS NTAPI RtlInitAnsiStringEx(
  IN OUT	PANSI_STRING DestinationString,
  IN		PCSZ SourceString
)
{
    SIZE_T Size;

	if (SourceString)
	{
		Size = strlen(SourceString);
		if (Size > (USHRT_MAX - sizeof(CHAR)))
			return STATUS_NAME_TOO_LONG;

		DestinationString->Length = (USHORT)Size;
		DestinationString->MaximumLength = (USHORT)Size + sizeof(CHAR);
	}
	else
	{
		DestinationString->Length = 0;
		DestinationString->MaximumLength = 0;
	}

	DestinationString->Buffer = (PCHAR)SourceString;
	return STATUS_SUCCESS;
}

/* MAKE_EXPORT RtlInitializeCriticalSection=RtlInitializeCriticalSection */
NTSTATUS NTAPI RtlInitializeCriticalSection(
	RTL_CRITICAL_SECTION *crit
)
{
	__try
	{
		InitializeCriticalSection(crit);
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		return STATUS_INVALID_ADDRESS;
	}

	return STATUS_SUCCESS;
}

/* MAKE_EXPORT RtlInitializeCriticalSectionAndSpinCount=RtlInitializeCriticalSectionAndSpinCount */
NTSTATUS NTAPI RtlInitializeCriticalSectionAndSpinCount(
	RTL_CRITICAL_SECTION *crit,
	ULONG spincount
)
{
	__try
	{
		InitializeCriticalSectionAndSpinCount_new(crit, spincount);
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		return STATUS_INVALID_ADDRESS;
	}

	return STATUS_SUCCESS;
}

/* MAKE_EXPORT RtlInitializeCriticalSectionEx=RtlInitializeCriticalSectionEx */
NTSTATUS NTAPI RtlInitializeCriticalSectionEx(
	RTL_CRITICAL_SECTION *crit,
	ULONG spincount,
	ULONG flags
)
{
	return RtlInitializeCriticalSectionAndSpinCount(crit, spincount);
}

/* MAKE_EXPORT RtlLeaveCriticalSection=RtlLeaveCriticalSection */
NTSTATUS NTAPI RtlLeaveCriticalSection(
	RTL_CRITICAL_SECTION *crit
)
{
	__try
	{
		LeaveCriticalSection(crit);
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		return STATUS_INVALID_ADDRESS;
	}

	return STATUS_SUCCESS;
}

/* MAKE_EXPORT RtlTryEnterCriticalSection=RtlTryEnterCriticalSection */
NTSTATUS NTAPI RtlTryEnterCriticalSection(
	RTL_CRITICAL_SECTION *crit
)
{
	__try
	{
		TryEnterCriticalSection_new(crit);
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		return STATUS_INVALID_ADDRESS;
	}

	return STATUS_SUCCESS;
}