/*
 *  KernelEx
 *  Copyright (C) 2008, Xeno86
 *  Copyright (C) 2010, Tihiy
 *	Copyright (C) 2013, Ley0k
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

#include "input.h"
#include "desktop.h"

PINPUTDATA pInputData = NULL;

/* MapVirtualKey translation types */
#define MAPVK_VK_TO_VSC     0
#define MAPVK_VSC_TO_VK     1
#define MAPVK_VK_TO_CHAR    2
#define MAPVK_VSC_TO_VK_EX  3
#define MAPVK_VK_TO_VSC_EX  4

/* Scan codes for standard keyboard */
#define VSC_LSHIFT 0x002A
#define VSC_RSHIFT 0x0036
#define VSC_LALT 0x0038
#define VSC_RALT 0xE038
#define VSC_LCONTROL 0x001D
#define VSC_RCONTROL 0xE01D

/*	Raw input functions should reset output size buffers to zero.
 *	While (UINT)-1 is "total failure" return, programs (e.g. mpc-hc)
 *	don't check for it, and fail hard thinking it's returned buffer size.
 */
#define RAWNOTIMPLEMETED(intptr) \
	if (intptr) *intptr = 0; \
	SetLastError(ERROR_CALL_NOT_IMPLEMENTED); \
	return 0;

/*
	From MS keyboard docs:
	Under all Microsoft operating systems, all keyboards actually transmit Scan Code Set 2 values down 
	the wire from the keyboard to the keyboard port. These values are translated to Scan Code Set 1 by 
	the i8042 port chip.
	The rest of the operating system, and all applications that handle scan codes 
	expect the values to be from Scan Code Set 1.
	From other MS docs about USB keyboards:
	The mapper driver translates the Keyboard Page HID usages to PS/2 Set 1 scan codes and forwards them
	to the keyboard class driver.
	Nice link btw: http://www.microsoft.com/taiwan/whdc/archive/w2kbd.mspx

	So we have guarantee AT set (Set 1) will be in scancode.
*/

static inline int NoLeftRightVK(int nVirtKey)
{
	switch(nVirtKey) 
	{
	case VK_LSHIFT:
	case VK_RSHIFT:
		nVirtKey = VK_SHIFT;
		break;
	case VK_LCONTROL:
	case VK_RCONTROL:
		nVirtKey = VK_CONTROL;
		break;
	case VK_LMENU:
	case VK_RMENU:
		nVirtKey = VK_MENU;
		break;
	}
	return nVirtKey;
}

BOOL FASTCALL InitInputSegment(void)
{
	HINSTANCE hUser16 = (HINSTANCE)LoadLibrary16("user");
	WORD *InputSegment = NULL;

	if((DWORD)hUser16 < 32)
		return FALSE;

	InputSegment = (WORD*)MapSL(GetProcAddress16(hUser16, "GETASYNCKEYSTATE") + 6);

	if(InputSegment == NULL)
	{
		FreeLibrary16(hUser16);
		return FALSE;
	}

	pInputData = (PINPUTDATA)MapSL((DWORD)*InputSegment << 16);

	if(pInputData == NULL)
	{
		FreeLibrary16(hUser16);
		return FALSE;
	}

	FreeLibrary16(hUser16);
	return TRUE;
}

/* MAKE_EXPORT GetAsyncKeyState_nothunk=GetAsyncKeyState */
SHORT WINAPI GetAsyncKeyState_nothunk(int vKey)
{
	PTDB98 Thread = get_tdb();
	UINT cState = 0;
	BYTE pKeyState = 0;

	if(Thread->Win32Thread != NULL && Thread->Win32Thread->rpdesk != gpdeskInputDesktop)
		return 0;

	if(vKey >= 0x100)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return 0;
	}

	vKey = vKey & 0Xff;
	pKeyState = pInputData->pKey[(vKey >> 2) + 1];

	if (pKeyState & (1 << ((vKey & 3)*2)))
		cState = 0x8000;
	
	pKeyState = pInputData->pKeyRecentDown[vKey >> 3];

	if (pKeyState & (1 << (vKey & 7)))
	{
		pKeyState = pKeyState & ~(1 << (vKey & 7));
		cState |= 1;		
	}
	return (SHORT)cState;
}

/* MAKE_EXPORT GetCursorPos_nothunk=GetCursorPos */
BOOL WINAPI GetCursorPos_nothunk( LPPOINT lpPoint )
{
	if (lpPoint == NULL || IsBadReadPtr(lpPoint , sizeof(POINT)))
		return FALSE;

	lpPoint->x = pInputData->MouseX;
	lpPoint->y = pInputData->MouseY;

	return TRUE;
}

/* MAKE_EXPORT GetLastInputInfo_new=GetLastInputInfo */
BOOL WINAPI GetLastInputInfo_new(PLASTINPUTINFO plii)
{
	if(plii == NULL || IsBadReadPtr(plii, sizeof(LASTINPUTINFO)))
		return FALSE;

	if(plii->cbSize != sizeof(LASTINPUTINFO))
		return FALSE;

	plii->dwTime = GetTickCount();

	return TRUE;
}

