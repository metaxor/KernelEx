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
#include <stdio.h>
#include <wchar.h>

/* MAKE_EXPORT RtlCompareUnicodeString=RtlCompareUnicodeString */
LONG
NTAPI
RtlCompareUnicodeString(
	IN PUNICODE_STRING String1,
	IN PUNICODE_STRING String2,
	IN BOOLEAN CaseInsensitive)
{
	UINT uLength;
	LONG result = 0;
	LPCWSTR p1, p2;

	uLength = min(String1->Length, String2->Length) / sizeof(WCHAR);
	p1 = String1->Buffer;
	p2 = String2->Buffer;

	if(CaseInsensitive)
	{
		while (!result && uLength--)
			result = towupper(*p1++) - towupper(*p2++);
	}
	else
	{
		while(!result && uLength--)
			result = *p1++ - *p2++;
	}

	if (!result)
		result = String1->Length - String2->Length;

	return result;
}

/* MAKE_EXPORT RtlCopySid=RtlCopySid */
NTSTATUS
NTAPI
RtlCopySid(
	IN ULONG BufferLength,
	IN PSID Dest,
	IN PSID Src
)
{
	ULONG SidLength;

	SidLength = RtlLengthSid(Src);

	if(SidLength > BufferLength)
		return STATUS_BUFFER_TOO_SMALL;

	RtlMoveMemory(Dest, Src, SidLength);
	return STATUS_SUCCESS;
}

/* MAKE_EXPORT RtlCreateSecurityDescriptor=RtlCreateSecurityDescriptor */
NTSTATUS
NTAPI
RtlCreateSecurityDescriptor(
	IN PSECURITY_DESCRIPTOR SecurityDescriptor,
	IN ULONG Revision 
) 		
{
	PISECURITY_DESCRIPTOR Sd = (PISECURITY_DESCRIPTOR)SecurityDescriptor;

	if(Revision != SECURITY_DESCRIPTOR_REVISION)
		return STATUS_UNKNOWN_REVISION;

	RtlZeroMemory(Sd, sizeof(*Sd));
	Sd->Revision = SECURITY_DESCRIPTOR_REVISION;

	return STATUS_SUCCESS;
}



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

