/*
 *  KernelEx
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

#include "common.h"
#include "thuni_layer.h"
#include "resource.h"

#define ET_CANCEL	1
#define ET_WAIT		2
#define ET_ENDTASK	3

extern int WaitToKillAppTimeout;
extern int HungAppTimeout;

HWND FASTCALL CreateGhostWindow(DWORD dwProcessId, DWORD dwThreadId, HWND hwndReplace);
ULONG FASTCALL ShowEndTaskDialog(HWND hwnd, BOOL fEndSession);
DWORD WINAPI HangManagerThread(PVOID lpParameter);