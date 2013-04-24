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

#include "common.h"

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

/* Kernel32 strings functions */
MAKE_HEADER(BOOL __stdcall k32CharToOemA(LPCSTR lpString, LPSTR lpBuffer))
MAKE_HEADER(BOOL __stdcall k32CharToOemBuffA(LPCSTR lpString, LPSTR lpBuffer, DWORD dwLength))
MAKE_HEADER(BOOL __stdcall k32OemToCharA(LPCSTR lpStringOem, LPSTR lpBuffer))
MAKE_HEADER(BOOL __stdcall k32OemToCharBuffA(LPCSTR lpStringOem, LPSTR lpBuffer, DWORD dwLength))
MAKE_HEADER(INT __stdcall k32LoadStringA(HINSTANCE hInstance, UINT uResId, LPSTR lpBuffer, INT iLength))
MAKE_HEADER(INT __cdecl k32wsprintfA(LPSTR buffer, LPCSTR spec, ...))
MAKE_HEADER(INT __stdcall k32wvsprintfA(LPSTR buffer, LPCSTR spec, va_list args))

/* DOS functions */
MAKE_HEADER(HANDLE __stdcall DosFileHandleToWin32Handle(HFILE hFile))
MAKE_HEADER(HFILE _stdcall Win32HandleToDosFileHandle(HANDLE hFile))
MAKE_HEADER(VOID __stdcall DisposeLZ32Handle(HANDLE hHandle))
MAKE_HEADER(DWORD __stdcall GetWin16DOSEnv(void))

/* VxD functions */
MAKE_HEADER(ULONG __stdcall VxDCall0(DWORD service))
MAKE_HEADER(ULONG __stdcall VxDCall1(DWORD service, ULONG p1))
MAKE_HEADER(ULONG __stdcall VxDCall2(DWORD service, ULONG p1, ULONG p2))
MAKE_HEADER(ULONG __stdcall VxDCall3(DWORD service, ULONG p1, ULONG p2, ULONG p3))
MAKE_HEADER(ULONG __stdcall VxDCall4(DWORD service, ULONG p1, ULONG p2, ULONG p3, ULONG p4))
MAKE_HEADER(ULONG __stdcall VxDCall5(DWORD service, ULONG p1, ULONG p2, ULONG p3, ULONG p4, ULONG p5))
MAKE_HEADER(ULONG __stdcall VxDCall6(DWORD service, ULONG p1, ULONG p2, ULONG p3, ULONG p4, ULONG p5, ULONG p6))
MAKE_HEADER(ULONG __stdcall VxDCall7(DWORD service, ULONG p1, ULONG p2, ULONG p3, ULONG p4, ULONG p5, ULONG p6, ULONG p7))
MAKE_HEADER(ULONG __stdcall VxDCall8(DWORD service, ULONG p1, ULONG p2, ULONG p3, ULONG p4, ULONG p5, ULONG p6, ULONG p7, ULONG p8))

/* Mutex/locks functions */
MAKE_HEADER(void __stdcall PK16FNF(LPSTR lpBuffer))
MAKE_HEADER(LPVOID __stdcall GetPK16SysVar(void))
MAKE_HEADER(void __stdcall _GetpWin16Lock(CRITICAL_SECTION** cs))
MAKE_HEADER(void __stdcall _CheckNotSysLevel(CRITICAL_SECTION* cs))
MAKE_HEADER(DWORD __stdcall _ConfirmSysLevel(CRITICAL_SECTION* cs))
MAKE_HEADER(DWORD __stdcall _ConfirmWin16Lock(void))
MAKE_HEADER(void __stdcall _EnterSysLevel(CRITICAL_SECTION* cs))
MAKE_HEADER(void __stdcall _ReleaseThunkLock(DWORD *pMutexCount))
MAKE_HEADER(void __stdcall _RestoreThunkLock(DWORD MutexCount))
MAKE_HEADER(void __stdcall _LeaveSysLevel(CRITICAL_SECTION* cs))
MAKE_HEADER(void __stdcall _LeaveSysLevel_NoThk(CRITICAL_SECTION* cs))

