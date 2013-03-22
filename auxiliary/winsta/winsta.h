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

#ifndef __WINSTA_H__
#define __WINSTA_H__

#include "../../common/common.h"

#define WINSTAAPI WINAPI

#ifdef WINSTA
#include <wtsapi32.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif
static const char* debugstr_a(const char* a)
{
	return a;
}

static const char* debugstr_w(const wchar_t* w)
{
	static char a[256];
	WideCharToMultiByte(CP_ACP, 0, w, -1, a, sizeof(a), NULL, NULL);
	return a;
}

HANDLE WINSTAAPI WinStationOpenServerA(LPSTR pServerName);
HANDLE WINSTAAPI WinStationOpenServerW(LPWSTR pServerName);
BOOL WINSTAAPI WinStationSendMessageA(HANDLE hServer,
	DWORD SessionId,
	LPSTR pTitle,
	DWORD TitleLength,
	LPSTR pMessage,
	DWORD MessageLength,
	DWORD Style,
	DWORD Timeout,
	DWORD* pResponse,
	BOOL bWait
);
BOOL WINSTAAPI WinStationSendMessageW(HANDLE hServer,
	DWORD SessionId,
	LPWSTR pTitle,
	DWORD TitleLength,
	LPWSTR pMessage,
	DWORD MessageLength,
	DWORD Style,
	DWORD Timeout,
	DWORD* pResponse,
	BOOL bWait
);
BOOL WINSTAAPI WinStationTerminateProcess(HANDLE hServer, DWORD ProcessId, DWORD ExitCode);
BOOL WINSTAAPI WinStationWaitSystemEvent(HANDLE hServer, DWORD Mask, DWORD* Flags);

#ifdef __cplusplus
}
#endif

#endif
