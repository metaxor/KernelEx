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

#define _DEBUG
#define WINSTA
#include "winsta.h"

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	switch (fdwReason)
	{
		case DLL_PROCESS_ATTACH:
			break;
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
		{
			break;
		}
	}

	return TRUE;
}

/************************************************************
 *                WinStationOpenServerA  (WINSTA.@)
 */
HANDLE WINSTAAPI WinStationOpenServerA(LPSTR pServerName)
{
	FIXME("(%s) stub\n", debugstr_a(pServerName));
	SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
	return NULL;
}

/************************************************************
 *                WinStationOpenServerW  (WINSTA.@)
 */
HANDLE WINSTAAPI WinStationOpenServerW(LPWSTR pServerName)
{
	FIXME("(%s) stub\n", debugstr_w(pServerName));
	SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
	return NULL;
}

/************************************************************
 *                WinStationSendMessageA  (WINSTA.@)
 */
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
)
{
	FIXME("Stub %p %d %s %d %s %d %d %d %d %d\n", hServer, SessionId, debugstr_a(pTitle),
			debugstr_a(pMessage), MessageLength, Style, Timeout, pResponse, bWait);
	SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
	return FALSE;
}

/************************************************************
 *                WinStationSendMessageW  (WINSTA.@)
 */
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
)
{
	FIXME("Stub %p %d %s %d %s %d %d %d %d %d\n", hServer, SessionId, debugstr_w(pTitle),
			debugstr_w(pMessage), MessageLength, Style, Timeout, pResponse, bWait);
	SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
	return FALSE;
}

/************************************************************
 *                WinStationTerminateProcess  (WINSTA.@)
 */
BOOL WINSTAAPI WinStationTerminateProcess(HANDLE hServer, DWORD ProcessId, DWORD ExitCode)
{
	FIXME("Stub %p 0x%08lx %d", hServer, ProcessId, ExitCode);
	SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
	return FALSE;
}

/************************************************************
 *                WinStationWaitSystemEvent (WINSTA.@)
 */
BOOL WINSTAAPI WinStationWaitSystemEvent(HANDLE hServer, DWORD Mask, DWORD* Flags)
{
	FIXME("Stub %p 0x%08lx %p\n", hServer, Mask, Flags);
	SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
	return FALSE;
}