/* Critical section functions */
MAKE_HEADER(void __stdcall MakeCriticalSectionGlobal(CRITICAL_SECTION* cs))
MAKE_HEADER(HANDLE __stdcall ConvertToGlobalHandle(HANDLE hObject))

/* System functions */
MAKE_HEADER(BOOL __stdcall CloseSystemHandle(HANDLE hObject))
MAKE_HEADER(HANDLE __stdcall CreateKernelThread(LPSECURITY_ATTRIBUTES lpThreadAttributes, SIZE_T dwStackSize, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, DWORD dwCreationFlags, LPDWORD lpThreadId))
MAKE_HEADER(BOOL __stdcall RegisterServiceProcess(DWORD dwProcessId, BOOL fRegister))
MAKE_HEADER(DWORD __stdcall GetProcessDword(DWORD dwProcessId, INT IdOffset))

MAKE_HEADER(ULONG __stdcall CommonUnimpStub())

/* Global16 functions */
MAKE_HEADER(WORD __stdcall GlobalAlloc16(UINT uFlags, SIZE_T dwBytes))
MAKE_HEADER(LPVOID __stdcall GlobalLock16(HGLOBAL hMem))
MAKE_HEADER(BOOL __stdcall GlobalUnlock16(HGLOBAL hMem))
MAKE_HEADER(WORD __stdcall GlobalFix16(HGLOBAL hMem))
MAKE_HEADER(WORD __stdcall GlobalUnfix16(HGLOBAL hMem))
MAKE_HEADER(BOOL __stdcall GlobalWire16(HGLOBAL hMem))
MAKE_HEADER(BOOL __stdcall GlobalUnWire16(HGLOBAL hMem))
MAKE_HEADER(HGLOBAL __stdcall GlobalFree16(HGLOBAL hMem))
MAKE_HEADER(SIZE_T __stdcall GlobalSize16(HGLOBAL hMem))

MAKE_HEADER(void __stdcall HouseCleanLogicallyDeadHandles(void))

/* WOW functions*/
MAKE_HEADER(DWORD __stdcall K32WOWCallback16(DWORD vpfn16, DWORD dwParam))
MAKE_HEADER(BOOL __stdcall K32WOWCallback16Ex(DWORD vpfn16, DWORD dwFlags, DWORD cbArgs, LPVOID pArgs, LPDWORD pdwRetCode))
MAKE_HEADER(LPVOID __stdcall K32WOWGetVDMPointer(DWORD vpfn1616, DWORD dwBytes, BOOL fProtectedMode))
MAKE_HEADER(LPVOID __stdcall K32WOWGetVDMPointerFix(DWORD vpfn1616, DWORD dwBytes, BOOL fProtectedMode))
MAKE_HEADER(void __stdcall K32WOWGetVDMPointerUnfix(DWORD vpfn1616))
MAKE_HEADER(HANDLE __stdcall K32WOWHandle32(WORD hHandle16, WOW_HANDLE_TYPE wType))
MAKE_HEADER(WORD __stdcall K32WOWHandle16(WORD hHandle32, WOW_HANDLE_TYPE wType))
MAKE_HEADER(WORD __stdcall K32WOWGlobalAlloc16(WORD wFlags, SIZE_T dwBytes))
MAKE_HEADER(BOOL __stdcall K32WOWGlobalLock16(WORD hMem))
MAKE_HEADER(BOOL __stdcall K32WOWGlobalUnlock16(WORD hMem))
MAKE_HEADER(WORD __stdcall K32WOWGlobalFree16(WORD hMem))
MAKE_HEADER(DWORD __stdcall K32WOWGlobalAllocLock16(WORD wFlags, SIZE_T dwBytes, WORD *phMem))
MAKE_HEADER(WORD __stdcall K32WOWGlobalUnlockFree16(DWORD vpMem))
MAKE_HEADER(DWORD __stdcall K32WOWGlobalLockSize16(WORD hMem, PDWORD pcb))
MAKE_HEADER(VOID __stdcall K32WOWYield16(void))
MAKE_HEADER(VOID __stdcall K32WOWDirectedYield16(WORD hTask16))
MAKE_HEADER(BOOL __stdcall K32WOWGetDescriptor(DWORD segPtr, LPLDT_ENTRY ldtent))

