/*
 *  KernelEx
 *  Copyright (C) 2008-2010, Xeno86
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
#include "thunks.h"
#include "internals.h"
#include "resolver.h"
#include "../vxd/interface.h"

__declspec(naked) 
PROC ExportFromOrdinalStatic_thunk(IMAGE_NT_HEADERS* PEh, WORD ordinal)
{
__asm {
	mov eax, [ebp-4Ch] //tested w98fe 1998, w98se 2222, w98se 2226, wme 3000
	mov ecx, [eax]
	mov ax, [ecx+10h]
	movzx eax, ax
	mov edx, ppmteModTable
	mov edx, [edx]
	mov eax, [edx+4*eax]
	push eax //target IMTE
	push dword ptr [esp+4] //return address
	mov eax, [ebp+8]
	mov [esp+8], eax //caller MODREF
	mov dword ptr [esp+12], 1
	jmp ExportFromOrdinal
	}
}

__declspec(naked) 
PROC ExportFromNameStatic_thunk(IMAGE_NT_HEADERS* PEh, WORD hint, LPCSTR name)
{
__asm {
	mov eax, [ebp-4Ch] //tested w98fe 1998, w98se 2222, w98se 2226, wme 3000
	mov ecx, [eax]
	mov ax, [ecx+10h]
	movzx eax, ax
	mov edx, ppmteModTable
	mov edx, [edx]
	mov eax, [edx+4*eax]
	push eax //target IMTE
	push dword ptr [esp+4] //return address
	mov eax, [ebp+8]
	mov [esp+8], eax //caller MODREF
	mov dword ptr [esp+12], 1
	jmp ExportFromName
	}
}

/* stack on entry:
 * ret_addr
 * PEh
 * ordinal
 * edi
 * esi
 * ebx
 * caller_addr
 */
__declspec(naked) 
PROC ExportFromOrdinalDynamic_thunk(IMAGE_NT_HEADERS* PEh, WORD ordinal)
{
__asm {
	push edx //target IMTE
	push dword ptr [esp+4]
	push dword ptr [esp+20h] //tested w98se 2225
	call MRfromCallerAddr
	add esp, 4
	mov [esp+8], eax //caller MODREF
	mov dword ptr [esp+12], 0
	jmp ExportFromOrdinal
	}
}

__declspec(naked) 
PROC ExportFromNameDynamic_thunk(IMAGE_NT_HEADERS* PEh, WORD hint, LPCSTR name)
{
__asm {
	push edx //target IMTE
	push dword ptr [esp+4]
	push dword ptr [esp+24h] //tested w98se 2225
	call MRfromCallerAddr
	add esp, 4
	mov [esp+8], eax //caller MODREF
	mov dword ptr [esp+12], 0
	jmp ExportFromName
	}
}

BYTE mod_ext_ena;

__declspec(naked)
void SubSysCheck()
{
__asm {
	seta    al    /* is subsystem value above supported by OS? */
	push	eax
	
	push    [ebp+8]
	call    are_extensions_enabled_module
	add     esp, 4
	mov     mod_ext_ena, al
	cmp     al, 1
	clc

	pop     ecx
	jz      __done	
	cmp     cl, 0
__done:
	jmp     [old_jtab+4*JTAB_SYS_CHK]
	}
}
