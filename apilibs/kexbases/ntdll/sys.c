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
#include "../user32/desktop.h"

DWORD WINAPI Ring0Idle(PVOID lParam)
{
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);

	DisableOEMLayer();

	while(1)
	{
		__asm {
			cli
		}
	}

	return 0;
}

DWORD WINAPI Ring0Reboot(PVOID lParam)
{

	// FIXME : jump at FFFF:0000h

	__asm int	19h ; Bootstrap loader

	return Ring0Idle(NULL);
}

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

/* MAKE_EXPORT NtQuerySystemInformation=NtQuerySystemInformation */
/* MAKE_EXPORT NtQuerySystemInformation=ZwQuerySystemInformation */
NTSTATUS NTAPI NtQuerySystemInformation(
	IN		ULONG SystemInformationClass,
	IN OUT	PVOID SystemInformation,
	IN		ULONG SystemInformationLength,
	OUT		PULONG ReturnLength OPTIONAL
)
{
	return 0;
}

/* MAKE_EXPORT NtQuerySystemTime=NtQuerySystemTime */
/* MAKE_EXPORT NtQuerySystemTime=ZwQuerySystemTime */
NTSTATUS NTAPI NtQuerySystemTime(PLARGE_INTEGER SystemTime)
{
	FILETIME SysTime;

	if(SystemTime == NULL || IsBadWritePtr(SystemTime, sizeof(LARGE_INTEGER)))
		return STATUS_INVALID_PARAMETER;

	GetSystemTimeAsFileTime(&SysTime);

	SystemTime->LowPart = SystemTime->u.LowPart = SysTime.dwLowDateTime;
	SystemTime->HighPart = SystemTime->u.HighPart = SysTime.dwHighDateTime;
	SystemTime->QuadPart = SysTime.dwLowDateTime + SysTime.dwHighDateTime;

	return STATUS_SUCCESS;
}

/* MAKE_EXPORT NtShutdownSystem=NtShutdownSystem */
/* MAKE_EXPORT NtShutdownSystem=ZwShutdownSystem */
NTSTATUS NTAPI NtShutdownSystem(IN SHUTDOWN_ACTION Action)
{
	typedef VOID (WINAPI *EXITKERNEL)(VOID);
	HMODULE hModule = NULL;
	EXITKERNEL ExitKernel = NULL;

	switch(Action)
	{
	case ShutdownNoReboot:
		CallRing0((DWORD)Ring0Idle, NULL);
		return STATUS_SUCCESS;

	case ShutdownReboot:
		CallRing0((DWORD)Ring0Reboot, NULL);
		return STATUS_SUCCESS;

	case ShutdownPowerOff:

		ExitKernel = (EXITKERNEL)GetProcAddress16(g_hKernel16, "EXITKERNEL");

		if(ExitKernel == NULL)
			return STATUS_INVALID_PARAMETER;

		__asm	push 0
		__asm	mov edx, [ExitKernel]
		__asm	call ds:QT_Thunk

		return STATUS_SUCCESS;
	}

	return STATUS_INVALID_PARAMETER;
}