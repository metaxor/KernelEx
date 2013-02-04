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

#include "common.h"
#include "desktop.h"
#include "kexcoresdk.h"
#include "_user32_apilist.h"
#include "thuni_layer.h"

IsHungThread_t IsHungThread_pfn;
DrawCaptionTempA_t DrawCaptionTempA_pfn;

PPDB98 Msg32Process = NULL;
PPDB98 MprProcess = NULL;

PTDB98 pHardErrorThread = NULL;
DWORD HardErrorThreadId = 0;

PHARDERRORDATA *pHardErrorData;
LONG NumHardError = 0;

BOOL SetParent_fix_init();

DWORD WINAPI HardErrorThread(PVOID lParam)
{
	HANDLE hEvent = NULL;
	HANDLE hDupEvent = NULL;
	HANDLE hProcess = NULL;
	PHARDERRORDATA pData = NULL;
	LONG CurrentArray = 0;
	BOOL fFound = FALSE;

	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_IDLE);

	pHardErrorThread = get_tdb();
	HardErrorThreadId = GetCurrentThreadId();

	pHardErrorData = (PHARDERRORDATA*)kexAllocObject(MAX_HARD_ERRORS * sizeof(DWORD));

	for(CurrentArray=0;CurrentArray<=MAX_HARD_ERRORS;CurrentArray++)
		pHardErrorData[CurrentArray] = NULL;

	while(1)
	{
		for(CurrentArray=0;CurrentArray<=MAX_HARD_ERRORS;CurrentArray++)
		{
			if(pHardErrorData[CurrentArray] != NULL)
			{
				fFound = TRUE;
				break;
			}
		}

		if(!fFound)
			goto _continue;

		pData = pHardErrorData[CurrentArray];

		if(pData == NULL || IsBadReadPtr(pData, sizeof(HARDERRORDATA)))
		{
			InterlockedExchangePointer(&pHardErrorData[CurrentArray], NULL);
			goto _continue;
		}

		hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pData->dwProcessId);

		if(hProcess != NULL)
		{
			hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

			if(hEvent != NULL)
			{
				DuplicateHandle(GetCurrentProcess(), hEvent, hProcess, &hDupEvent, 0, FALSE, DUPLICATE_SAME_ACCESS);
				pData->hEvent = hDupEvent;
			}
			else
				pData->hEvent = INVALID_HANDLE_VALUE;

			CloseHandle(hProcess);
		}

		MessageBox(NULL, pData->lpHardErrorMessage, pData->lpHardErrorTitle, MB_ICONERROR | MB_SYSTEMMODAL | pData->uType);

		if(hEvent != NULL)
		{
			SetEvent(hEvent);
			CloseHandle(hEvent);
			CloseHandle(hDupEvent);
		}

		kexFreeObject((PVOID)pData->lpHardErrorMessage);
		kexFreeObject((PVOID)pData->lpHardErrorTitle);
		kexFreeObject(pData);

		pHardErrorData[CurrentArray] = NULL;

_continue:
		Sleep(10);
	}

	return 0;
}

BOOL WINAPI Win32RaiseHardError(LPCSTR lpszMessage, LPCSTR lpszTitle, UINT uMsgType, BOOL fWait)
{
	LONG i;
	BOOL result = FALSE;
	PHARDERRORDATA pData = NULL;
	BOOL fFound = FALSE;

	pData = (PHARDERRORDATA)kexAllocObject(sizeof(HARDERRORDATA));

	if(pData == NULL)
		return FALSE;

	pData->Result = -1;

	pData->lpHardErrorMessage = (LPCSTR)kexAllocObject(strlen(lpszMessage) + 1);
	pData->lpHardErrorTitle = (LPCSTR)kexAllocObject(strlen(lpszTitle) + 1);
	pData->uType = uMsgType;

	strcpy((char*)pData->lpHardErrorMessage, lpszMessage);
	strcpy((char*)pData->lpHardErrorTitle, lpszTitle);

	for(i=0;i<=MAX_HARD_ERRORS;i++)
	{
		if(pHardErrorData[i] == NULL)
		{
			InterlockedExchangePointer(&pHardErrorData[i], pData);
			fFound = TRUE;
			break;
		}
	}

	if(fFound == FALSE)
	{
		kexFreeObject((PVOID)pData->lpHardErrorMessage);
		kexFreeObject((PVOID)pData->lpHardErrorTitle);
		kexFreeObject(pData);
		ExitProcess(0);
	}

	if(fWait)
	{
		while(pData->hEvent == NULL)
		{
			MSG msg;

			PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE);
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			Sleep(1);
		}

		WaitForSingleObject(pData->hEvent, INFINITE);
	}
	return TRUE;
}

