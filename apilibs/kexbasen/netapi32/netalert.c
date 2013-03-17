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

/* MAKE_EXPORT NetAlertRaise_new=NetAlertRaise */
NET_API_STATUS WINAPI NetAlertRaise_new(LPCWSTR AlertEventName,
	LPVOID Buffer,
	DWORD BufferSize)
{
	FIXME("Stub %s 0x%08lx %d\n", AlertEventName, Buffer, BufferSize);
	return NERR_Success;
}

/* MAKE_EXPORT NetAlertRaiseEx_new=NetAlertRaiseEx */
NET_API_STATUS WINAPI NetAlertRaiseEx_new(LPCWSTR AlertEventName,
	LPVOID VariableInfo,
	DWORD VariableInfoSize,
	LPCWSTR ServiceName)
{
	FIXME("Stub %s 0x%08lx %d %s\n", AlertEventName,
		VariableInfo, VariableInfoSize, ServiceName);
	return NERR_Success;
}