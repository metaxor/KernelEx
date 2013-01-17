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

#include "common.h"
#include <rpc.h>

/* MAKE_EXPORT RpcStringFreeW_new=RpcStringFreeW */
RPC_STATUS
RPC_ENTRY
RpcStringFreeW_new(IN OUT LPWSTR * String)
{
	HeapFree(GetProcessHeap(), 0, *String);
	return RPC_S_OK;
}

/* MAKE_EXPORT UuidFromStringW_new=UuidFromStringW */
RPC_STATUS
RPC_ENTRY
UuidFromStringW_new(IN LPWSTR StringUuidW, OUT UUID * Uuid)
{
	ALLOC_WtoA(StringUuid);
	return UuidFromStringA((unsigned char*) StringUuidA, Uuid);
}

/* MAKE_EXPORT UuidToStringW_new=UuidToStringW */
RPC_STATUS
RPC_ENTRY
UuidToStringW_new(IN UUID * Uuid, OUT LPWSTR * StringUuid)
{
	RPC_STATUS ret;
	unsigned char* uuidA;
	ret = UuidToStringA(Uuid, &uuidA);
	if (ret == RPC_S_OK)
	{
		int size = (strlen((char*) uuidA) + 1) * sizeof(short);
		*StringUuid = (LPWSTR) HeapAlloc(GetProcessHeap(), 0, size);
		if (!*StringUuid)
		{
			RpcStringFreeA(&uuidA);
			return RPC_S_OUT_OF_MEMORY;
		}

		MultiByteToWideChar(CP_ACP, 0, (char*)uuidA, -1, *StringUuid, size);
		RpcStringFreeA(&uuidA);
	}
	return ret;
}
