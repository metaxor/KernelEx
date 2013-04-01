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

	if(!result)
	{
		free(lpDstA);
		return 0;
	}

	STACK_AtoW(lpDstA, lpDstW);

	free(lpDstA);

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
DWORD WINAPI FormatMessageW_new(DWORD dwFlags, LPCVOID lpSource, DWORD dwMessageId, DWORD dwLanguageId, LPWSTR lpBufferW, DWORD nSize, va_list* Arguments)
{
	DWORD result = 0;
	LPSTR lpBufferA = (dwFlags & FORMAT_MESSAGE_ALLOCATE_BUFFER) ? NULL : (LPSTR)malloc(nSize);

	if(lpBufferA == NULL && (dwFlags & FORMAT_MESSAGE_ALLOCATE_BUFFER))
		return 0;

	result = FormatMessageA(dwFlags, lpSource, dwMessageId, dwLanguageId, lpBufferA, nSize, Arguments);

	if(!result)
	{
		if(dwFlags & FORMAT_MESSAGE_ALLOCATE_BUFFER)
			free(lpBufferA);

		return 0;
	}

	STACK_AtoW(lpBufferA, lpBufferW);

	free(lpBufferA);

	return result;
}

/* MAKE_EXPORT GetCPInfoExW_new=GetCPInfoExW */
BOOL WINAPI GetCPInfoExW_new(UINT CodePage, DWORD dwFlags, LPCPINFOEXW lpCPInfoExW)
{
	CPINFOEXA CPInfoExA;
	BOOL result = 0;
	WCHAR *CodePageName = NULL;

	memset(&CPInfoExA, 0, sizeof(CPINFOEXA));
	result = GetCPInfoExA(CodePage, dwFlags, &CPInfoExA);

	if(!result)
		return FALSE;

	memcpy(lpCPInfoExW, &CPInfoExA, sizeof(CPINFOEXA));

	STACK_AtoW(CPInfoExA.CodePageName, CodePageName);

	wcscpy(lpCPInfoExW->CodePageName, CodePageName);

	return result;
}

/* MAKE_EXPORT GetCalendarInfoW_new=GetCalendarInfoW */
int WINAPI GetCalendarInfoW_new(LCID Locale, CALID Calendar, CALTYPE CalType, LPWSTR lpCalDataW, int cchData, LPDWORD lpValue)
{
	int result = 0;
	LPSTR lpCalDataA = (LPSTR)malloc(cchData);

	if(lpCalDataA == NULL)
		return 0;

	result = GetCalendarInfoA(Locale, Calendar, CalType, lpCalDataA, cchData, lpValue);

	if(!result)
	{
		free(lpCalDataA);
		return 0;
	}

	STACK_AtoW(lpCalDataA, lpCalDataW);

	free(lpCalDataA);

	return result;
}

/* MAKE_EXPORT GetComputerNameW_new=GetComputerNameW */
BOOL WINAPI GetComputerNameW_new(LPWSTR lpBufferW, LPDWORD lpnSize)
{
	BOOL result = 0;
	LPSTR lpBufferA = NULL;

	if(IsBadReadPtr(lpnSize, sizeof(DWORD)) || IsBadWritePtr(lpnSize, sizeof(DWORD)))
		return FALSE;

	lpBufferA = (LPSTR)malloc(*lpnSize);

	if(lpBufferA == NULL)
		return 0;

	result = GetComputerNameA(lpBufferA, lpnSize);

	if(!result)
	{
		free(lpBufferA);
		return 0;
	}

	STACK_AtoW(lpBufferA, lpBufferW);

	free(lpBufferA);

	return result;
}

/* MAKE_EXPORT GetConsoleTitleW_new=GetConsoleTitleW */
DWORD WINAPI GetConsoleTitleW_new(LPWSTR lpConsoleTitleW, DWORD nSize)
{
	DWORD result = 0;
	LPSTR lpConsoleTitleA = (LPSTR)malloc(nSize);

	if(lpConsoleTitleA == NULL)
		return 0;

	result = GetConsoleTitleA(lpConsoleTitleA, nSize);

	if(!result)
	{
		free(lpConsoleTitleA);
		return 0;
	}

	STACK_AtoW(lpConsoleTitleA, lpConsoleTitleW);

	free(lpConsoleTitleA);

	return result;
}

