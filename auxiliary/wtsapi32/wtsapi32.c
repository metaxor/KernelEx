/* Copyright 2007 Xeno86
 * Copyright 2005 Ulrich Czekalla
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

//#include "config.h"
#include <stdarg.h>
#include <stdlib.h>
#include <windows.h>
/*#include "windef.h"
#include "winbase.h"*/
#include "wtsapi32.h"
#include "../../common/common.h"
#include "../../common/listhead.h"
//#include "wine/debug.h"
#include <stdio.h>

typedef struct _HWNDLIST
{
	HWND hWnd;
	LIST_ENTRY ListEntry;
} HWNDLIST, *PHWNDLIST;

LIST_ENTRY RegisteredWindowListHead;

BOOL fInitialized = FALSE;

//WINE_DEFAULT_DEBUG_CHANNEL(wtsapi);
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

static void debug_output(const char* func, const char* fmt, ...)
{
	va_list args;
	char buf[200];
	
	va_start(args, fmt);
	strcpy(buf, func);
	strcat(buf, ": ");
	vsprintf(buf + strlen(buf), fmt, args);
	va_end(args);
	OutputDebugStringA(buf);
}

static HMODULE WTSAPI32_hModule = 0;

BOOL WINAPI DllMain (HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	TRACE("%p,%lx,%p\n", hinstDLL, fdwReason, lpvReserved);

	switch (fdwReason)
	{
		case DLL_PROCESS_ATTACH:
		{
			OSVERSIONINFO osv;
			osv.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
			GetVersionEx(&osv);
			if (osv.dwMajorVersion < 5)
				return FALSE;

			if(!fInitialized)
			{
				fInitialized = TRUE;
				memset(&RegisteredWindowListHead, 0, sizeof(LIST_ENTRY));
			}

			DisableThreadLibraryCalls(hinstDLL);
			WTSAPI32_hModule = hinstDLL;
			break;
		}
		case DLL_PROCESS_DETACH:
		{
			break;
		}
	}

	return TRUE;
}

/************************************************************
 *                WTSCloseServer  (WTSAPI32.@)
 */
void WINAPI WTSCloseServer(HANDLE hServer)
{
	FIXME("Stub %p\n", hServer);
}

/************************************************************
 *                WTSDisconnectSession  (WTSAPI32.@)
 */
BOOL WINAPI WTSDisconnectSession(HANDLE hServer, DWORD SessionId, BOOL bWait)
{
	FIXME("Stub %p 0x%08lx %d\n", hServer, SessionId, bWait);
	return TRUE;
}

/************************************************************
 *                WTSEnumerateProcessesA  (WTSAPI32.@)
 */
BOOL WINAPI WTSEnumerateProcessesA(HANDLE hServer, DWORD Reserved, DWORD Version,
                                   PWTS_PROCESS_INFOA* ppProcessInfo, DWORD* pCount)
{
	FIXME("Stub %p 0x%08lx 0x%08lx %p %p\n", hServer, Reserved, Version,
			ppProcessInfo, pCount);

	if (!ppProcessInfo || !pCount) return FALSE;

	*pCount = 0;
	*ppProcessInfo = NULL;

	return TRUE;
}

/************************************************************
 *                WTSEnumerateProcessesW  (WTSAPI32.@)
 */
BOOL WINAPI WTSEnumerateProcessesW(HANDLE hServer, DWORD Reserved, DWORD Version,
                                   PWTS_PROCESS_INFOW* ppProcessInfo, DWORD* pCount)
{
	FIXME("Stub %p 0x%08lx 0x%08lx %p %p\n", hServer, Reserved, Version,
			ppProcessInfo, pCount);

	if (!ppProcessInfo || !pCount) return FALSE;

	*pCount = 0;
	*ppProcessInfo = NULL;

	return TRUE;
}

/************************************************************
 *                WTSEnumerateServersA  (WTSAPI32.@)
 */
BOOL WINAPI WTSEnumerateServersA(LPSTR pDomainName, DWORD Reserved, DWORD Version,
                                 PWTS_SERVER_INFOA* ppServerInfo, DWORD* pCount)
{
	FIXME("Stub %s 0x%08lx 0x%08lx %p %p\n", debugstr_a(pDomainName), Reserved, Version,
		  ppServerInfo, pCount);

	if (!ppServerInfo || !pCount) return FALSE;

	*pCount = 0;
	*ppServerInfo = NULL;

	return TRUE;
}