DWORD WINAPI ShutdownThread(PVOID lParam);

BOOL init_user32()
{
	HMODULE hUser32 = GetModuleHandle("USER32.DLL");
	HMODULE hKernel32 = GetModuleHandle("KERNEL32.DLL");
	HANDLE hThread = NULL;
	HANDLE hThread2 = NULL;
	HANDLE hThread3 = NULL;
	DWORD dwThreadId = 0;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	MprProcess = get_pdb();

	/* Get MSGSRV32 */
	Msg32Process = MprProcess->ParentPDB->ParentPDB;

	IsHungThread_pfn = (IsHungThread_t)kexGetProcAddress(hUser32, "IsHungThread");
	DrawCaptionTempA_pfn = (DrawCaptionTempA_t)kexGetProcAddress(hUser32, "DrawCaptionTempA");
	GetMouseMovePoints_pfn = (GetMouseMovePoints_t)kexGetProcAddress(hUser32, "GetMouseMovePoints");

	if(gpdeskInputDesktop == NULL)
		if(!CreateWindowStationAndDesktops())
			return 0;

	/* Create the desktop thread */
	hThread = CreateKernelThread(NULL, 0, DesktopThread, NULL, 0, &dwThreadId);

	if(hThread == NULL)
	{
		TRACE_OUT("Failed to create the desktop thread !\n");
		CloseHandle(hThread);
	}

	/* Create the shutdown thread */
	hThread2 = CreateKernelThread(NULL, 0, ShutdownThread, NULL, 0, &dwThreadId);

	if(hThread2 == NULL)
	{
		TRACE_OUT("Failed to create the shutdown thread !\n");
		CloseHandle(hThread2);
	}

	/* Create the hard-error messages thread */
	hThread3 = CreateKernelThread(NULL, 0, HardErrorThread, NULL, 0, &dwThreadId);

	if(hThread3 == NULL)
	{
		TRACE_OUT("Failed to create the hard-error messages thread !\n");
		CloseHandle(hThread3);
	}

	memset(&si, 0, sizeof(STARTUPINFO));
	memset(&pi, 0, sizeof(PROCESS_INFORMATION));

	si.cb = sizeof(STARTUPINFO);

#ifdef _DEBUG
	CreateProcess(NULL,
				"DRWATSON.EXE",
				NULL,
				NULL,
				FALSE,
				0,
				NULL,
				NULL,
				&si,
				&pi);
#endif

	return IsHungThread_pfn && DrawCaptionTempA_pfn && GetMouseMovePoints_pfn
			&& InitUniThunkLayer() && hThread && hThread2 && hThread3;
}

BOOL thread_user32_init(PTDB98 Thread)
{
	return InitDesktops();
}

BOOL process_user32_init(PPDB98 Process)
{
	TRACE("GDI resources: %u%%\n", GetFreeSystemResources(GFSR_GDIRESOURCES));
	TRACE("System resources: %u%%\n", GetFreeSystemResources(GFSR_SYSTEMRESOURCES));
	TRACE("USER resources: %u%%\n", GetFreeSystemResources(GFSR_USERRESOURCES));
	if(GetFreeSystemResources(GFSR_USERRESOURCES) < 12 && GetLastError() == 0)
	{
		WARN("Out of USER resources ! Process %p cannot start\n", Process);
		return FALSE;
	}

	return TRUE;
}