/* MAKE_EXPORT GetCurrencyFormatW_new=GetCurrencyFormatW */
int WINAPI GetCurrencyFormatW_new(LCID Locale, DWORD dwFlags, LPCWSTR lpValueW, CONST CURRENCYFMTW *lpFormatW, LPWSTR lpCurrencyStrW, int cchCurrency)
{
	CURRENCYFMTA FormatA;
	int result = 0;
	LPSTR lpCurrencyStrA = (LPSTR)malloc(cchCurrency);

	if(lpCurrencyStrA == NULL || IsBadReadPtr(lpFormatW, sizeof(CURRENCYFMTW)))
		return 0;

	ALLOC_WtoA(lpValue);

	memcpy(&FormatA, lpFormatW, sizeof(CURRENCYFMTW));

	STACK_WtoA(lpFormatW->lpDecimalSep, FormatA.lpDecimalSep);
	STACK_WtoA(lpFormatW->lpThousandSep, FormatA.lpThousandSep);

	result = GetCurrencyFormatA(Locale, dwFlags, lpValueA, &FormatA, lpCurrencyStrA, cchCurrency);

	if(!result)
	{
		free(lpCurrencyStrA);
		return 0;
	}

	STACK_AtoW(lpCurrencyStrA, lpCurrencyStrW);

	free(lpCurrencyStrA);

	return result;
}

/* MAKE_EXPORT GetDateFormatW_new=GetDateFormatW */
int WINAPI GetDateFormatW_new(LCID Locale, DWORD dwFlags, CONST SYSTEMTIME *lpDate, LPCWSTR lpFormatW, LPWSTR lpDateStrW, int cchDate)
{
	int result = 0;
	LPSTR lpDateStrA = (LPSTR)malloc(cchDate);

	if(lpDateStrA == NULL || IsBadReadPtr(lpDate, sizeof(SYSTEMTIME)))
		return 0;

	ALLOC_WtoA(lpFormat);

	result = GetDateFormatA(Locale, dwFlags, lpDate, lpFormatA, lpDateStrA, cchDate);

	if(!result)
	{
		free(lpDateStrA);
		return 0;
	}

	STACK_AtoW(lpDateStrA, lpDateStrW);

	free(lpDateStrA);

	return result;
}

/* MAKE_EXPORT GetDriveTypeW_new=GetDriveTypeW */
UINT WINAPI GetDriveTypeW_new(LPCWSTR lpRootPathNameW)
{
	ALLOC_WtoA(lpRootPathName);

	return GetDriveTypeA(lpRootPathNameA);
}

/* MAKE_EXPORT GetEnvironmentVariableW_new=GetEnvironmentVariableW */
DWORD WINAPI GetEnvironmentVariableW_new(LPCWSTR lpNameW, LPWSTR lpBufferW, DWORD nSize)
{
	DWORD result = 0;
	LPSTR lpBufferA = (LPSTR)malloc(nSize);

	if(lpBufferA == NULL)
		return 0;

	ALLOC_WtoA(lpName);

	result = GetEnvironmentVariableA(lpNameA, lpBufferA, nSize);

	if(!result)
	{
		free(lpBufferA);
		return 0;
	}

	STACK_AtoW(lpBufferA, lpBufferW);

	free(lpBufferA);

	return result;
}

/* MAKE_EXPORT GetLogicalDriveStringsW_new=GetLogicalDriveStringsW */
DWORD WINAPI GetLogicalDriveStringsW_new(DWORD nBufferLength, LPWSTR lpBufferW)
{
	DWORD result = 0;
	LPSTR lpBufferA = (LPSTR)malloc(nBufferLength);

	if(lpBufferA == NULL)
		return 0;

	result = GetLogicalDriveStringsA(nBufferLength, lpBufferA);

	if(!result)
	{
		free(lpBufferA);
		return 0;
	}

	STACK_AtoW(lpBufferA, lpBufferW);

	free(lpBufferA);

	return result;
}

