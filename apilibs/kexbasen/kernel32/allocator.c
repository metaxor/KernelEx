/*
 *  KernelEx
 *  Copyright (C) 2009-2010, Xeno86
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

/************************************************************************/
/*                          N O T E S                                   */
/************************************************************************/

/* Shared heap and default process heap (GetProcessHeap) is handled by
 * standard system implementation. This releases us from the hassle of
 * making special hacks for handling GlobalAlloc and LocalAlloc.
 * This covers 99% of applications.
 *
 * One custom heap is used per process, regardless of the number of
 * HeapCreate calls. Two distinct handle values are used to indicate 
 * belonging to jemalloc and whether HEAP_GENERATE_EXCEPTIONS flag 
 * was set.
 * All calls which do not have our handles are forwarded to system.
 *
 * One arena is used and TLS usage is disabled for optimal performance 
 * on single processor system. HEAP_NO_SERIALIZE flag is ignored
 * because there is single heap per process.
 */

/************************************************************************/
/*              M A C R O S   A N D   W R A P P E R S                   */
/************************************************************************/

#include <stdlib.h>

#define	malloc(a)	je_malloc(a)
#define	valloc(a)	je_valloc(a)
#define	calloc(a, b)	je_calloc(a, b)
#define	realloc(a, b)	je_realloc(a, b)
#define	free(a)		je_free(a)

#define MOZ_MEMORY
#define MOZ_MEMORY_WINDOWS
#define NO_TLS

#if (_MSC_VER < 1300)
#define __STDC_LIMIT_MACROS
#include "stdint.h"
#endif

#include "jemalloc/jemalloc.c"

int* _errno(void)
{
	static int myerrno = 0;
	return &myerrno;
}

#define TAG(A, B, C, D) (ULONG)(((A)<<0) + ((B)<<8) + ((C)<<16) + ((D)<<24))
#define JM_HEAP_NORM ((HANDLE) TAG('J','E','M','N'))
#define JM_HEAP_EXCP ((HANDLE) TAG('J','E','M','E'))

#define HEAP_SHARED   0x04000000

BOOL init_jemalloc()
{
	if (malloc_init_hard())
		return FALSE;
	return TRUE;
}

/************************************************************************/
/*                        T H E   C O D E                               */
/************************************************************************/

typedef union
{
	BYTE db;
	WORD dw;
	DWORD dd;
} UFooter;

static inline 
int footer_size(DWORD alloc_bytes)
{
	if (alloc_bytes <= 256 - 1)
		return 1;
	else if (alloc_bytes < 64*1024 - 2)
		return 2;
	else
		return 4;
}

static inline
int footer_size_for_usable_size(size_t usable)
{
	if (usable <= 256)
		return 1;
	else if (usable <= 64*1024)
		return 2;
	else
		return 4;
}

static inline
DWORD read_footer(const void* ptr, size_t usable, int fs)
{
	DWORD ret;
	UFooter* footer;
	if (!ptr)
		return 0;
	footer = (UFooter*) ((DWORD) ptr + usable - fs);
	if (fs == sizeof(BYTE))
		ret = footer->db;
	else if (fs == sizeof(WORD))
		ret = footer->dw;
	else
		ret = footer->dd;
	if (ret > usable - fs) //heap corruption detected
		ret = usable - fs;
	return ret;
}

static inline
void write_footer(void* ptr, size_t usable, int fs, DWORD value)
{
	UFooter* footer = (UFooter*) ((DWORD) ptr + usable - fs);
	if (fs == sizeof(BYTE))
		footer->db = (BYTE) value;
	else if (fs == sizeof(WORD))
		footer->dw = (WORD) value;
	else
		footer->dd = (DWORD) value;
}

/* MAKE_EXPORT HeapCreate_new=HeapCreate */
HANDLE WINAPI HeapCreate_new(DWORD flOptions, DWORD dwInitialSize, DWORD dwMaximumSize)
{
	if (flOptions & HEAP_SHARED || dwMaximumSize != 0)
		return HeapCreate(flOptions, dwInitialSize, dwMaximumSize);
	if (flOptions & HEAP_GENERATE_EXCEPTIONS)
		return JM_HEAP_EXCP;
	else
		return JM_HEAP_NORM;
}

/* MAKE_EXPORT HeapDestroy_new=HeapDestroy */
BOOL WINAPI HeapDestroy_new(HANDLE hHeap)
{
	if (hHeap == JM_HEAP_NORM || hHeap == JM_HEAP_EXCP)
		return TRUE;
	else
		return HeapDestroy(hHeap);
}

