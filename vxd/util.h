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

#pragma once

#include <vmm.h>

#define MINIVDD
#include <minivdd.h>

#define __STR(x)		#x
#define STR(x)			__STR(x)

#define EXTERNC extern "C"

extern int CurrentLine;

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

__declspec(naked)
int
VXDINLINE
_lstrcmpi(PCHAR pString1, PCHAR pString2)
{
    VMMJmp(_lstrcmpi);
}

ULONG VXDINLINE
VDD_Msg_ClrScrn(HVM hvm, DWORD bgcolor)
{
    int column;
    int row;

    _asm mov eax, [bgcolor]
    _asm mov ebx, [hvm]
    VxDCall(VDD_Msg_ClrScrn);
    _asm mov [column], eax
    _asm mov [row], edx

    return MAKELONG(row, column);
}

VOID VXDINLINE
VDD_Msg_BakColor(HVM hvm, DWORD bgcolor)
{
    _asm mov eax, [bgcolor]
    _asm mov ebx, [hvm]
    VxDCall(VDD_Msg_BakColor);
}
VOID VXDINLINE
VDD_Msg_ForColor(HVM hvm, DWORD fgcolor)
{
    _asm mov eax, [fgcolor]
    _asm mov ebx, [hvm]
    VxDCall(VDD_Msg_ForColor);
}

VOID VXDINLINE
VDD_Msg_SetCursPos(HVM hvm, int row, int column)
{
    _asm mov eax, [row]
    _asm mov edx, [column]
    _asm mov ebx, [hvm]
    VxDCall(VDD_Msg_SetCursPos);
}

VOID VXDINLINE
VDD_Msg_TextOut(HVM hvm, int row, int msglength, int column, PCHAR pszMessage)
{
    _asm mov eax, [row]
    _asm mov ecx, [msglength]
    _asm mov edx, [column]
    _asm mov ebx, [hvm]
    _asm mov esi, [pszMessage]
    VxDCall(VDD_Msg_TextOut);
}

VOID VXDINLINE
ReplaceLine(int Line, PCHAR String)
{
	HVM CurVM = Get_Cur_VM_Handle();
    ULONG Length = 0;
    int i;

    if(String == NULL)
    {
        for(i=0; i<=300; i++)
            VDD_Msg_TextOut(CurVM, Line, 1, i, " ");
        goto _return;
    }

    Length = _lstrlen(String);
    VDD_Msg_TextOut(CurVM, Line, Length, 0, String);

_return:
    VDD_Msg_SetCursPos(CurVM, Line, Length);
}

int VXDINLINE
DisplayString(PCHAR String)
{
	HVM CurVM = Get_Cur_VM_Handle();
    ULONG Length = 0;
    int OldLine = CurrentLine;

    if(String == NULL)
        goto _return;

    Length = _lstrlen(String);
    VDD_Msg_TextOut(CurVM, CurrentLine, Length, 0, String);

_return:
    VDD_Msg_SetCursPos(CurVM, CurrentLine, Length);
    CurrentLine++;
    return OldLine;
}

#pragma warning (default:4035)		// turn on no return code warning

#endif
