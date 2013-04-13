/*
 *  KernelEx
 *  Copyright (C) 2009, Xeno86
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

#ifndef __K32ORD_H
#define __K32ORD_H

#include <windows.h>

#ifdef K32ORD_IMPLIB

#ifdef _MSC_VER
#pragma warning(disable:4716)
#endif
#define MAKE_HEADER(decl) \
	__declspec(dllexport) decl {}

#else

#define MAKE_HEADER(decl) \
	__declspec(dllimport) decl ;

#endif

#ifdef __cplusplus
extern "C" {
#endif

MAKE_HEADER(void __stdcall _CheckNotSysLevel(CRITICAL_SECTION* cs))
MAKE_HEADER(DWORD __stdcall _ConfirmSysLevel(CRITICAL_SECTION* cs))
MAKE_HEADER(DWORD __stdcall _ConfirmWin16Lock(void))
MAKE_HEADER(void __stdcall _GetpWin16Lock(CRITICAL_SECTION** cs))
MAKE_HEADER(void __stdcall _EnterSysLevel(CRITICAL_SECTION* cs))
MAKE_HEADER(void __stdcall _ReleaseThunkLock(DWORD *pMutexCount))
MAKE_HEADER(void __stdcall _RestoreThunkLock(DWORD MutexCount))
MAKE_HEADER(void __stdcall _LeaveSysLevel(CRITICAL_SECTION* cs))
MAKE_HEADER(void __stdcall _LeaveSysLevel_NoThk(CRITICAL_SECTION* cs))

MAKE_HEADER(ULONG __stdcall VxDCall0(DWORD service))
MAKE_HEADER(ULONG __stdcall VxDCall1(DWORD service, ULONG p1))
MAKE_HEADER(ULONG __stdcall VxDCall2(DWORD service, ULONG p1, ULONG p2))
MAKE_HEADER(ULONG __stdcall VxDCall3(DWORD service, ULONG p1, ULONG p2, ULONG p3))
MAKE_HEADER(ULONG __stdcall VxDCall4(DWORD service, ULONG p1, ULONG p2, ULONG p3, ULONG p4))
MAKE_HEADER(ULONG __stdcall VxDCall5(DWORD service, ULONG p1, ULONG p2, ULONG p3, ULONG p4, ULONG p5))
MAKE_HEADER(ULONG __stdcall VxDCall6(DWORD service, ULONG p1, ULONG p2, ULONG p3, ULONG p4, ULONG p5, ULONG p6))
MAKE_HEADER(ULONG __stdcall VxDCall7(DWORD service, ULONG p1, ULONG p2, ULONG p3, ULONG p4, ULONG p5, ULONG p6, ULONG p7))
MAKE_HEADER(ULONG __stdcall VxDCall8(DWORD service, ULONG p1, ULONG p2, ULONG p3, ULONG p4, ULONG p5, ULONG p6, ULONG p7, ULONG p8))

MAKE_HEADER(void __stdcall MakeCriticalSectionGlobal(CRITICAL_SECTION* cs))
MAKE_HEADER(HANDLE __stdcall ConvertToGlobalHandle(HANDLE hObject))
MAKE_HEADER(BOOL __stdcall CloseSystemHandle(HANDLE hObject))
MAKE_HEADER(HANDLE __stdcall CreateKernelThread(LPSECURITY_ATTRIBUTES lpThreadAttributes, SIZE_T dwStackSize, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, DWORD dwCreationFlags, LPDWORD lpThreadId))
MAKE_HEADER(BOOL __stdcall RegisterServiceProcess(DWORD dwProcessId, BOOL fRegister))

MAKE_HEADER(ULONG __stdcall CommonUnimpStub())

MAKE_HEADER(WORD __stdcall GlobalAlloc16(UINT uFlags, SIZE_T dwBytes))
MAKE_HEADER(LPVOID __stdcall GlobalLock16(HGLOBAL hMem))
MAKE_HEADER(SIZE_T __stdcall GlobalSize16(HGLOBAL hMem))
MAKE_HEADER(BOOL __stdcall GlobalUnlock16(HGLOBAL hMem))

MAKE_HEADER(LPVOID __stdcall K32WOWGetVDMPointerFix(DWORD vp1616, DWORD dwBytes, BOOL fProtect))
MAKE_HEADER(void __stdcall K32WOWGetVDMPointerUnfix(DWORD vp1616))
MAKE_HEADER(WORD __stdcall MapHModuleLS(HMODULE hModule))
MAKE_HEADER(DWORD __stdcall MapSL(DWORD addr16))
MAKE_HEADER(DWORD __stdcall MapLS(DWORD addr32))

MAKE_HEADER(DWORD __stdcall LoadLibrary16(LPSTR libname))
MAKE_HEADER(DWORD __stdcall GetProcAddress16(HANDLE hModule, LPSTR procname))
MAKE_HEADER(DWORD __stdcall FreeLibrary16(HANDLE hModule))

MAKE_HEADER(void __cdecl  QT_Thunk())
MAKE_HEADER(ULONG __stdcall K32_RtlNtStatusToDosError(LONG status))

MAKE_HEADER(void __stdcall GDIReallyCares(HMODULE hModule))

MAKE_HEADER(BOOL __stdcall ThunkConnect32(PVOID thunkdata, LPSTR thunkname, LPSTR dll16, LPSTR dll32, HINSTANCE hInst, DWORD dwReason))

#ifdef __cplusplus
}
#endif

#endif
