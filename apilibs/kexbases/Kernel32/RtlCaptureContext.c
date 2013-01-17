/*
 *  KernelEx
 *  Copyright (C) 2010, Xeno86
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

#define CONTEXT_CONTEXTFLAGS                    0x00
#define CONTEXT_SEGGS                           0x8C
#define CONTEXT_SEGFS                           0x90
#define CONTEXT_SEGES                           0x94
#define CONTEXT_SEGDS                           0x98
#define CONTEXT_EDI                             0x9C
#define CONTEXT_ESI                             0xA0
#define CONTEXT_EBX                             0xA4
#define CONTEXT_EDX                             0xA8
#define CONTEXT_ECX                             0xAC
#define CONTEXT_EAX                             0xB0
#define CONTEXT_EBP                             0xB4
#define CONTEXT_EIP                             0xB8
#define CONTEXT_SEGCS                           0xBC
#define CONTEXT_EFLAGS                          0xC0
#define CONTEXT_ESP                             0xC4
#define CONTEXT_SEGSS                           0xC8

#define CONTEXT_FULL                            0x10007

#define WINAPI                                  __stdcall

/* MAKE_EXPORT RtlCaptureContext_new=RtlCaptureContext */
__declspec(naked)
void WINAPI RtlCaptureContext_new(void* cr)
{
__asm {
	push ebx
	mov ebx, [esp+8]
	mov [ebx+CONTEXT_EAX], eax
	mov eax, [esp]
	mov [ebx+CONTEXT_EBX], eax
	mov [ebx+CONTEXT_ECX], ecx
	mov [ebx+CONTEXT_EDX], edx
	mov [ebx+CONTEXT_ESI], esi
	mov [ebx+CONTEXT_EDI], edi
	mov [ebx+CONTEXT_SEGCS], cs
	mov [ebx+CONTEXT_SEGDS], ds
	mov [ebx+CONTEXT_SEGES], es
	mov [ebx+CONTEXT_SEGFS], fs
	mov [ebx+CONTEXT_SEGGS], gs
	mov [ebx+CONTEXT_SEGSS], ss
	pushf
	pop [ebx+CONTEXT_EFLAGS]
	mov eax, [ebp]
	mov [ebx+CONTEXT_EBP], eax
	mov eax, [ebp+4]
	mov [ebx+CONTEXT_EIP], eax
	lea eax, [ebp+8]
	mov [ebx+CONTEXT_ESP], eax
	mov [ebx+CONTEXT_CONTEXTFLAGS], CONTEXT_FULL
	pop ebx
	ret 4
	}
}