/* MAKE_EXPORT RtlDosPathNameToNtPathName_U=RtlDosPathNameToNtPathName_U */
BOOLEAN
NTAPI
RtlDosPathNameToNtPathName_U(
	IN PCWSTR DosName,
	OUT PUNICODE_STRING NtName,
	OUT PCWSTR *PartName,
	OUT PVOID RelativeName
)
{
	DWORD dwLength;
	DWORD result;

	if(IsBadUnicodeStringPtr(DosName, -1) ||
	   IsBadWritePtr(NtName, sizeof(UNICODE_STRING)) ||
	   IsBadWritePtr(PartName, sizeof(DWORD)) ||
	   IsBadUnicodeStringPtr(*PartName, -1))
		return FALSE;

	dwLength = wcslen(DosName);

	NtName->Buffer = (PWSTR)malloc(dwLength);

	result = GetLongPathNameW_new(DosName,
								NtName->Buffer,
								dwLength);

	if(!result)
	{
		free(NtName->Buffer);
		return FALSE;
	}

	return TRUE;
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

/* MAKE_EXPORT RtlEqualUnicodeString=RtlEqualUnicodeString */
BOOLEAN
NTAPI
RtlEqualUnicodeString(
	IN CONST PUNICODE_STRING String1,
	IN CONST PUNICODE_STRING String2,
	IN BOOLEAN CaseInsensitive
)
{
	if (String1->Length != String2->Length)
		return FALSE;

    return !RtlCompareUnicodeString(String1, String2, CaseInsensitive);
}

/* MAKE_EXPORT RtlExpandEnvironmentStrings_U=RtlExpandEnvironmentStrings_U */
NTSTATUS
NTAPI
RtlExpandEnvironmentStrings_U(
	IN PWSTR Environment,
	IN PUNICODE_STRING Source,
	OUT PUNICODE_STRING Destination,
	OUT PULONG Length
)
{
	DWORD dwLength;

	dwLength = ExpandEnvironmentStringsW_new(Source->Buffer,
											Destination->Buffer,
											Destination->Length);

	if(dwLength == 0)
		return STATUS_INVALID_PARAMETER;

	*Length = dwLength;

	return STATUS_SUCCESS;
}

/* MAKE_EXPORT RtlFreeOemString=RtlFreeOemString */
VOID
NTAPI
RtlFreeOemString(IN POEM_STRING OemString)
{
    if(OemString->Buffer)
	{
		HeapFree(GetProcessHeap(),
				0,
				OemString->Buffer);
	}
}

/* MAKE_EXPORT RtlFreeSid=RtlFreeSid */
PVOID
NTAPI
RtlFreeSid(IN PSID Sid)
{
	RtlZeroMemory(Sid, sizeof(SID));
	return NULL;
}

/* MAKE_EXPORT RtlFreeUnicodeString=RtlFreeUnicodeString */
VOID
NTAPI
RtlFreeUnicodeString(IN PUNICODE_STRING UnicodeString)
{
	if(UnicodeString->Buffer)
	{
		HeapFree(GetProcessHeap(),
				0,
				UnicodeString->Buffer);

		RtlZeroMemory(UnicodeString, sizeof(UNICODE_STRING));
	}
}

/* MAKE_EXPORT RtlInitAnsiString=RtlInitAnsiString */
VOID
NTAPI
RtlInitAnsiString(
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

/* MAKE_EXPORT RtlInitializeBitMap=RtlInitializeBitMap */
VOID
NTAPI
RtlInitializeBitMap(
	OUT PRTL_BITMAP BitMapHeader,
	IN PULONG BitMapBuffer,
	IN ULONG SizeOfBitMap
)
{
	BitMapHeader->SizeOfBitMap = SizeOfBitMap;
	BitMapHeader->Buffer = BitMapBuffer;
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

/* MAKE_EXPORT RtlInitializeSid=RtlInitializeSid */
NTSTATUS
NTAPI
RtlInitializeSid(
	IN PSID Sid,
	IN PSID_IDENTIFIER_AUTHORITY IdentifierAuthority,
	IN UCHAR SubAuthorityCount
)
{
    PISID ISid = (PISID)Sid;

    ISid->Revision = SID_REVISION;
    ISid->SubAuthorityCount = SubAuthorityCount;
    ISid->IdentifierAuthority = *IdentifierAuthority;

    return STATUS_SUCCESS;
}

/* MAKE_EXPORT RtlInitUnicodeString=RtlInitUnicodeString */
VOID NTAPI RtlInitUnicodeString(
    IN OUT PUNICODE_STRING DestinationString,
    IN PCWSTR SourceString
)
{
	SIZE_T Size;

	if(SourceString)
	{
		Size = wcslen(SourceString) * sizeof(WCHAR);
		DestinationString->Length = (USHORT)Size;
		DestinationString->MaximumLength = (USHORT)Size + sizeof(WCHAR);
	}
	else
	{
		DestinationString->Length = 0;
		DestinationString->MaximumLength = 0;
	}

	DestinationString->Buffer = (PWCHAR)SourceString;
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

/* MAKE_EXPORT RtlLengthRequiredSid=RtlLengthRequiredSid */
ULONG
NTAPI
RtlLengthRequiredSid(IN ULONG SubAuthorityCount)
{
	return (ULONG)FIELD_OFFSET(SID,
							SubAuthority[SubAuthorityCount]);
}

/* MAKE_EXPORT RtlLengthSid=RtlLengthSid */
ULONG
NTAPI
RtlLengthSid(IN PSID Sid)
{
	PISID ISid = (PISID)Sid;

	return (ULONG)FIELD_OFFSET(SID,
							SubAuthority[ISid->SubAuthorityCount]);
}

/* MAKE_EXPORT RtlOemToUnicodeN=RtlOemToUnicodeN */
NTSTATUS
NTAPI
RtlOemToUnicodeN(
	PWCHAR UnicodeString,
	ULONG UnicodeSize,
	PULONG ResultSize,
	PCCH OemString,
	ULONG OemSize
)
{
	ULONG uSize;

	uSize = MultiByteToWideChar(CP_ACP,
								0,
								OemString,
								OemSize,
								UnicodeString,
								UnicodeSize);

	if(uSize == 0)
		return STATUS_INVALID_PARAMETER;

	if(!IsBadWritePtr(ResultSize, sizeof(ULONG)))
		*ResultSize = uSize;

	return STATUS_SUCCESS;
}

/* MAKE_EXPORT RtlPrefixUnicodeString=RtlPrefixUnicodeString */
BOOLEAN
NTAPI
RtlPrefixUnicodeString(
    PUNICODE_STRING String1,
    PUNICODE_STRING String2,
    BOOLEAN  CaseInsensitive)
{
	PWCHAR pc1;
	PWCHAR pc2;
	ULONG Length;

	if(String2->Length < String1->Length)
		return FALSE;

	Length = String1->Length / 2;
	pc1 = String1->Buffer;
	pc2  = String2->Buffer;

	if(pc1 && pc2)
	{
		if(CaseInsensitive)
		{
			while(Length--)
			{
				if (towupper(*pc1++) !=
					towupper(*pc2++))
					return FALSE;
			}
		}
		else
		{
			while(Length--)
			{
				if(*pc1++ != *pc2++)
					return FALSE;
			}
		}

		return TRUE;
	}

	return FALSE;
}

/* MAKE_EXPORT RtlRaiseStatus=RtlRaiseStatus */
VOID
NTAPI
RtlRaiseStatus(IN NTSTATUS Status)
{
	DWORD dwError = RtlNtStatusToDosError(Status);

	RaiseException(dwError,
				0,
				0,
				NULL);
}

/* MAKE_EXPORT RtlSubAuthoritySid=RtlSubAuthoritySid */
PULONG
NTAPI
RtlSubAuthoritySid(
	IN PSID Sid,
	IN ULONG SubAuthority
)
{
    PISID ISid = (PISID)Sid;

    /* Return the offset */
    return (PULONG)&ISid->SubAuthority[SubAuthority];
}

/* MAKE_EXPORT RtlSystemTimeToLocalTime=RtlSystemTimeToLocalTime */
NTSTATUS
NTAPI
RtlSystemTimeToLocalTime(
	IN PLARGE_INTEGER SystemTime,
	OUT PLARGE_INTEGER LocalTime
)
{
	FILETIME FileTime, LocalFileTime;
	SYSTEMTIME cSystemTime;

	GetSystemTime(&cSystemTime);
	SystemTimeToFileTime(&cSystemTime, &FileTime);

	FileTimeToLocalFileTime(&FileTime, &LocalFileTime);

	LocalTime->QuadPart = LocalFileTime.dwLowDateTime +
						  LocalFileTime.dwHighDateTime;

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

/* MAKE_EXPORT RtlUnicodeStringToOemString=RtlUnicodeStringToOemString */
NTSTATUS
NTAPI
RtlUnicodeStringToOemString(
	IN OUT POEM_STRING OemDest,
	IN PUNICODE_STRING UniSource,
	IN BOOLEAN  AllocateDestinationString
)
{
	NTSTATUS Status;
	ULONG Length;
	ULONG Index;
 
	Length = wcslen(UniSource->Buffer);
 
	if(Length > 65535)
		return STATUS_INVALID_PARAMETER_2;
 
	OemDest->Length = (USHORT)Length - sizeof(CHAR);
 
	if(AllocateDestinationString)
	{
		OemDest->Buffer = (PSTR)HeapAlloc(GetProcessHeap,
									0,
									Length);

		OemDest->MaximumLength = (USHORT)Length;
 
		if(!OemDest->Buffer)
			return STATUS_NO_MEMORY;
	}
	else if(OemDest->Length >= OemDest->MaximumLength)
		return STATUS_BUFFER_OVERFLOW;
 
	Status = RtlUnicodeToOemN(UniSource->Buffer,
							UniSource->Length,
							&Index,
							OemDest->Buffer,
							OemDest->Length);
 
	if(!NT_SUCCESS(Status) && AllocateDestinationString)
	{
		HeapFree(GetProcessHeap(),
				0,
				OemDest->Buffer);

		OemDest->Buffer = NULL;
		return Status;
	}
 
	OemDest->Buffer[Index] = ANSI_NULL;
	return Status;
}

/* MAKE_EXPORT RtlUnicodeToOemN=RtlUnicodeToOemN */
NTSTATUS
NTAPI
RtlUnicodeToOemN(
	PWCHAR UnicodeString,
	ULONG UnicodeSize,
	PULONG ResultSize,
	PCCH OemString,
	ULONG OemSize
)
{
	ULONG uSize;

	uSize = WideCharToMultiByte(CP_ACP,
								0,
								UnicodeString,
								UnicodeSize,
								(LPSTR)OemString,
								OemSize,
								NULL,
								NULL);

	if(uSize == 0)
		return STATUS_INVALID_PARAMETER;

	if(!IsBadWritePtr(ResultSize, sizeof(ULONG)))
		*ResultSize = uSize;

	return STATUS_SUCCESS;
}

/* MAKE_EXPORT RtlUpcaseUnicodeChar=RtlUpcaseUnicodeChar */
WCHAR NTAPI
RtlUpcaseUnicodeChar(IN WCHAR Source)
{
	return towupper(Source);
}

/* MAKE_EXPORT RtlUpcaseUnicodeString=RtlUpcaseUnicodeString */
NTSTATUS
NTAPI
RtlUpcaseUnicodeString(
	IN OUT PUNICODE_STRING UniDest,
	IN PUNICODE_STRING UniSource,
	IN BOOLEAN AllocateDestinationString 
)
{
	ULONG i, j;

	if(AllocateDestinationString == TRUE)
	{
		UniDest->MaximumLength = UniSource->Length;
		UniDest->Buffer = (PWSTR)HeapAlloc(GetProcessHeap(),
										0,
										UniDest->MaximumLength);

		if(UniDest->Buffer == NULL)
			return STATUS_NO_MEMORY;
    }
	else if(UniSource->Length > UniDest->MaximumLength)
		return STATUS_BUFFER_OVERFLOW;

	j = UniSource->Length / sizeof(WCHAR);

	for (i = 0; i < j; i++)
		UniDest->Buffer[i] = RtlUpcaseUnicodeChar(UniSource->Buffer[i]);

	UniDest->Length = UniSource->Length;
	return STATUS_SUCCESS;
}