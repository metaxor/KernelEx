/*
 *  KernelEx
 *  Copyright (C) 2008, Xeno86
 *  Copyright (C) 2008, Tihiy
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

/* MAKE_EXPORT CreateJobObjectA_new=CreateJobObjectA */
HANDLE WINAPI CreateJobObjectA_new(LPSECURITY_ATTRIBUTES sa, LPCSTR name)
{
	return (HANDLE) 0x1ee7;
}

/* MAKE_EXPORT CreateJobObjectW_new=CreateJobObjectW */
HANDLE WINAPI CreateJobObjectW_new(LPSECURITY_ATTRIBUTES sa, LPCWSTR name)
{
	return (HANDLE) 0x1ee7;
}

/* MAKE_EXPORT TerminateJobObject_new=TerminateJobObject */
BOOL WINAPI TerminateJobObject_new(HANDLE job, UINT exit_code)
{
	return TRUE;
}

/* MAKE_EXPORT OpenJobObjectA_new=OpenJobObjectA */
HANDLE WINAPI OpenJobObjectA_new(DWORD access, BOOL inherit, LPCSTR name)
{
	SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
	return 0;
}

/* MAKE_EXPORT OpenJobObjectW_new=OpenJobObjectW */
HANDLE WINAPI OpenJobObjectW_new(DWORD access, BOOL inherit, LPCWSTR name)
{
	SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
	return 0;
}

/* MAKE_EXPORT QueryInformationJobObject_new=QueryInformationJobObject */
BOOL WINAPI QueryInformationJobObject_new(HANDLE job, PVOID classs, LPVOID info, DWORD len, DWORD *ret_len)
{
	SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
	return FALSE;
}

/* MAKE_EXPORT SetInformationJobObject_new=SetInformationJobObject */
BOOL WINAPI SetInformationJobObject_new(HANDLE job, PVOID classs, LPVOID info, DWORD len)
{
	return TRUE;
}

/* MAKE_EXPORT AssignProcessToJobObject_new=AssignProcessToJobObject */
BOOL WINAPI AssignProcessToJobObject_new(HANDLE job, HANDLE process)
{
	return TRUE;
}

/* MAKE_EXPORT IsProcessInJob_new=IsProcessInJob */
BOOL WINAPI IsProcessInJob_new(HANDLE process, HANDLE job, PBOOL result)
{
	*result = FALSE;
	return TRUE;
}
