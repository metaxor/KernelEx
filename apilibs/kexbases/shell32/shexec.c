/*
 *  KernelEx
 *  Copyright (C) 2007, Xeno86
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
#include "common.h"
#include <shellapi.h>
#include "../kernel32/_kernel32_apilist.h"

/*  ShellExecuteExA_fix=ShellExecuteExA */
BOOL WINAPI ShellExecuteExA_fix(LPSHELLEXECUTEINFOA lpExecInfo)
{
	BOOL res = FALSE;
	DWORD dwFlags = 0;
	STARTUPINFOA si;
	PROCESS_INFORMATION pi;

	if(lpExecInfo == NULL)
		return FALSE;

	if(IsBadReadPtr(lpExecInfo, sizeof(SHELLEXECUTEINFOA)))
		return FALSE;

	if(IsBadStringPtr(lpExecInfo->lpVerb, -1))
		lpExecInfo->lpVerb = "open";

	if(strcmpi(lpExecInfo->lpVerb,  "open"))
		return ShellExecuteExA(lpExecInfo);

	__try
	{
		if(strpbrk(lpExecInfo->lpFile, ".exe") == NULL)
			return ShellExecuteExA(lpExecInfo);
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		return ShellExecuteExA(lpExecInfo);
	}

	memset(&si, 0, sizeof(STARTUPINFOA));
	memset(&pi, 0, sizeof(PROCESS_INFORMATION));

	si.dwFlags |= STARTF_USESHOWWINDOW;
	si.wShowWindow = lpExecInfo->nShow;
	si.lpDesktop = NULL;

	if(lpExecInfo->fMask & SEE_MASK_UNICODE)
		dwFlags |= CREATE_UNICODE_ENVIRONMENT;

	res = CreateProcessA_fix(lpExecInfo->lpFile,
							(LPSTR)lpExecInfo->lpParameters,
							NULL,
							NULL,
							FALSE,
							dwFlags,
							NULL,
							lpExecInfo->lpDirectory,
							&si,
							&pi);

	if(!res)
	{
		lpExecInfo->hInstApp = (HINSTANCE)GetLastError();
		return FALSE;
	}

	CloseHandle(pi.hThread);

	if(lpExecInfo->fMask & SEE_MASK_NOCLOSEPROCESS)
		lpExecInfo->hProcess = pi.hProcess;
	else
		CloseHandle(pi.hProcess);

	lpExecInfo->hInstApp = GetModuleHandleA(lpExecInfo->lpFile);

	return TRUE;
}

/*  ShellExecuteA_fix=ShellExecuteA */
HINSTANCE WINAPI ShellExecuteA_fix(HWND hwnd, LPCSTR lpOperation, LPCSTR lpFile, LPCSTR lpParameters, LPCSTR lpDirectory, INT nShowCmd)
{
	SHELLEXECUTEINFOA sei;

	sei.cbSize = sizeof(SHELLEXECUTEINFOA);
	sei.fMask = SEE_MASK_FLAG_NO_UI;
    sei.hwnd = hwnd;
    sei.lpVerb = lpOperation;
    sei.lpFile = lpFile;
    sei.lpParameters = lpParameters;
    sei.lpDirectory = lpDirectory;
    sei.nShow = nShowCmd;
    sei.lpIDList = 0;
    sei.lpClass = 0;
    sei.hkeyClass = 0;
    sei.dwHotKey = 0;
    sei.hProcess = 0;

	ShellExecuteExA_fix(&sei);

	return sei.hInstApp;
}