/*
 *  KernelEx
 *  Copyright (C) 2011, Xeno86
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

#ifndef __VXDMAIN_H
#define __VXDMAIN_H

BOOL _stdcall VKernelEx_Critical_Init(void);
BOOL _stdcall VKernelEx_Dynamic_Init(void);
BOOL _stdcall VKernelEx_Dynamic_Exit(void);
DWORD _stdcall VKernelEx_W32_DeviceIOControl(DWORD, DWORD, PDIOCPARAMETERS);
DWORD _stdcall VKernelEx_CleanUp(void);
DWORD _stdcall VKernelEx_IOCTL_Connect(DWORD, PDIOCPARAMETERS);
BOOL _stdcall VKernelEx_Create_Process(DWORD pid);
BOOL _stdcall VKernelEx_Destroy_Process(DWORD pid);
BOOL _stdcall VKernelEx_Begin_PM_App(HVM hVM);

#endif