/* MAKE_EXPORT GetNamedPipeHandleStateW_new=GetNamedPipeHandleStateW */
BOOL WINAPI GetNamedPipeHandleStateW_new(HANDLE hNamedPipe, LPDWORD lpState, LPDWORD lpCurInstances, LPDWORD lpMaxCollectionCount, LPDWORD lpCollectDataTimeout, LPWSTR lpUserNameW, DWORD nMaxUserNameSize)
{
	BOOL result = 0;
	LPSTR lpUserNameA = (LPSTR)malloc(nMaxUserNameSize);

	if(lpUserNameA == NULL)
		return 0;

	result = GetNamedPipeHandleStateA(hNamedPipe, lpState, lpCurInstances, lpMaxCollectionCount, lpCollectDataTimeout, lpUserNameA, nMaxUserNameSize);

	if(!result)
	{
		free(lpUserNameA);
		return 0;
	}

	STACK_AtoW(lpUserNameA, lpUserNameW);

	free(lpUserNameA);

	return result;
}

/* MAKE_EXPORT GetNumberFormatW_new=GetNumberFormatW */
int WINAPI GetNumberFormatW_new(LCID Locale, DWORD dwFlags, LPCWSTR lpValueW, CONST NUMBERFMT *lpFormatW, LPWSTR lpNumberStrW, int cchNumber)
{
	NUMBERFMT FormatA;
	int result = 0;
	LPSTR lpNumberStrA = (LPSTR)malloc(cchNumber);

	if(lpNumberStrA == NULL || IsBadReadPtr(lpFormatW, sizeof(NUMBERFMT)))
		return 0;

	ALLOC_WtoA(lpValue);

	memcpy(&FormatA, lpFormatW, sizeof(NUMBERFMT));

	STACK_WtoA(lpFormatW->lpDecimalSep, FormatA.lpDecimalSep);
	STACK_WtoA(lpFormatW->lpThousandSep, FormatA.lpThousandSep);

	result = GetNumberFormatA(Locale, dwFlags, lpValueA, &FormatA, lpNumberStrA, cchNumber);

	if(!result)
	{
		free(lpNumberStrA);
		return 0;
	}

	STACK_AtoW(lpNumberStrA, lpNumberStrW);

	free(lpNumberStrA);

	return result;
}

/* MAKE_EXPORT GetPrivateProfileIntW_new=GetPrivateProfileIntW */
UINT WINAPI GetPrivateProfileIntW_new(LPCWSTR lpAppNameW, LPCWSTR lpKeyNameW, INT nDefault, LPCWSTR lpFileNameW)
{
	ALLOC_WtoA(lpAppName);
	ALLOC_WtoA(lpKeyName);
	ALLOC_WtoA(lpFileName);

	return GetPrivateProfileIntA(lpAppNameA, lpKeyNameA, nDefault, lpFileNameA);
}

/* MAKE_EXPORT GetPrivateProfileSectionNamesW_new=GetPrivateProfileSectionNamesW */
DWORD WINAPI GetPrivateProfileSectionNamesW_new(LPWSTR lpszReturnBufferW, DWORD nSize, LPCWSTR lpFileNameW)
{
	LPSTR lpszReturnBufferA = (LPSTR)malloc(nSize);
	DWORD result = 0;

	if(lpszReturnBufferA == NULL)
		return 0;

	ALLOC_WtoA(lpFileName);

	result = GetPrivateProfileSectionNamesA(lpszReturnBufferA, nSize, lpFileNameA);

	if(!result)
	{
		free(lpszReturnBufferA);
		return 0;
	}

	STACK_AtoW(lpszReturnBufferA, lpszReturnBufferW);

	free(lpszReturnBufferA);

	return result;
}