VOID thread_user_uninit(PTDB98 Thread)
{
	__try
	{
		/* Dereference the desktop if this is the last thread in the process */
		if(Thread->tib.pProcess->cNotTermThreads < 1)
		{
			TRACE_OUT("Dereferencing thread's desktop\n");
			kexDereferenceObject(Thread->Win32Thread->rpdesk);
		}

		Thread->Win32Thread->rpdesk = NULL;
		Thread->Win32Thread->hdesk = NULL;
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
	}
	return;
}

VOID process_user_uninit(PPDB98 Process)
{
	PHANDLE_TABLE pHandleTable = Process->pHandleTable;
	PK32OBJHEAD Object;
	UINT index;

	__try
	{
		Process->Win32Process->rpdeskStartup = NULL;
		Process->Win32Process->hdeskStartup = NULL;

		kexDereferenceObject(Process->Win32Process->rpwinsta);
		Process->Win32Process->rpwinsta = NULL;
		Process->Win32Process->hwinsta = NULL;

		TRACE_OUT("Trying to free some desktops\n");
		for(index=1;index<=pHandleTable->cEntries;index++)
		{
			Object = (PK32OBJHEAD)pHandleTable->array[index].pObject;

			if(IsBadReadPtr(Object, sizeof(K32OBJHEAD)))
				continue;

			if(Object->Type == K32OBJ_DESKTOP)
				CloseDesktop_new((HDESK)(index*4));
			else if(Object->Type == K32OBJ_WINSTATION)
				CloseWindowStation_new((HWINSTA)(index*4));
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
	}
	return;
}

/* APIs which don't require Unicode implementations in thuni model
 * MAKE_EXPORT CallMsgFilterA=CallMsgFilterW
 * MAKE_EXPORT CopyAcceleratorTableA=CopyAcceleratorTableW
 * MAKE_EXPORT CreateAcceleratorTableA=CreateAcceleratorTableW
 * MAKE_EXPORT DispatchMessageA=DispatchMessageW
 * MAKE_EXPORT IsDialogMessageA=IsDialogMessageW
 * MAKE_EXPORT GetAltTabInfo=GetAltTabInfoA
 * MAKE_EXPORT RealGetWindowClass=RealGetWindowClassA
 * MAKE_EXPORT SetWindowsHookExA=SetWindowsHookExW
 * MAKE_EXPORT SetWindowsHookA=SetWindowsHookW
 * MAKE_EXPORT TranslateAcceleratorA=TranslateAcceleratorW
 */

/* Shell hook APIs. Can be implemented with shell, 
 * but would be too sensitive for cruel world.
 * MAKE_EXPORT IsWindow=DeregisterShellHookWindow
 * MAKE_EXPORT IsWindow=RegisterShellHookWindow
 */

static const apilib_named_api user32_named_apis[] = 
{
/*** AUTOGENERATED APILIST NAMED EXPORTS BEGIN ***/
	DECL_API("AllowSetForegroundWindow", AllowSetForegroundWindow_98),
	DECL_API("AnyPopup", AnyPopup_nothunk),
	DECL_API("AppendMenuW", AppendMenuW_new),
	DECL_API("CallMsgFilterW", CallMsgFilterA),
	DECL_API("CallWindowProcA", CallWindowProcA_fix),
	DECL_API("CallWindowProcW", CallWindowProcW_new),
	DECL_API("ChangeDisplaySettingsA", ChangeDisplaySettingsA_fix),
	DECL_API("ChangeDisplaySettingsExA", ChangeDisplaySettingsExA_fix),
	DECL_API("ChangeMenuW", ChangeMenuW_new),
	DECL_API("CharLowerBuffW", CharLowerBuffW_new),
	DECL_API("CharLowerW", CharLowerW_new),
	DECL_API("CharNextExW", CharNextExW_new),
	DECL_API("CharNextW", CharNextW_new),
	DECL_API("CharPrevExW", CharPrevExW_new),
	DECL_API("CharPrevW", CharPrevW_new),
	DECL_API("CharToOemBuffW", CharToOemBuffW_new),
	DECL_API("CharToOemW", CharToOemW_new),
	DECL_API("CharUpperBuffW", CharUpperBuffW_new),
	DECL_API("CharUpperW", CharUpperW_new),
	DECL_API("CloseDesktop", CloseDesktop_new),
	DECL_API("CloseWindowStation", CloseWindowStation_new),
	DECL_API("CopyAcceleratorTableW", CopyAcceleratorTableA),
	DECL_API("CreateAcceleratorTableW", CreateAcceleratorTableA),
	DECL_API("CreateDesktopA", CreateDesktopA_new),
	DECL_API("CreateDesktopExA", CreateDesktopExA_new),
	DECL_API("CreateDesktopExW", CreateDesktopExW_new),
	DECL_API("CreateDesktopW", CreateDesktopW_new),
	DECL_API("CreateDialogIndirectParamA", CreateDialogIndirectParamA_fix),
	DECL_API("CreateDialogIndirectParamW", CreateDialogIndirectParamW_NEW),
	DECL_API("CreateDialogParamA", CreateDialogParamA_fix),
	DECL_API("CreateDialogParamW", CreateDialogParamW_NEW),
	DECL_API("CreateMDIWindowA", CreateMDIWindowA_fix),
	DECL_API("CreateMDIWindowW", CreateMDIWindowW_NEW),
	DECL_API("CreateWindowExA", CreateWindowExA_fix),
	DECL_API("CreateWindowExW", CreateWindowExW_NEW),
	DECL_API("CreateWindowStationA", CreateWindowStationA_new),
	DECL_API("CreateWindowStationW", CreateWindowStationW_new),
	DECL_API("CreateWindowW", CreateWindowW_NEW),
	DECL_API("DefDlgProcW", DefDlgProcW_NEW),
	DECL_API("DefFrameProcW", DefFrameProcW_NEW),
	DECL_API("DefMDIChildProcW", DefMDIChildProcW_NEW),
	DECL_API("DefRawInputProc", DefRawInputProc_new),
	DECL_API("DefWindowProcW", DefWindowProcW_NEW),
	DECL_API("DeregisterShellHookWindow", IsWindow),
	DECL_API("DialogBoxIndirectParamW", DialogBoxIndirectParamW_NEW),
	DECL_API("DialogBoxParamW", DialogBoxParamW_NEW),
	DECL_API("DisableProcessWindowsGhosting", DisableProcessWindowsGhosting_new),
	DECL_API("DispatchMessageW", DispatchMessageA),
	DECL_API("DrawCaptionTempW", DrawCaptionTempW_new),
	DECL_API("EnableWindow", EnableWindow_nothunk),
	DECL_API("EnumChildWindows", EnumChildWindows_nothunk),
	DECL_API("EnumDesktopWindows", EnumDesktopWindows_new),
	DECL_API("EnumDesktopsA", EnumDesktopsA_new),
	DECL_API("EnumDesktopsW", EnumDesktopsW_new),
	DECL_API("EnumDisplaySettingsA", EnumDisplaySettingsA_fix),
	DECL_API("EnumDisplaySettingsExA", EnumDisplaySettingsExA_fix),
	DECL_API("EnumThreadWindows", EnumThreadWindows_nothunk),
	DECL_API("EnumWindowStationsA", EnumWindowStationsA_new),
	DECL_API("EnumWindowStationsW", EnumWindowStationsW_new),
	DECL_API("EnumWindows", EnumWindows_nothunk),
	DECL_API("ExitWindowsEx", ExitWindowsEx_fix),
	DECL_API("GetAltTabInfoA", GetAltTabInfo),
	DECL_API("GetAncestor", GetAncestor_fix),
	DECL_API("GetClassInfoExW", GetClassInfoExW_NEW),
	DECL_API("GetClassInfoW", GetClassInfoW_NEW),
	DECL_API("GetClassLongW", GetClassLongW_NEW),
	DECL_API("GetDlgItemTextW", GetDlgItemTextW_NEW),
	DECL_API("GetForegroundWindow", GetForegroundWindow_fix),
	DECL_API("GetInputDesktop", GetInputDesktop_new),
	DECL_API("GetLastInputInfo", GetLastInputInfo_new),
	DECL_API("GetLastInputInfo", GetLastInputInfo_NEW),
	DECL_API("GetMessageA", GetMessageA_NEW),
	DECL_API("GetMessageW", GetMessageW_NEW),
	DECL_API("GetMouseMovePointsEx", GetMouseMovePointsEx_98),
	DECL_API("GetNextDlgTabItem", GetNextDlgTabItem_fix),
	DECL_API("GetParent", GetParent_nothunk),
	DECL_API("GetProcessWindowStation", GetProcessWindowStation_new),
	DECL_API("GetRawInputBuffer", GetRawInputBuffer_new),
	DECL_API("GetRawInputData", GetRawInputData_new),
	DECL_API("GetRawInputDeviceInfoA", GetRawInputDeviceInfo_new),
	DECL_API("GetRawInputDeviceInfoW", GetRawInputDeviceInfo_new),
	DECL_API("GetRawInputDeviceList", GetRawInputDeviceList_new),
	DECL_API("GetRegisteredRawInputDevices", GetRegisteredRawInputDevices_new),
	DECL_API("GetShellWindow", GetShellWindow_new),
	DECL_API("GetThreadDesktop", GetThreadDesktop_new),
	DECL_API("GetUserObjectInformationA", GetUserObjectInformationA_new),
	DECL_API("GetUserObjectInformationW", GetUserObjectInformationW_new),
	DECL_API("GetUserObjectSecurity", GetUserObjectSecurity_new),
	DECL_API("GetWindowLongA", GetWindowLongA_NEW),
	DECL_API("GetWindowLongW", GetWindowLongW_NEW),
	DECL_API("GetWindowTextLengthW", GetWindowTextLengthW_NEW),
	DECL_API("GetWindowTextW", GetWindowTextW_NEW),
	DECL_API("InitSharedTable", InitSharedTable_stub),
	DECL_API("InsertMenuW", InsertMenuW_new),
	DECL_API("IsCharAlphaNumericW", IsCharAlphaNumericW_new),
	DECL_API("IsCharAlphaW", IsCharAlphaW_new),
	DECL_API("IsCharLowerW", IsCharLowerW_new),
	DECL_API("IsCharUpperW", IsCharUpperW_new),
	DECL_API("IsDialogMessageW", IsDialogMessageA),
	DECL_API("IsHungAppWindow", IsHungAppWindow_new),
	DECL_API("IsWindowUnicode", IsWindowUnicode_NEW),
	DECL_API("IsWindowVisible", IsWindowVisible_fix),
	DECL_API("LoadMenuA", LoadMenuA_fix),
	DECL_API("LoadMenuIndirectW", LoadMenuIndirectA),
	DECL_API("LoadMenuW", LoadMenuW_new),
	DECL_API("LoadStringW", LoadStringW_new),
	DECL_API("LockSetForegroundWindow", LockSetForegroundWindow_98),
	DECL_API("LockWindowStation", LockWindowStation_new),
	DECL_API("LockWorkStation", LockWorkStation_new),
	DECL_API("MapVirtualKeyA", MapVirtualKeyA_new),
	DECL_API("MapVirtualKeyExA", MapVirtualKeyExA_new),
	DECL_API("ModifyMenuW", ModifyMenuW_new),
	DECL_API("OemToCharBuffW", OemToCharBuffW_new),
	DECL_API("OemToCharW", OemToCharW_new),
	DECL_API("OpenDesktopA", OpenDesktopA_new),
	DECL_API("OpenDesktopW", OpenDesktopW_new),
	DECL_API("OpenInputDesktop", OpenInputDesktop_new),
	DECL_API("OpenWindowStationA", OpenWindowStationA_new),
	DECL_API("OpenWindowStationW", OpenWindowStationW_new),
	DECL_API("PeekMessageA", PeekMessageA_NEW),
	DECL_API("PeekMessageW", PeekMessageW_NEW),
	DECL_API("PostMessageW", PostMessageW_NEW),
	DECL_API("PostThreadMessageW", PostThreadMessageW_NEW),
	DECL_API("PrintWindow", PrintWindow_stub),
	DECL_API("RealGetWindowClassA", RealGetWindowClass),
	DECL_API("RealGetWindowClassW", RealGetWindowClassW_new),
	DECL_API("RegisterClassExW", RegisterClassExW_NEW),
	DECL_API("RegisterClassW", RegisterClassW_NEW),
	DECL_API("RegisterRawInputDevices", RegisterRawInputDevices_new),
	DECL_API("RegisterServicesProcess", RegisterServicesProcess_stub),
	DECL_API("RegisterShellHookWindow", IsWindow),
	DECL_API("SendDlgItemMessageW", SendDlgItemMessageW_NEW),
	DECL_API("SendMessageA", SendMessageA_fix),
	DECL_API("SendMessageCallbackW", SendMessageCallbackW_NEW),
	DECL_API("SendMessageTimeoutW", SendMessageTimeoutW_NEW),
	DECL_API("SendMessageW", SendMessageW_NEW),
	DECL_API("SendNotifyMessageW", SendNotifyMessageW_NEW),
	DECL_API("SetClassLongW", SetClassLongW_NEW),
	DECL_API("SetDlgItemTextW", SetDlgItemTextW_NEW),
	DECL_API("SetForegroundWindow", SetForegroundWindow_fix),
	DECL_API("SetLayeredWindowAttributes", SetLayeredWindowAttributes_stub),
	DECL_API("SetParent", SetParent_fix),
	DECL_API("SetProcessWindowStation", SetProcessWindowStation_new),
	DECL_API("SetThreadDesktop", SetThreadDesktop_new),
	DECL_API("SetUserObjectInformationA", SetUserObjectInformationA_new),
	DECL_API("SetUserObjectSecurity", SetUserObjectSecurity_new),
	DECL_API("SetWindowLongA", SetWindowLongA_NEW),
	DECL_API("SetWindowLongW", SetWindowLongW_NEW),
	DECL_API("SetWindowPos", SetWindowPos_fix),
	DECL_API("SetWindowTextW", SetWindowTextW_NEW),
	DECL_API("SetWindowsHookExW", SetWindowsHookExA),
	DECL_API("SetWindowsHookW", SetWindowsHookA),
	DECL_API("ShowWindow", ShowWindow_fix),
	DECL_API("ShowWindowAsync", ShowWindowAsync_fix),
	DECL_API("SwitchDesktop", SwitchDesktop_new),
	DECL_API("SystemParametersInfoA", SystemParametersInfoA_fix),
	DECL_API("ToUnicode", ToUnicode_new),
	DECL_API("ToUnicodeEx", ToUnicodeEx_new),
	DECL_API("TranslateAcceleratorW", TranslateAcceleratorA),
	DECL_API("UnlockWindowStation", UnlockWindowStation_new),
	DECL_API("UnregisterClassW", UnregisterClassW_NEW),
	DECL_API("UpdateLayeredWindow", UpdateLayeredWindow_new),
/*** AUTOGENERATED APILIST NAMED EXPORTS END ***/
};

#if 0
static const apilib_unnamed_api user32_ordinal_apis[] =
{
/*** AUTOGENERATED APILIST ORDINAL EXPORTS BEGIN ***/
/*** AUTOGENERATED APILIST ORDINAL EXPORTS END ***/
};
#endif

const apilib_api_table apitable_user32 = DECL_TAB("USER32.DLL", user32_named_apis, 0 /*user32_ordinal_apis*/);
