/*
 *  KernelEx
 *
 *  Copyright (C) 2009, Tihiy
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

#ifndef __THUNIMACRO_H
#define __THUNIMACRO_H

#define VALID_PTR(x) (HIWORD(x))

#define MAX_STACK 512

//Out macroses: allocate buffer, call W>-<A function, convert A<->W
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

#define ABUFFER_toW(bufferA,bufferW,lenW) MultiByteToWideChar(CP_ACP, 0, bufferA, -1, (LPWSTR)bufferW, lenW);
#define WBUFFER_toA(bufferW,bufferA,lenA) WideCharToMultiByte(CP_ACP, 0, bufferW, -1, (LPSTR)bufferA, lenA, NULL, NULL);

#define BUFFER_FREE(buffer) \
	if ( buffer##heap ) HeapFree(GetProcessHeap(),0,buffer##heap); \

#endif
