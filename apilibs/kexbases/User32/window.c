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

#include <windows.h>
#include <stdio.h>
#include "common.h"
#include "desktop.h"
#include "_user32_apilist.h"
#include "thuni_layer.h"

DWORD ConfirmShowCommand(HWND hWnd, int nCmdShow)
{
	PTHREADINFO pti = get_tdb()->Win32Thread;
	DWORD newCmdShow = nCmdShow;
	PWND pwnd;

	if(nCmdShow > SW_MAX)
		return -1;

	pwnd = HWNDtoPWND(hWnd);

	if(pwnd == NULL)
		return -1;

	/* If it's a child (button, textbox, ...) keep the show state */
	if(pwnd->style & WS_CHILD)
		return newCmdShow;

	if(nCmdShow == SW_HIDE)
		pwnd->style &= ~WS_INTERNAL_WASVISIBLE;

	if(pti->rpdesk != gpdeskInputDesktop && nCmdShow != SW_HIDE)
	{
		newCmdShow = SW_HIDE;
		pwnd->style |= WS_INTERNAL_WASVISIBLE;
	}

	return newCmdShow;
}

/* MAKE_EXPORT AllowSetForegroundWindow_98=AllowSetForegroundWindow */
BOOL WINAPI AllowSetForegroundWindow_98(DWORD procid)
{
	return TRUE;
}

/* MAKE_EXPORT AnyPopup_nothunk=AnyPopup */
BOOL WINAPI AnyPopup_nothunk(VOID)
{
	PWND pwnd = HWNDtoPWND(GetDesktopWindow());
	PWND pwndChild = HWNDtoPWND(GetDesktopWindow());
	BOOL fFound = FALSE;

	GrabWin16Lock();

	/* Enumerates child window in the desktop window */
	for(; pwndChild != NULL; pwndChild = (PWND)REBASEUSER(pwnd->spwndNext))
	{
		for(; pwnd != NULL; pwnd = (PWND)REBASEUSER(pwnd->spwndNext))
		{
			/* Skip if not visible */
			if(!(pwnd->style & WS_VISIBLE))
				continue;

			if((pwnd->style & WS_POPUP) || (pwnd->style & WS_OVERLAPPED))
			{
				/* Found a popup/overlapped window, return */
				fFound = TRUE;
				goto _ret;
			}
		}
	}

_ret:

	ReleaseWin16Lock();
	return fFound;
}

/* MAKE_EXPORT CreateMDIWindowA_fix=CreateMDIWindowA */
HWND WINAPI CreateMDIWindowA_fix(LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HINSTANCE hInstance, LPARAM lParam)
{
	DWORD newStyle = dwStyle;
	BOOL fWasVisible = FALSE;
	PTHREADINFO pti = get_tdb()->Win32Thread;
	HWND hwnd = NULL;
	PWND pwnd = NULL;

	if(newStyle & WS_VISIBLE)
	{
		if(pti->rpdesk != gpdeskInputDesktop)
		{
			newStyle &= ~WS_VISIBLE;
			fWasVisible = TRUE;
		}
	}

	hwnd = CreateMDIWindowA(lpClassName,
						lpWindowName,
						newStyle,
						X,
						Y,
						nWidth,
						nHeight,
						hWndParent,
						hInstance,
						lParam);

	if(hwnd == NULL)
		return NULL;

	pwnd = HWNDtoPWND(hwnd);

	if(fWasVisible)
		pwnd->style |= WS_INTERNAL_WASVISIBLE;

	return hwnd;
}

