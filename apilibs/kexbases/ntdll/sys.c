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

/* MAKE_EXPORT RtlGetVersion=RtlGetVersion */
NTSTATUS NTAPI RtlGetVersion(
	OUT	PRTL_OSVERSIONINFOW lpVersionInformation
)
{
	return GetVersionExW_ORIG(lpVersionInformation);
}

/* MAKE_EXPORT RtlVerifyVersionInfo=RtlVerifyVersionInfo */
NTSTATUS NTAPI RtlVerifyVersionInfo(
  IN	PRTL_OSVERSIONINFOEXW VersionInfo,
  IN	ULONG TypeMask,
  IN	ULONGLONG ConditionMask
)
{
	return VerifyVersionInfoW_WINXP(VersionInfo, TypeMask, ConditionMask);
}

/* MAKE_EXPORT ZwQuerySystemInformation=NtQuerySystemInformation */
/* MAKE_EXPORT ZwQuerySystemInformation=ZwQuerySystemInformation */
NTSTATUS NTAPI ZwQuerySystemInformation(
	IN		ULONG SystemInformationClass,
	IN OUT	PVOID SystemInformation,
	IN		ULONG SystemInformationLength,
	OUT		PULONG ReturnLength OPTIONAL
)
{
	return 0;
}

/* MAKE_EXPORT ZwShutdownSystem=NtShutdownSystem */
/* MAKE_EXPORT ZwShutdownSystem=ZwShutdownSystem */
NTSTATUS NTAPI ZwShutdownSystem(IN SHUTDOWN_ACTION Action)
{
	typedef VOID (WINAPI *EXITKERNEL)(VOID);
	HMODULE hModule = NULL;
	EXITKERNEL ExitKernel = NULL;

	switch(Action)
	{
	case ShutdownNoReboot:
		ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE, 0);
		return STATUS_SUCCESS;
		break;

	case ShutdownReboot:
		ExitWindowsEx(EWX_REBOOT | EWX_FORCE, 0);
		return STATUS_SUCCESS;
		break;

	case ShutdownPowerOff:
		hModule = (HMODULE)LoadLibrary16("KRNL386.EXE");

		if((DWORD)hModule < 32)
			return (NTSTATUS)hModule;

		ExitKernel = (EXITKERNEL)GetProcAddress16(hModule, "EXITKERNEL");

		if(ExitKernel == NULL)
		{
			FreeLibrary16(hModule);
			return STATUS_INVALID_PARAMETER;
		}

		__asm	push 0
		__asm	mov edx, [ExitKernel]
		__asm	call ds:QT_Thunk
		FreeLibrary16(hModule);
		return STATUS_SUCCESS;
		break;
	}

	return STATUS_INVALID_PARAMETER;
}