#include <windows.h>

void* malloc(size_t size)
{
	return HeapAlloc(GetProcessHeap(), 0, size);
}

void* calloc(size_t count, size_t size)
{
	return HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, count * size);
}

void free(void* ptr)
{
	if (ptr)
		HeapFree(GetProcessHeap(), 0, ptr);
}

void* realloc(void* ptr, size_t size)
{
	if (ptr)
		return HeapReAlloc(GetProcessHeap(), 0, ptr, size);
	else 
		return HeapAlloc(GetProcessHeap(), 0, size);
}

void* recalloc(void* ptr, size_t size)
{
	if (ptr)
		return HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, ptr, size);
	else
		return HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size);
}
