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

#ifndef __UTIL_H
#define __UTIL_H

#define __STR(x)		#x
#define STR(x)			__STR(x)

#define EXTERNC extern "C"

#define	_Declare_Virtual_Device(name, ver_major, ver_minor, ctrl_proc, device_num, init_order, V86_proc, PM_proc, ref_data) \
BOOL __stdcall ControlDispatcher(DWORD, DWORD, DWORD, DWORD, DWORD, DWORD); \
\
void __declspec(naked) name##_CTRL(void) \
{ \
	__asm	PUSH	EDI \
	__asm	PUSH	ESI \
	__asm	PUSH	EDX \
	__asm	PUSH	ECX \
	__asm	PUSH	EBX \
	__asm	PUSH	EAX \
	__asm	CALL	ctrl_proc \
	__asm	CMP 	EAX, TRUE \
	__asm	RET \
} \
\
EXTERNC struct VxD_Desc_Block name##_DDB = \
{ \
	0, DDK_VERSION, device_num, ver_major, ver_minor, 0, \
	#name, init_order, (ULONG)name##_CTRL, (ULONG)V86_proc, (ULONG)PM_proc, \
	0, 0, ref_data, 0, 0, 0, 'Prev', sizeof(struct VxD_Desc_Block), \
	'Rsv1', 'Rsv2', 'Rsv3' \
}



#pragma warning (disable:4035)		// turn off no return code warning

static inline
bool Hook_Device_Service(ULONG service, ULONG hookproc)
{
	_asm mov  eax, [service]
	_asm mov  esi, [hookproc]
	VMMCall(Hook_Device_Service)
	_asm setnc al
#ifndef __BOOL_DEFINED
	_asm movzx eax, al
#endif
}

#pragma warning (default:4035)		// turn on no return code warning

#endif
