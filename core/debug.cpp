/*
 *  KernelEx
 *  Copyright (C) 2009, Xeno86
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

#include <stdio.h>
#include <stdarg.h>
#include <windows.h>
#include "debug.h"
#include "kexcoresdk.h"

//#define USEWDEB
//#define USECOMPORT
#define WRITETOLOG

BOOL fCommInitialized = FALSE;

extern "C"
int vsnprintf(char *buffer, size_t n, const char *format, va_list ap);
int sprintf (char *str, const char * format, ...);

void InitComm(void)
{
	HANDLE hFile = NULL;
	DWORD dwBytes = 0;
	ULONG Length = 0;
	DWORD ComError = 0;
	COMSTAT ComStat;
	DCB dcb;
	COMMTIMEOUTS CommTimeouts;

	fCommInitialized = TRUE;

	SetLastError(0);

	hFile = CreateFile("\\\\.\\COM1",
					GENERIC_READ|GENERIC_WRITE,
					0,
					NULL,
					OPEN_EXISTING,
					FILE_ATTRIBUTE_NORMAL,
					NULL);

	if(hFile == INVALID_HANDLE_VALUE)
		return;

	SetupComm(hFile, DEBUGMSG_MAXLEN, DEBUGMSG_MAXLEN);

	dcb.fBinary = 1;
	dcb.fParity = 0;
	dcb.fNull = 0;
	dcb.XonChar = 17;
	dcb.XoffChar = 19;
	dcb.XonLim = (WORD)( ( DEBUGMSG_MAXLEN) / 4 );
	dcb.XoffLim = (WORD)( ( DEBUGMSG_MAXLEN ) / 4 );
	dcb.EofChar = 0;
	dcb.EvtChar = 0;
	dcb.fOutxDsrFlow = 0;
	dcb.fOutxCtsFlow = 0;
	dcb.fDtrControl = DTR_CONTROL_ENABLE;
	dcb.fRtsControl = RTS_CONTROL_ENABLE;
    
	GetCommState(hFile, &dcb);

    GetCommTimeouts(hFile, &CommTimeouts);
        
    CommTimeouts.ReadIntervalTimeout = 1000;
    CommTimeouts.ReadTotalTimeoutMultiplier = 1000;
    CommTimeouts.ReadTotalTimeoutConstant = 1000;
    CommTimeouts.WriteTotalTimeoutMultiplier = 1000;
    CommTimeouts.WriteTotalTimeoutConstant = 1000;
    
    SetCommTimeouts(hFile, &CommTimeouts);

	ClearCommError(hFile, &ComError, &ComStat);

	Length = DEBUGMSG_MAXLEN - ComStat.cbOutQue;

	if(GetCommState(hFile, &dcb))
	{
		dcb.fRtsControl = RTS_CONTROL_ENABLE;
		if(BuildCommDCB("baud=115200 parity=N data=8 stop=1", &dcb))
			SetCommState(hFile, &dcb);
		else
			return;
	}
	else
		return;

	CloseHandle(hFile);
}

void WriteToComm(LPCSTR lpBuffer, DWORD dwBufferSize)
{
	HANDLE hFile = NULL;
	DWORD dwBytes = 0;

	if(!fCommInitialized)
		InitComm();

	if(IsBadStringPtr(lpBuffer, dwBufferSize))
		return;

	if(!dwBufferSize)
		dwBufferSize = strlen(lpBuffer);

	hFile = CreateFile("\\\\.\\COM1",
					GENERIC_READ|GENERIC_WRITE,
					0,
					NULL,
					OPEN_EXISTING,
					FILE_ATTRIBUTE_NORMAL,
					NULL);

	if(hFile == INVALID_HANDLE_VALUE)
		return;

	WriteFile(hFile, lpBuffer, dwBufferSize, &dwBytes, NULL);

	CloseHandle(hFile);
}

void WriteToLogFile(LPCSTR lpBuffer, DWORD dwBufferSize, BOOL fCarriageReturn)
{
	CHAR Directory[255];
	CHAR Path[255];
	DWORD dwBytesWritten = 0;
	CHAR NewLine[] = "\r\n";
	HANDLE hFile = NULL;

	if(!kexGetKernelExDirectory(Directory, sizeof(Directory)))
		return;

	sprintf(Path, "%sKernelEx.log", Directory);
	hFile = CreateFile(Path,
						GENERIC_WRITE,
						FILE_SHARE_READ|FILE_SHARE_WRITE,
						NULL,
						OPEN_ALWAYS,
						FILE_ATTRIBUTE_NORMAL,
						NULL);

	if(hFile == INVALID_HANDLE_VALUE)
		return;

	SetFilePointer(hFile, 0, NULL, FILE_END);

	WriteFile(hFile, lpBuffer, dwBufferSize, &dwBytesWritten, NULL);

	if(fCarriageReturn)
		WriteFile(hFile, &NewLine, 2, &dwBytesWritten, NULL);

	CloseHandle(hFile);
}

void dbgvprintf(const char* format, void* _argp)
{
	char msg[DEBUGMSG_MAXLEN];
#ifndef WRITETOLOG
	char msg2[DEBUGMSG_MAXLEN];
#endif
	DWORD dwBytes = 0;
	BOOL fCarriageReturn = FALSE;
	int cnt = 0;
	va_list argp = (va_list) _argp;

	memset(&msg, '\0', sizeof(msg));
#ifndef WRITETOLOG
	memset(&msg2, '\0', sizeof(msg2));
#endif

	__try
	{
		cnt = vsnprintf(msg, sizeof(msg), format, argp);
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		return;
	}

#ifdef WRITETOLOG
	if(strrchr(msg, '\n') != NULL)
	{
		PCHAR Character = strrchr(msg, '\n');
		if(Character != NULL)
			Character[0] = '\0';
			Character[1] = '\0';
#else
	if(strrchr(msg, '\n') != NULL && strrchr(msg, '\r') == NULL)
	{
#endif
		fCarriageReturn = TRUE;
#ifndef WRITETOLOG
		sprintf(msg2, "%s\r", msg);
		cnt++;
#endif
	}

#ifdef USECOMPORT
	WriteToComm(fCarriageReturn ? msg2 : msg, cnt);
#endif
#ifdef WRITETOLOG
	WriteToLogFile(msg, cnt, fCarriageReturn);
#endif
#ifdef USEWDEB
	OutputDebugString(fCarriageReturn ? msg2 : msg);
#endif
}

void dbgprintf(const char* format, ...)
{
	va_list argp;
	va_start(argp, format);
	dbgvprintf(format, argp);
	va_end(argp);
}

