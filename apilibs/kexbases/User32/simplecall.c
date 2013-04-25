/*
 *  KernelEx
 *  Copyright (C) 2008, Xeno86
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

#include "simplecall.h"

DWORD pfnUserCall1;
DWORD pfnUserCall2;

DWORD pfnUserCall3;
DWORD pfnUserCall4;

BOOL FASTCALL InitSimpleCall(void)
{
	HINSTANCE hUser32 = GetModuleHandle("USER32.DLL");
	DWORD *dwAddr;

	dwAddr = (DWORD*)GetProcAddress(hUser32, "HasSystemSleepStarted");
	pfnUserCall1 = *(dwAddr + 0x15);

	if(pfnUserCall1 == NULL)
	{
		TRACE_OUT("pfnUserCall1 not found !\n");
		return FALSE;
	}

	pfnUserCall2 = pfnUserCall1 + 4;

	dwAddr = (DWORD*)CreateIconFromResourceEx;
	pfnUserCall3 = *(dwAddr + 0x33);

	if(pfnUserCall3 == NULL)
	{
		TRACE_OUT("pfnUserCall3 not found !\n");
		return FALSE;
	}

	pfnUserCall4 = pfnUserCall3 + 4;

	return TRUE;
}
