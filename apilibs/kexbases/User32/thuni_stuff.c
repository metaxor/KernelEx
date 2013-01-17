/*
 *  KernelEx
 *
 *  Copyright (C) 2010, Tihiy
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

/* various thuni routines here */

#include <windows.h>
#include <kexcoresdk.h>
#include "thuni_layer.h"

static LPCRITICAL_SECTION pWin16Mutex;
DWORD g_UserBase;
DWORD g_SharedInfo;
HMODULE g_hUser32;

BOOL InitUniThunkLayerStuff()
{
	HINSTANCE hUser16;

	hUser16 = (HINSTANCE)LoadLibrary16("user");

	if((DWORD)hUser16 < 32)
		return FALSE;

	_GetpWin16Lock( &pWin16Mutex );
	g_UserBase = MapSL((DWORD)hUser16 << 16);
	g_hUser32 = GetModuleHandleA("user32");	
    GetDialogBaseUnits();

	//FreeLibrary16(hUser16);
    /* ecx holds gSharedInfo structure */
    _asm mov [g_SharedInfo], ecx

	return (g_UserBase && g_hUser32 && g_SharedInfo);
}

void GrabWin16Lock()
{
	if(pWin16Mutex == NULL)
	{
		kexGrabWin16Lock();
		return;
	}

	_EnterSysLevel(pWin16Mutex);
}

void ReleaseWin16Lock()
{
	if(pWin16Mutex == NULL)
	{
		kexReleaseWin16Lock();
		return;
	}

	_LeaveSysLevel(pWin16Mutex);
}

DWORD GetWindowProcessId( HWND hwnd )
{
	DWORD procID = 0;
	GetWindowThreadProcessId( hwnd, &procID );
	return procID;
}

__declspec(naked)
LRESULT WINAPI CallWindowProc_stdcall( WNDPROC lpPrevWndFunc, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
__asm {
	push ebp
	mov ebp, esp
	push edi
	push esi
	push ebx
	sub esp, 12
	push [ebp+24]
	push [ebp+20]
	push [ebp+16]
	push [ebp+12]
	mov eax, [ebp+8]
	call eax
	lea esp, [ebp-12]
	pop ebx
	pop esi
	pop edi
	leave
	ret 20
	}	
}

int GetCPFromLocale(LCID Locale)
{
	int cp;	
	Locale = LOWORD(Locale);
	if (GetLocaleInfoA(Locale,LOCALE_IDEFAULTANSICODEPAGE | LOCALE_RETURN_NUMBER,(LPSTR)&cp,sizeof(int)))
		return cp;
	else
		return CP_ACP;
}

UINT GetCurrentKeyboardCP()
{
	return GetCPFromLocale((LCID)GetKeyboardLayout(0));
}

#pragma warning (disable:4035)
static inline
WORD GetCurrentThreadhQueue() 
{
	__asm mov ax, fs:[28h]
}
#pragma warning (default:4035)

PMSGQUEUE GetCurrentThreadQueue()
{
	return (PMSGQUEUE)MapSL( GetCurrentThreadhQueue() << 16 );
}

//per-thread keyboard state
static PQUEUEKEYBUFFER GetCurrentThreadKeyBuffer()
{	
	PMSGQUEUE msgq = GetCurrentThreadQueue();
	if (!msgq) return NULL;
	PPERQUEUEDATA queuedata = (PPERQUEUEDATA)REBASEUSER(msgq->npPerQueue);
	if (!queuedata) return NULL;
	return (PQUEUEKEYBUFFER)MapSL(queuedata->keysbuffer);	
}

void UpdateLRKeyState(LPMSG msg)
{
	switch(msg->message) {
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
		if (msg->wParam == VK_SHIFT)
		{
			PQUEUEKEYBUFFER keybuffer = GetCurrentThreadKeyBuffer();
			if (keybuffer)
			{
				BYTE scancode = LOBYTE(HIWORD(msg->lParam));
				if ( scancode == MapVirtualKey(VK_SHIFT,0) ) //left shift
					keybuffer->keystate[VK_LSHIFT] = keybuffer->keystate[VK_SHIFT];
				else
					keybuffer->keystate[VK_RSHIFT] = keybuffer->keystate[VK_SHIFT];
			}
		}
		else if (msg->wParam == VK_CONTROL)
		{
			PQUEUEKEYBUFFER keybuffer = GetCurrentThreadKeyBuffer();
			if (keybuffer)
			{
				if ( msg->lParam & 0x1000000 ) //extended bit -> right
					keybuffer->keystate[VK_RCONTROL] = keybuffer->keystate[VK_CONTROL];
				else
					keybuffer->keystate[VK_LCONTROL] = keybuffer->keystate[VK_CONTROL];					
			}
		}
		else if (msg->wParam == VK_MENU)
		{
			PQUEUEKEYBUFFER keybuffer = GetCurrentThreadKeyBuffer();
			if (keybuffer)
			{
				if ( msg->lParam & 0x1000000 ) //extended bit -> right
					keybuffer->keystate[VK_RMENU] = keybuffer->keystate[VK_MENU];
				else
					keybuffer->keystate[VK_LMENU] = keybuffer->keystate[VK_MENU];					
			}
		}
		break;
	}	
}

