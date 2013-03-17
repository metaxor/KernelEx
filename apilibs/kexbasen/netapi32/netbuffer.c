/*
 *  KernelEx
 *  Copyright (C) 2008, Ley0k
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

#include "_netapi32_apilist.h"

/* MAKE_EXPORT NetApiBufferAllocate_new=NetApiBufferAllocate */
NET_API_STATUS WINAPI NetApiBufferAllocate_new(DWORD ByteCount,  LPVOID* Buffer)
{
	if(Buffer == NULL || IsBadWritePtr(Buffer, ByteCount))
		return ERROR_INVALID_PARAMETER;

	*Buffer = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, ByteCount);

	if(*Buffer == NULL)
		return GetLastError();
	else
		return NERR_Success;
}

/* MAKE_EXPORT NetApiBufferFree_new=NetApiBufferFree */
NET_API_STATUS WINAPI NetApiBufferFree_new(LPVOID Buffer)
{
	if(Buffer == NULL)
		return ERROR_INVALID_PARAMETER;

	if(!HeapFree(GetProcessHeap(), 0, Buffer))
		return GetLastError();
	else
		return NERR_Success;
}

/* MAKE_EXPORT NetApiBufferReallocate_new=NetApiBufferReallocate */
NET_API_STATUS WINAPI NetApiBufferReallocate_new(LPVOID OldBuffer,
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

/* MAKE_EXPORT NetApiBufferSize_new=NetApiBufferSize */
NET_API_STATUS WINAPI NetApiBufferSize_new(LPVOID Buffer, LPDWORD ByteCount)
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