/* MAKE_EXPORT HeapAlloc_new=HeapAlloc */
LPVOID WINAPI HeapAlloc_new(HANDLE hHeap, DWORD dwFlags, DWORD dwBytes)
{
	if (hHeap == JM_HEAP_NORM || hHeap == JM_HEAP_EXCP)
	{
		LPVOID ret;
		int fs = footer_size(dwBytes);

		if (dwFlags & HEAP_ZERO_MEMORY)
			ret = calloc(1, dwBytes + fs);
		else
			ret = malloc(dwBytes + fs);
		if (!ret && (hHeap == JM_HEAP_EXCP || (dwFlags & HEAP_GENERATE_EXCEPTIONS)))
			RaiseException(STATUS_NO_MEMORY, 0, 0, NULL);

		if (ret)
			write_footer(ret, malloc_usable_size(ret), fs, dwBytes);

		return ret;
	}
	else 
		return HeapAlloc(hHeap, dwFlags, dwBytes);
}

/* MAKE_EXPORT HeapFree_new=HeapFree */
BOOL WINAPI HeapFree_new(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem)
{
	if (hHeap == JM_HEAP_NORM || hHeap == JM_HEAP_EXCP)
	{
		if (lpMem && idalloc(lpMem))
		{
			return TRUE;
		}
		else
		{
			SetLastError(ERROR_INVALID_PARAMETER);
			return FALSE;
		}
	}
	else 
		return HeapFree(hHeap, dwFlags, lpMem);
}

/* MAKE_EXPORT HeapSize_new=HeapSize */
DWORD WINAPI HeapSize_new(HANDLE hHeap, DWORD dwFlags, LPCVOID lpMem)
{
	if (hHeap == JM_HEAP_NORM || hHeap == JM_HEAP_EXCP)
	{
		size_t usable = malloc_usable_size(lpMem);
		if (usable == 0)
			return (DWORD) -1;
		int fs = footer_size_for_usable_size(usable);

		return read_footer(lpMem, usable, fs);
	}
	else 
		return HeapSize(hHeap, dwFlags, lpMem);
}

/* MAKE_EXPORT HeapReAlloc_new=HeapReAlloc */
LPVOID WINAPI HeapReAlloc_new(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem, DWORD dwBytes)
{
	if (hHeap == JM_HEAP_NORM || hHeap == JM_HEAP_EXCP)
	{
		LPVOID ret;
		int fs = footer_size(dwBytes);
		size_t usable = lpMem ? malloc_usable_size(lpMem) : 0;

		if (lpMem && usable == 0)
		{
			if (hHeap == JM_HEAP_EXCP || (dwFlags & HEAP_GENERATE_EXCEPTIONS))
				RaiseException(STATUS_ACCESS_VIOLATION, 0, 0, NULL);
			return NULL;
		}

		if (dwFlags & HEAP_REALLOC_IN_PLACE_ONLY)
		{
			if (usable > dwBytes + fs)
			{
				size_t fsu = footer_size_for_usable_size(usable);
				if (dwFlags & HEAP_ZERO_MEMORY)
				{
					DWORD old = read_footer(lpMem, usable, fsu);
					if (dwBytes > old)
						memset((void*) ((DWORD) lpMem + old), 0, dwBytes - old);
				}

				write_footer(lpMem, usable, fsu, dwBytes);
				ret = lpMem;
			}
			else
				ret = NULL;
		}
		else if (dwFlags & HEAP_ZERO_MEMORY)
		{
			DWORD old = read_footer(lpMem, usable, footer_size_for_usable_size(usable));
			ret = realloc(lpMem, dwBytes + fs);
			if (ret)
			{
				if (dwBytes > old)
					memset((void*) ((DWORD) ret + old), 0, dwBytes - old);
				write_footer(ret, malloc_usable_size(ret), fs, dwBytes);
			}
		}
		else
		{
			ret = realloc(lpMem, dwBytes + fs);
			if (ret)
				write_footer(ret, malloc_usable_size(ret), fs, dwBytes);
		}
		if (!ret && (hHeap == JM_HEAP_EXCP || (dwFlags & HEAP_GENERATE_EXCEPTIONS)))
			RaiseException(STATUS_NO_MEMORY, 0, 0, NULL);
		return ret;
	}
	else 
		return HeapReAlloc(hHeap, dwFlags, lpMem, dwBytes);
}

