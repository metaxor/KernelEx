/*
 *  KernelEx
 *  Copyright (C) 2008, Tihiy
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

#include <windows.h>
#include "desktop.h"

#pragma warning (disable:4035)

BOOL WINAPI LockWorkStation_RUNDLL(HWND hWnd, HINSTANCE hInstance, LPSTR lpArgs, int nCmdShow)
{
	BOOL Result;
	PROCESS_INFORMATION pi;
	STARTUPINFO si;

	//return WinExec("conlock.mod -uSeR",SW_SHOWDEFAULT) > 31 ? TRUE : FALSE;

	memset(&pi, 0, sizeof(PROCESS_INFORMATION));
	memset(&si, 0, sizeof(STARTUPINFO));

	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_SHOWDEFAULT;

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
		goto _ret;

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

_ret:
	if(!IsWindow(hWnd) || (DWORD)hInstance != 0x00400000 || IsBadStringPtr(lpArgs, -1))
	{
		__asm	mov eax, [Result]
		__asm	pop edi
		__asm	pop esi
		__asm	pop ebx
		__asm	leave
		__asm	retn
	}
	else // RUNDLL called this
		return Result;
}

/* MAKE_EXPORT LockWorkStation_new=LockWorkStation */
__declspec(naked)
BOOL WINAPI LockWorkStation_new(void)
{
	__asm jmp LockWorkStation_RUNDLL
}

#pragma warning (default:4035)
