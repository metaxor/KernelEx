/*
 *  KernelEx Thunking Unicode Layer
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
#include <commctrl.h>
#include "thuni_thunk.h"
#include "thuni_layer.h"
#include "thuni_macro.h"
#include "_user32_apilist.h"

static CRITICAL_SECTION wndproc_cs;

BOOL InitUniThunkLayer()
{		
	InitializeCriticalSection( &wndproc_cs );
	MakeCriticalSectionGlobal( &wndproc_cs );

	return InitUniThunkLayerStuff();
}

WNDPROC WINAPI GetWindowProc32(PWND pwnd)
{
	if ( !pwnd ) return NULL;
	if ( !IS32BITWIN(pwnd) ) return NULL;
	PTHUNK16 proc32 = (PTHUNK16)MapSL( (DWORD)pwnd->lpfnWndProc );
	if ( !proc32 ) return NULL;
	if ( proc32->push1 != pushl16_code ) return NULL; //NOT 16-bit pushl?
	return proc32->proc;
}

PMSGQUEUE GetWindowQueue(PWND pwnd)
{
	if(IsBadReadPtr(pwnd, sizeof(WND)))
		return NULL;

	return pwnd ? (PMSGQUEUE)MapSL( (DWORD)pwnd->hQueue << 16 ) : NULL;
}

static void _SetWindowUnicode(PWND pwnd, BOOL bUnicode)
{
	if ( pwnd )
	{
		if (bUnicode)
			pwnd->ExStyle |= WS_EX_UNICODE;
		else
			pwnd->ExStyle &= ~WS_EX_UNICODE;
	}
}

BOOL IsWindowReallyUnicode(HWND hwnd)
{
	/* window is really unicode if it has unicode procedure */
	PWND pwnd = HWNDtoPWND(hwnd);
	if ( pwnd && IS32BITWIN(pwnd) )
	{
		THUNKPROC proc = (THUNKPROC)GetWindowProc32( pwnd );
		if ( proc && proc->sign == wtoa_code && IsValidThunk(proc) ) return TRUE;			
	}
	return FALSE;
}

#ifdef __cplusplus
extern "C"
#endif
__declspec(dllexport)
void SetWindowUnicode(HWND hWnd, BOOL bUnicode)
{
	GrabWin16Lock();
	_SetWindowUnicode( HWNDtoPWND(hWnd), bUnicode );
	ReleaseWin16Lock();
}

/* MAKE_EXPORT GetWindowLongA_NEW=GetWindowLongA */
LONG WINAPI GetWindowLongA_NEW(HWND hWnd, int nIndex)
{
	LONG ret = GetWindowLongA( hWnd, nIndex );
	if (nIndex == GWL_EXSTYLE) ret = ret & ~WS_EX_UNICODE; //mask out our flag
	return ret;
}

/* MAKE_EXPORT IsWindowUnicode_NEW=IsWindowUnicode */
BOOL WINAPI IsWindowUnicode_NEW( HWND hWnd )
{
	return GetWindowLongA(hWnd, GWL_EXSTYLE) & WS_EX_UNICODE ? TRUE : FALSE;
}

/* MAKE_EXPORT SetWindowLongA_NEW=SetWindowLongA */
LONG WINAPI SetWindowLongA_NEW(HWND hWnd, int nIndex, LONG dwNewLong)
{
	BOOL WasUnicode = FALSE;
	LONG ret;
	if ( nIndex == GWL_EXSTYLE )
	{
		WasUnicode = IsWindowUnicode_NEW( hWnd );
		dwNewLong &= ~WS_EX_UNICODE; //don't let reset nor see unicode flag!!!
	}
	ret = SetWindowLongA( hWnd, nIndex, dwNewLong );
	if ( WasUnicode ) 
		SetWindowUnicode( hWnd, TRUE ); //restore unicode flag if someone changed ExStyle
	if ( nIndex == GWL_WNDPROC && ret )	
		SetWindowUnicode( hWnd, FALSE ); //remove 'Unicode' flag
	return ret;
}



