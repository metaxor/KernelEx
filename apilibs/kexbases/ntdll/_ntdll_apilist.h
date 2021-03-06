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

#ifndef _NTDLL_APILIST_H
#define _NTDLL_APILIST_H

#include "ntstatus.h"
#define WIN32_NO_STATUS
#include "common.h"
#include <ntsecapi.h>
#include "../kernel32/_kernel32_apilist.h"
#include "../user32/_user32_apilist.h"

#define NT_SUCCESS(Status) ((NTSTATUS)(Status) >= 0)

DWORD __fastcall VKernelExCall(DWORD command, DWORD param1);

typedef PLSA_OBJECT_ATTRIBUTES POBJECT_ATTRIBUTES;

typedef struct _TIME_FIELDS
{
	WORD Year;
	WORD Month;
	WORD Day;
	WORD Hour;
	WORD Minute;
	WORD Second;
	WORD Milliseconds;
	WORD Weekday;
} TIME_FIELDS, *PTIME_FIELDS;

typedef struct _RTL_BITMAP
{
	ULONG SizeOfBitMap;
	PULONG Buffer;
} RTL_BITMAP, *PRTL_BITMAP;

typedef struct _CLIENT_ID
{
     DWORD UniqueProcessId;
     DWORD UniqueThreadId;
} CLIENT_ID, *PCLIENT_ID;

typedef enum _SHUTDOWN_ACTION {
	ShutdownNoReboot,
	ShutdownReboot,
	ShutdownPowerOff
} SHUTDOWN_ACTION, *PSHUTDOWN_ACTION;

//
// Responses for NtRaiseHardError
//
typedef enum _HARDERROR_RESPONSE_OPTION
{
    OptionAbortRetryIgnore,
    OptionOk,
    OptionOkCancel,
    OptionRetryCancel,
    OptionYesNo,
    OptionYesNoCancel,
    OptionShutdownSystem,
    OptionOkNoWait,
    OptionCancelTryContinue
} HARDERROR_RESPONSE_OPTION, *PHARDERROR_RESPONSE_OPTION;

typedef enum _HARDERROR_RESPONSE
{
    ResponseReturnToCaller,
    ResponseNotHandled,
    ResponseAbort,
    ResponseCancel,
    ResponseIgnore,
    ResponseNo,
    ResponseOk,
    ResponseRetry,
    ResponseYes,
    ResponseTryAgain,
    ResponseContinue
} HARDERROR_RESPONSE, *PHARDERROR_RESPONSE;

typedef struct _INITIAL_TEB {
	PVOID StackBase;
	PVOID StackLimit;
	PVOID StackCommit;
	PVOID StackCommitMax;
	PVOID StackReserved;
} INITIAL_TEB, *PINITIAL_TEB;

typedef STRING ANSI_STRING;
typedef PSTRING PANSI_STRING;
typedef STRING OEM_STRING;
typedef PSTRING POEM_STRING;
typedef CONST STRING* PCOEM_STRING;
typedef STRING CANSI_STRING;
typedef PSTRING PCANSI_STRING;

typedef enum _EVENT_TYPE
{
	NotificationEvent,
	SynchronizationEvent
} EVENT_TYPE, *PEVENT_TYPE;

typedef struct _IO_STATUS_BLOCK {
  union {
    NTSTATUS Status;
    PVOID    Pointer;
  };
  ULONG_PTR Information;
} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;

typedef enum _SECTION_INFORMATION_CLASS
{
	SectionBasicInformation,
	SectionImageInformation
} SECTION_INFORMATION_CLASS, *PSECTION_INFORMATION_CLASS;

typedef struct _SECTION_BASIC_INFORMATION
{
	PVOID BaseAddress;
	ULONG Attributes;
	LARGE_INTEGER Size;
} SECTION_BASIC_INFORMATION, *PSECTION_BASIC_INFORMATION;

typedef struct _SECTION_IMAGE_INFORMATION
{
	PVOID TransferAddress;
	ULONG ZeroBits;
	SIZE_T MaximumStackSize;
	SIZE_T CommittedStackSize;
	ULONG SubSystemType;
	union
	{
		struct
		{
			USHORT SubSystemMinorVersion;
			USHORT SubSystemMajorVersion;
		};
		ULONG SubSystemVersion;
	};
	ULONG GpValue;
	USHORT ImageCharacteristics;
	USHORT DllCharacteristics;
	USHORT Machine;
	UCHAR ImageContainsCode;
	UCHAR Spare1;
	ULONG LoaderFlags;
	ULONG ImageFileSize;
	ULONG Reserved[1];
} SECTION_IMAGE_INFORMATION, *PSECTION_IMAGE_INFORMATION;

