/*
 *  KernelEx
 *
 *  Copyright (C) 2009-2010, Tihiy
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
#include "thuni_macro.h"
#include "thuni_thunk.h"
#include "thuni_layer.h"

static DWORD g_LastInputTime;

__declspec(naked)
/* MAKE_EXPORT CallWindowProcA_fix=CallWindowProcA */
LRESULT WINAPI CallWindowProcA_fix(WNDPROC lpPrevWndFunc, HWND hWnd, 
		UINT Msg, WPARAM wParam, LPARAM lParam)
{
	/* We shouldn't write it in C because some weird programs depend 
	 * on CallWindowProc calling function directly! */
__asm {
	mov ecx, [esp+4] ;lpPrevWndFunc
	jecxz FAIL
	jmp dword ptr [CallWindowProcA]
FAIL:
	xor eax,eax
	ret 20
	}
}


/* MAKE_EXPORT CallWindowProcW_new=CallWindowProcW */
LRESULT WINAPI CallWindowProcW_new( WNDPROC lpPrevWndFunc, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	if ( !lpPrevWndFunc ) return 0L;
	THUNKPROC testthunk = (THUNKPROC)lpPrevWndFunc;
	if ( testthunk->sign == wtoa_code && IsValidThunk(testthunk) )
		lpPrevWndFunc = testthunk->proc;
	return CallWindowProc_stdcall( lpPrevWndFunc, hWnd, Msg, wParam, lParam );
}

/* BORING default Unicode window procedures */

/* MAKE_EXPORT DefDlgProcW_NEW=DefDlgProcW */
LRESULT WINAPI DefDlgProcW_NEW( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	return CallProcAnsiWithUnicode( DefDlgProcA, hWnd, Msg, wParam, lParam );
}

/* MAKE_EXPORT DefMDIChildProcW_NEW=DefMDIChildProcW */
LRESULT WINAPI DefMDIChildProcW_NEW( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	return CallProcAnsiWithUnicode( DefMDIChildProcA, hWnd, Msg, wParam, lParam );
}

/* those APIs won't need conversion for any but WM_CHAR msg */
/* MAKE_EXPORT PostMessageW_NEW=PostMessageW */
LRESULT WINAPI PostMessageW_NEW( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	return CallProcAnsiWithUnicode( (WNDPROC)PostMessageA, hWnd, Msg, wParam, lParam );
}

/* MAKE_EXPORT SendNotifyMessageW_NEW=SendNotifyMessageW */
LRESULT WINAPI SendNotifyMessageW_NEW( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	return CallProcAnsiWithUnicode( (WNDPROC)SendNotifyMessageA, hWnd, Msg, wParam, lParam );
}

/* MAKE_EXPORT PostThreadMessageW_NEW=PostThreadMessageW */
LRESULT WINAPI PostThreadMessageW_NEW( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	//BUGBUG removeme
	return CallProcAnsiWithUnicode( (WNDPROC)PostThreadMessageA, hWnd, Msg, wParam, lParam );
}

/* MAKE_EXPORT DefFrameProcW_NEW=DefFrameProcW */
LRESULT WINAPI DefFrameProcW_NEW( HWND hWnd, HWND hWndMDIClient, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg) {
	case WM_SETTEXT:
	{
		LPSTR textA;
		STACK_WtoA( lParam, textA );
		return DefFrameProcA( hWnd, hWndMDIClient, uMsg, wParam, (LPARAM)textA );
	}
	case WM_NCACTIVATE:
	case WM_COMMAND:
	case WM_SYSCOMMAND:
	case WM_SIZE:
	case WM_SETFOCUS:
	case WM_NEXTMENU:
	case WM_MENUCHAR:
		return DefFrameProcA( hWnd, hWndMDIClient, uMsg, wParam, lParam );
	default:
		return DefWindowProcW_NEW( hWnd, uMsg, wParam, lParam );
	}
}