/* MAKE_EXPORT GetPrivateProfileSectionW_new=GetPrivateProfileSectionW */
DWORD WINAPI GetPrivateProfileSectionW_new(LPCWSTR lpAppNameW, LPWSTR lpReturnedStringW, DWORD nSize, LPCWSTR lpFileNameW)
{
	LPSTR lpReturnedStringA = (LPSTR)malloc(nSize);
	DWORD result = 0;

	if(lpReturnedStringA == NULL)
		return 0;

	ALLOC_WtoA(lpAppName);
	ALLOC_WtoA(lpFileName);

	result = GetPrivateProfileSection(lpAppNameA, lpReturnedStringA, nSize, lpFileNameA);

	if(!result)
	{
		free(lpReturnedStringA);
		return 0;
	}

	STACK_AtoW(lpReturnedStringA, lpReturnedStringW);

	free(lpReturnedStringA);

	return result;
}

/* MAKE_EXPORT GetPrivateProfileStringW_new=GetPrivateProfileStringW */
DWORD WINAPI GetPrivateProfileStringW_new(LPCWSTR lpAppNameW, LPCWSTR lpKeyNameW, LPCWSTR lpDefaultW, LPWSTR lpReturnedStringW, DWORD nSize, LPCWSTR lpFileNameW)
{
	LPSTR lpReturnedStringA = (LPSTR)malloc(nSize);
	DWORD result = 0;

	if(lpReturnedStringA == NULL)
		return 0;

	ALLOC_WtoA(lpAppName);
	ALLOC_WtoA(lpKeyName);
	ALLOC_WtoA(lpDefault);
	ALLOC_WtoA(lpFileName);

	result = GetPrivateProfileStringA(lpAppNameA, lpKeyNameA, lpDefaultA, lpReturnedStringA, nSize, lpFileNameA);

	if(!result)
	{
		free(lpReturnedStringA);
		return 0;
	}

	STACK_AtoW(lpReturnedStringA, lpReturnedStringW);

	free(lpReturnedStringA);

	return result;
}

/* MAKE_EXPORT GetPrivateProfileStructW_new=GetPrivateProfileStructW */
BOOL WINAPI GetPrivateProfileStructW_new(LPCWSTR lpszSectionW, LPCWSTR lpszKeyW, LPVOID lpStruct, UINT uSizeStruct, LPCWSTR szFileW)
{
	ALLOC_WtoA(lpszSection);
	ALLOC_WtoA(lpszKey);
	ALLOC_WtoA(szFile);

	return GetPrivateProfileStructA(lpszSectionA, lpszKeyA, lpStruct, uSizeStruct, szFileA);
}

/* MAKE_EXPORT GetProfileIntW_new=GetProfileIntW */
UINT WINAPI GetProfileIntW_new(LPCWSTR lpAppNameW, LPCWSTR lpKeyNameW, INT nDefault)
{
	ALLOC_WtoA(lpAppName);
	ALLOC_WtoA(lpKeyName);

	return GetProfileIntA(lpAppNameA, lpKeyNameA, nDefault);
}

/* MAKE_EXPORT GetProfileSectionW_new=GetProfileSectionW */
DWORD WINAPI GetProfileSectionW_new(LPCWSTR lpAppNameW, LPWSTR lpReturnedStringW, DWORD nSize, LPCWSTR lpFileNameW)
{
	LPSTR lpReturnedStringA = (LPSTR)malloc(nSize);
	DWORD result = 0;

	if(lpReturnedStringA == NULL)
		return 0;

	ALLOC_WtoA(lpAppName);

	result = GetProfileSection(lpAppNameA, lpReturnedStringA, nSize);

	if(!result)
	{
		free(lpReturnedStringA);
		return 0;
	}

	STACK_AtoW(lpReturnedStringA, lpReturnedStringW);

	free(lpReturnedStringA);

	return result;
}

