/*
 *  KernelEx
 *  Copyright (C) 2009, Xeno86
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
#include "common.h"
#include "..\user32\desktop.h"

/* MAKE_EXPORT CreateProcessA_fix=CreateProcessA */
BOOL WINAPI CreateProcessA_fix(LPCSTR lpApplicationName, LPSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCSTR lpCurrentDirectory, LPSTARTUPINFOA lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation)
{
    PTDB98 Thread = get_tdb();
	PPDB98 Process = get_pdb();
	PCHAR pszDesktop = NULL;

    if(IsBadWritePtr(lpStartupInfo, sizeof(STARTUPINFO)) || IsBadWritePtr(lpProcessInformation, sizeof(PROCESS_INFORMATION)))
        return FALSE;

    /* Remove unsupported flags (and fix them) */
    if(dwCreationFlags & CREATE_NO_WINDOW)
    {
        if(!lpStartupInfo->dwFlags & STARTF_USESHOWWINDOW)
            lpStartupInfo->dwFlags |= STARTF_USESHOWWINDOW;

        lpStartupInfo->wShowWindow = SW_HIDE;
        dwCreationFlags &= ~CREATE_NO_WINDOW;
    }

    if(dwCreationFlags & CREATE_SEPARATE_WOW_VDM)
        dwCreationFlags &= ~CREATE_SEPARATE_WOW_VDM;

    if(dwCreationFlags & CREATE_SHARED_WOW_VDM)
        dwCreationFlags &= ~CREATE_SHARED_WOW_VDM;

    if(dwCreationFlags & CREATE_UNICODE_ENVIRONMENT)
        dwCreationFlags &= ~CREATE_UNICODE_ENVIRONMENT;


	__try
	{
		if(lpStartupInfo->lpDesktop == NULL)
		{
			lpStartupInfo->lpDesktop = Process->Win32Process->rpdeskStartup->lpName;
		}

		/* Make the desktop name a shared string */
		pszDesktop = (PCHAR)kexAllocObject(strlen(lpStartupInfo->lpDesktop));
		strcpy(pszDesktop, lpStartupInfo->lpDesktop);
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		pszDesktop = NULL;
	}

	lpStartupInfo->lpDesktop = pszDesktop;

	return CreateProcessA(lpApplicationName,
						 lpCommandLine,
						 lpProcessAttributes,
						 lpThreadAttributes,
						 bInheritHandles,
						 dwCreationFlags,
						 lpEnvironment,
						 lpCurrentDirectory,
						 lpStartupInfo,
						 lpProcessInformation);
}

/* MAKE_EXPORT CreateProcessAsUserA_new=CreateProcessAsUserA*/
BOOL WINAPI CreateProcessAsUserA_new(HANDLE hToken, 
	LPCSTR lpApplicationName,
	LPSTR lpCommandLine,
	LPSECURITY_ATTRIBUTES lpProcessAttributes,
	LPSECURITY_ATTRIBUTES lpThreadAttributes, 
	BOOL bInheritHandles,
	DWORD dwCreationFlags,
	LPVOID lpEnvironment,
	LPCSTR lpCurrentDirectory,
	LPSTARTUPINFOA lpStartupInfo,
	LPPROCESS_INFORMATION lpProcessInformation)
{
	return CreateProcessA_fix(lpApplicationName,
							lpCommandLine,
							lpProcessAttributes,
							lpThreadAttributes,
							bInheritHandles,
							dwCreationFlags,
							lpEnvironment,
							lpCurrentDirectory,
							lpStartupInfo,
							lpProcessInformation);
}

/* MAKE_EXPORT ExitProcess_fix=ExitProcess */
VOID WINAPI ExitProcess_fix( UINT uExitCode )
{
	PDB98* pdb = get_pdb();
	
	//process is already terminating. we would badly crash 
	if (pdb->Flags & fTerminating)
	{
		//so instead silently fail
		SetErrorMode(SEM_NOGPFAULTERRORBOX | SEM_FAILCRITICALERRORS);
		RaiseException(EXCEPTION_ACCESS_VIOLATION, 0, 0, NULL);
	}

	//set calling thread priority to the lowest possible. this way 
	//we greatly improve chance for thread which calls ExitProcess
	//to finish latest and call dll process detach like NT
	SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_IDLE);
	ExitProcess(uExitCode);
}

/* MAKE_EXPORT GetProcessId_new=GetProcessId */
DWORD WINAPI GetProcessId_new(
  HANDLE hProcess
)
{
	PPDB98 Process = (PPDB98)kexGetHandleObject(hProcess, WIN98_K32OBJ_PROCESS, 0);
	DWORD Obfuscator = (DWORD)get_pdb() ^ GetCurrentProcessId();

	if(Process == NULL)
		return 0;

	return (DWORD)Process ^ Obfuscator;
}

/* This is less reliable */
#if 0
DWORD WINAPI GetProcessId_new(
  HANDLE hProcess
)
{
	typedef DWORD (WINAPI *MPH) (HANDLE hProcess);
	static MPH MapProcessHandle = 0;
	
	if (!MapProcessHandle)
	{
		DWORD *faddr;
		DWORD addr;
		
		faddr = (DWORD *) ( (DWORD)SetFilePointer + 0x1D ); //there is jmp _SetFilePointer	
		addr = (DWORD) faddr + *faddr + 4 - 0x16; //0x16 bytes before _SetFilePointer there is MapProcessHandle, just what we need	
		faddr = (DWORD *) addr;
		if (*faddr != 0x206A006A)
			fatal_error("GetProcessId: ASSERTION FAILED"); //push 0; push 0x20
		MapProcessHandle = (MPH) addr;
	}
	return MapProcessHandle(hProcess);
}
#endif

/* MAKE_EXPORT IsWow64Process_new=IsWow64Process */
BOOL WINAPI IsWow64Process_new(HANDLE hProcess, PBOOL Wow64Process)
{
	if (!Wow64Process) 
		return FALSE;
	*Wow64Process = FALSE;
	return TRUE;
}

/* MAKE_EXPORT ProcessIdToSessionId_new=ProcessIdToSessionId */
BOOL WINAPI ProcessIdToSessionId_new(DWORD dwProcessId, DWORD *pSessionId)
{
	if (!pSessionId)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	/* Process not running under RDS session */
	*pSessionId = 0;
	return TRUE;
}