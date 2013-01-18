/*
 *  KernelEx
 *  Copyright (C) 2009, Xeno86
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
#include "function.h"

#define NDBG

extern HWND DialogWnd;

extern HDESK Desktop1;
extern HDESK Desktop0;

extern HWINSTA WindowStation1;

BOOL UnloadEverything(BOOL bForce);
void SwitchToSession1(void);
void RevertToSession0(void);
//void RevertToSession0(void);
BOOL CreateMultiSessionWindow();

VOID DEBUG(LPCSTR Message, ...);