/************************************************************
 *                WTSEnumerateServersW  (WTSAPI32.@)
 */
BOOL WINAPI WTSEnumerateServersW(LPWSTR pDomainName, DWORD Reserved, DWORD Version,
                                 PWTS_SERVER_INFOW* ppServerInfo, DWORD* pCount)
{
	FIXME("Stub %s 0x%08lx 0x%08lx %p %p\n", debugstr_w(pDomainName), Reserved, Version,
			ppServerInfo, pCount);

	if (!ppServerInfo || !pCount) return FALSE;

	*pCount = 0;
	*ppServerInfo = NULL;

	return TRUE;
}

/************************************************************
 *                WTSEnumerateEnumerateSessionsA  (WTSAPI32.@)
 */
BOOL WINAPI WTSEnumerateSessionsA(HANDLE hServer, DWORD Reserved, DWORD Version,
                                  PWTS_SESSION_INFOA* ppSessionInfo, DWORD* pCount)
{
	FIXME("Stub %p 0x%08lx 0x%08lx %p %p\n", hServer, Reserved, Version,
			ppSessionInfo, pCount);

	if (!ppSessionInfo || !pCount) return FALSE;

	*pCount = 0;
	*ppSessionInfo = NULL;

	return TRUE;
}

/************************************************************
 *                WTSEnumerateEnumerateSessionsW  (WTSAPI32.@)
 */
BOOL WINAPI WTSEnumerateSessionsW(HANDLE hServer, DWORD Reserved, DWORD Version,
                                  PWTS_SESSION_INFOW* ppSessionInfo, DWORD* pCount)
{
	FIXME("Stub %p 0x%08lx 0x%08lx %p %p\n", hServer, Reserved, Version,
			ppSessionInfo, pCount);

	if (!ppSessionInfo || !pCount) return FALSE;

	*pCount = 0;
	*ppSessionInfo = NULL;

	return TRUE;
}

/************************************************************
 *                WTSFreeMemory (WTSAPI32.@)
 */
void WINAPI WTSFreeMemory(PVOID pMemory)
{
	FIXME("Stub %p\n", pMemory);
	return;
}

/************************************************************
 *                WTSNotifySession (WTSAPI32.@1)
 *                Internal to KernelEx
 */
void WINAPI WTSNotifySession(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PLIST_ENTRY WindowList = NULL;
	PHWNDLIST hWndList = NULL;
	HWND hWnd = NULL;

	for(WindowList = RegisteredWindowListHead.Flink; WindowList != &RegisteredWindowListHead; WindowList = WindowList->Flink)
	{
		hWndList = CONTAINING_RECORD(WindowList, HWNDLIST, ListEntry);

		hWnd = hWndList->hWnd;

		if(!IsWindow(hWnd))
		{
			RemoveEntryList(&hWndList->ListEntry);
			continue;
		}

		PostMessage(hWnd, uMsg, wParam, lParam);
	}
}

/************************************************************
 *                WTSOpenServerA (WTSAPI32.@)
 */
HANDLE WINAPI WTSOpenServerA(LPSTR pServerName)
{
	FIXME("(%s) stub\n", debugstr_a(pServerName));
	SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
	return NULL;
}

/************************************************************
 *                WTSOpenServerW (WTSAPI32.@)
 */
HANDLE WINAPI WTSOpenServerW(LPWSTR pServerName)
{
	FIXME("(%s) stub\n", debugstr_w(pServerName));
	SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
	return NULL;
}

/************************************************************
 *                WTSQuerySessionInformationA  (WTSAPI32.@)
 */
BOOL WINAPI WTSQuerySessionInformationA(
	HANDLE hServer,
	DWORD SessionId,
	WTS_INFO_CLASS WTSInfoClass,
	LPSTR* Buffer,
	DWORD* BytesReturned)
{
	/* FIXME: Forward request to winsta.dll::WinStationQueryInformationA */
	FIXME("Stub %p 0x%08lx %d %p %p\n", hServer, SessionId, WTSInfoClass,
			Buffer, BytesReturned);

	return FALSE;
}