/* MAKE_EXPORT MapVirtualKeyA_new=MapVirtualKeyA */
UINT WINAPI MapVirtualKeyA_new(UINT uCode, UINT uMapType)
{
	switch(uMapType) {
	case MAPVK_VK_TO_VSC_EX:
		if ( uCode == VK_RCONTROL )	return VSC_RCONTROL;
		if ( uCode == VK_RMENU ) return VSC_RALT;
		//fall down
	case MAPVK_VK_TO_VSC:
		if ( uCode == VK_RSHIFT ) return VSC_RSHIFT; //separate scan code
		return MapVirtualKeyA(NoLeftRightVK(uCode),MAPVK_VK_TO_VSC);
	case MAPVK_VSC_TO_VK_EX:		
		switch(uCode) {
		case VSC_RSHIFT:
			return VK_RSHIFT;
		case VSC_LSHIFT:
			return VK_LSHIFT;
		case VSC_LCONTROL:
			return VK_LCONTROL;
		case VSC_RCONTROL:
			return VK_RCONTROL;
		case VSC_LALT:
			return VK_LMENU;
		case VSC_RALT:
			return VK_RMENU;
		default:
			uMapType = MAPVK_VSC_TO_VK;
		}
	}
	return MapVirtualKeyA(uCode,uMapType);
}

/* MAKE_EXPORT MapVirtualKeyExA_new=MapVirtualKeyExA */
UINT WINAPI MapVirtualKeyExA_new(UINT uCode, UINT uMapType, HKL dwhkl)
{
	switch(uMapType) {
	case MAPVK_VK_TO_VSC_EX:
		if ( uCode == VK_RCONTROL )	return VSC_RCONTROL;
		if ( uCode == VK_RMENU ) return VSC_RALT;
		//fall down
	case MAPVK_VK_TO_VSC:
		if ( uCode == VK_RSHIFT ) return VSC_RSHIFT; //separate scan code
		return MapVirtualKeyExA(NoLeftRightVK(uCode),MAPVK_VK_TO_VSC,dwhkl);
	case MAPVK_VSC_TO_VK_EX:		
		switch(uCode) {
		case VSC_RSHIFT:
			return VK_RSHIFT;
		case VSC_LSHIFT:
			return VK_LSHIFT;
		case VSC_LCONTROL:
			return VK_LCONTROL;
		case VSC_RCONTROL:
			return VK_RCONTROL;
		case VSC_LALT:
			return VK_LMENU;
		case VSC_RALT:
			return VK_RMENU;
		default:
			uMapType = MAPVK_VSC_TO_VK;
		}
	}
	return MapVirtualKeyExA(uCode,uMapType,dwhkl);
}

/* MAKE_EXPORT DefRawInputProc_new=DefRawInputProc */
LRESULT WINAPI DefRawInputProc_new(PVOID paRawInput, INT nInput, UINT cbSizeHeader)
{
	return E_NOTIMPL;
}

/* MAKE_EXPORT GetRawInputBuffer_new=GetRawInputBuffer */
UINT WINAPI GetRawInputBuffer_new(PVOID pData, PUINT pcbSize, UINT cbSizeHeader)
{
	RAWNOTIMPLEMETED(pcbSize);
}

/* MAKE_EXPORT GetRawInputData_new=GetRawInputData */
UINT WINAPI GetRawInputData_new(PVOID hRawInput, UINT uiCommand, LPVOID pData, PUINT pcbSize, UINT cbSizeHeader)
{
	RAWNOTIMPLEMETED(pcbSize);
}

/* MAKE_EXPORT GetRawInputDeviceList_new=GetRawInputDeviceList */
UINT WINAPI GetRawInputDeviceList_new(PVOID pRawInputDeviceList, PUINT puiNumDevices, UINT cbSize)
{
	RAWNOTIMPLEMETED(puiNumDevices);
}

/* MAKE_EXPORT GetRawInputDeviceInfo_new=GetRawInputDeviceInfoA */
/* MAKE_EXPORT GetRawInputDeviceInfo_new=GetRawInputDeviceInfoW */
UINT WINAPI GetRawInputDeviceInfo_new(HANDLE hDevice, UINT uiCommand,  LPVOID pData, PUINT pcbSize)
{
	RAWNOTIMPLEMETED(pcbSize);
}

/* MAKE_EXPORT GetRegisteredRawInputDevices_new=GetRegisteredRawInputDevices */
UINT WINAPI GetRegisteredRawInputDevices_new(PVOID pRawInputDevices, PUINT puiNumDevices, UINT cbSize)
{
	RAWNOTIMPLEMETED(puiNumDevices);
}

/* MAKE_EXPORT RegisterRawInputDevices_new=RegisterRawInputDevices */
BOOL WINAPI RegisterRawInputDevices_new(PVOID pRawInputDevices, UINT uiNumDevices, UINT cbSize)
{
	SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
	return FALSE;
}