/**********************************************************************
* allocate a thunk, AtoW / WtoA
*/
static WNDPROC AllocWndProc( PROCESS_THUNKS thunks, WNDPROC oldproc, BOOL AtoW )
{	
	int thunk_number = thunks->count;
	if (thunk_number == CHUNK_THUNKS) //if chunk is full, grow chunk list (thunks points to last chunk)
	{		
		thunks = GrowProcessThunks(thunks);
		DBGPRINTF(("Thunks array has grown (new chunk %p)\n",thunks));
		if (!thunks) return oldproc;
		thunk_number = 0;
	}		
	DWORD thunkfunc = AtoW ? (DWORD)CallProcAnsiWithUnicode : (DWORD)CallProcUnicodeWithAnsi;	 //logic is inverted!!! AtoW=Unicode2Ansi, vice versa
	thunks->array[thunk_number].sign = AtoW ? atow_code : wtoa_code;
	thunks->array[thunk_number].pop_eax = pop_eax_code;
	thunks->array[thunk_number].push_func = push_func_code;
	thunks->array[thunk_number].proc = oldproc;
	thunks->array[thunk_number].push_eax = push_eax_code;
	thunks->array[thunk_number].jmp_func = jmp_func_code;
	thunks->array[thunk_number].relay_offset = thunkfunc - (DWORD)&thunks->array[thunk_number].relay_offset - sizeof(DWORD);
	thunks->count++;
	
	return (WNDPROC)&thunks->array[thunk_number];
}

/* Window procedure conversions */

/*	ConvertWndProc W<->A are using per-process
 * 	growable array of executable code thunks.
 *	The rules are simple:
 *  WToA(procW) = procA, AToW(procA) = procW
 *	WToA(procA) or AToW(procW) are invalid, 
 *	but we fix this case too.
 */

static WNDPROC ConvertWndProcWToA( WNDPROC ProcW )
{
	int i;
	WNDPROC ret = NULL;
	THUNKPROC thunk = (THUNKPROC)ProcW;
	if ( !HIWORD(ProcW) ) return ProcW;
	if ( thunk->sign == atow_code && IsValidThunk(thunk) ) return thunk->proc;
	if ( thunk->sign == wtoa_code && IsValidThunk(thunk) ) return ProcW; //invalid
	if ( ProcW == DefWindowProcW_NEW ) return DefWindowProcA;
	if ( ProcW == DefDlgProcW_NEW ) return DefDlgProcA;
	if ( ProcW == DefMDIChildProcW_NEW ) return DefMDIChildProcA;
	EnterCriticalSection( &wndproc_cs );
	PROCESS_THUNKS procthunks = GetProcessThunks();
	while (true)
	{
		for ( i=0; i<procthunks->count; i++)
		{	
			if ( procthunks->array[i].proc == ProcW && procthunks->array[i].sign == wtoa_code )
			{
				//we have an WtoA thunk already
				ret = (WNDPROC)&procthunks->array[i];
				break;
			}
		}		
		if (ret || !procthunks->nextChunk) break;
		procthunks = procthunks->nextChunk;
	}
	if ( !ret ) ret = AllocWndProc( procthunks, ProcW, FALSE );
	LeaveCriticalSection( &wndproc_cs );
	return ret;
}

