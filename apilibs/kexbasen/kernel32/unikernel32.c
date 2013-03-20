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

/* MAKE_EXPORT IsBadStringPtrW_new=IsBadStringPtrW */
BOOL WINAPI IsBadStringPtrW_new(LPCWSTR lpsz, UINT_PTR ucchMax)
{
    BOOLEAN Result = FALSE;
    volatile WCHAR *Current;
    PWCHAR Last;
    WCHAR Char;

    /* Quick cases */
    if (!ucchMax) return FALSE;
    if (!lpsz) return TRUE;

    /* Calculate start and end */
    Current = (volatile WCHAR*)lpsz;
    Last = (PWCHAR)((ULONG_PTR)lpsz + (ucchMax * 2) - 2);

    __try
    {
        /* Probe the entire range */
        Char = *Current++;
        while ((Char) && (Current != Last)) Char = *Current++;
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        /* We hit an exception, so return true */
        Result = TRUE;
    }

    /* Return exception status */
    return Result;
}


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
