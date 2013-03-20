/*
 *  KernelEx
 *  Copyright (C) 2006-2008, Xeno86
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

#include "common.h"
#include "unifwd.h"

#define DUMMY_FUNCTION(x)

/* MAKE_EXPORT BuildCommDCBAndTimeoutsW_new=BuildCommDCBAndTimeoutsW */
BOOL WINAPI BuildCommDCBAndTimeoutsW_new(LPCWSTR lpDefW, LPDCB lpDCB, LPCOMMTIMEOUTS lpCommTimeouts)
{
	ALLOC_WtoA(lpDef);

	return BuildCommDCBAndTimeoutsA(lpDefA, lpDCB, lpCommTimeouts);
}

/* MAKE_EXPORT BuildCommDCBW_new=BuildCommDCBW */
BOOL WINAPI BuildCommDCBW_new(LPCWSTR lpDefW, LPDCB lpDCB)
{
	ALLOC_WtoA(lpDef);

	return BuildCommDCBA(lpDefA, lpDCB);
}

/* MAKE_EXPORT CallNamedPipeW_new=CallNamedPipeW */
BOOL WINAPI CallNamedPipeW_new(LPCWSTR lpNamedPipeNameW, LPVOID lpInBuffer, DWORD nInBufferSize, LPVOID lpOutBuffer, DWORD nOutBufferSize, LPDWORD lpBytesRead, DWORD nTimeOut)
{
	ALLOC_WtoA(lpNamedPipeName);

	return CallNamedPipeA(lpNamedPipeNameA, lpInBuffer, nInBufferSize, lpOutBuffer, nOutBufferSize, lpBytesRead, nTimeOut);
}

/* MAKE_EXPORT CommConfigDialogW_new=CommConfigDialogW */
BOOL WINAPI CommConfigDialogW_new(LPCWSTR lpszNameW, HWND hWnd, LPCOMMCONFIG lpCC)
{
	ALLOC_WtoA(lpszName);

	return CommConfigDialogA(lpszNameA, hWnd, lpCC);
}

/* MAKE_EXPORT CreateEventW_new=CreateEventW */
HANDLE WINAPI CreateEventW_new(LPSECURITY_ATTRIBUTES lpEventAttributes, BOOL bManualReset, BOOL bInitialState, LPCWSTR lpNameW)
{
	ALLOC_WtoA(lpName);

	return CreateEventA(lpEventAttributes, bManualReset, bInitialState, lpNameA);
}

/* MAKE_EXPORT CreateFileMappingW_new=CreateFileMapping */
HANDLE WINAPI CreateFileMappingW_new(HANDLE hFile, LPSECURITY_ATTRIBUTES lpAttributes, DWORD flProtect, DWORD dwMaximumSizeHigh, DWORD dwMaximumSizeLow, LPCWSTR lpNameW)
{
	ALLOC_WtoA(lpName);

	return CreateFileMappingA(hFile, lpAttributes, flProtect, dwMaximumSizeHigh, dwMaximumSizeLow, lpNameA);
}