static WNDPROC ConvertWndProcAToW( WNDPROC ProcA )
{
	int i;
	WNDPROC ret = NULL;
	THUNKPROC thunk = (THUNKPROC)ProcA;	
	if ( !HIWORD(ProcA) ) return ProcA;
	if ( thunk->sign == wtoa_code && IsValidThunk(thunk) ) return thunk->proc;
	if ( thunk->sign == atow_code && IsValidThunk(thunk) ) return ProcA; //invalid
	if ( ProcA == DefWindowProcA ) return DefWindowProcW_NEW;
	if ( ProcA == DefDlgProcA ) return DefDlgProcW_NEW;
	if ( ProcA == DefMDIChildProcA ) return DefMDIChildProcW_NEW;
	EnterCriticalSection( &wndproc_cs );	
	PROCESS_THUNKS procthunks = GetProcessThunks();
	while (true)
	{
		for ( i=0; i<procthunks->count; i++)
		{	
			if ( procthunks->array[i].proc == ProcA && procthunks->array[i].sign == atow_code )
			{
				//we have an WtoA thunk already
				ret = (WNDPROC)&procthunks->array[i];
				break;
			}
		}
		if (ret || !procthunks->nextChunk) break;
		procthunks = procthunks->nextChunk;
	}
	if ( !ret ) ret = AllocWndProc( procthunks, ProcA, TRUE );
	LeaveCriticalSection( &wndproc_cs );
	return ret;
}

/* MAKE_EXPORT GetWindowLongW_NEW=GetWindowLongW */
LONG WINAPI GetWindowLongW_NEW(HWND hWnd, int nIndex)
{
	LONG ret = GetWindowLongA_NEW(hWnd, nIndex);
	if ( nIndex == GWL_WNDPROC )
	{
		if ( !ISOURPROCESSHWND(hWnd) )
		{
			SetLastError(ERROR_ACCESS_DENIED);
			return 0L;
		}
		ret = (LONG)ConvertWndProcAToW( (WNDPROC)ret );
	}
	return ret;
}

/* MAKE_EXPORT SetWindowLongW_NEW=SetWindowLongW */
LONG WINAPI SetWindowLongW_NEW(HWND hWnd, int nIndex, LONG dwNewLong)
{
	LONG ret;
	if ( nIndex == GWL_WNDPROC )
	{		
		if ( !ISOURPROCESSHWND(hWnd) )
		{
			SetLastError(ERROR_ACCESS_DENIED);
			return 0L;
		}
		dwNewLong = (LONG)ConvertWndProcWToA( (WNDPROC)dwNewLong );
	}
			
	ret = SetWindowLongA_NEW( hWnd, nIndex, dwNewLong );
	
	if ( nIndex == GWL_WNDPROC && ret ) //oh, you're unicode subclassed!
	{
		SetWindowUnicode( hWnd, TRUE );
		ret = (LONG)ConvertWndProcAToW( (WNDPROC)ret );
	}
	return ret;
}

/* Class manipulation functions*/

/* MAKE_EXPORT GetClassLongW_NEW=GetClassLongW */
DWORD WINAPI GetClassLongW_NEW(HWND hWnd, int nIndex)
{
	DWORD ret = GetClassLongA(hWnd, nIndex);
	if ( nIndex == GCL_WNDPROC )
	{
		if ( !ISOURPROCESSHWND(hWnd) )
		{
			SetLastError(ERROR_ACCESS_DENIED);
			return 0L;
		}	
		ret = (DWORD)ConvertWndProcAToW( (WNDPROC)ret );
	}
	return ret;
}

/* MAKE_EXPORT SetClassLongW_NEW=SetClassLongW */
DWORD WINAPI SetClassLongW_NEW(HWND hWnd, int nIndex, LONG dwNewLong)
{
	DWORD ret;
	if ( nIndex == GCL_WNDPROC )
	{
		if ( !ISOURPROCESSHWND(hWnd) )
		{
			SetLastError(ERROR_ACCESS_DENIED);
			return 0L;
		}
		dwNewLong = (DWORD)ConvertWndProcWToA( (WNDPROC)dwNewLong );
	}
	ret = SetClassLongA( hWnd, nIndex, dwNewLong );
	if ( nIndex == GCL_WNDPROC && ret )
		ret = (DWORD)ConvertWndProcAToW( (WNDPROC)ret );	
	return ret;
}

