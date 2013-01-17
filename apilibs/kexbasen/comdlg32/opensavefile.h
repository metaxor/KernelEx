/*
 *  KernelEx
 *  Copyright (C) 2010, Tihiy
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

#include <malloc.h>
#include "common.h"
#include "kexcoresdk.h"

/*
 *	stlenAA: returns length of string list, including terminators;
 *	strings are separated by \0, terminated by \0\0.
 */
static int strlenAA(LPSTR strAA)
{
	int len = 2;
	while (*(WORD*)strAA)
	{
		len++;
		strAA++;
	}	
	return len;
}

/*
 *	stlenWW: same as for strlenAA, but for wide string list
 */
static int strlenWW(LPWSTR strWW)
{
	int len = 2;
	while (*(DWORD*)strWW)
	{
		len++;
		strWW++;
	}	
	return len;
}

//W<->A macros
#define MAX_STACK 512

//Out macros: allocate buffer, call W>-<A function, convert A<->W
#define ABUFFER_ALLOC(buffer,len) \
	int buffer##size = ((len+1) * 2); \
	LPSTR buffer##heap = NULL; \
	if (buffer##size>MAX_STACK) \
	{ \
		buffer##heap = (LPSTR)HeapAlloc(GetProcessHeap(),0,buffer##size); \
		buffer = buffer##heap; \
	} \
	else \
		buffer = (LPSTR)alloca( buffer##size ); \
	buffer[0]='\0'; \
	buffer[len]='\0'; \
	buffer[buffer##size-1]='\0';

#define WBUFFER_ALLOC(buffer,len) \
	int buffer##size = ((len+1) * sizeof(WCHAR)); \
	LPWSTR buffer##heap = NULL; \
	if (buffer##size>MAX_STACK) \
	{ \
		buffer##heap = (LPWSTR)HeapAlloc(GetProcessHeap(),0,buffer##size); \
		buffer = buffer##heap; \
	} \
	else \
		buffer = (LPWSTR)alloca( buffer##size ); \
	buffer[0]='\0'; \
	buffer[len]='\0';

#define ABUFFER_toW(bufferA,bufferW,lenW) \
	MultiByteToWideChar(CP_ACP, 0, bufferA, -1, (LPWSTR)bufferW, lenW);
#define WBUFFER_toA(bufferW,bufferA,lenA) \
	WideCharToMultiByte(CP_ACP, 0, bufferW, -1, (LPSTR)bufferA, lenA, NULL, NULL);

#define BUFFER_FREE(buffer) \
	if ( buffer##heap ) HeapFree(GetProcessHeap(),0,buffer##heap); \

//In macros: allocate buffer on stack, convert input
#define STACK_WWtoAA(strWW,strAA) \
	strAA = (LPSTR)strWW; \
	if (HIWORD(strWW)) \
	{ \
		int c = strlenWW((LPWSTR)strWW); \
		strAA = (LPSTR)alloca(c*2); \
		WideCharToMultiByte(CP_ACP, 0, (LPWSTR)strWW, c, (LPSTR)strAA, c*2, NULL, NULL); \
	}


//misc
#define StrAllocA(size) (LPSTR)HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,size)
#define StrFree(ptr) if (ptr) HeapFree(GetProcessHeap(),0,ptr)

BOOL WINAPI GetSaveFileNameA_fix(LPOPENFILENAMEA lpofn);
BOOL WINAPI GetOpenFileNameA_fix(LPOPENFILENAMEA lpofn);

#define DIALOG_ATOM (ATOM)0x8002

typedef void (* SWU) (HWND, BOOL);
static SWU SetWindowUnicode_dld;

static void SetWindowUnicode(HWND hwnd, BOOL bUnicode)
{
	if (!SetWindowUnicode_dld)
		SetWindowUnicode_dld = (SWU)kexGetProcAddress(GetModuleHandle("KEXBASES.DLL"),"SetWindowUnicode");
	SetWindowUnicode_dld(hwnd,bUnicode);
}

#define OLDDLGPROCPROP "kex.dlgproc"

#define pop_eax_code 0x58
#define push_eax_code 0x50
#define push_func_code 0x68
#define jmp_func_code 0xE9

#include <pshpack1.h>
typedef struct
{
	BYTE        pop_eax;        /* pop eax (return address) */
	BYTE        push_param;     /* push $param */
	LPOPENFILENAMEW	lpofn;
	BYTE        push_func;      /* push $proc */
	LPOFNHOOKPROC	proc;
	BYTE        push_eax;       /* push eax */
	BYTE        jmp_func;       /* jmp relay*/
	DWORD       relay_offset;	/* relative func addr */
} OFN_THUNK, *POFN_THUNK;

#define ALLOC_OFNTHUNK(thunk,ofn,hook,func) \
		thunk = (POFN_THUNK)alloca(sizeof(OFN_THUNK)); \
		thunk->pop_eax = pop_eax_code; \
		thunk->push_param = push_func_code; \
		thunk->lpofn = ofn; \
		thunk->push_func = push_func_code; \
		thunk->proc = hook; \
		thunk->push_eax = push_eax_code; \
		thunk->jmp_func = jmp_func_code; \
		thunk->relay_offset = (DWORD)func - (DWORD)&thunk->relay_offset - sizeof(DWORD)

#include <poppack.h>
