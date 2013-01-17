#pragma once

#include "k32ord.h"
#include "hwnd9x.h"

static const char c_szDBCSProp[]="kexDBCS";

#define REBASEUSER(x) ((x) != 0 ? g_UserBase + (DWORD)(x) : 0)

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

//stuff
extern DWORD g_UserBase;
extern DWORD g_SharedInfo;
extern HMODULE g_hUser32;
BOOL InitUniThunkLayerStuff();
void GrabWin16Lock();
void ReleaseWin16Lock();
DWORD GetWindowProcessId( HWND hwnd );
LRESULT WINAPI CallWindowProc_stdcall( WNDPROC lpPrevWndFunc, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
int GetCPFromLocale(LCID Locale);
UINT GetCurrentKeyboardCP();
PMSGQUEUE GetCurrentThreadQueue();
void UpdateLRKeyState(LPMSG msg);

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


#ifdef __cplusplus
extern "C"
#endif
__declspec(dllexport)
void SetWindowUnicode(HWND hWnd, BOOL bUnicode);