/* MAKE_EXPORT RegisterClassExW_NEW=RegisterClassExW */
ATOM WINAPI RegisterClassExW_NEW( CONST WNDCLASSEXW *lpwcx )
{
	WNDCLASSEXA wnda;
	if ( !lpwcx || !HIWORD(lpwcx->lpfnWndProc) ) return FALSE;
	CopyMemory( &wnda, lpwcx, sizeof(WNDCLASSEXA));
	STACK_WtoA(lpwcx->lpszClassName, wnda.lpszClassName);
	STACK_WtoA(lpwcx->lpszMenuName, wnda.lpszMenuName);
	wnda.lpfnWndProc = ConvertWndProcWToA( wnda.lpfnWndProc );
	return RegisterClassExA(&wnda);
}

/* MAKE_EXPORT RegisterClassW_NEW=RegisterClassW */
ATOM WINAPI RegisterClassW_NEW( CONST WNDCLASSW *lpwcx )
{
	WNDCLASSEXW wndw;
	wndw.cbSize = sizeof(WNDCLASSEXW);
	wndw.style = lpwcx->style;
	wndw.lpfnWndProc = lpwcx->lpfnWndProc;
	wndw.cbClsExtra = lpwcx->cbClsExtra;
	wndw.cbWndExtra = lpwcx->cbWndExtra;
	wndw.hInstance = lpwcx->hInstance;
	wndw.hIcon = lpwcx->hIcon;	
	wndw.hCursor = lpwcx->hCursor;
	wndw.hbrBackground = lpwcx->hbrBackground;
	wndw.lpszMenuName = lpwcx->lpszMenuName;	
	wndw.lpszClassName = lpwcx->lpszClassName;	
	wndw.hIconSm = NULL;
	return RegisterClassExW_NEW( &wndw );
}

//Those two are both ATOMs
/* MAKE_EXPORT GetClassInfoExW_NEW=GetClassInfoExW */
ATOM WINAPI GetClassInfoExW_NEW(HINSTANCE hinst, LPCWSTR lpszClass, WNDCLASSEXW *lpwcx)
{
	LPSTR lpszClassA;
	STACK_WtoA(lpszClass, lpszClassA);
	ATOM ret = GetClassInfoExA( hinst, lpszClassA, (WNDCLASSEXA*)lpwcx );
	if ( ret )
	{
		lpwcx->lpfnWndProc = ConvertWndProcAToW( lpwcx->lpfnWndProc );
		//lpwcx->lpszClassName = lpszClass;
		/* BUGBUG this is not correct behavior. lpszClassName/lpszMenuName have to be 
		 * permanent pointers! Maybe it can be solved with static buffer like MSLU does.
		 */		
	}
	return ret;
}

/* MAKE_EXPORT GetClassInfoW_NEW=GetClassInfoW */
ATOM WINAPI GetClassInfoW_NEW(HINSTANCE hinst, LPCWSTR lpszClass, WNDCLASSW *wc)
{
	WNDCLASSEXW wcex;
	wcex.cbSize = sizeof(WNDCLASS);
	ATOM ret = GetClassInfoExW_NEW( hinst, lpszClass, &wcex );
	if (ret && wc)
	{
		wc->style         = wcex.style;
		wc->lpfnWndProc   = wcex.lpfnWndProc;
		wc->cbClsExtra    = wcex.cbClsExtra;
		wc->cbWndExtra    = wcex.cbWndExtra;
		wc->hInstance     = wcex.hInstance;
		wc->hIcon         = wcex.hIcon;
		wc->hCursor       = wcex.hCursor;
		wc->hbrBackground = wcex.hbrBackground;
		wc->lpszMenuName  = wcex.lpszMenuName;
		wc->lpszClassName = wcex.lpszClassName;
	}
	return ret;
}