/* MAKE_EXPORT CreateWindowExA_fix=CreateWindowEx */
HWND WINAPI CreateWindowExA_fix(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
	DWORD newStyle = dwStyle;
	BOOL fWasVisible = FALSE;
	PTHREADINFO pti = get_tdb()->Win32Thread;
	HWND hwnd = NULL;
	PWND pwnd = NULL;

	/* If the style contains the visible bit and there isn't any predefined class/scrollbars, remove the bit */
	if((newStyle & WS_VISIBLE) && (!(newStyle & WS_HSCROLL) || !(newStyle & WS_VSCROLL))
		&& strcmpi("button", lpClassName) && strcmpi("combobox", lpClassName) && strcmpi("edit", lpClassName)
		&& strcmpi("listbox", lpClassName) && strcmpi("mdiclient", lpClassName) && strcmpi("richedit", lpClassName)
		&& strcmpi("richedit_class", lpClassName) && strcmpi("scrollbar", lpClassName) && strcmpi("static", lpClassName))
	{
		if(pti->rpdesk != gpdeskInputDesktop)
		{
			//newStyle &= ~WS_VISIBLE;
			fWasVisible = TRUE;
		}
	}

	hwnd = CreateWindowExA(dwExStyle,
						lpClassName,
						lpWindowName,
						newStyle,
						x,
						y,
						nWidth,
						nHeight,
						hWndParent,
						hMenu,
						hInstance,
						lpParam);

	if(hwnd == NULL)
		return NULL;

	pwnd = HWNDtoPWND(hwnd);

	if(fWasVisible)
	{
		pwnd->style |= WS_INTERNAL_WASVISIBLE;
		ShowWindow(hwnd, SW_HIDE);
	}

	return hwnd;
}

/* MAKE_EXPORT DisableProcessWindowsGhosting_new=DisableProcessWindowsGhosting */
void __stdcall DisableProcessWindowsGhosting_new(void)
{
}

/* 
 * Win9x seems to look at only the low 16 bits of bEnable.
 * This causes a problem, for example, if the caller passes
 * a count of items with the intent to enable a window when
 * the count is >0.  That will fail when the count is
 * a multiple of 64K 
 */

/* MAKE_EXPORT EnableWindow_new=EnableWindow */
BOOL WINAPI EnableWindow_new(HWND hWnd, BOOL bEnable)
{
	return EnableWindow(hWnd, bEnable ? TRUE : FALSE);
}

/* MAKE_EXPORT EnumChildWindows_nothunk=EnumChildWindows */
BOOL WINAPI EnumChildWindows_nothunk(HWND hWndParent,
    WNDENUMPROC lpEnumFunc,
    LPARAM lParam
)
{
	PWND pwndParent = NULL;
	PWND pwndChild = NULL;

	if(hWndParent == NULL)
		return EnumWindows(lpEnumFunc, lParam);

	if(IsBadCodePtr((FARPROC)lpEnumFunc))
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	pwndParent = HWNDtoPWND(hWndParent);

	if(pwndParent == NULL)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	for(pwndChild = pwndParent->spwndChild; pwndChild != NULL; pwndChild = pwndParent->spwndChild)
	{
		pwndChild = (PWND)REBASEUSER(pwndChild);

		if(!(*lpEnumFunc)((HWND)pwndChild->hWnd16, lParam))
			return FALSE;
	}

	return TRUE;
}

/* MAKE_EXPORT EnumThreadWindows_nothunk=EnumThreadWindows */
BOOL WINAPI EnumThreadWindows_nothunk(DWORD dwThreadId,
    WNDENUMPROC lpfn,
    LPARAM lParam
)
{
	PTDB98 Thread = NULL;
	PWND pwnd = NULL;
	PMSGQUEUE msgQueue = NULL;

	if(IsBadCodePtr((FARPROC)lpfn))
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	Thread = (PTDB98)kexGetThread(dwThreadId);

	if(Thread == NULL)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	pwnd = HWNDtoPWND(GetDesktopWindow());

	if(pwnd == NULL)
		return FALSE;

	for(; pwnd != NULL; pwnd = pwnd->spwndNext)
	{
		pwnd = (PWND)REBASEUSER(pwnd);

		msgQueue = GetWindowQueue(pwnd);

		if(msgQueue == NULL)
			continue;

		if(msgQueue->threadId == dwThreadId)
		{
			if(!(*lpfn)((HWND)pwnd->hWnd16, lParam))
				return FALSE;
		}
	}

	return TRUE;
}

