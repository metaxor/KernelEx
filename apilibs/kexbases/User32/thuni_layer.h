/*
 *  KernelEx
 *
 *  Copyright (C) 2010, Tihiy
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

#pragma once

#include "k32ord.h"

static const char c_szDBCSProp[]="kexDBCS";

#define REBASEUSER(x) ((x) != 0 ? g_UserBase + (DWORD)(x) : 0)
#define USER32FAR16(x) ((x) != 0 ? (DWORD)(x) - g_UserBase : 0)

#define ISOURPROCESSHWND(hwnd) ( GetWindowProcessId(hwnd) == GetCurrentProcessId() )
#define IS_SHARED(x) (((DWORD)x) & 0x80000000)

#define WS_EX_UNICODE 0x80000000
#define WF_EX_WIN32 0x02000000
#define WF_M_DIALOG	0x2

/* Custom flags */
#define WS_INTERNAL_MUSTPAINT	0x00004000
#define WS_INTERNAL_WASVISIBLE  0x00008000 // Reseved flag for desktop switching
 
#define IS32BITWIN(pwnd) (pwnd->dwFlags & WF_EX_WIN32)
#define ISDIALOG(pwnd) (pwnd->moreFlags & WF_M_DIALOG)

#define SetWinCreateEvent(proc) SetWinEventHook(EVENT_OBJECT_CREATE, EVENT_OBJECT_CREATE, g_hUser32, \
				(WINEVENTPROC)(proc), GetCurrentProcessId(), GetCurrentThreadId(), WINEVENT_INCONTEXT)

#define GFSR_SYSTEMRESOURCES   0x0000
#define GFSR_GDIRESOURCES      0x0001
#define GFSR_USERRESOURCES     0x0002

//stuff
extern LPCRITICAL_SECTION pWin16Mutex;
extern DWORD g_UserBase;
extern HMODULE g_hUser32;
BOOL InitUniThunkLayerStuff();
void GrabWin16Lock();
void ReleaseWin16Lock();

/* The hwnd9x header will need some functions from the thuni layer */
#include "hwnd9x.h"

DWORD GetWindowProcessId( HWND hwnd );
LRESULT WINAPI CallWindowProc_stdcall( WNDPROC lpPrevWndFunc, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
int GetCPFromLocale(LCID Locale);
UINT GetCurrentKeyboardCP();
PMSGQUEUE GetCurrentThreadQueue();
void UpdateLRKeyState(LPMSG msg);
WNDPROC WINAPI GetWindowProc32(PWND pwnd);
PMSGQUEUE GetWindowQueue(PWND pwnd);

BOOL __fastcall EnumWindowsEx(DWORD dwThreadId, WNDENUMPROC lpEnumFunc, LPARAM lParam, BOOL fEnumThread, HDESK hDesktop, BOOL fEnumDesktop);

//conv
WPARAM wparam_AtoW( HWND hwnd, UINT message, WPARAM wParam, BOOL messDBCS );
WPARAM wparam_WtoA( UINT message, WPARAM wParam );
LRESULT WINAPI CallProcUnicodeWithAnsi( WNDPROC callback, HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI CallProcAnsiWithUnicode( WNDPROC callback, HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

//proc
LRESULT WINAPI DefWindowProcW_NEW( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI DefDlgProcW_NEW( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI DefMDIChildProcW_NEW( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

//layer
BOOL IsWindowReallyUnicode(HWND hwnd);
BOOL WINAPI IsWindowUnicode_NEW( HWND hWnd );
BOOL InitUniThunkLayer();

//sendmessage_fix
LRESULT WINAPI SendMessageA_fix(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

//resources
UINT __fastcall GetFreeSystemResources(UINT uFlags);

#ifdef __cplusplus
extern "C"
#endif
__declspec(dllexport)
void SetWindowUnicode(HWND hWnd, BOOL bUnicode);