/* Unicode flagging */
static void CALLBACK UnicodeEvent( HWINEVENTHOOK hWinEventHook, DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime )
{		
	if ( idObject != OBJID_WINDOW ) return;
	//NOTE: we don't grab Win16Lock!! we touch windows only belonging to current thread.
	BOOL isUnicode = FALSE;
	PWND pwnd = HWNDtoPWND(hwnd);
	if ( !pwnd ) return; //wtf
	if ( !IS32BITWIN(pwnd) )		
		isUnicode = TRUE; //16-bit window, will be Unicode
	else
	{
		THUNKPROC proc = (THUNKPROC)GetWindowProc32( pwnd );
		if ( IS_SHARED(proc) || (proc && proc->sign == wtoa_code && IsValidThunk(proc)) ) //shared control or Unicode thunk
			isUnicode = TRUE;
	}
	if ( isUnicode ) _SetWindowUnicode( pwnd, TRUE );
	if ( isUnicode ) SendMessage( hwnd, CCM_SETUNICODEFORMAT, TRUE, 0 );
}

/* Window creation functions */

/* MAKE_EXPORT CreateWindowExW_NEW=CreateWindowExW */
HWND WINAPI CreateWindowExW_NEW(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
	LPSTR lpClassNameA;
	LPSTR lpWindowNameA;
	HWINEVENTHOOK uniEvent;
	HWND ret;
	
	STACK_WtoA(lpClassName, lpClassNameA);
	STACK_WtoA(lpWindowName, lpWindowNameA);	
	uniEvent = SetWinCreateEvent(UnicodeEvent);
	ret = CreateWindowExA_fix(dwExStyle,lpClassNameA,lpWindowNameA,dwStyle,x,y,nWidth,nHeight,hWndParent,hMenu,hInstance,lpParam);
	UnhookWinEvent(uniEvent);
	return ret;
}

/* MAKE_EXPORT CreateWindowW_NEW=CreateWindowW */
HWND WINAPI CreateWindowW_NEW( LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
	return CreateWindowExW_NEW(0, lpClassName,lpWindowName,dwStyle,x,y,nWidth,nHeight,hWndParent,hMenu,hInstance,lpParam);
}

/* MAKE_EXPORT CreateMDIWindowW_NEW=CreateMDIWindowW */
HWND WINAPI CreateMDIWindowW_NEW( LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, 
	int X, int Y, int nWidth, int nHeight, HWND hWndParent, HINSTANCE hInstance, LPARAM lParam)
{
	LPSTR lpClassNameA;
	LPSTR lpWindowNameA;
	HWINEVENTHOOK uniEvent;
	HWND ret;
	
	STACK_WtoA(lpClassName, lpClassNameA);
	STACK_WtoA(lpWindowName, lpWindowNameA);	
	uniEvent = SetWinCreateEvent(UnicodeEvent);
	ret = CreateMDIWindowA(lpClassNameA,lpWindowNameA,dwStyle,X,Y,nWidth,nHeight,hWndParent,hInstance,lParam);
	UnhookWinEvent(uniEvent);
	return ret;
}

/* MAKE_EXPORT CreateDialogParamW_NEW=CreateDialogParamW */
HWND WINAPI CreateDialogParamW_NEW( HINSTANCE hInstance, LPCTSTR lpTemplateName, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam)
{
	LPSTR lpTemplateNameA;
	HWINEVENTHOOK uniEvent;
	HWND ret;
	
	STACK_WtoA(lpTemplateName, lpTemplateNameA);
	uniEvent = SetWinCreateEvent(UnicodeEvent);
	ret = CreateDialogParamA( hInstance, lpTemplateNameA, hWndParent, lpDialogFunc, dwInitParam );
	UnhookWinEvent(uniEvent);
	return ret;		
}

/* MAKE_EXPORT CreateDialogIndirectParamW_NEW=CreateDialogIndirectParamW */
HWND WINAPI CreateDialogIndirectParamW_NEW( HINSTANCE hInstance, LPCDLGTEMPLATE lpTemplate, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM lParamInit)
{
	HWINEVENTHOOK uniEvent;
	HWND ret;
	
	uniEvent = SetWinCreateEvent(UnicodeEvent);
	ret = CreateDialogIndirectParamA( hInstance, lpTemplate, hWndParent, lpDialogFunc, lParamInit );
	UnhookWinEvent(uniEvent);
	return ret;	
}

