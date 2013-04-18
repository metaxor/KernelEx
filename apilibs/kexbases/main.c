/*
 *  KernelEx
 *  Copyright (C) 2008, Xeno86
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

#include <stdio.h>
#include "ntstatus.h"
#define WIN32_NO_STATUS
#include "common.h"
#include "user32/desktop.h"
#include "kernel32/_kernel32_apilist.h"
#include "gdi32/_gdi32_apilist.h"
#include "user32/_user32_apilist.h"
#include "advapi32/_advapi32_apilist.h"
#include "comdlg32/_comdlg32_apilist.h"
#include "shell32/_shell32_apilist.h"
#include "version/_version_apilist.h"
#include "comctl32/_comctl32_apilist.h"
#include "ntdll/_ntdll_apilist.h"
#include "user32/thuni_layer.h"
//#include "/__apilist.h"

DWORD Obfuscator = 0;
static LONG inited = 0;
static apilib_api_table api_table[10];

static void fill_apitable()
{
	api_table[0] = apitable_kernel32;
	api_table[1] = apitable_gdi32;
	api_table[2] = apitable_user32;
	api_table[3] = apitable_advapi32;
	api_table[4] = apitable_comdlg32;
	api_table[5] = apitable_shell32;
	api_table[6] = apitable_version;
	api_table[7] = apitable_comctl32;
	api_table[8] = apitable_ntdll;
	//last entry is null terminator
}

extern "C"
__declspec(dllexport)
const apilib_api_table* get_api_table()
{
	fill_apitable();
	return api_table;
}

BOOL init_once()
{
	Obfuscator = (DWORD)get_pdb() ^ GetCurrentProcessId();
	return common_init() && init_kernel32() && init_gdi32() && init_user32() && init_advapi32() && init_comdlg32() && init_shell32() && init_version() && init_comctl32() && init_ntdll();
}

/* Initialization phase */
BOOL ppi_init(PPDB98 Process)
{
	PPROCESSINFO ppi = NULL;
	PPROCESSINFO ppiParent = NULL;

	if(IsBadReadPtr(Process, sizeof(PDB98)))
		Process = get_pdb();

	ppi = (PPROCESSINFO)kexAllocObject(sizeof(PROCESSINFO));

	if(ppi == NULL)
	{
		TRACE("Couldn't allocate Win32Process for Process 0x%X !\n", Process);
		return FALSE;
	}

	if(Process->ParentPDB != NULL)
		ppiParent = Process->ParentPDB->Win32Process;

	Process->Win32Process = ppi;

	memset(ppi, 0, sizeof(PROCESSINFO));

	ppi->Process = Process;
	ppi->UniqueProcessId = Obfuscator ^ (DWORD)Process;
	ppi->hdeskStartup = NULL;
	ppi->rpdeskStartup = NULL;
	ppi->ShutdownLevel = 0x280;
	ppi->WindowsGhosting = TRUE;

	if(ppiParent != NULL) {
		ppi->SessionId = ppiParent->SessionId;
	}
	else {
		ppi->SessionId = gpidSession;
	}

	return TRUE;
}

BOOL pti_init(PTDB98 Thread)
{
	PTHREADINFO pti = NULL;

	if(IsBadReadPtr(Thread, sizeof(TDB98)))
		Thread = get_tdb();

	pti = (PTHREADINFO)kexAllocObject(sizeof(THREADINFO));

	if(pti == NULL)
	{
		TRACE("Couldn't allocate Win32Thread for Thread 0x%X !\n", Thread);
		return FALSE;
	}

	Thread->Win32Thread = pti;

	memset(pti, 0, sizeof(THREADINFO));

	pti->UniqueThreadId = Obfuscator ^ (DWORD)Thread;
	pti->Thread = Thread;

	GetSystemTimeAsFileTime(&pti->CreationTime);
	GetSystemTimeAsFileTime(&pti->KernelTime);
	GetSystemTimeAsFileTime(&pti->UserTime);

	return TRUE;
}

BOOL thread_init(PTDB98 Thread)
{
	return thread_user32_init(Thread);
}

BOOL process_init(PPDB98 Process)
{
	return process_user32_init(Process);
}

/* Uninitialization phase */
VOID pti_uninit(PTDB98 Thread)
{
	PTHREADINFO pti;

	if(IsBadReadPtr(Thread, sizeof(TDB98)))
		Thread = get_tdb();

	pti = Thread->Win32Thread;

	if(IsBadReadPtr(pti, sizeof(THREADINFO)))
		return;

	GetSystemTimeAsFileTime(&pti->ExitTime);

	/* Should we free the win32 thread ? One thread could call GetThreadTimes
	   to retrieve the thread's exit time */
}

VOID ppi_uninit(PPDB98 Process)
{
	if(IsBadReadPtr(Process, sizeof(PDB98)))
		Process = get_pdb();

	if(!IsBadReadPtr(Process->Win32Process, sizeof(PROCESSINFO)))
		kexFreeObject(Process->Win32Process);
}

VOID thread_uninit(PTDB98 Thread)
{
	if(IsBadReadPtr(Thread, sizeof(TDB98)))
		Thread = get_tdb();

	thread_user_uninit(Thread);
	return;
}

VOID process_uninit(PPDB98 Process)
{
	if(IsBadReadPtr(Process, sizeof(PDB98)))
		Process = get_pdb();

	process_user_uninit(Process);
	return;
}

BOOL APIENTRY DllMain(HINSTANCE instance, DWORD reason, BOOL load_static)
{
	PPDB98 Process = get_pdb();
	PTDB98 Thread = get_tdb();

	if(Process == ppdbKernelProcess)
		return TRUE;

	switch (reason) 
	{
	case DLL_PROCESS_ATTACH:

		if(inited)
		{
			TRACE("GDI resources: %u%%\n", GetFreeSystemResources(GFSR_GDIRESOURCES));
			TRACE("System resources: %u%%\n", GetFreeSystemResources(GFSR_SYSTEMRESOURCES));
			TRACE("USER resources: %u%%\n", GetFreeSystemResources(GFSR_USERRESOURCES));
			if(GetFreeSystemResources(GFSR_SYSTEMRESOURCES) < 10)
			{
				ERR("Out of system resources ! Process %p cannot start\n", Process);
				TerminateProcess(GetCurrentProcess(), 0);
				return FALSE;
			}
		}

		if(fShutdown)
		{
			ULONG Response = 0;

			NtRaiseHardError(STATUS_DLL_INIT_FAILED_LOGOFF,
							0,
							0,
							NULL,
							OptionOk,
							&Response);

			ExitProcess(0);
			return FALSE;
		}

		if(!ppi_init(Process) || !pti_init(Thread))
		{
			pti_uninit(Thread);
			ppi_uninit(Process);
			ExitProcess(0);
			return FALSE;
		}

		if(!process_init(Process) || !thread_init(Thread))
		{
			thread_uninit(Thread);
			process_uninit(Process);
			pti_uninit(Thread);
			ppi_uninit(Process);
			ExitProcess(0);
			return FALSE;
		}

		if (InterlockedExchange(&inited, 1) == 0)
		{
			if (!init_once())
				return FALSE;
		}
		break;
	case DLL_THREAD_ATTACH:
		return pti_init(Thread) && thread_init(Thread);
		break;
	case DLL_THREAD_DETACH:
		thread_uninit(Thread);
		pti_uninit(Thread);
		break;
	case DLL_PROCESS_DETACH:
		thread_uninit(Thread);
		process_uninit(Process);
		pti_uninit(Thread);
		ppi_uninit(Process);
		break;
	}
	return TRUE;
}
