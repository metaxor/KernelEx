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

/* MAKE_EXPORT NtCreateSection=NtCreateSection */
/* MAKE_EXPORT NtCreateSection=ZwCreateSection */
NTSTATUS
NTAPI
NtCreateSection(
	OUT PHANDLE SectionHandle,
	IN ACCESS_MASK DesiredAccess,
	IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
	IN PLARGE_INTEGER MaximumSize OPTIONAL,
	IN ULONG SectionPageProtection OPTIONAL,
	IN ULONG AllocationAttributes,
	IN HANDLE FileHandle OPTIONAL
)
{
	HANDLE hFileMapping, hGlobalHandle;
	SECURITY_ATTRIBUTES sa;
	BOOL fInherit, fGlobal;
	DWORD dwMaximumSizeLow, dwMaximumSizeHigh;

	if(FileHandle == NULL)
		FileHandle = INVALID_HANDLE_VALUE;

    /* Check for non-existing flags */
    if ((AllocationAttributes & ~(SEC_COMMIT | SEC_RESERVE |
								SEC_LARGE_PAGES | SEC_IMAGE | SEC_NOCACHE)))
	{
		if(!(AllocationAttributes & 1))
		{
			ERR("Bogus allocation attribute: %lx\n", AllocationAttributes);
			return STATUS_INVALID_PARAMETER_6;
		}
	}

	/* Check for no allocation type */
	if (!(AllocationAttributes & (SEC_COMMIT | SEC_RESERVE | SEC_IMAGE)))
	{
		ERR_OUT("Missing allocation type in allocation attributes\n");
		return STATUS_INVALID_PARAMETER_6;
	}

    /* Check for image allocation with invalid attributes */
    if((AllocationAttributes & SEC_IMAGE) &&
		(AllocationAttributes & (SEC_COMMIT | SEC_RESERVE | SEC_LARGE_PAGES |
								SEC_NOCACHE)))
	{
		ERR_OUT("Image allocation with invalid attributes\n");
		return STATUS_INVALID_PARAMETER_6;
	}

	/* Check for allocation type is both commit and reserve */
	if((AllocationAttributes & SEC_COMMIT) && (AllocationAttributes & SEC_RESERVE))
	{
		ERR_OUT("Commit and reserve in the same time\n");
		return STATUS_INVALID_PARAMETER_6;
	}

	/* Now check for valid protection */
	if((SectionPageProtection & PAGE_NOCACHE) ||
		(SectionPageProtection & PAGE_WRITECOMBINE) ||
		(SectionPageProtection & PAGE_GUARD) ||
		(SectionPageProtection & PAGE_NOACCESS))
	{
		ERR_OUT("Sections don't support these protections\n");
		return STATUS_INVALID_PAGE_PROTECTION;
	}

	if(IsBadWritePtr(SectionHandle, sizeof(DWORD)))
		return STATUS_INVALID_PARAMETER_1;

	if(ObjectAttributes != NULL)
	{
		fInherit = (ObjectAttributes->Attributes & OBJ_INHERIT) ? TRUE : FALSE;
		fGlobal = (ObjectAttributes->Attributes & OBJ_KERNEL_HANDLE) ? TRUE : FALSE;
		sa.nLength = sizeof(SECURITY_ATTRIBUTES);
		sa.lpSecurityDescriptor = NULL;
		sa.bInheritHandle = fInherit;
	}
	else
	{
		fInherit = FALSE;
		fGlobal = FALSE;
	}

	if(MaximumSize != NULL)
	{
		dwMaximumSizeLow = MaximumSize->LowPart;
		dwMaximumSizeHigh = MaximumSize->HighPart;
	}

	hFileMapping = CreateFileMapping(FileHandle, fInherit ? &sa : NULL,
									SectionPageProtection | AllocationAttributes,
									dwMaximumSizeHigh,
									dwMaximumSizeLow,
									NULL);

	if(hFileMapping == NULL)
		return GetLastError(); // FIXME: Return a status code

	/* If this is a global handle, return the global one and close the other handle */
	if(fGlobal)
	{
		CloseHandle(hFileMapping);
		hGlobalHandle = ConvertToGlobalHandle(hFileMapping);
		*SectionHandle = hGlobalHandle;
	}
	else
		*SectionHandle = hFileMapping;

	return STATUS_SUCCESS;
}

/* MAKE_EXPORT NtDeviceIoControlFile=NtDeviceIoControlFile */
/* MAKE_EXPORT NtDeviceIoControlFile=ZwDeviceIoControlFile */
NTSTATUS
NTAPI
NtDeviceIoControlFile(
	IN HANDLE FileHandle,
	IN HANDLE Event,
	IN PVOID ApcRoutine,
	IN PVOID ApcContext,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	IN ULONG IoControlCode,
	IN PVOID InputBuffer,
	IN ULONG InputBufferLength,
	OUT PVOID OutputBuffer,
	IN ULONG OutputBufferLength
)
{
	BOOL result;
	DWORD BytesReturned = 0;

	result = DeviceIoControl(FileHandle,
							IoControlCode,
							InputBuffer,
							InputBufferLength,
							OutputBuffer,
							OutputBufferLength,
							&BytesReturned,
							NULL);

	if(!result)
		return STATUS_INVALID_PARAMETER;

	return STATUS_SUCCESS;
}

