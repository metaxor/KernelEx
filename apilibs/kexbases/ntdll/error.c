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

/* MAKE_EXPORT RtlNtStatusToDosError=RtlNtStatusToDosError */
ULONG NTAPI RtlNtStatusToDosError(NTSTATUS Status)
{
	return K32_RtlNtStatusToDosError(Status);
}

/* MAKE_EXPORT NtRaiseHardError=NtRaiseHardError */
NTSTATUS NTAPI NtRaiseHardError(IN NTSTATUS ErrorStatus,
		IN ULONG NumberOfParameters,
		IN ULONG UnicodeStringParameterMask,
		IN PULONG_PTR Parameters,
		IN ULONG ValidResponseOptions,
		OUT PULONG Response)
{
	ULONG DosError = 0;
	char lpAppName[80];
	char lpTitle[80];
	LPVOID lpMessage = NULL;
	BOOL result = FALSE;
	UINT uType = 0;
	BOOL fWait = FALSE;

	if(IsBadReadPtr(Response, sizeof(ULONG)))
		return STATUS_INVALID_PARAMETER;

	if(ValidResponseOptions > OptionCancelTryContinue)
		return STATUS_INVALID_PARAMETER;

	result = kexGetProcessName(GetCurrentProcessId(), lpAppName);

	if(!result)
		wsprintf(lpTitle, "<unknown> - Application Error");
	else
		wsprintf(lpTitle, "%s - Application Error", lpAppName);

	DosError = RtlNtStatusToDosError(ErrorStatus);

	if(ErrorStatus == STATUS_DLL_INIT_FAILED_LOGOFF)
	{
		lpMessage = "The application failed to initialize because the window station is shutting down.";
		DosError = 0;
	}

	if(DosError == ERROR_MR_MID_NOT_FOUND)
	{
		if(!result)
			wsprintf(lpTitle, "<unknown> - Unknown Hard Error");
		else
			wsprintf(lpTitle, "%s - Unknown Hard Error", lpAppName);
		lpMessage = malloc(40 * sizeof(CHAR));
		wsprintf((char*)lpMessage, "0x%08lx: Unknown Hard Error", ErrorStatus);
	}

	if(lpMessage == NULL)
		kexErrorCodeToString(DosError, (LPSTR)&lpMessage);

	switch(ValidResponseOptions)
	{
	case OptionAbortRetryIgnore:
		uType = MB_ABORTRETRYIGNORE;
		break;

	case OptionOk:
		uType = MB_OK;
		break;

	case OptionOkCancel:
		uType = MB_OKCANCEL;
		break;

	case OptionRetryCancel:
		uType = MB_RETRYCANCEL;
		break;

	case OptionYesNo:
		uType = MB_YESNO;
		break;

	case OptionYesNoCancel:
		uType = MB_YESNOCANCEL;
		break;

	case OptionShutdownSystem:
		uType = MB_OK;
		break;

	case OptionOkNoWait:
		fWait = FALSE;
		uType = MB_OK;
		break;

	case OptionCancelTryContinue:
		uType = MB_CANCELTRYCONTINUE;
		break;
	}

	*Response = Win32RaiseHardError((LPCSTR)lpMessage, lpTitle, uType, fWait);

	return STATUS_SUCCESS;
}
