/*
 *  KernelEx
 *  Copyright (C) 2008-2009, Xeno86
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
#include "sharedmem.h"
#include "debug.h"

#define HEAP_SHARED   0x04000000

HANDLE shared_heap;

int create_shared_heap()
{
	shared_heap = HeapCreate(HEAP_SHARED, 0, 0);
	return shared_heap ? 1 : 0;
}

void destroy_shared_heap()
{
	HeapDestroy(shared_heap);
}

void* malloc(size_t size)
{
	void* ptr = HeapAlloc(shared_heap, HEAP_ZERO_MEMORY, size);
	if (!ptr)
		DBGPRINTF(("ERROR: malloc failed (%d B)\n", size));
	return ptr;
}

void* calloc(size_t count, size_t size)
{
	void* ptr = HeapAlloc(shared_heap, HEAP_ZERO_MEMORY, count * size);
	if (!ptr)
		DBGPRINTF(("ERROR: malloc failed (%d B)\n", size));
	return ptr;
}

void free(void* ptr)
{
	if (ptr)
		HeapFree(shared_heap, 0, ptr);
	else
		DBGPRINTF(("ERROR: trying to free NULL\n"));
}

void* realloc(void* ptr, size_t size)
{
	void* nptr;
	if (ptr)
		nptr = HeapReAlloc(shared_heap, HEAP_ZERO_MEMORY, ptr, size);
	else 
		nptr = HeapAlloc(shared_heap, HEAP_ZERO_MEMORY, size);
	if (!nptr)
		DBGPRINTF(("ERROR: malloc failed (%d B)\n", size));
	return nptr;
}

void* recalloc(void* ptr, size_t size)
{
	void* nptr;
	if (ptr)
		nptr = HeapReAlloc(shared_heap, HEAP_ZERO_MEMORY, ptr, size);
	else
		nptr = HeapAlloc(shared_heap, HEAP_ZERO_MEMORY, size);
	if (!nptr)
		DBGPRINTF(("ERROR: malloc failed (%d B)\n", size));
	return nptr;
}

void* operator new(size_t size)
{
	return malloc(size);
}

void operator delete(void* ptr)
{
	free(ptr);
}

void* operator new[](size_t size)
{
	return malloc(size);
}

void operator delete[](void* ptr)
{
	free(ptr);
}