/* MAKE_EXPORT EnumWindows_nothunk=EnumWindows */
BOOL WINAPI EnumWindows_nothunk(WNDENUMPROC lpEnumFunc,
    LPARAM lParam
)
{
	BOOL fFirst = TRUE;
	PWND pWnd = NULL;
	PWND pWndParent = NULL;

	if(IsBadCodePtr((FARPROC)lpEnumFunc))
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	pWndParent = HWNDtoPWND(GetDesktopWindow());

	if(pWndParent == NULL)
		return FALSE;

	pWnd = pWndParent->spwndChild;

	if(pWnd == NULL)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	pWnd = (PWND)REBASEUSER(pWnd);

	while(TRUE)
	{
		if(!(*lpEnumFunc)((HWND)pWnd->hWnd16, lParam))
			return FALSE;

		if (pWnd->spwndNext != NULL)
		{
			pWnd = (PWND)REBASEUSER(pWnd->spwndNext);
			continue;
		}

		pWnd = (PWND)REBASEUSER(pWnd->spwndParent);

		if (pWnd == pWndParent)
			break;
	}

	return TRUE;
}

/* MAKE_EXPORT GetParent_nothunk=GetParent */
HWND WINAPI GetParent_nothunk(HWND hWnd)
{
	PWND pwnd = NULL;
	HWND hwndParent = NULL;

	GrabWin16Lock();

	pwnd = HWNDtoPWND(hWnd);

	if(pwnd == NULL)
		goto _ret;

	pwnd = (PWND)REBASEUSER(pwnd->spwndParent);

	if(pwnd != NULL)
		hwndParent = (HWND)pwnd->hWnd16;

_ret:
	ReleaseWin16Lock();
	return hwndParent;
}

/* MAKE_EXPORT LockSetForegroundWindow_98=LockSetForegroundWindow */
BOOL WINAPI LockSetForegroundWindow_98(UINT lockcode)
{
	return TRUE;
}

/* MAKE_EXPORT GetAncestor_fix=GetAncestor */
HWND APIENTRY GetAncestor_fix(HWND hwnd, UINT gaFlags)
{
	// Believe it or not, HWND_MESSAGE works under Windows 98 -- and if you call GetAncestor()
	// on such a window, user32 crashes in 16-bit code. :(
	if (gaFlags == GA_ROOT && GetParent(hwnd) == NULL)
		return hwnd;

	return GetAncestor(hwnd, gaFlags);
}

/* MAKE_EXPORT GetForegroundWindow_fix=GetForegroundWindow */
HWND WINAPI GetForegroundWindow_fix(VOID)
{
	HWND hWnd;

	/* Should we return the global foreground window or the current thread desktop's foreground window ?*/

	GrabWin16Lock();

	__try
	{
		hWnd = (HWND)get_tdb()->Win32Thread->rpdesk->spwndForeground->hWnd16;
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		hWnd = NULL;
	}

	ReleaseWin16Lock();
	return hWnd;
}

/* MAKE_EXPORT GetShellWindow_new=GetShellWindow */
HWND APIENTRY GetShellWindow_new(VOID)
{
	//return FindWindow("SysListView32", NULL);
	return FindWindow("Shell_TrayWnd", NULL);
}

/* MAKE_EXPORT IsHungAppWindow_new=IsHungAppWindow */
BOOL WINAPI IsHungAppWindow_new(HWND hWnd)
{
	return IsHungThread_pfn(GetWindowThreadProcessId(hWnd, NULL));
}

