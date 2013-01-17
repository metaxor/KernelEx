/*
 *  KernelEx
 *  Copyright (C) 2011, Xeno86
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

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
extern "C" {
#define WANTVXDWRAPS
#include <vmm.h>
#include <debug.h>
#include <vxdwraps.h>
#include <vwin32.h>
};
#include <winerror.h>
#include "vxdmain.h"
#include "util.h"
#include "debug.h"
#include "pemanip.h"
#include "patch_kernel32.h"
#include "patch_ifsmgr.h"
#include "interface.h"

#define V_MAJOR		1
#define V_MINOR		0

_Declare_Virtual_Device(VKRNLEX, V_MAJOR, V_MINOR, ControlDispatcher, UNDEFINED_DEVICE_ID, UNDEFINED_INIT_ORDER, 0, 0, 0);

DWORD ( _stdcall *VKernelEx_W32_Proc[] )(DWORD, PDIOCPARAMETERS) = {
        VKernelEx_IOCTL_Connect
};

#define MAX_VKERNELEX_W32_API (sizeof(VKernelEx_W32_Proc)/sizeof(DWORD))

static ioctl_connect_params s_connect_params;

BOOL __stdcall ControlDispatcher(
    DWORD dwControlMessage,
    DWORD EBX,
    DWORD ECX,
    DWORD EDX,
    DWORD ESI,
    DWORD EDI)
{
	switch(dwControlMessage)
	{
	case SYS_CRITICAL_INIT:
		return VKernelEx_Critical_Init();
	case DEVICE_INIT:
		return VKernelEx_Dynamic_Init();
	case SYS_DYNAMIC_DEVICE_INIT:
		return VKernelEx_Dynamic_Init();
	case SYS_DYNAMIC_DEVICE_EXIT:
		return VKernelEx_Dynamic_Exit();
	case W32_DEVICEIOCONTROL:
		return VKernelEx_W32_DeviceIOControl(ECX, EDX, (PDIOCPARAMETERS) ESI);
	case BEGIN_PM_APP:
		return VKernelEx_Begin_PM_App(EBX);
	case CREATE_PROCESS:
		return VKernelEx_Create_Process(EDX);
	case DESTROY_PROCESS:
		return VKernelEx_Destroy_Process(EDX);
	default:
		return TRUE;
	}
}

/****************************************************************************
                  VKernelEx_W32_DeviceIOControl
****************************************************************************/
DWORD _stdcall VKernelEx_W32_DeviceIOControl(
	DWORD  dwService,
	DWORD  hDevice,
    PDIOCPARAMETERS lpDIOCParms
)
{
    DWORD dwRetVal = 0;

    // DIOC_OPEN is sent when VxD is loaded w/ CreateFile 
    //  (this happens just after SYS_DYNAMIC_INIT)
    if (dwService == DIOC_OPEN)
    {
        DBGPRINTF(("WIN32 DEVIOCTL supported here!"));
        // Must return 0 to tell WIN32 that this VxD supports DEVIOCTL
        dwRetVal = 0;
    }
    // DIOC_CLOSEHANDLE is sent when VxD is unloaded w/ CloseHandle
    //  (this happens just before SYS_DYNAMIC_EXIT)
    else if (dwService == DIOC_CLOSEHANDLE)
    {
        // Dispatch to cleanup proc
        dwRetVal = VKernelEx_CleanUp();
    }
    else if (dwService > MAX_VKERNELEX_W32_API)
    {
        // Returning a positive value will cause the WIN32 DeviceIOControl
        // call to return FALSE, the error code can then be retrieved
        // via the WIN32 GetLastError
        dwRetVal = ERROR_NOT_SUPPORTED;
    }
    else
    {
        // CALL requested service
        dwRetVal = (VKernelEx_W32_Proc[dwService-1])(hDevice, lpDIOCParms);
    }
    return dwRetVal;
}

DWORD _stdcall VKernelEx_IOCTL_Connect(DWORD hDevice, PDIOCPARAMETERS lpDIOCParms)
{
    DBGPRINTF(("VKernelEx_IOCTL_Connect"));

	if (lpDIOCParms->cbOutBuffer < sizeof(s_connect_params))
		return ERROR_INVALID_PARAMETER;

    memcpy((void*) lpDIOCParms->lpvOutBuffer, &s_connect_params, sizeof(s_connect_params));
	*(DWORD*) lpDIOCParms->lpcbBytesReturned = sizeof(s_connect_params);

    return NO_ERROR;
}

BOOL _stdcall VKernelEx_Create_Process(DWORD pid)
{
	return VXD_SUCCESS;
}

BOOL _stdcall VKernelEx_Destroy_Process(DWORD pid)
{
	return VXD_SUCCESS;
}

BOOL _stdcall VKernelEx_Begin_PM_App(HVM hVM)
{
	WORD version;
	void* k32ptr;
	void* backup = NULL;
	bool allOK = false;

	if (!Test_Sys_VM_Handle(hVM))
		return TRUE;

	version = Get_VMM_Version();

	if (version >= 0x045A)
		k32ptr = (void*) 0xBFF60000; //WINME
	else
		k32ptr = (void*) 0xBFF70000; //WIN98

	__try
	{
		WORD sign = 0;

		__try
		{
			sign = *(WORD*) k32ptr;
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
		}

		if (sign == 'ZM')
		{
			DBGPRINTF(("KERNEL32 located @ 0x%08x", k32ptr));
			PEmanip pem;
			pem.OpenMemory(k32ptr);
			backup = pem.CreateBackup();
			Patch_kernel32 patch(pem);
			if (patch.apply())
			{
				allOK = true;
				s_connect_params.status = true;
				s_connect_params.vxd_version = MAKEWORD(V_MINOR, V_MAJOR);
				s_connect_params.stub_ptr = patch.stub_address;
			}
		}
		else
			DBGPRINTF(("couldn't locate KERNEL32"));
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
	}

	if (!allOK)
	{
		DBGPRINTF(("KERNEL32 patch failed!"));
		if (backup != NULL)
			memcpy(k32ptr, backup, _HeapGetSize(backup, 0));
	}

	if (backup != NULL)
		_HeapFree(backup, 0);

	return TRUE;
}

BOOL _stdcall VKernelEx_Dynamic_Exit(void)
{
    DBGPRINTF(("Exit"));

    return VXD_SUCCESS;
}

DWORD _stdcall VKernelEx_CleanUp(void)
{
    DBGPRINTF(("Cleaning Up"));
    return VXD_SUCCESS;
}

#pragma VxD_ICODE_SEG
#pragma VxD_IDATA_SEG

BOOL _stdcall VKernelEx_Critical_Init(void)
{
	DBGPRINTF(("KernelEx Virtual Device v" STR(V_MAJOR) "." STR(V_MINOR)));
    DBGPRINTF(("Critical Init"));

	Patch_ifsmgr patch;
	patch.apply();

    return VXD_SUCCESS;
}

BOOL _stdcall VKernelEx_Dynamic_Init(void)
{
    DBGPRINTF(("Init"));

    return VXD_SUCCESS;
}
