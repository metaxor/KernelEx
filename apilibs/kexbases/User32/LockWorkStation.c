/*
 *  KernelEx
 *  Copyright (C) 2008, Tihiy
 *  Copyright (C) 2013, Ley0k
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

#include <windows.h>
#include "desktop.h"

/* MAKE_EXPORT LockWorkStation_new=LockWorkStation */
BOOL WINAPI LockWorkStation_new(void)
{
	BOOL Result;
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	//FIXME: rundll32 likes to call it and crashes because stack would be incorrect
	//return WinExec("conlock.mod -uSeR",SW_SHOWDEFAULT) > 31 ? TRUE : FALSE;

    Result = CreateProcess("conlock.mod",
                  "-uSeR",
                  NULL,
                  NULL,
                  FALSE,
                  0,
                  NULL,
                  NULL,
                  &si,
                  &pi);

	if(!Result)
		return FALSE;

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	return TRUE;
}