/* MAKE_EXPORT DialogBoxParamW_NEW=DialogBoxParamW */
INT_PTR WINAPI DialogBoxParamW_NEW( HINSTANCE hInstance,  LPCWSTR lpTemplateName, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam) 
{
	LPSTR lpTemplateNameA;	
	HWINEVENTHOOK uniEvent;
	INT_PTR ret;

	STACK_WtoA( lpTemplateName, lpTemplateNameA );
	uniEvent = SetWinCreateEvent(UnicodeEvent);
	ret = DialogBoxParamA( hInstance, lpTemplateNameA, hWndParent, lpDialogFunc, dwInitParam );
	UnhookWinEvent(uniEvent);
	return ret;	
}

/* MAKE_EXPORT DialogBoxIndirectParamW_NEW=DialogBoxIndirectParamW */
INT_PTR WINAPI DialogBoxIndirectParamW_NEW( HINSTANCE hInstance, LPCDLGTEMPLATE hDialogTemplate, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam)
{
	HWINEVENTHOOK uniEvent;
	INT_PTR ret;
	

	uniEvent = SetWinCreateEvent(UnicodeEvent);
	ret = DialogBoxIndirectParamA( hInstance, hDialogTemplate, hWndParent, lpDialogFunc, dwInitParam );
	UnhookWinEvent(uniEvent);
	return ret;
}


/* MAKE_EXPORT SendMessageW_NEW=SendMessageW */
LRESULT WINAPI SendMessageW_NEW( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	if ( hWnd && LOWORD(hWnd)!=0xFFFF ) //broadcasts won't work
	{
		/* If window is in the same thread, sent message directly
		 * BUGBUG: take global message filter (Spy++) into account somehow 
		 */
		WNDPROC procW = NULL;
		GrabWin16Lock();
		PWND pwnd = HWNDtoPWND(hWnd);		
		if ( pwnd )
		{
			PMSGQUEUE msgq = GetWindowQueue(pwnd);
			PMSGQUEUE ourmsgq = GetCurrentThreadQueue();
			if ( ourmsgq == msgq && !msgq->block1 && !msgq->block2 ) //hooray! we're in the same queue and can be 32-bit!
				procW = ConvertWndProcAToW( GetWindowProc32(pwnd) );
		}
		ReleaseWin16Lock();
		if ( procW )
		{
			//DBGPRINTF(("SendMessageW [DIRECT]: %p, %p, %p, %p\n",hWnd, Msg, wParam, lParam));			
			return CallWindowProc_stdcall( procW, hWnd, Msg, wParam, lParam );
		}
	}
	//DBGPRINTF(("SendMessageW [THUNK]: %p, %p, %p, %p\n",hWnd, Msg, wParam, lParam));
	return CallProcAnsiWithUnicode( (WNDPROC)SendMessageA_fix, hWnd, Msg, wParam, lParam );
}

/* MAKE_EXPORT DefWindowProcW_NEW=DefWindowProcW */
LRESULT WINAPI DefWindowProcW_NEW( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	return CallProcAnsiWithUnicode( DefWindowProcA, hWnd, Msg, wParam, lParam );
}

/* MAKE_EXPORT SetWindowTextW_NEW=SetWindowTextW */
BOOL WINAPI SetWindowTextW_NEW( HWND hWnd, LPCWSTR lpString)
{
	if ( !hWnd || !lpString ) return FALSE;
	if ( ISOURPROCESSHWND(hWnd) )
		return SendMessageW_NEW( hWnd, WM_SETTEXT, 0, (LPARAM)lpString );
	else
		return DefWindowProcW_NEW( hWnd, WM_SETTEXT, 0, (LPARAM)lpString );

}

/* MAKE_EXPORT SetDlgItemTextW_NEW=SetDlgItemTextW */
BOOL WINAPI SetDlgItemTextW_NEW( HWND hDlg, int nIDDlgItem, LPCWSTR lpString )
{
	return SetWindowTextW_NEW( GetDlgItem(hDlg, nIDDlgItem), lpString );
}

