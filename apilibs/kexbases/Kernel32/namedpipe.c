/*
 *  KernelEx
 *  Copyright (C) 2013 Ley0k
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

#include "pipe.h"

LIST_ENTRY NamedPipeList;

/* Currently unimplemented */

/* MAKE_EXPORT ConnectNamedPipe_new=ConnectNamedPipe */
BOOL WINAPI ConnectNamedPipe_new(HANDLE hNamedPipe,
  LPOVERLAPPED lpOverlapped
)
{
	SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
	return NULL;
}

/* MAKE_EXPORT CreateNamedPipeA_new=CreateNamedPipeA */
HANDLE WINAPI CreateNamedPipeA_new(LPCSTR lpName,
	DWORD dwOpenMode,
	DWORD dwPipeMode,
	DWORD nMaxInstances,
	DWORD nOutBufferSize,
	DWORD nInBufferSize,
	DWORD nDefaultTimeOut,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes
)
{
	if(NamedPipeList.Flink == NULL && NamedPipeList.Blink == NULL)
		InitializeListHead(&NamedPipeList);

	/* Fail if dwOpenMode is 0 or have an unknown flag */
	if(dwOpenMode == 0 || dwOpenMode & ~(PIPE_ACCESS_DUPLEX | PIPE_ACCESS_INBOUND | PIPE_ACCESS_OUTBOUND |
		FILE_FLAG_FIRST_PIPE_INSTANCE | FILE_FLAG_WRITE_THROUGH | FILE_FLAG_OVERLAPPED | WRITE_DAC |
		WRITE_OWNER | ACCESS_SYSTEM_SECURITY))
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return NULL;
	}

	SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
	return NULL;
}

/* MAKE_EXPORT DisconnectNamedPipe_new=DisconnectNamedPipe */
BOOL WINAPI DisconnectNamedPipe_new(HANDLE hNamedPipe)
{
	SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
	return NULL;
}