/* MAKE_EXPORT CreateMailslotW_new=CreateMailslotW */
HANDLE WINAPI CreateMailslotW_new(LPCWSTR lpNameW, DWORD nMaxMessageSize, DWORD lReadTimeout, LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{
	ALLOC_WtoA(lpName);

	return CreateMailslotA(lpNameA, nMaxMessageSize, lReadTimeout, lpSecurityAttributes);
}

/* MAKE_EXPORT CreateMutexW_new=CreateMutexW */
HANDLE WINAPI CreateMutexW_new(LPSECURITY_ATTRIBUTES lpMutexAttributes, BOOL bInitialOwner, LPCWSTR lpNameW)
{
	ALLOC_WtoA(lpName);

	return CreateMutexA(lpMutexAttributes, bInitialOwner, lpNameA);
}

/* MAKE_EXPORT CreateSemaphoreW_new=CreateSemaphoreW */
HANDLE WINAPI CreateSemaphoreW_new(LPSECURITY_ATTRIBUTES lpSemaphoreAttributes, LONG lInitialCount, LONG lMaximumCount, LPCWSTR lpNameW)
{
	ALLOC_WtoA(lpName);

	return CreateSemaphoreA(lpSemaphoreAttributes, lInitialCount, lMaximumCount, lpNameA);
}

/* MAKE_EXPORT CreateWaitableTimerW_new=CreateWaitableTimerW */
HANDLE WINAPI CreateWaitableTimerW_new(LPSECURITY_ATTRIBUTES lpTimerAttributes, BOOL bManualReset, LPCWSTR lpTimerNameW)
{
	ALLOC_WtoA(lpTimerName);

	return CreateWaitableTimerA(lpTimerAttributes, bManualReset, lpTimerNameA);
}

/* MAKE_EXPORT EnumCalendarInfoExW_new=EnumCalendarInfoExW */
BOOL WINAPI EnumCalendarInfoExW_new(CALINFO_ENUMPROCEX pCalInfoEnumProcEx, LCID Locale, CALID Calendar, CALTYPE CalType)
{
	SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
	return FALSE;
}

/* MAKE_EXPORT EnumCalendarInfoW_new=EnumCalendarInfoW */
BOOL WINAPI EnumCalendarInfoW_new(CALINFO_ENUMPROC pCalInfoEnumProc, LCID Locale, CALID Calendar, CALTYPE CalType)
{
	SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
	return FALSE;
}

/* MAKE_EXPORT EnumDateFormatsExW_new=EnumDateFormatsExW */
BOOL WINAPI EnumDateFormatsExW_new(DATEFMT_ENUMPROCEX lpDateFmtEnumProcEx, LCID Locale, DWORD dwFlags)
{
	SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
	return FALSE;
}

/* MAKE_EXPORT EnumDateFormatsW_new=EnumDateFormatsW */
BOOL WINAPI EnumDateFormatsW_new(DATEFMT_ENUMPROC lpDateFmtEnumProc, LCID Locale, DWORD dwFlags)
{
	SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
	return FALSE;
}

/* MAKE_EXPORT EnumSystemCodePagesW_new=EnumSystemCodePagesW */
BOOL WINAPI EnumSystemCodePagesW_new(CODEPAGE_ENUMPROC lpCodePageEnumProc, DWORD dwFlags)
{
	SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
	return FALSE;
}

/* MAKE_EXPORT EnumSystemLocalesW_new=EnumSystemLocalesW */
BOOL WINAPI EnumSystemLocalesW_new(LOCALE_ENUMPROC lpLocaleEnumProc, DWORD dwFlags)
{
	SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
	return FALSE;
}

/* MAKE_EXPORT EnumTimeFormatsW_new=EnumTimeFormatsW */
BOOL WINAPI EnumTimeFormatsW_new(TIMEFMT_ENUMPROC lpTimeFmtEnumProc, LCID Locale, DWORD dwFlags)
{
	SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
	return FALSE;
}

/* MAKE_EXPORT ExpandEnvironmentStringsW_new=ExpandEnvironmentStringsW */
DWORD WINAPI ExpandEnvironmentStringsW_new(LPCWSTR lpSrcW, LPWSTR lpDstW, DWORD nSize)
{
	char *lpDstA = (char*)malloc(nSize);
	DWORD result = 0;

	if(lpDstA == NULL)
		return 0;

	ALLOC_WtoA(lpSrc);

	result = ExpandEnvironmentStringsA(lpSrcA, lpDstA, nSize);

	STACK_AtoW(lpDstA, lpDstW);

	return result;
}

/* MAKE_EXPORT FatalAppExitW_new=FatalAppExitW */
void WINAPI FatalAppExitW_new(UINT uAction, LPCWSTR lpMessageTextW)
{
	ALLOC_WtoA(lpMessageText);

	FatalAppExitA(uAction, lpMessageTextA);
}

/* MAKE_EXPORT FillConsoleOutputCharacterW_new=FillConsoleOutputCharacterW */
BOOL WINAPI FillConsoleOutputCharacterW_new(HANDLE hConsoleOutput, WCHAR cCharacterW, DWORD nLength, COORD dwWriteCoord, LPDWORD lpNumberOfCharsWritten)
{
	CHAR cCharacterA = '\0';

	WtoA(cCharacter, 1);

	return FillConsoleOutputCharacterA(hConsoleOutput, cCharacterA, nLength, dwWriteCoord, lpNumberOfCharsWritten);
}

/* MAKE_EXPORT FormatMessageW_new=FormatMessageW */
DWORD WINAPI FormatMessageW_new(DWORD dwFlags, LPCVOID lpSource, DWORD dwMessageId,
  DWORD dwLanguageId, LPWSTR lpBufferW, DWORD nSize, va_list* Arguments)
{
	DWORD result = 0;
	LPSTR lpBufferA = (dwFlags & FORMAT_MESSAGE_ALLOCATE_BUFFER) ? NULL : (LPSTR)malloc(nSize);

	if(lpBufferA == NULL && (dwFlags & FORMAT_MESSAGE_ALLOCATE_BUFFER))
		return 0;

	FormatMessageA(dwFlags, lpSource, dwMessageId, dwLanguageId, lpBufferA, nSize, Arguments);

	STACK_AtoW(lpBufferA, lpBufferW);

	return result;
}

DUMMY_FUNCTION(GetCPInfoExW);
DUMMY_FUNCTION(GetCalendarInfoW);
DUMMY_FUNCTION(GetComputerNameW);
DUMMY_FUNCTION(GetConsoleTitleW);
DUMMY_FUNCTION(GetCurrencyFormatW);
DUMMY_FUNCTION(GetDateFormatW);
DUMMY_FUNCTION(GetDriveTypeW);
DUMMY_FUNCTION(GetEnvironmentVariableW);
DUMMY_FUNCTION(GetLogicalDriveStringsW);
DUMMY_FUNCTION(GetNamedPipeHandleStateW);
DUMMY_FUNCTION(GetNumberFormatW);
DUMMY_FUNCTION(GetPrivateProfileIntW);
DUMMY_FUNCTION(GetPrivateProfileSectionNamesW);
DUMMY_FUNCTION(GetPrivateProfileSectionW);
DUMMY_FUNCTION(GetPrivateProfileStringW);
DUMMY_FUNCTION(GetPrivateProfileStructW);
DUMMY_FUNCTION(GetProfileIntW);
DUMMY_FUNCTION(GetProfileSectionW);
DUMMY_FUNCTION(GetProfileStringW);
DUMMY_FUNCTION(GetTimeFormatW);
DUMMY_FUNCTION(GetVolumeInformationW);
DUMMY_FUNCTION(IsBadStringPtrW);
DUMMY_FUNCTION(OpenEventW);
DUMMY_FUNCTION(OpenFileMappingW);
DUMMY_FUNCTION(OpenMutexW);
DUMMY_FUNCTION(OpenSemaphoreW);
DUMMY_FUNCTION(OpenWaitableTimerW);
DUMMY_FUNCTION(PeekConsoleInputW);
DUMMY_FUNCTION(QueryDosDeviceW);
DUMMY_FUNCTION(ReadConsoleOutputCharacterW);
DUMMY_FUNCTION(ReadConsoleOutputW);
DUMMY_FUNCTION(ReadConsoleW);
DUMMY_FUNCTION(ScrollConsoleScreenBufferW);
DUMMY_FUNCTION(SetCalendarInfoW);
DUMMY_FUNCTION(SetComputerNameW);
DUMMY_FUNCTION(SetDefaultCommConfigW);
DUMMY_FUNCTION(SetEnvironmentVariableW);
DUMMY_FUNCTION(SetLocaleInfoW);
DUMMY_FUNCTION(SetVolumeLabelW);
DUMMY_FUNCTION(WaitNamedPipeW);
DUMMY_FUNCTION(WriteConsoleInputW);
DUMMY_FUNCTION(WriteConsoleOutputCharacterW);
DUMMY_FUNCTION(WriteConsoleOutputW);
DUMMY_FUNCTION(WriteConsoleW);
DUMMY_FUNCTION(WritePrivateProfileSectionW);
DUMMY_FUNCTION(WritePrivateProfileStringW);
DUMMY_FUNCTION(WritePrivateProfileStructW);
DUMMY_FUNCTION(WriteProfileSectionW);
DUMMY_FUNCTION(WriteProfileStringW);
