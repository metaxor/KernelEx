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

#ifndef __UNIFWD_H
#define __UNIFWD_H

#pragma warning(disable:4409)

int unifwd_init(void);
unsigned long __stdcall GetUnicowsAddress(const char* name);


/* Q: Why FORWARD_TO_UNICOWS macro is in asm? 
 * A: We don't know the parameters of called function
 *    so we can't call the function in C
 * 
 * Note that the code below would be correct if all compilers translated
 * the 'return' line to JMP instruction but it is not always the case
 * and we can't rely on that so we workaround this writing the macro in asm.
 *
 *	int __stdcall fun_fwd()
 *	{
 *		static const char* n = "fun";
 *		static unsigned long ref = 0;
 *		if (!ref)
 *			ref = GetUnicowsAddress("fun");
 *		return ((int (__stdcall *)())ref)();
 *	}
 */

#ifdef __GNUC__

/* How to make it less messy? */
#define FORWARD_TO_UNICOWS(name) \
	__asm__( \
		".section .rdata,\"dr\"\n" \
	"LC_" #name ":\n\t" \
		".ascii \"" #name "\\0\"\n\t" \
		".text\n" \
	".globl _" #name "_fwd@0\n\t" \
		".def   _" #name "fwd@0;	.scl	2;	.type	32;	.endef\n" \
	"_" #name "_fwd@0:\n\t" \
		"cmpl $0, _ref_" #name "\n\t" \
		"jnz LOC_" #name "\n\t" \
		"pushl $LC_" #name "\n\t" \
		"call _GetUnicowsAddress@4\n\t" \
		"movl %eax, _ref_" #name "\n" \
	"LOC_" #name ":\n\t" \
		"movl _ref_" #name ", %eax\n\t" \
		"jmp *%eax\n" \
	".lcomm _ref_" #name ",4\n\t"

#else

#define FORWARD_TO_UNICOWS(name) \
	int __declspec(naked) __stdcall name##_fwd(void) \
	{ \
		static const char n[] = #name; \
		static unsigned long ref = 0; \
		__asm { cmp dword ptr [ref], 0} \
		__asm { jnz L1 } \
		__asm { push offset n } \
		__asm { call GetUnicowsAddress } \
		__asm { mov dword ptr [ref], eax } \
		__asm { L1: jmp [ref] } \
	}

#endif /* #ifdef __GNUC__ */

#endif
