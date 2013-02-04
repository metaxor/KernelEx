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
#include "winsta.h"

BOOL WINAPI DllMain (HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
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
 *                WinStationTerminateProcess  (WINSTA.@)
 */
BOOL WINAPI WinStationTerminateProcess(HANDLE hServer, DWORD ProcessId, DWORD ExitCode)
{
	FIXME("Stub %p 0x%08x 0x%08x\n", hServer, ProcessId, ExitCode);
	SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
	return FALSE;
}