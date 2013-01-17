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

#ifndef __COMMON_H
#define __COMMON_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <malloc.h>
#include "kexcoresdk.h"
#include "kstructs.h"
#include "k32ord.h"

#ifdef __cplusplus
extern "C" {
#endif

void* __cdecl recalloc(void* ptr, size_t size);

typedef int __stdcall STUB(void);
typedef int __stdcall FWDPROC(void);

extern int acp_mcs;
BOOL common_init(void);
char* file_fixWprefix(char* in);
void fatal_error(const char* msg);

size_t lstrlenAnull(LPCSTR s);
size_t lstrlenWnull(LPCWSTR s);

#ifdef __cplusplus
};
#endif

#ifdef __GNUC__
#define UNIMPL_FUNC(name,params) \
	__asm__( ".text\n" \
			 ".globl _" #name "_stub@0\n" \
			 "_" #name "_stub@0:\n\t" \
			 "xor %eax, %eax\n\t" \
			 "movb $" #params ", %cl\n\t" \
			 "jmp _CommonUnimpStub@0\n\t" \
	)
#else
#define UNIMPL_FUNC(name,params) \
	int __declspec(naked) __stdcall name##_stub() \
	{ \
		__asm xor eax,eax \
		__asm mov cl, params \
		__asm jmp dword ptr [CommonUnimpStub] \
	}
#endif

#define AtoW(str, bsize) MultiByteToWideChar(CP_ACP, 0, (LPCSTR)str##A, -1, (LPWSTR)str##W, bsize)
#define WtoA(str, bsize) WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)str##W, -1, (LPSTR)str##A, bsize, NULL, NULL)

#define _ALLOC_WtoA(str) \
	const WCHAR* p; \
	int c; \
	for (p = str##W ; *p ; p++); \
	c = p - str##W + 1; \
	c *= acp_mcs; \
	str##A = (char*) alloca(c); \
	WtoA(str, c)
	

#define ALLOC_WtoA(str) \
	char* str##A; \
	if (str##W) \
	{ \
		_ALLOC_WtoA(str); \
	} \
	else str##A = NULL

#define ALLOC_A(str, cnt) \
	char* str##A = str##W ? ((char*) alloca(cnt)) : NULL

#define ABUFtoW(str, inbsize, outbsize) \
	MultiByteToWideChar(CP_ACP, 0, str##A, inbsize, str##W, outbsize)

#define file_GetCP() \
	UINT file_CP = AreFileApisANSI() ? CP_ACP : CP_OEMCP
#define file_AtoW(str, bsize) MultiByteToWideChar(file_CP, 0, str##A, -1, str##W, bsize)
#define file_WtoA(str, bsize) WideCharToMultiByte(file_CP, 0, str##W, -1, str##A, bsize, "_", NULL); \
	str##A = file_fixWprefix(str##A)

#define file_ALLOC_WtoA(str) \
	char* str##A; \
	if (str##W) \
	{ \
		const WCHAR* p; \
		int c; \
		for (p = str##W ; *p ; p++); \
		c = p - str##W + 1; \
		c *= acp_mcs; \
		str##A = (char*) alloca(c); \
		file_WtoA(str, c); \
	} \
	else str##A = NULL

#define file_ABUFtoW(str, cntsrc, bsize) \
	MultiByteToWideChar(file_CP, 0, str##A, cntsrc, str##W, bsize)

//In macros: convert A<->W on stack
#define STACK_WtoA(strW,strA) \
	{ \
		strA = (LPSTR)strW; \
		if (HIWORD(strW)) \
		{ \
			int c = lstrlenWnull((LPCWSTR)strW); \
			if (c) \
			{ \
				c *= 2; \
				strA = (LPSTR)alloca(c); \
				WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)strW, -1, (LPSTR)strA, c, NULL, NULL); \
			} \
		} \
	}
	
#define STACK_AtoW(strA,strW) \
	{ \
		strW = (LPWSTR)strA; \
		if (HIWORD(strA)) \
		{ \
			int c = lstrlenAnull((LPCSTR)strA); \
			if (c) \
			{ \
				strW = (LPWSTR)alloca(c*sizeof(WCHAR)); \
				MultiByteToWideChar(CP_ACP, 0, (LPCSTR)strA, -1, (LPWSTR)strW, c); \
			} \
		} \
	}

#endif
