#include <windows.h>
#include "thuni_thunk.h"
#include "kexcoresdk.h"

static int thunindex = -1;

PROCESS_THUNKS GetProcessThunks()
{
	if (thunindex == -1)
		thunindex = kexPsAllocIndex();
	LPVOID secret = kexPsGetValue(thunindex);
	if (! secret ) //initialize thunks
	{	
		secret = HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(THUNKS_STRUCT));
		kexPsSetValue(thunindex,secret);		
	}
	return (PROCESS_THUNKS)secret;
}

PROCESS_THUNKS GrowProcessThunks(PROCESS_THUNKS lastChunk)
{
	PROCESS_THUNKS newChunk = (PROCESS_THUNKS)HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(THUNKS_STRUCT));
	if (newChunk)
		lastChunk->nextChunk = newChunk;
	return newChunk;
}