/* MAKE_EXPORT GetProfileStringW_new=GetProfileStringW */
DWORD WINAPI GetProfileStringW_new(LPCWSTR lpAppNameW, LPCWSTR lpKeyNameW, LPCWSTR lpDefaultW, LPWSTR lpReturnedStringW, DWORD nSize)
{
	LPSTR lpReturnedStringA = (LPSTR)malloc(nSize);
	DWORD result = 0;

	if(lpReturnedStringA == NULL)
		return 0;

	ALLOC_WtoA(lpAppName);
	ALLOC_WtoA(lpKeyName);
	ALLOC_WtoA(lpDefault);

	result = GetProfileStringA(lpAppNameA, lpKeyNameA, lpDefaultA, lpReturnedStringA, nSize);

	if(!result)
	{
		free(lpReturnedStringA);
		return 0;
	}

	STACK_AtoW(lpReturnedStringA, lpReturnedStringW);

	free(lpReturnedStringA);

	return result;
}

/* MAKE_EXPORT GetTimeFormatW_new=GetTimeFormatW */
int WINAPI GetTimeFormatW_new(LCID Locale, DWORD dwFlags, CONST SYSTEMTIME *lpTime, LPCWSTR lpFormatW, LPWSTR lpTimeStrW, int cchTime)
{
	int result = 0;
	LPSTR lpTimeStrA = (LPSTR)malloc(cchTime);

	if(lpTimeStrA == NULL)
		return 0;

	ALLOC_WtoA(lpFormat);

	result = GetTimeFormatA(Locale, dwFlags, lpTime, lpFormatA, lpTimeStrA, cchTime);

	if(!result)
	{
		free(lpTimeStrA);
		return 0;
	}

	STACK_AtoW(lpTimeStrA, lpTimeStrW);

	free(lpTimeStrA);

	return result;
}

/* MAKE_EXPORT GetVolumeInformationW_new=GetVolumeInformationW */
BOOL WINAPI GetVolumeInformationW_new(LPCWSTR lpRootPathNameW, LPWSTR lpVolumeNameBufferW, DWORD nVolumeNameSize, LPDWORD lpVolumeSerialNumber, LPDWORD lpMaximumComponentLength, LPDWORD lpFileSystemFlags, LPWSTR lpFileSystemNameBufferW, DWORD nFileSystemNameSize)
{
	BOOL result = 0;
	LPSTR lpVolumeNameBufferA = (LPSTR)malloc(nVolumeNameSize);
	LPSTR lpFileSystemNameBufferA = (LPSTR)malloc(nFileSystemNameSize);

	if(lpVolumeNameBufferA == NULL || lpFileSystemNameBufferA == NULL)
		return FALSE;

	ALLOC_WtoA(lpRootPathName);

	result = GetVolumeInformationA(lpRootPathNameA,
								lpVolumeNameBufferA,
								nVolumeNameSize,
								lpVolumeSerialNumber,
								lpMaximumComponentLength,
								lpFileSystemFlags,
								lpFileSystemNameBufferA,
								nFileSystemNameSize);

	if(!result)
	{
		free(lpVolumeNameBufferA);
		free(lpFileSystemNameBufferA);
		return FALSE;
	}

	STACK_AtoW(lpVolumeNameBufferA, lpVolumeNameBufferW);
	STACK_AtoW(lpFileSystemNameBufferA, lpFileSystemNameBufferW);

	free(lpVolumeNameBufferA);
	free(lpFileSystemNameBufferA);

	return result;
}

/* MAKE_EXPORT IsBadUnicodeStringPtr=IsBadStringPtrW */

/* MAKE_EXPORT OpenEventW_new=OpenEventW */
HANDLE WINAPI OpenEventW_new(DWORD dwDesiredAccess, BOOL bInheritHandle, LPCWSTR lpNameW)
{
	ALLOC_WtoA(lpName);

	return OpenEventA(dwDesiredAccess, bInheritHandle, lpNameA);
}

/* MAKE_EXPORT OpenFileMappingW_new=OpenFileMappingW */
HANDLE WINAPI OpenFileMappingW_new(DWORD dwDesiredAccess, BOOL bInheritHandle, LPCWSTR lpNameW)
{
	ALLOC_WtoA(lpName);

	return OpenFileMappingA(dwDesiredAccess, bInheritHandle, lpNameA);
}