MAKE_HEADER(BOOL __stdcall IsThreadId(DWORD dwThreadId))

/* Internal kernel32 runtime functions */
MAKE_HEADER(LONGLONG __stdcall K32RtlLargeIntegerAdd(LONGLONG Addend1, LONGLONG Addend2))
MAKE_HEADER(LONGLONG __stdcall K32RtlEnlargedIntegerMultiply(LONG Multiplicand, LONG Multiplier))
MAKE_HEADER(LONGLONG __stdcall K32RtlEnlargedUnsignedMultiply(ULONG Multiplicand, ULONG Multiplier))
MAKE_HEADER(ULONG __stdcall K32RtlEnlargedUnsignedDivide(ULONGLONG Dividend, ULONG Divisor, PULONG Remainder))
MAKE_HEADER(ULONG __stdcall K32RtlExtendedLargeIntegerDivide(LONGLONG Dividend, ULONG Divisor, PULONG Remainder))
MAKE_HEADER(LONGLONG __stdcall K32RtlExtendedMagicDivide(LONGLONG Dividend, LONGLONG MagicDivisor, CCHAR ShiftCount))
MAKE_HEADER(LONGLONG __stdcall K32RtlExtendedIntegerMultiply(LONGLONG Multiplicand, LONG Multiplier))
MAKE_HEADER(LONGLONG __stdcall K32RtlLargeIntegerShiftLeft(LONGLONG LargeInteger, CCHAR ShiftCount))
MAKE_HEADER(LONGLONG __stdcall K32RtlLargeIntegerShiftRight(LONGLONG LargeInteger, CCHAR ShiftCount))
MAKE_HEADER(LONGLONG __stdcall K32RtlLargeIntegerArithmeticShift(LONGLONG LargeInteger, CCHAR ShiftCount))
MAKE_HEADER(LONGLONG __stdcall K32RtlLargeIntegerNegate(LONGLONG Subtrahend))
MAKE_HEADER(LONGLONG __stdcall K32RtlLargeIntegerSubtract(LONGLONG Minuend, LONGLONG Subtrahend))
MAKE_HEADER(LONGLONG __stdcall K32RtlConvertLongToLargeInteger(LONG SignedInteger))
MAKE_HEADER(LONGLONG __stdcall K32RtlConvertUlongToLargeInteger(ULONG UnsignedInteger))

/* 16/32-bit (module) mapping */
MAKE_HEADER(WORD __stdcall MapHModuleLS(HMODULE hModule))
MAKE_HEADER(DWORD __stdcall MapSL(DWORD addr16))
MAKE_HEADER(DWORD __stdcall MapLS(DWORD addr32))

/* Library functions */
MAKE_HEADER(DWORD __stdcall LoadLibrary16(LPSTR libname))
MAKE_HEADER(DWORD __stdcall FreeLibrary16(HANDLE hModule))
MAKE_HEADER(DWORD __stdcall GetProcAddress16(HANDLE hModule, LPSTR procname))

/* Thunk functions */
MAKE_HEADER(void __stdcall FT_Thunk())
MAKE_HEADER(void __stdcall  QT_Thunk())
MAKE_HEADER(void __stdcall FT_PrologPrime())
MAKE_HEADER(void __stdcall QT_ThunkPrime())
MAKE_HEADER(ULONG __stdcall K32_RtlNtStatusToDosError(LONG status))
MAKE_HEADER(BOOL __stdcall ThunkConnect32(PVOID pThunkData, LPSTR lpThunkName, LPSTR lpModule16, LPSTR lpModule32, HINSTANCE hInstance, DWORD dwReason))

MAKE_HEADER(void __stdcall GDIReallyCares(HMODULE hModule))

#ifdef __cplusplus
}
#endif

#endif
