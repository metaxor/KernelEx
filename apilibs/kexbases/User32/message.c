/*
 *  KernelEx
 *  Copyright (C) 2008, Xeno86
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

/* MAKE_EXPORT SendMessageA_fix=SendMessageA */
__declspec(naked)
LRESULT WINAPI SendMessageA_fix(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
__asm {
	mov eax, [esp+8] ;Msg
	cmp eax, 402 ;LB_SETTABSTOPS
	jz fix_settabstops
	cmp eax, 203 ;EM_SETTABSTOPS
	jz fix_settabstops
orig_func:
	jmp dword ptr [SendMessageA]
fix_settabstops:
	mov eax, [esp+12] ;wParam
	cmp eax, 0
	jbe orig_func ;sanity check: wParam > 0 ?
	shl eax, 2 ;wParam * sizeof(DWORD)
	push eax ;HeapAlloc.dwBytes
	push eax ;cb
	push dword ptr [esp+16+8]
	call dword ptr [IsBadReadPtr] ;sanity check: ok to read ?
	test eax, eax
	jz L2
	add esp, 4 ;can't read => return failure
	xor eax, eax
	ret 16
L2:
	push 0
	call dword ptr [GetProcessHeap]
	push eax
	call dword ptr [HeapAlloc]
	test eax, eax
	jnz L3 ;alloc successful?
	ret 16
L3:
	mov ecx, [esp+12]	
	push esi
	push edi
	mov esi, [esp+16+8]
	mov edi, eax
	cld
	rep movsd
	pop edi
	pop esi
	push ebx
	mov ebx, eax ;alloc'd mem ptr
	push eax
	push dword ptr [esp+12+8]
	push dword ptr [esp+8+12]
	push dword ptr [esp+4+16]
	call dword ptr [SendMessageA]
	push eax ;remember result
	push ebx
	push 0
	call dword ptr [GetProcessHeap]
	push eax
	call dword ptr [HeapFree]
	pop eax ;remembered result
	pop ebx
	ret 16
	}
}