/* MAKE_EXPORT SendDlgItemMessageW_NEW=SendDlgItemMessageW */
LRESULT WINAPI SendDlgItemMessageW_NEW( HWND hDlg, int nIDDlgItem, UINT Msg, WPARAM wParam, LPARAM lParam )
{
	HWND hCtl = GetDlgItem(hDlg, nIDDlgItem);
	if ( hCtl )
		return SendMessageW_NEW( hCtl, Msg, wParam, lParam );
	return 0L;
}

/* MAKE_EXPORT GetWindowTextW_NEW=GetWindowTextW */
int WINAPI GetWindowTextW_NEW( HWND hWnd, LPWSTR lpString, int nMaxCount)
{
	if ( !lpString || nMaxCount<=0 ) return 0; //NT does not set error here
	if ( !IsWindow(hWnd) )
	{
		SetLastError(ERROR_INVALID_WINDOW_HANDLE);
		return 0;
	}
	if ( ISOURPROCESSHWND(hWnd) )
		return SendMessageW_NEW( hWnd, WM_GETTEXT, nMaxCount, (LPARAM)lpString );
	else
		return DefWindowProcW_NEW( hWnd, WM_GETTEXT, nMaxCount, (LPARAM)lpString );
}

/* MAKE_EXPORT GetDlgItemTextW_NEW=GetDlgItemTextW */
int WINAPI GetDlgItemTextW_NEW( HWND hDlg, int nIDDlgItem, LPWSTR lpString, int nMaxCount)
{
	hDlg = GetDlgItem(hDlg, nIDDlgItem);
	if (hDlg)
		return GetWindowTextW_NEW( hDlg , lpString, nMaxCount );
	//NULL in fail case
	*lpString = WCHAR(0);
	return 0;
}

/* MAKE_EXPORT GetWindowTextLengthW_NEW=GetWindowTextLengthW */
int WINAPI GetWindowTextLengthW_NEW( HWND hWnd )
{
	return GetWindowTextLengthA(hWnd); //BUGBUG looks like i care about DBCS?
}

/* MAKE_EXPORT UnregisterClassW_NEW=UnregisterClassW */
BOOL WINAPI UnregisterClassW_NEW( LPCWSTR lpClassName, HINSTANCE hInstance)
{
	LPCSTR lpClassNameA;
	STACK_WtoA(lpClassName, lpClassNameA);
	return UnregisterClassA( lpClassNameA, hInstance );
}

/* Those functions were born ugly */

/* MAKE_EXPORT SendMessageCallbackW_NEW=SendMessageCallbackW */
BOOL WINAPI SendMessageCallbackW_NEW( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam, SENDASYNCPROC lpCallBack, DWORD dwData)
{
	//TODO: add WM_CHAR and alike conversion?
	return SendMessageCallbackA( hWnd, Msg, wParam, lParam, lpCallBack, dwData);
}

LRESULT WINAPI SendMessageTimeoutW_Helper( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	PSMTIMEOUT_THUNK psm = (PSMTIMEOUT_THUNK)GetMessageExtraInfo();
	return SendMessageTimeoutA(hWnd,Msg,wParam,lParam,psm->fuFlags,psm->uTimeout,psm->lpdwResult);
}

/* MAKE_EXPORT SendMessageTimeoutW_NEW=SendMessageTimeoutW */
LRESULT WINAPI SendMessageTimeoutW_NEW( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam, UINT fuFlags, UINT uTimeout, LPDWORD lpdwResult)
{
	LONG saveExtraInfo = GetMessageExtraInfo();
	SMTIMEOUT_THUNK smt;
	smt.fuFlags = fuFlags;
	smt.lpdwResult = lpdwResult;
	smt.uTimeout = uTimeout;
	SetMessageExtraInfo((LONG)&smt);
	LRESULT ret = CallProcAnsiWithUnicode(SendMessageTimeoutW_Helper,hWnd,Msg,wParam,lParam);
	SetMessageExtraInfo(saveExtraInfo);
	return ret;
}

