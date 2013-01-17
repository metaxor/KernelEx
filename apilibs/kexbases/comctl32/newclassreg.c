/*
 *  KernelEx
 *  Copyright (C) 2010, Tihiy, Xeno86
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

#if defined(_WIN32_WINNT) && _WIN32_WINNT < 0x0600
#undef _WIN32_WINNT
#endif
#define _WIN32_WINNT 0x0600 

#include "common.h"
#include <commctrl.h>


typedef BOOL (WINAPI* ICCEX) (LPINITCOMMONCONTROLSEX);

VOID SYSLINK_Register(void);

/* MAKE_EXPORT InitCommonControlsEx_new=InitCommonControlsEx */
BOOL WINAPI InitCommonControlsEx_new(LPINITCOMMONCONTROLSEX lpInitCtrls)
{
	static ICCEX InitCommonControlsEx_dld;

	//sanity check
	if (!lpInitCtrls || lpInitCtrls->dwSize != sizeof(INITCOMMONCONTROLSEX))
		return FALSE;

	//register SysLink
	if (lpInitCtrls->dwICC & ICC_LINK_CLASS || 
		lpInitCtrls->dwICC == (ICC_WIN95_CLASSES|ICC_DATE_CLASSES|ICC_USEREX_CLASSES|ICC_COOL_CLASSES))
	{
		SYSLINK_Register();
		if (lpInitCtrls->dwICC == ICC_LINK_CLASS)
			return TRUE; //no other classes to register

		//remove SysLink from list of classes to register - COMCTL32 will fail otherwise
		lpInitCtrls->dwICC &= ~ICC_LINK_CLASS;
	}
	if (!InitCommonControlsEx_dld)
		InitCommonControlsEx_dld = (ICCEX)kexGetProcAddress(GetModuleHandle("COMCTL32.DLL"),"InitCommonControlsEx");

	return InitCommonControlsEx_dld(lpInitCtrls);
}