/* MAKE_EXPORT OpenMutexW_new=OpenMutexW */
HANDLE WINAPI OpenMutexW_new(DWORD dwDesiredAccess, BOOL bInheritHandle, LPCWSTR lpNameW)
{
	ALLOC_WtoA(lpName);

	return OpenMutexA(dwDesiredAccess, bInheritHandle, lpNameA);
}

/* MAKE_EXPORT OpenSemaphoreW_new=OpenSemaphoreW */
HANDLE WINAPI OpenSemaphoreW_new(DWORD dwDesiredAccess, BOOL bInheritHandle, LPCWSTR lpNameW)
{
	ALLOC_WtoA(lpName);

	return OpenSemaphoreA(dwDesiredAccess, bInheritHandle, lpNameA);
}

/* MAKE_EXPORT OpenWaitableTimerW_new=OpenWaitableTimerW */
HANDLE WINAPI OpenWaitableTimerW_new(DWORD dwDesiredAccess, BOOL bInheritHandle, LPCWSTR lpTimerNameW)
{
	ALLOC_WtoA(lpTimerName);

	return OpenWaitableTimerA(dwDesiredAccess, bInheritHandle, lpTimerNameA);
}

/* MAKE_EXPORT PeekConsoleInputW_new=PeekConsoleInputW */
BOOL WINAPI PeekConsoleInputW_new(HANDLE hConsoleInput, PINPUT_RECORD lpBuffer, DWORD nLength, LPDWORD lpNumberOfEventsRead)
{
	/* Any differences with the unicode version here ? */
	return PeekConsoleInputA(hConsoleInput, lpBuffer, nLength, lpNumberOfEventsRead);
}

/* MAKE_EXPORT QueryDosDeviceW_new=QueryDosDeviceW */
DWORD WINAPI QueryDosDeviceW_new(LPCWSTR lpDeviceNameW, LPWSTR lpTargetPathW, DWORD ucchMax)
{
	LPSTR lpTargetPathA = (LPSTR)malloc(ucchMax);
	DWORD result = 0;

	if(lpTargetPathA == NULL)
		return 0;

	ALLOC_WtoA(lpDeviceName);

	result = QueryDosDeviceA(lpDeviceNameA, lpTargetPathA, ucchMax);

	if(!result)
	{
		free(lpTargetPathA);
		return 0;
	}

	STACK_AtoW(lpTargetPathA, lpTargetPathW);

	free(lpTargetPathA);

	return result;
}

/* MAKE_EXPORT ReadConsoleOutputCharacterW_new=ReadConsoleOutputCharacterW */
BOOL WINAPI ReadConsoleOutputCharacterW_new(HANDLE hConsoleOutput, LPWSTR lpCharacterW, DWORD nLength, COORD dwReadCoord, LPDWORD lpNumberOfCharsRead)
{
	LPSTR lpCharacterA = (LPSTR)malloc(nLength);
	DWORD result = 0;

	if(lpCharacterA == NULL)
		return 0;

	result = ReadConsoleOutputCharacterA(hConsoleOutput, lpCharacterA, nLength, dwReadCoord, lpNumberOfCharsRead);

	if(!result)
	{
		free(lpCharacterA);
		return 0;
	}

	STACK_AtoW(lpCharacterA, lpCharacterW);

	free(lpCharacterA);

	return result;
}

/* MAKE_EXPORT ReadConsoleOutputW_new=ReadConsoleOutputW */
BOOL WINAPI ReadConsoleOutputW_new(HANDLE hConsoleOutput, PCHAR_INFO lpBuffer, COORD dwBufferSize, COORD dwBufferCoord, PSMALL_RECT lpReadRegion)
{
	return ReadConsoleOutputA(hConsoleOutput, lpBuffer, dwBufferSize, dwBufferCoord, lpReadRegion);
}

