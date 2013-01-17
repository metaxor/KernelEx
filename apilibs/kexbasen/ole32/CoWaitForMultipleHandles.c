/*
 *  KernelEx
 *  Copyright 2010, Tihiy
 *
 *  Copyright 1995  Martin von Loewis
 *  Copyright 1998  Justin Bradford
 *  Copyright 1999  Francis Beaudet
 *  Copyright 1999  Sylvain St-Germain
 *  Copyright 2002  Marcus Meissner
 *  Copyright 2004  Mike Hearn
 *  Copyright 2005-2006 Robert Shearman (for CodeWeavers)
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

#define COBJMACROS
#define CINTERFACE
#include <windows.h>
#include "kexcoresdk.h"

#define COFLAGS_APARTMENTTHREADED 0x80

typedef struct 
{
	DWORD Unknown;			//+0x00
	DWORD dwCOMFlags;		//+0x04
	BYTE  filler[0x40];		//+0x08
	IMessageFilter *filter;	//+0x48
} APARTMENT, *PAPARTMENT;


static PAPARTMENT COM_CurrentApt()
{
	DWORD** ppdwTlsIndex = (DWORD**)
		( (DWORD)kexGetProcAddress(GetModuleHandle("OLE32.DLL"),"CoGetState")+2 );
	return (PAPARTMENT)TlsGetValue(**ppdwTlsIndex);
}

BOOL CALLBACK FindCOMWindow( HWND hwnd, LPARAM lParam )
{
	char wndname[200];
	if ( GetWindowTextA(hwnd,wndname,200) && strcmp(wndname,"OleMainThreadWndName") == 0
		&& (HINSTANCE)GetWindowLongA(hwnd,GWL_HINSTANCE) == GetModuleHandle("OLE32.DLL") )	
	{
		*(PHANDLE)lParam = hwnd;
		return FALSE;
	}
	return TRUE;	
}

/* processes COM messages, returns FALSE if thread quits */
static BOOL COM_PumpMessages()
{
	HWND comWindow = NULL;
	EnumThreadWindows(GetCurrentThreadId(),FindCOMWindow,(LPARAM)&comWindow);
	MSG msg;
	BOOL haveMsgs = 
		PeekMessageA(&msg, comWindow, WM_USER, WM_APP - 1, PM_REMOVE|PM_NOYIELD) ||
		/* next retrieve other messages necessary for the app to remain responsive */
		PeekMessageA(&msg, NULL, WM_DDE_FIRST, WM_DDE_LAST, PM_REMOVE|PM_NOYIELD) ||
		PeekMessageA(&msg, NULL, 0, 0, PM_QS_PAINT|PM_QS_SENDMESSAGE|PM_REMOVE|PM_NOYIELD);
	if (haveMsgs)
	{
		DispatchMessage(&msg);
		if (msg.message == WM_QUIT) //resend 
		{
			PostQuitMessage(msg.wParam);
			return FALSE;
		}
	}
	return TRUE;
}


/* MAKE_EXPORT CoWaitForMultipleHandles_new=CoWaitForMultipleHandles */
HRESULT WINAPI CoWaitForMultipleHandles_new(
  DWORD dwFlags,
  DWORD dwTimeout,
  ULONG cHandles,
  LPHANDLE pHandles,
  LPDWORD  lpdwindex
)
{
	if (!pHandles || !lpdwindex)
		return E_INVALIDARG;

	HRESULT hr = S_OK;
	DWORD start_time = GetTickCount();
	PAPARTMENT apt = COM_CurrentApt();	
	BOOL message_loop = apt && (apt->dwCOMFlags & COFLAGS_APARTMENTTHREADED);

	while (TRUE)
	{
		DWORD now = GetTickCount();
		DWORD res;
		if (now - start_time > dwTimeout)
		{
			hr = RPC_S_CALLPENDING;
			break;
		}
		if (message_loop)
		{
			DWORD wait_flags = ((dwFlags & COWAIT_WAITALL) ? MWMO_WAITALL : 0) | 
							((dwFlags & COWAIT_ALERTABLE ) ? MWMO_ALERTABLE : 0);

			res	= MsgWaitForMultipleObjectsEx(cHandles,pHandles,
				(dwTimeout == INFINITE) ? INFINITE : start_time + dwTimeout - now,
				QS_ALLINPUT,wait_flags);
			
			if (res == WAIT_OBJECT_0 + cHandles) /* messages available */
			{						
				if (apt->filter) /* call message filter */
				{
					//don't know htask or pendingtype :(
					DWORD handled = IMessageFilter_MessagePending(apt->filter,0,
						now-start_time,PENDINGTYPE_TOPLEVEL);
					if (handled == PENDINGMSG_CANCELCALL)
						hr = RPC_E_CALL_CANCELED;
				}
				message_loop = COM_PumpMessages();
				continue;
			}
		}
		else
		{
			res = WaitForMultipleObjectsEx(cHandles,pHandles,
				(dwFlags & COWAIT_WAITALL) ? TRUE : FALSE,
				(dwTimeout == INFINITE) ? INFINITE : start_time + dwTimeout - now,
				(dwFlags & COWAIT_ALERTABLE) ? TRUE : FALSE);

		}
		if (res < WAIT_OBJECT_0 + cHandles)
		{
			/* handle signaled, store index */
			*lpdwindex = (res - WAIT_OBJECT_0);
			break;
		}
		else if (res == WAIT_TIMEOUT)
		{
			hr = RPC_S_CALLPENDING;
			break;
		}
		else
		{
			hr = E_UNEXPECTED;
			break;
		}
	}
	return hr;
}
