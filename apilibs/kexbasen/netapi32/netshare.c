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

/* MAKE_EXPORT NetShareEnum_new=NetShareEnum */
NET_API_STATUS NET_API_FUNCTION
NetShareEnum_new(LMSTR servername,
	DWORD   level,
	LPBYTE  *bufptr,
	DWORD   prefmaxlen,
	LPDWORD entriesread,
	LPDWORD totalentries,
	LPDWORD resume_handle
)
{
	return ERROR_CALL_NOT_IMPLEMENTED;
}

/* MAKE_EXPORT NetUserGetInfo_new=NetUserGetInfo */
NET_API_STATUS NET_API_FUNCTION
NetUserGetInfo_new(LPCWSTR servername,
	LPCWSTR username,
	DWORD level,
	LPBYTE* bufptr
)
{
	return ERROR_CALL_NOT_IMPLEMENTED;
}