/* MAKE_EXPORT ReadConsoleW_new=ReadConsoleW */
BOOL WINAPI ReadConsoleW_new(HANDLE hConsoleInput, LPVOID lpBuffer, DWORD nNumberOfCharsToRead, LPDWORD lpNumberOfCharsRead, LPVOID lpReserved)
{
	return ReadConsoleA(hConsoleInput, lpBuffer, nNumberOfCharsToRead, lpNumberOfCharsRead, lpReserved);
}

/* MAKE_EXPORT ScrollConsoleScreenBufferW_new=ScrollConsoleScreenBufferW */
BOOL WINAPI ScrollConsoleScreenBufferW_new(HANDLE hConsoleOutput, const SMALL_RECT* lpScrollRectangle, const SMALL_RECT* lpClipRectangle, COORD dwDestinationOrigin, const CHAR_INFO* lpFill)
{
	return ScrollConsoleScreenBufferA(hConsoleOutput, lpScrollRectangle, lpClipRectangle, dwDestinationOrigin, lpFill);
}

/* MAKE_EXPORT SetCalendarInfoW_new=SetCalendarInfoW */
int WINAPI SetCalendarInfoW_new(LCID Locale, CALID Calendar, CALTYPE CalType, LPCWSTR lpCalDataW)
{
	ALLOC_WtoA(lpCalData);

	return SetCalendarInfoA(Locale, Calendar, CalType, lpCalDataA);
}

/* MAKE_EXPORT SetComputerNameW_new=SetComputerNameW */
BOOL WINAPI SetComputerNameW_new(LPCWSTR lpComputerNameW)
{
	ALLOC_WtoA(lpComputerName);

	return SetComputerNameA(lpComputerNameA);
}

/* MAKE_EXPORT SetDefaultCommConfigW_new=SetDefaultCommConfigW_new */
BOOL WINAPI SetDefaultCommConfigW_new(LPCWSTR lpszNameW, LPCOMMCONFIG lpCC, DWORD dwSize)
{
	ALLOC_WtoA(lpszName);

	return SetDefaultCommConfigA(lpszNameA, lpCC, dwSize);
}

BOOL WINAPI SetEnvironmentVariableW_new(LPCWSTR lpNameW, LPCWSTR lpValueW)
{
	ALLOC_WtoA(lpName);
	ALLOC_WtoA(lpValue);

	return SetEnvironmentVariableA(lpNameA, lpValueA);
}

/* MAKE_EXPORT SetLocaleInfoW_new=SetLocaleInfoW */
BOOL WINAPI SetLocaleInfoW_new(LCID Locale, LCTYPE LCType, LPCWSTR lpLCDataW)
{
	ALLOC_WtoA(lpLCData);

	return SetLocaleInfoA(Locale, LCType, lpLCDataA);
}

/* MAKE_EXPORT SetVolumeLabelW_new=SetVolumeLabelW */
BOOL WINAPI SetVolumeLabelW_new(LPCWSTR lpRootPathNameW, LPCWSTR lpVolumeNameW)
{
	ALLOC_WtoA(lpRootPathName);
	ALLOC_WtoA(lpVolumeName);

	return SetVolumeLabelA(lpRootPathNameA, lpVolumeNameA);
}

/* MAKE_EXPORT WaitNamedPipeW_new=WaitNamedPipeW */
BOOL WINAPI WaitNamedPipeW_new(LPCWSTR lpNamedPipeNameW, DWORD nTimeOut)
{
	ALLOC_WtoA(lpNamedPipeName);

	return WaitNamedPipeA(lpNamedPipeNameA, nTimeOut);
}

/* MAKE_EXPORT WriteConsoleInputW_new=WriteConsoleInputW */
BOOL WINAPI WriteConsoleInputW_new(HANDLE hConsoleInput, const INPUT_RECORD* lpBuffer, DWORD nLength, LPDWORD lpNumberOfEventsWritten)
{
	return WriteConsoleInputA(hConsoleInput, lpBuffer, nLength, lpNumberOfEventsWritten);
}