/************************************************************
 *                WTSQuerySessionInformationW  (WTSAPI32.@)
 */
BOOL WINAPI WTSQuerySessionInformationW(
	HANDLE hServer,
	DWORD SessionId,
	WTS_INFO_CLASS WTSInfoClass,
	LPWSTR* Buffer,
	DWORD* BytesReturned)
{
	/* FIXME: Forward request to winsta.dll::WinStationQueryInformationW */
	FIXME("Stub %p 0x%08lx %d %p %p\n", hServer, SessionId, WTSInfoClass,
			Buffer, BytesReturned);

	return FALSE;
}

/************************************************************
 *                WTSQueryUserConfigA  (WTSAPI32.@)
 */
BOOL WINAPI WTSQueryUserConfigA(
	LPSTR pServerName,
	LPSTR pUserName,
	WTS_CONFIG_CLASS WTSConfigClass,
	LPSTR* ppBuffer,
	DWORD* pBytesReturned)
{
	FIXME("Stub %s %s 0x%08lx %p %p\n", debugstr_a(pServerName), debugstr_a(pUserName),
			WTSConfigClass, ppBuffer, pBytesReturned);
	return FALSE;
}

/************************************************************
 *                WTSQueryUserConfigW  (WTSAPI32.@)
 */
BOOL WINAPI WTSQueryUserConfigW(
	LPWSTR pServerName,
	LPWSTR pUserName,
	WTS_CONFIG_CLASS WTSConfigClass,
	LPWSTR* ppBuffer,
	DWORD* pBytesReturned)
{
	FIXME("Stub %s %s 0x%08lx %p %p\n", debugstr_w(pServerName), debugstr_w(pUserName),
			WTSConfigClass, ppBuffer, pBytesReturned);
	return FALSE;
}

/************************************************************
 *                WTSQueryUserToken  (WTSAPI32.@)
 */
BOOL WINAPI WTSQueryUserToken(ULONG SessionId, PHANDLE phToken)
{
	SetLastError(ERROR_NO_TOKEN);
	return FALSE;
}

/************************************************************
 *                WTSRegisterSessionNotification  (WTSAPI32.@)
 */
BOOL WINAPI WTSRegisterSessionNotification(HWND hWnd, DWORD dwFlags)
{
	HWNDLIST hWndList;

	if(dwFlags > NOTIFY_FOR_ALL_SESSIONS)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	if(!IsWindow(hWnd))
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	hWndList.hWnd = hWnd;

	InitializeListHead(&hWndList.ListEntry);
	InsertTailList(&RegisteredWindowListHead, &hWndList.ListEntry);

	return TRUE;
}

/************************************************************
 *                WTSTerminateProcess  (WTSAPI32.@)
 */
BOOL WINAPI WTSTerminateProcess(HANDLE hServer, DWORD ProcessId, DWORD ExitCode)
{
	FIXME("Stub %p 0x%08x 0x%08x\n", hServer, ProcessId, ExitCode);
	SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
	return FALSE;
}

/************************************************************
 *                WTSUnRegisterSessionNotification  (WTSAPI32.@)
 */
BOOL WINAPI WTSUnRegisterSessionNotification(HWND hWnd)
{
	PLIST_ENTRY WindowList;
	PHWNDLIST hWndList;

	if(!IsWindow(hWnd))
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	for(WindowList = RegisteredWindowListHead.Flink; WindowList != &RegisteredWindowListHead; WindowList = WindowList->Flink)
	{
		hWndList = CONTAINING_RECORD(WindowList, HWNDLIST, ListEntry);

		if(hWndList->hWnd == hWnd)
		{
			RemoveEntryList(&hWndList->ListEntry);
			return TRUE;
		}
	}

	return TRUE;
}

/************************************************************
 *                WTSWaitSystemEvent (WTSAPI32.@)
 */
BOOL WINAPI WTSWaitSystemEvent(HANDLE hServer, DWORD Mask, DWORD* Flags)
{
	/* FIXME: Forward request to winsta.dll::WinStationWaitSystemEvent */
	FIXME("Stub %p 0x%08lx %p\n", hServer, Mask, Flags);
	return FALSE;
}