/* MAKE_EXPORT NtDisplayString=NtDisplayString */
/* MAKE_EXPORT NtDisplayString=ZwDisplayString */
NTSTATUS
NTAPI
NtDisplayString(IN PUNICODE_STRING String)
{
	OEM_STRING OemString;
	DWORD dwBytesWritten;

	RtlUnicodeStringToOemString(&OemString, String, TRUE);

	WriteFile(GetStdHandle(STD_OUTPUT_HANDLE),
			OemString.Buffer,
			OemString.Length,
			&dwBytesWritten,
			NULL);

	RtlFreeOemString(&OemString);

	return STATUS_SUCCESS;
}

/* MAKE_EXPORT NtOpenFile=NtOpenFile */
/* MAKE_EXPORT NtOpenFile=ZwOpenFile */
NTSTATUS
NTAPI
NtOpenFile(
	OUT PHANDLE FileHandle,
	IN ACCESS_MASK DesiredAccess,
	IN POBJECT_ATTRIBUTES ObjectAttributes,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	IN ULONG ShareAccess,
	IN ULONG OpenOptions
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

/* MAKE_EXPORT NtQuerySection=NtQuerySection */
/* MAKE_EXPORT NtQuerySection=ZwQuerySection */
NTSTATUS
NTAPI
NtQuerySection(
	IN HANDLE SectionHandle,
	IN SECTION_INFORMATION_CLASS SectionInformationClass,
	OUT PVOID SectionInformation,
	IN SIZE_T SectionInformationLength,
	OUT PSIZE_T ResultLength OPTIONAL
)
{
	PSECTION_BASIC_INFORMATION pSectionBasicInfo;
	PSECTION_IMAGE_INFORMATION pSectionImageInfo;
	LPVOID lpFileBase;
	PIMAGE_DOS_HEADER dosHeader;
	PIMAGE_NT_HEADERS pNTHeader;
	PIMAGE_OPTIONAL_HEADER32 pOptHeader;
	PIMAGE_FILE_HEADER pFileHeader;

	if(SectionHandle == NULL)
		return STATUS_INVALID_PARAMETER_1;

	if(SectionInformationLength != sizeof(SECTION_BASIC_INFORMATION) &&
		SectionInformationLength != sizeof(SECTION_IMAGE_INFORMATION))
		return STATUS_INVALID_PARAMETER;
		
	if(IsBadWritePtr(SectionInformation, SectionInformationLength))
		return STATUS_INVALID_PARAMETER_3;

	pSectionBasicInfo = (PSECTION_BASIC_INFORMATION)SectionInformation;
	pSectionImageInfo = (PSECTION_IMAGE_INFORMATION)SectionInformation;

	lpFileBase = MapViewOfFile(SectionHandle,
							FILE_MAP_READ,
							0,
							0,
							0);

	if(lpFileBase == NULL)
		return STATUS_INVALID_PARAMETER; // FIXME: Return other values

	dosHeader = (PIMAGE_DOS_HEADER)lpFileBase;

	__try
	{
		pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)lpFileBase + dosHeader->e_lfanew);
		pOptHeader = &pNTHeader->OptionalHeader;
		pFileHeader = &pNTHeader->FileHeader;

		/* FIXME: Now copy the remaining data to the SECTION_BASIC_INFORMATION/SECTION_IMAGE_INFORMATION
		   structures */

		pSectionBasicInfo->BaseAddress = lpFileBase;

		if(SectionInformationLength == sizeof(SECTION_IMAGE_INFORMATION))
		{
			pSectionImageInfo->DllCharacteristics = pOptHeader->DllCharacteristics;
			pSectionImageInfo->ImageCharacteristics = pFileHeader->Characteristics;
			pSectionImageInfo->Machine = pFileHeader->Machine;
			pSectionImageInfo->SubSystemMajorVersion = pOptHeader->MajorSubsystemVersion;
			pSectionImageInfo->SubSystemMinorVersion = pOptHeader->MinorSubsystemVersion;
			pSectionImageInfo->SubSystemType = pOptHeader->Subsystem;
		}
		else
		{
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
	}

	return STATUS_SUCCESS;
}

/* MAKE_EXPORT NtReadFile=NtReadFile */
/* MAKE_EXPORT NtReadFile=ZwReadFile */
NTSTATUS
NTAPI NtReadFile(
	IN HANDLE FileHandle,
	IN HANDLE Event OPTIONAL,
	IN PVOID ApcRoutine OPTIONAL,
	IN PVOID ApcContext OPTIONAL,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	OUT PVOID Buffer,
	IN ULONG Length,
	IN PLARGE_INTEGER ByteOffset OPTIONAL,
	IN PULONG Key OPTIONAL 
) 	
{
	BOOL result;
	OVERLAPPED ol;

	result = ReadFileEx(FileHandle,
						Buffer,
						Length,
						&ol,
						NULL);

	if(!result)
		return STATUS_INVALID_PARAMETER;

	IoStatusBlock->Pointer = ol.Pointer;

	return STATUS_SUCCESS;
}

/* MAKE_EXPORT NtWriteFile=NtWriteFile */
/* MAKE_EXPORT NtWriteFile=ZwWriteFile */
NTSTATUS
NTAPI
NtWriteFile(
	IN HANDLE FileHandle,
	IN HANDLE Event OPTIONAL,
	IN PVOID ApcRoutine OPTIONAL,
	IN PVOID ApcContext OPTIONAL,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	IN PVOID Buffer,
	IN ULONG Length,
	IN PLARGE_INTEGER ByteOffset OPTIONAL,
	IN PULONG Key OPTIONAL
)
{
	BOOL result;
	OVERLAPPED ol;

	result = WriteFileEx(FileHandle,
						Buffer,
						Length,
						&ol,
						NULL);

	if(!result)
		return STATUS_INVALID_PARAMETER;

	IoStatusBlock->Pointer = ol.Pointer;

	return STATUS_SUCCESS;
}