/* IsWindowVisible_fix=IsWindowVisible */
BOOL WINAPI IsWindowVisible_fix(HWND hWnd)
{
	PWND pwnd = NULL;

	GrabWin16Lock();

	pwnd = HWNDtoPWND(hWnd);

	if(pwnd == NULL)
	{
		ReleaseWin16Lock();
		return FALSE;
	}

	/* Also check for the WS_INTERNAL_WASVISIBLE flag */

	for(; pwnd != NULL; pwnd = pwnd->spwndParent)
	{
		pwnd = (PWND)REBASEUSER(pwnd->spwndParent);

		if(!(pwnd->style & WS_VISIBLE) && !(pwnd->style & WS_INTERNAL_WASVISIBLE))
		{
			ReleaseWin16Lock();
			return FALSE;
		}
	}

	ReleaseWin16Lock();

	return TRUE;
}

/* MAKE_EXPORT SetForegroundWindow_fix=SetForegroundWindow */
BOOL WINAPI SetForegroundWindow_fix(HWND hWnd)
{
	PWND pwnd = NULL;
	BOOL result = FALSE;
	PDESKTOP Desktop = NULL;
	PTHREADINFO pti = NULL;

	GrabWin16Lock();

	pti = get_tdb()->Win32Thread;

	if(pti == NULL || pti->rpdesk == NULL)
		goto _ret;

	pwnd = HWNDtoPWND(hWnd);

	if(pwnd == NULL)
		goto _ret;

	result = SetForegroundWindow(hWnd);

	if(!result)
		goto _ret;

	pti->rpdesk->spwndForeground = pwnd;

_ret:
	ReleaseWin16Lock();
	return result;
}


/* MAKE_EXPORT SetWindowPos_fix=SetWindowPos */
BOOL WINAPI SetWindowPos_fix(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags)
{
	DWORD result = 0;
	DWORD newFlags = uFlags;
	int nCmdShow = 0;

	if(uFlags & SWP_HIDEWINDOW)
	{
		nCmdShow = SW_HIDE;
		result = ConfirmShowCommand(hWnd, nCmdShow);
	}
	else if(uFlags & SWP_SHOWWINDOW)
	{
		nCmdShow = SW_SHOW;
		result = ConfirmShowCommand(hWnd, nCmdShow);

		if(result == SW_HIDE)
		{
			newFlags &= ~SWP_SHOWWINDOW;
			newFlags |= SWP_HIDEWINDOW;
		}
	}

	if(result == -1)
		return FALSE;

	return SetWindowPos(hWnd, hWndInsertAfter, X, Y, cx, cy, newFlags);
}

/* MAKE_EXPORT ShowWindow_fix=ShowWindow */
BOOL WINAPI ShowWindow_fix(HWND hWnd, int nCmdShow)
{
	DWORD result = nCmdShow;

	result = ConfirmShowCommand(hWnd, nCmdShow);

	if(result != -1)
		return ShowWindow(hWnd, result);

	return FALSE;
}

/* MAKE_EXPORT ShowWindowAsync_fix=ShowWindowAsync */
BOOL WINAPI ShowWindowAsync_fix(HWND hWnd, int nCmdShow)
{
	DWORD result = nCmdShow;

	result = ConfirmShowCommand(hWnd, nCmdShow);

	if(result != -1)
		return ShowWindowAsync(hWnd, result);

	return FALSE;
}

/* returns TRUE if hwnd is a parent of hwndNewParent */
static BOOL WINAPI TestChild(HWND hwnd, HWND hwndNewParent)
{
	BOOL ret = FALSE;
	PWND pwnd, pwndT;
	GrabWin16Lock();
	pwnd = HWNDtoPWND(hwnd);
	pwndT = HWNDtoPWND(hwndNewParent);
	if ( pwnd && pwndT )
	{
		for ( ; pwndT != NULL; pwndT = (PWND)REBASEUSER(pwndT->spwndParent))
		{
			if (pwnd == pwndT)
			{
				ret = TRUE;
				break;
			}
		}
	}
	ReleaseWin16Lock();	
	return ret;
}

