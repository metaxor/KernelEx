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

#include "vxd.h"

void WINAPI PageModifyPermissions(DWORD dwAddress, DWORD dwSize, BOOL fWriteable)
{
	/* The address must reside in shared memory */
	if (!(dwAddress & 0x80000000))
		return;

	dwAddress = dwAddress >> 12;
	dwSize = dwSize >> 12;

	if (dwSize == 0)
		dwSize = 1;

	/* 4th parameter is the permission to remove (permand) and 5th is the permission to add (permor) */

	if (fWriteable)
		VxDCall4(0x001000D, dwAddress, dwSize, 0, PMP_WRITEABLE);
	else
		VxDCall4(0x001000D, dwAddress, dwSize, 0, PMP_NOTWRITEABLE);
}