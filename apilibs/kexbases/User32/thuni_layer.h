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

#define REBASEUSER(x) ((x) != 0 ? (DWORD)gSharedInfo + (DWORD)(x) : 0)
#define USER32FAR16(x) ((x) != 0 ? (DWORD)(x) - (DWORD)gSharedInfo : 0)

#define REBASEPWND(pwnd) ((PWND)REBASEUSER(pwnd))

#define ISOURPROCESSHWND(hwnd) ( GetWindowProcessId(hwnd) == GetCurrentProcessId() )
#define IS_SHARED(x) (((DWORD)x) & 0x80000000)

#define WF_M_DIALOG					0x00000002
#define WF_WMPAINTSENT				0x00000020

#define MOREFLAGS_HASDATA			0x0080
#define MOREFLAGS_SCREENSAVER		0x0800

#define WF_EX_PAINTNOTPROCESSED		0x00020000
#define WF_EX_WIN32					0x02000000
#define WS_EX_UNICODE				0x80000000

/* Custom flags */
#define WS_INTERNAL_HUNGWASVISIBLE	0x00002000 // Reserved for ghost windows
#define WS_INTERNAL_MUSTPAINT		0x00004000 // currently unused
#define WS_INTERNAL_WASVISIBLE		0x00008000 // Reseved flag for desktop switching
#define WS_EX_INTERNAL_WASTOPMOST	0x80000000
 
#define IS32BITWIN(pwnd) (pwnd->dwFlags & WF_EX_WIN32)
#define ISDIALOG(pwnd) (pwnd->moreFlags & WF_M_DIALOG)

#define SetWinCreateEvent(proc) SetWinEventHook(EVENT_OBJECT_CREATE, EVENT_OBJECT_CREATE, g_hUser32, \
				(WINEVENTPROC)(proc), GetCurrentProcessId(), GetCurrentThreadId(), WINEVENT_INCONTEXT)

/* GetFreeSystemResources flags */
#define GFSR_SYSTEMRESOURCES   0x0000
#define GFSR_GDIRESOURCES      0x0001
#define GFSR_USERRESOURCES     0x0002

/* UserSeeUserDo flags */
#define USUD_ALLOC_DGROUP			1	// Allocate in the USER's heap
#define USUD_FREE_DGROUP			2	// Free in the USER's heap
#define USUD_COMPACT_DGROUP			3	// Compact the USER's heap
#define USUD_GET_MENUHEAP			4	// Get the menu heap handle
#define USUD_GET_PCLSLIST			5	// Get the system class list
#define USUD_GET_DGROUP				6	// Get the user's DGROUP (gSharedInfo)
#define USUD_GET_POCEFIRST			8	// Get the first device context entry
#define USUD_GET_PWNDDESKTOP		9	// Return the desktop PWND
#define USUD_ALLOC_WINDOWMENUHEAP	10	// If param1 is TRUE, allocate in the menu heap, otherwise allocate in the window heap
#define USUD_FREE_WINDOWMENUHEAP	11	// If param1 is TRUE, free memory in the menu heap, otherwise free memory in the window heap
#define USUD_UNUSED1				7	// USUD_UNUSEDx always return 0xFFFF
#define USUD_UNUSED2				12
#define USUD_UNUSED3				13
#define USUD_UNKNOWN1				14
#define USUD_UNUSED4				15
#define USUD_UNKNOWN2				16

/* Undocumented ExitWindowsEx flags */
#define EWX_FASTRESTART			0x00000040

/* The hwnd9x header will need some functions from the thuni layer */
#include "hwnd9x.h"

//stuff
extern LPCRITICAL_SECTION pWin16Mutex;
extern PUSERDGROUP gSharedInfo;
extern HMODULE g_hUser32;
extern HMODULE g_hUser16;
extern HMODULE g_hKernel16;
BOOL InitUniThunkLayerStuff();
void GrabWin16Lock();
void ReleaseWin16Lock();

#define GETWNDCLASS(hwnd) (HWNDtoPWND(hwnd)->wndClass)
#define GETWNDCLASSPTR(hwnd) (PUSER_DGROUP_WNDCLASS)REBASEUSER(GETWNDCLASS(hwnd))

DWORD GetWindowProcessId( HWND hwnd );
LRESULT WINAPI CallWindowProc_stdcall( WNDPROC lpPrevWndFunc, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
int GetCPFromLocale(LCID Locale);
UINT GetCurrentKeyboardCP();
PMSGQUEUE GetCurrentThreadQueue();
void UpdateLRKeyState(LPMSG msg);
WNDPROC WINAPI GetWindowProc32(PWND pwnd);
PMSGQUEUE GetWindowQueue(PWND pwnd);

//windows
BOOL __fastcall EnumWindowsEx(DWORD dwThreadId, WNDENUMPROC lpEnumFunc, LPARAM lParam, BOOL fEnumThread, HDESK hDesktop, BOOL fEnumDesktop);
BOOL __fastcall IntCompleteRedrawWindow(PWND pWnd);

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
UINT GetFreeSystemResources(UINT uFlags);

//user
UINT UserSeeUserDo(WORD wAction, WORD wParam1, WORD wParam2, WORD wParam3);

//unexported remade functions
BOOL WINAPI GetClientRect_source(HWND hWnd, LPRECT lpRect);
LONG WINAPI GetDialogBaseUnits_source(VOID);
BOOL WINAPI GetWindowRect_source(HWND hWnd, LPRECT lpRect);
BOOL WINAPI OffsetRect_source(LPRECT lprc, int dx, int dy);

#ifdef __cplusplus
extern "C"
#endif
__declspec(dllexport)
void SetWindowUnicode(HWND hWnd, BOOL bUnicode);
