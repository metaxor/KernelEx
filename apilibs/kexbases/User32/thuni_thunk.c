#include <windows.h>
#include "thuni_thunk.h"
#include "thuni_layer.h"
#include "kexcoresdk.h"

static int thunindex = -1;
static DWORD g_ThunkCode = 0;

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

/* Credit goes to RP9 for these functions */
void MakeSelectorExecutable(WORD wSelector)
{
	PLDT_ENTRY entry = (PLDT_ENTRY)(pLDT + (wSelector &~7));
	entry->HighWord.Bits.Type = (entry->HighWord.Bits.Type & 0xF7) | 8;
}

void GlobalSetOwner(WORD gHandle, WORD wOwner)
{
	DWORD arena = ( pBurgerMaster + *(DWORD*)(pSelTables + ((gHandle &~7)>>1) ) );

	*(DWORD*)(arena + 0x12) = wOwner;
}

DWORD WINAPI CreateThunk16(DWORD dwSize)
{
	DWORD retcode;

	dwSize = (dwSize + 0x1F) & ~0x1F;

	if (!g_ThunkCode) //allocate global memory, convert it into code segment
	{
		WORD code16 = GlobalAlloc16(GMEM_FIXED, 4000);
		if (code16)
		{
			MakeSelectorExecutable(code16);
			GlobalSetOwner(code16, MapHModuleLS(GetModuleHandle(0)));
			g_ThunkCode = code16 << 16;
			K32WOWGetVDMPointerFix(g_ThunkCode, 4000, TRUE);				
		}
	}

	retcode = g_ThunkCode;
	g_ThunkCode = g_ThunkCode + dwSize;

	return retcode;
}

DWORD GetCall16Addr(DWORD code16)
{
	PCALL16 call16 = (PCALL16)MapSL(code16);

	if (!call16)
		return 0; // not a valid address at all

	if (call16->callnear != 0xE8 && call16->callnear != 0xE9)
		return 0; //it's not a 16-bit jmp or call?

	return (code16 & 0xFFFF0000) | (LOWORD(code16) + call16->call_reladdr + 1 + 2);
}