typedef enum _KEY_VALUE_INFORMATION_CLASS { 
	KeyValueBasicInformation,
	KeyValueFullInformation,
	KeyValuePartialInformation,
	KeyValueFullInformationAlign64,
	KeyValuePartialInformationAlign64,
	MaxKeyValueInfoClass
} KEY_VALUE_INFORMATION_CLASS;

typedef struct _KEY_VALUE_BASIC_INFORMATION {
	ULONG TitleIndex;
	ULONG Type;
	ULONG NameLength;
	WCHAR Name[1];
} KEY_VALUE_BASIC_INFORMATION, *PKEY_VALUE_BASIC_INFORMATION;

typedef struct _KEY_VALUE_FULL_INFORMATION {
	ULONG TitleIndex;
	ULONG Type;
	ULONG DataOffset;
	ULONG DataLength;
	ULONG NameLength;
	WCHAR Name[1];
} KEY_VALUE_FULL_INFORMATION, *PKEY_VALUE_FULL_INFORMATION;

typedef struct _KEY_VALUE_PARTIAL_INFORMATION {
	ULONG TitleIndex;
	ULONG Type;
	ULONG DataLength;
	UCHAR Data[1];
} KEY_VALUE_PARTIAL_INFORMATION, *PKEY_VALUE_PARTIAL_INFORMATION;

typedef CONST char *PCSZ;

BOOL init_ntdll();
extern const apilib_api_table apitable_ntdll;

