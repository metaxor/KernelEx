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

#include "netapi32.h"

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	switch (fdwReason)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_PROCESS_DETACH:
			break;
	}

	return TRUE;
}

/************************************************************
 *                NetAlertRaise  (NETAPI32.@)
 */
NET_API_STATUS WINAPI NetAlertRaise(LPCWSTR AlertEventName,
	LPVOID Buffer,
	DWORD BufferSize)
{
	FIXME("Stub %s 0x%08lx %d\n", AlertEventName, Buffer, BufferSize);
	return NERR_Success;
}

/************************************************************
 *                NetAlertRaiseEx  (NETAPI32.@)
 */
NET_API_STATUS WINAPI NetAlertRaiseEx(LPCWSTR AlertEventName,
	LPVOID VariableInfo,
	DWORD VariableInfoSize,
	LPCWSTR ServiceName)
{
	FIXME("Stub %s 0x%08lx %d %s\n", AlertEventName,
		VariableInfo, VariableInfoSize, ServiceName);
	return NERR_Success;
}

/************************************************************
 *                NetApiBufferAllocate  (NETAPI32.@)
 */
NET_API_STATUS WINAPI NetApiBufferAllocate(DWORD ByteCount,  LPVOID* Buffer)
{
	if(Buffer == NULL || IsBadWritePtr(Buffer, ByteCount))
		return ERROR_INVALID_PARAMETER;

	*Buffer = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, ByteCount);

	if(*Buffer == NULL)
		return GetLastError();
	else
		return NERR_Success;
}

/************************************************************
 *                NetApiBufferFree  (NETAPI32.@)
 */
NET_API_STATUS WINAPI NetApiBufferFree(LPVOID Buffer)
{
	if(Buffer == NULL)
		return ERROR_INVALID_PARAMETER;

	if(!HeapFree(GetProcessHeap(), 0, Buffer))
		return GetLastError();
	else
		return NERR_Success;
}

/************************************************************
 *                NetApiBufferReallocate  (NETAPI32.@)
 */
NET_API_STATUS WINAPI NetApiBufferReallocate(LPVOID OldBuffer,
	DWORD NewByteCount,
	LPVOID* NewBuffer)
{
	if(NewBuffer == NULL || IsBadWritePtr(NewBuffer, NewByteCount))
		return ERROR_INVALID_PARAMETER;

	*NewBuffer = HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, OldBuffer, NewByteCount);

	if(*NewBuffer == NULL)
		return GetLastError();
	else
		return NERR_Success;
}

/************************************************************
 *                NetApiBufferSize  (NETAPI32.@)
 */
NET_API_STATUS WINAPI NetApiBufferSize(LPVOID Buffer, LPDWORD ByteCount)
{
	DWORD dwSize = 0;

	if(ByteCount == NULL || IsBadWritePtr(ByteCount, sizeof(DWORD)))
		return ERROR_INVALID_PARAMETER;

	if(Buffer == NULL || IsBadWritePtr(Buffer, *ByteCount))
		return ERROR_INVALID_PARAMETER;

	dwSize = HeapSize(GetProcessHeap(), 0, Buffer);

	if(dwSize == 0)
		return GetLastError();
	else
	{
		*ByteCount = dwSize;
		return NERR_Success;
	}
}