/* MAKE_EXPORT WriteConsoleOutputCharacterW_new=WriteConsoleOutputCharacterW */
BOOL WINAPI WriteConsoleOutputCharacterW_new(HANDLE hConsoleOutput, LPCWSTR lpCharacterW, DWORD nLength, COORD dwWriteCoord, LPDWORD lpNumberOfCharsWritten)
{
	ALLOC_WtoA(lpCharacter);

	return WriteConsoleOutputCharacterA(hConsoleOutput, lpCharacterA, nLength, dwWriteCoord, lpNumberOfCharsWritten);
}

/* MAKE_EXPORT WriteConsoleOutputW_new=WriteConsoleOutputW */
BOOL WINAPI WriteConsoleOutputW_new(HANDLE hConsoleOutput, const CHAR_INFO* lpBuffer, COORD dwBufferSize, COORD dwBufferCoord, PSMALL_RECT lpWriteRegion)
{
	return WriteConsoleOutputA(hConsoleOutput, lpBuffer, dwBufferSize, dwBufferCoord, lpWriteRegion);
}

/* MAKE_EXPORT WriteConsoleW_new=WriteConsoleW */
BOOL WINAPI WriteConsoleW_new(HANDLE hConsoleOutput, const VOID* lpBuffer, DWORD nNumberOfCharsToWrite, LPDWORD lpNumberOfCharsWritten, LPVOID lpReserved)
{
	return WriteConsoleA(hConsoleOutput, lpBuffer, nNumberOfCharsToWrite, lpNumberOfCharsWritten, lpReserved);
}

/* MAKE_EXPORT WritePrivateProfileSectionW_new=WritePrivateProfileSectionW */
BOOL WINAPI WritePrivateProfileSectionW_new(LPCWSTR lpAppNameW, LPCWSTR lpStringW, LPCWSTR lpFileNameW)
{
	ALLOC_WtoA(lpAppName);
	ALLOC_WtoA(lpString);
	ALLOC_WtoA(lpFileName);

	return WritePrivateProfileSectionA(lpAppNameA, lpStringA, lpFileNameA);
}

/* MAKE_EXPORT WritePrivateProfileStringW_new=WritePrivateProfileStringW */
BOOL WINAPI WritePrivateProfileStringW_new(LPCWSTR lpAppNameW, LPCWSTR lpKeyNameW, LPCWSTR lpStringW, LPCWSTR lpFileNameW)
{
	ALLOC_WtoA(lpAppName);
	ALLOC_WtoA(lpKeyName);
	ALLOC_WtoA(lpString);
	ALLOC_WtoA(lpFileName);

	return WritePrivateProfileStringA(lpAppNameA, lpKeyNameA, lpStringA, lpFileNameA);
}

/* MAKE_EXPORT WritePrivateProfileStructW_new=WritePrivateProfileStructW */
BOOL WINAPI WritePrivateProfileStructW_new(LPCWSTR lpszSectionW, LPCWSTR lpszKeyW, LPVOID lpStruct, UINT uSizeStruct, LPCWSTR szFileW)
{
	ALLOC_WtoA(lpszSection);
	ALLOC_WtoA(lpszKey);
	ALLOC_WtoA(szFile);

	return WritePrivateProfileStructA(lpszSectionA, lpszKeyA, lpStruct, uSizeStruct, szFileA);
}

/* MAKE_EXPORT WriteProfileSectionW_new=WriteProfileSectionW */
BOOL WINAPI WriteProfileSectionW_new(LPCWSTR lpAppNameW, LPCWSTR lpStringW)
{
	ALLOC_WtoA(lpAppName);
	ALLOC_WtoA(lpString);

	return WriteProfileSectionA(lpAppNameA, lpStringA);
}

/* MAKE_EXPORT WriteProfileStringW_new=WriteProfileStringW */
BOOL WINAPI WriteProfileStringW_new(LPCWSTR lpAppNameW, LPCWSTR lpKeyNameW, LPCWSTR lpStringW)
{
	ALLOC_WtoA(lpAppName);
	ALLOC_WtoA(lpKeyName);
	ALLOC_WtoA(lpString);

	return WriteProfileStringA(lpAppNameA, lpKeyNameA, lpStringA);
}