/*** AUTOGENERATED APILIST DECLARATIONS BEGIN ***/
STUB NtCreateProcess_stub;
STUB NtFsControlFile_stub;
STUB NtOpenDirectoryObject_stub;
STUB NtOpenSymbolicLinkObject_stub;
STUB NtQueryDirectoryObject_stub;
STUB NtQueryInformationFile_stub;
STUB NtQuerySymbolicLinkObject_stub;
STUB NtSetInformationFile_stub;
STUB RtlAddAccessAllowedAce_stub;
STUB RtlAddAce_stub;
STUB RtlClearBits_stub;
STUB RtlCreateAcl_stub;
STUB RtlDecompressBuffer_stub;
STUB RtlDeleteElementGenericTable_stub;
STUB RtlEnumerateGenericTableWithoutSplaying_stub;
STUB RtlFindMessage_stub;
STUB RtlFindSetBits_stub;
STUB RtlFormatMessage_stub;
STUB RtlInitializeGenericTable_stub;
STUB RtlInsertElementGenericTable_stub;
STUB RtlLengthSecurityDescriptor_stub;
STUB RtlLookupElementGenericTable_stub;
STUB RtlNewSecurityObject_stub;
STUB RtlNormalizeProcessParams_stub;
STUB RtlNumberOfSetBits_stub;
STUB RtlQueryInformationAcl_stub;
STUB RtlQueryRegistryValues_stub;
STUB RtlSetBits_stub;
STUB RtlSetDaclSecurityDescriptor_stub;
STUB RtlSetGroupSecurityDescriptor_stub;
STUB RtlSetOwnerSecurityDescriptor_stub;
STUB RtlTimeToTimeFields_stub;
STUB RtlValidSecurityDescriptor_stub;
STUB RtlWriteRegistryValue_stub;
STUB ZwCreateProcess_stub;
STUB ZwFsControlFile_stub;
STUB ZwOpenDirectoryObject_stub;
STUB ZwOpenSymbolicLinkObject_stub;
STUB ZwQueryDirectoryObject_stub;
STUB ZwQueryInformationFile_stub;
STUB ZwQuerySymbolicLinkObject_stub;
STUB ZwSetInformationFile_stub;
ULONG NTAPI RtlNtStatusToDosError(NTSTATUS Status);
NTSTATUS NTAPI NtRaiseHardError(IN NTSTATUS ErrorStatus, IN ULONG NumberOfParameters, IN ULONG UnicodeStringParameterMask, IN PULONG_PTR Parameters, IN ULONG ValidResponseOptions, OUT PULONG Response);
NTSTATUS NTAPI LdrLoadDll(IN PWCHAR PathToFile OPTIONAL, IN ULONG Flags OPTIONAL, IN PUNICODE_STRING ModuleFileName, OUT PHANDLE ModuleHandle);
NTSTATUS NTAPI LdrUnloadDll(IN PHANDLE ModuleHandle);
NTSTATUS NTAPI NtCreateSection(OUT PHANDLE SectionHandle, IN ACCESS_MASK DesiredAccess, IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL, IN PLARGE_INTEGER MaximumSize OPTIONAL, IN ULONG SectionPageProtection OPTIONAL, IN ULONG AllocationAttributes, IN HANDLE FileHandle OPTIONAL);
NTSTATUS NTAPI NtDeviceIoControlFile(IN HANDLE FileHandle, IN HANDLE Event, IN PVOID ApcRoutine, IN PVOID ApcContext, OUT PIO_STATUS_BLOCK IoStatusBlock, IN ULONG IoControlCode, IN PVOID InputBuffer, IN ULONG InputBufferLength, OUT PVOID OutputBuffer, IN ULONG OutputBufferLength);
NTSTATUS NTAPI NtDisplayString(IN PUNICODE_STRING String);
NTSTATUS NTAPI NtOpenFile(OUT PHANDLE FileHandle, IN ACCESS_MASK DesiredAccess, IN POBJECT_ATTRIBUTES ObjectAttributes, OUT PIO_STATUS_BLOCK IoStatusBlock, IN ULONG ShareAccess, IN ULONG OpenOptions);
NTSTATUS NTAPI NtQuerySection(IN HANDLE SectionHandle, IN SECTION_INFORMATION_CLASS SectionInformationClass, OUT PVOID SectionInformation, IN SIZE_T SectionInformationLength, OUT PSIZE_T ResultLength OPTIONAL);
NTSTATUS NTAPI NtReadFile(IN HANDLE FileHandle, IN HANDLE Event OPTIONAL, IN PVOID ApcRoutine OPTIONAL, IN PVOID ApcContext OPTIONAL, OUT PIO_STATUS_BLOCK IoStatusBlock, OUT PVOID Buffer, IN ULONG Length, IN PLARGE_INTEGER ByteOffset OPTIONAL, IN PULONG Key OPTIONAL);
NTSTATUS NTAPI NtWriteFile(IN HANDLE FileHandle, IN HANDLE Event OPTIONAL, IN PVOID ApcRoutine OPTIONAL, IN PVOID ApcContext OPTIONAL, OUT PIO_STATUS_BLOCK IoStatusBlock, IN PVOID Buffer, IN ULONG Length, IN PLARGE_INTEGER ByteOffset OPTIONAL, IN PULONG Key OPTIONAL);
NTSTATUS NTAPI NtClose(IN HANDLE Handle);
NTSTATUS NTAPI NtCreateEvent(OUT PHANDLE EventHandle, IN ACCESS_MASK DesiredAccess, IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL, IN EVENT_TYPE EventType, IN BOOLEAN InitialState);
NTSTATUS NTAPI NtDuplicateObject(IN HANDLE SourceProcessHandle, IN HANDLE SourceHandle, IN HANDLE TargetProcessHandle OPTIONAL, OUT PHANDLE TargetHandle OPTIONAL, IN ACCESS_MASK DesiredAccess, IN ULONG HandleAttributes, IN ULONG Options);
NTSTATUS NTAPI NtSetEvent(IN HANDLE EventHandle, OUT PLONG PreviousState OPTIONAL);
NTSTATUS NTAPI NtWaitForSingleObject(IN HANDLE Handle, IN BOOLEAN Alertable, IN PLARGE_INTEGER Timeout);
NTSTATUS NtAllocateVirtualMemory(IN HANDLE ProcessHandle, IN OUT PVOID *BaseAddress, IN ULONG_PTR ZeroBits, IN OUT PSIZE_T RegionSize, IN ULONG AllocationType, IN ULONG Protect);
NTSTATUS NtFreeVirtualMemory(IN HANDLE ProcessHandle, IN OUT PVOID *BaseAddress, IN OUT PSIZE_T RegionSize, IN ULONG FreeType);
NTSTATUS NTAPI NtOpenProcess(OUT PHANDLE ProcessHandle, IN ACCESS_MASK AccessMask, IN POBJECT_ATTRIBUTES ObjectAttributes, IN PCLIENT_ID ClientId);
NTSTATUS NTAPI NtResumeProcess(IN HANDLE ProcessHandle);
NTSTATUS NTAPI NtSuspendProcess(IN HANDLE ProcessHandle);
NTSTATUS NTAPI NtTerminateProcess(IN HANDLE ProcessHandle OPTIONAL, IN NTSTATUS ExitStatus);
NTSTATUS NTAPI NtOpenKey(OUT PHANDLE KeyHandle, IN ACCESS_MASK DesiredAccess, IN POBJECT_ATTRIBUTES ObjectAttributes);
NTSTATUS NTAPI NtQueryValueKey(IN HANDLE KeyHandle, IN PUNICODE_STRING ValueName, IN KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass, OUT PVOID KeyValueInformation OPTIONAL, IN ULONG Length, OUT PULONG ResultLength);
NTSTATUS NTAPI NtCreateThread(OUT PHANDLE ThreadHandle, IN ACCESS_MASK DesiredAccess, IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL, IN HANDLE ProcessHandle, OUT PCLIENT_ID ClientId, IN PCONTEXT ThreadContext, IN PINITIAL_TEB InitialTeb, IN BOOLEAN CreateSuspended);
NTSTATUS NTAPI NtGetContextThread(IN HANDLE ThreadHandle, OUT PCONTEXT pContext);
NTSTATUS NTAPI NtSetContextThread(IN HANDLE ThreadHandle, IN PCONTEXT Context);
NTSTATUS NTAPI NtOpenThread(OUT PHANDLE ThreadHandle, IN ACCESS_MASK AccessMask, IN POBJECT_ATTRIBUTES ObjectAttributes, IN PCLIENT_ID ClientId);
NTSTATUS NTAPI NtResumeThread(IN HANDLE ThreadHandle, OUT PULONG SuspendCount OPTIONAL);
NTSTATUS NTAPI NtSuspendThread(IN HANDLE ThreadHandle, OUT PULONG PreviousSuspendCount OPTIONAL);
NTSTATUS NTAPI NtTerminateThread(IN HANDLE ThreadHandle, IN NTSTATUS ExitStatus);
LONG NTAPI RtlCompareUnicodeString(IN PUNICODE_STRING String1, IN PUNICODE_STRING String2, IN BOOLEAN CaseInsensitive);
NTSTATUS NTAPI RtlCopySid(IN ULONG BufferLength, IN PSID Dest, IN PSID Src);
NTSTATUS NTAPI RtlCreateSecurityDescriptor(IN PSECURITY_DESCRIPTOR SecurityDescriptor, IN ULONG Revision);
NTSTATUS NTAPI RtlDeleteCriticalSection(RTL_CRITICAL_SECTION *crit);
BOOLEAN NTAPI RtlDosPathNameToNtPathName_U(IN PCWSTR DosName, OUT PUNICODE_STRING NtName, OUT PCWSTR *PartName, OUT PVOID RelativeName);
NTSTATUS NTAPI RtlEnterCriticalSection(RTL_CRITICAL_SECTION *crit);
BOOLEAN NTAPI RtlEqualUnicodeString(IN CONST PUNICODE_STRING String1, IN CONST PUNICODE_STRING String2, IN BOOLEAN CaseInsensitive);
NTSTATUS NTAPI RtlExpandEnvironmentStrings_U(IN PWSTR Environment, IN PUNICODE_STRING Source, OUT PUNICODE_STRING Destination, OUT PULONG Length);
VOID NTAPI RtlFreeOemString(IN POEM_STRING OemString);
PVOID NTAPI RtlFreeSid(IN PSID Sid);
VOID NTAPI RtlFreeUnicodeString(IN PUNICODE_STRING UnicodeString);
VOID NTAPI RtlInitAnsiString(IN OUT PANSI_STRING DestinationString, IN PCSZ SourceString);
NTSTATUS NTAPI RtlInitAnsiStringEx(IN OUT PANSI_STRING DestinationString, IN PCSZ SourceString);
VOID NTAPI RtlInitializeBitMap(OUT PRTL_BITMAP BitMapHeader, IN PULONG BitMapBuffer, IN ULONG SizeOfBitMap);
NTSTATUS NTAPI RtlInitializeCriticalSection(RTL_CRITICAL_SECTION *crit);
NTSTATUS NTAPI RtlInitializeCriticalSectionAndSpinCount(RTL_CRITICAL_SECTION *crit, ULONG spincount);
NTSTATUS NTAPI RtlInitializeCriticalSectionEx(RTL_CRITICAL_SECTION *crit, ULONG spincount, ULONG flags);
NTSTATUS NTAPI RtlInitializeSid(IN PSID Sid, IN PSID_IDENTIFIER_AUTHORITY IdentifierAuthority, IN UCHAR SubAuthorityCount);
VOID NTAPI RtlInitUnicodeString(IN OUT PUNICODE_STRING DestinationString, IN PCWSTR SourceString);
NTSTATUS NTAPI RtlLeaveCriticalSection(RTL_CRITICAL_SECTION *crit);
ULONG NTAPI RtlLengthRequiredSid(IN ULONG SubAuthorityCount);
ULONG NTAPI RtlLengthSid(IN PSID Sid);
NTSTATUS NTAPI RtlOemToUnicodeN(PWCHAR UnicodeString, ULONG UnicodeSize, PULONG ResultSize, PCCH OemString, ULONG OemSize);
BOOLEAN NTAPI RtlPrefixUnicodeString(PUNICODE_STRING String1, PUNICODE_STRING String2, BOOLEAN CaseInsensitive);
VOID NTAPI RtlRaiseStatus(IN NTSTATUS Status);
PULONG NTAPI RtlSubAuthoritySid(IN PSID Sid, IN ULONG SubAuthority);
NTSTATUS NTAPI RtlSystemTimeToLocalTime(IN PLARGE_INTEGER SystemTime, OUT PLARGE_INTEGER LocalTime);
NTSTATUS NTAPI RtlTryEnterCriticalSection(RTL_CRITICAL_SECTION *crit);
NTSTATUS NTAPI RtlUnicodeStringToOemString(IN OUT POEM_STRING OemDest, IN PUNICODE_STRING UniSource, IN BOOLEAN AllocateDestinationString);
NTSTATUS NTAPI RtlUnicodeToOemN(PWCHAR UnicodeString, ULONG UnicodeSize, PULONG ResultSize, PCCH OemString, ULONG OemSize);
WCHAR NTAPI RtlUpcaseUnicodeChar(IN WCHAR Source);
NTSTATUS NTAPI RtlUpcaseUnicodeString(IN OUT PUNICODE_STRING UniDest, IN PUNICODE_STRING UniSource, IN BOOLEAN AllocateDestinationString);
NTSTATUS NTAPI RtlGetVersion(OUT PRTL_OSVERSIONINFOW lpVersionInformation);
NTSTATUS NTAPI RtlVerifyVersionInfo(IN PRTL_OSVERSIONINFOEXW VersionInfo, IN ULONG TypeMask, IN ULONGLONG ConditionMask);
NTSTATUS NTAPI NtQuerySystemInformation(IN ULONG SystemInformationClass, IN OUT PVOID SystemInformation, IN ULONG SystemInformationLength, OUT PULONG ReturnLength OPTIONAL);
NTSTATUS NTAPI NtQuerySystemTime(PLARGE_INTEGER SystemTime);
NTSTATUS NTAPI NtShutdownSystem(IN SHUTDOWN_ACTION Action);
NTSTATUS NTAPI NtAdjustPrivilegesToken(IN HANDLE TokenHandle, IN BOOLEAN DisableAllPrivileges, IN PTOKEN_PRIVILEGES TokenPrivileges, IN ULONG PreviousPrivilegesLength, OUT PTOKEN_PRIVILEGES PreviousPrivileges OPTIONAL, OUT PULONG RequiredLength OPTIONAL);
NTSTATUS NTAPI NtOpenProcessToken(IN HANDLE ProcessHandle, IN ACCESS_MASK DesiredAccess, OUT PHANDLE TokenHandle);
/*** AUTOGENERATED APILIST DECLARATIONS END ***/

#endif