/* MAKE_EXPORT GetMessageA_NEW=GetMessageA */
BOOL WINAPI GetMessageA_NEW( LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax)
{
	BOOL ret;
	if ( !lpMsg ) return -1;		//9x fails to check that
	ret = GetMessageA( lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax );
	if ( ret && ((lpMsg->message>=WM_KEYFIRST && lpMsg->message<=WM_KEYLAST)||(lpMsg->message>=WM_MOUSEFIRST && lpMsg->message<=WM_MOUSELAST)) )
	{
		g_LastInputTime = GetTickCount();
		UpdateLRKeyState(lpMsg);
	}
	return ret;
}

/* MAKE_EXPORT PeekMessageA_NEW=PeekMessageA */
BOOL WINAPI PeekMessageA_NEW( LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg)
{
	BOOL ret;
	if ( !lpMsg ) return FALSE;		//9x fails to check that
	ret = PeekMessageA( lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg );
	if ( ret && ((lpMsg->message>=WM_KEYFIRST && lpMsg->message<=WM_KEYLAST)||(lpMsg->message>=WM_MOUSEFIRST && lpMsg->message<=WM_MOUSELAST)) )
	{
		g_LastInputTime = GetTickCount();
		UpdateLRKeyState(lpMsg);
	}
	return ret;	
}

/* MAKE_EXPORT GetMessageW_NEW=GetMessageW */
BOOL WINAPI GetMessageW_NEW( LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax)
{
	BOOL ret = GetMessageA_NEW( lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax );
	if ( ret == 1 && lpMsg->message == WM_CHAR && IsWindowReallyUnicode(lpMsg->hwnd) )
	{
		lpMsg->wParam = wparam_AtoW( NULL, lpMsg->message, lpMsg->wParam, FALSE );
	}
	return ret;
}

/* MAKE_EXPORT PeekMessageW_NEW=PeekMessageW */
BOOL WINAPI PeekMessageW_NEW( LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg)
{
	BOOL ret = PeekMessageA_NEW( lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg );
	if ( ret == 1 && lpMsg->message == WM_CHAR && IsWindowReallyUnicode(lpMsg->hwnd) )
		lpMsg->wParam = wparam_AtoW( NULL, lpMsg->message, lpMsg->wParam, FALSE );
	return ret;
}

/* MAKE_EXPORT GetLastInputInfo_NEW=GetLastInputInfo */
BOOL WINAPI GetLastInputInfo_NEW( PLASTINPUTINFO plii )
{
	if ( !plii ) return FALSE;
	plii->dwTime = g_LastInputTime;
	return TRUE;
}

/* MAKE_EXPORT ToUnicodeEx_new=ToUnicodeEx */
int WINAPI ToUnicodeEx_new(
	UINT wVirtKey,
	UINT wScanCode,
	const PBYTE lpKeyState,
	LPWSTR pwszBuff,
	int cchBuff,
	UINT wFlags,
	HKL dwhkl
)
{
	int result;
	WORD asciibuf[4];
	char ansibuf[4];
	if (!lpKeyState || !pwszBuff)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}
	result = ToAsciiEx(wVirtKey,wScanCode,lpKeyState,asciibuf,wFlags,dwhkl);
	if (result && cchBuff)	
	{
		int i;
		for (i = 0; i < result; i++) ansibuf[i]=(char)asciibuf[i];
		MultiByteToWideChar(GetCPFromLocale((LCID)dwhkl),0,ansibuf,result,pwszBuff,cchBuff);
	}
	return result;
}

/* MAKE_EXPORT ToUnicode_new=ToUnicode */
int WINAPI ToUnicode_new(
	UINT wVirtKey,
	UINT wScanCode,
	const PBYTE lpKeyState,
	LPWSTR pwszBuff,
	int cchBuff,
	UINT wFlags
)
{	
	return ToUnicodeEx_new(wVirtKey,wScanCode,lpKeyState,pwszBuff,cchBuff,wFlags,GetKeyboardLayout(GetCurrentThreadId()));
}