/* MAKE_EXPORT SetParent_fix=SetParent */
__declspec(naked)
HWND WINAPI SetParent_fix(HWND hWndChild, HWND hWndNewParent)
{
__asm {

	/* forbid changing parent of system windows */
	mov     ecx, [esp+4]     /* hWndChild */
	cmp     ecx, 0x88
	ja      __hwndok
	cmp     ecx, 0x80
	jb      __hwndok

	/* hwnd is system one - disallow */
	push    ERROR_ACCESS_DENIED
	jmp     __error

__hwndok:
	/* test hwnds to avoid circular references */
	mov     eax, [esp+8]     /* hWndNewParent */
	push    eax
	push    ecx
	call    TestChild
	test    eax, eax
	jnz     __childfail

	/* return control to SetParent (nothunk version this time) */
	jmp     dword ptr [SetParent_nothunk]

__childfail:
	/* circular reference detected - stop! */
	push    ERROR_INVALID_PARAMETER

__error:
	/* return error */
	call    dword ptr [SetLastError]
	xor     eax, eax
	retn    8

	}
}

/* MAKE_EXPORT SetParent_nothunk=SetParent */
HWND WINAPI SetParent_nothunk(HWND hWndChild, HWND hWndNewParent)
{
	PWND pwnd = NULL;
	PWND pwndNewParent = NULL;
	HWND hwndPrevParent = NULL;
	PMSGQUEUE pQueue = NULL;
	PMSGQUEUE pQueueParent = NULL;

	GrabWin16Lock();

	pwnd = HWNDtoPWND(hWndChild);
	pwndNewParent = HWNDtoPWND(hWndNewParent);

	if(pwnd == NULL || pwndNewParent == NULL)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		goto _ret;
	}

	pQueue = (PMSGQUEUE)GetWindowQueue(pwnd);
	pQueueParent = (PMSGQUEUE)GetWindowQueue(pwndNewParent);

	if(pQueue == NULL || pQueueParent == NULL)
		goto _ret;

	/* Child and parent must belong to the same application */
	if(pQueue->threadId != pQueueParent->threadId) // FIXME: Process or thread ?
	{
		SetLastError(ERROR_ACCESS_DENIED);
		goto _ret;
	}

	if(pwnd->spwndParent != NULL)
		hwndPrevParent = (HWND)REBASEUSER(pwnd->spwndParent);

	/* Set the new parent */
	pwnd->spwndParent = (PWND)USER32FAR16(pwndNewParent);

	/* Set the new child */
	pwndNewParent->spwndChild = (PWND)USER32FAR16(pwnd);

_ret:
	ReleaseWin16Lock();

	return hwndPrevParent;
}

/* MAKE_EXPORT UpdateLayeredWindow_new=UpdateLayeredWindow */
BOOL WINAPI UpdateLayeredWindow_new(
	HWND hwnd,             // handle to layered window
	HDC hdcDst,            // handle to screen DC
	POINT *pptDst,         // new screen position
	SIZE *psize,           // new size of the layered window
	HDC hdcSrc,            // handle to surface DC
	POINT *pptSrc,         // layer position
	COLORREF crKey,        // color key
	BLENDFUNCTION *pblend, // blend function
	DWORD dwFlags          // options
)
{
	BOOL ret = FALSE;
	if ( (GetWindowLong(hwnd,GWL_EXSTYLE) & WS_EX_LAYERED) && hdcSrc && pptSrc )
	{
		RECT rc;
		GetWindowRect(hwnd,&rc);
		if (pptDst)
		{
			OffsetRect(&rc,-rc.left,-rc.top);
			OffsetRect(&rc,pptDst->x,pptDst->y);
		}
		if (psize)
		{
			rc.right = rc.left + psize->cx;
			rc.bottom = rc.top + psize->cy;
		}
		MoveWindow(hwnd,rc.left,rc.top,rc.right-rc.left,rc.bottom-rc.top,FALSE);
		HDC hdc = GetDC( hwnd );
		OffsetRect(&rc,-rc.left,-rc.top);
		ret = BitBlt(hdc,0,0,rc.right-rc.left,rc.bottom-rc.top,hdcSrc,pptSrc->x,pptSrc->y,SRCCOPY);
		ReleaseDC(hwnd,hdc);
	}
	return ret;	
}