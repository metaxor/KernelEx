#ifndef __THUNITHUNK_H
#define __THUNITHUNK_H

/* winproc thunks, look a bit like Wine ones */
#define atow_code 0xC987    /*xchg ecx,ecx*/
#define wtoa_code 0xDB87    /*xchg ebx,ebx*/
#define pop_eax_code 0x58
#define push_eax_code 0x50
#define push_func_code 0x68
#define jmp_func_code 0xE9
#define CHUNK_THUNKS 60     /*so THUNKS_STRUCT is around 1KB*/

#define pushl16_code 0x6866

#include "pshpack1.h"
typedef struct
{
	WORD        sign;           /* xchg reg, reg*/
	BYTE        pop_eax;        /* pop eax (return address) */
	BYTE        push_func;      /* push $proc */
	WNDPROC     proc;
	BYTE        push_eax;       /* push eax */
	BYTE        jmp_func;           /* jmp relay*/
	DWORD       relay_offset;   /* _call_wndproc */
	WORD        padding16;      /* total size 16 */
} WINPROC_THUNK, *THUNKPROC;

typedef struct THUNKS_STRUCT
{
	int count;
	struct THUNKS_STRUCT* nextChunk;
	WINPROC_THUNK array[CHUNK_THUNKS]; 
} THUNKS_STRUCT, *PROCESS_THUNKS;

typedef struct
{
	WORD        push1;          /* push16 */
	WNDPROC     proc;
	WORD        push2;
	DWORD       type;           /* thunk variant */
} THUNK16, *PTHUNK16;

typedef struct
{
	UINT fuFlags;
	UINT uTimeout;
	LPDWORD lpdwResult;
} SMTIMEOUT_THUNK, *PSMTIMEOUT_THUNK;

#include "poppack.h"

/* looks like thunk? */
__forceinline
static BOOL IsValidThunk( THUNKPROC proc )
{
	if ( proc->pop_eax == pop_eax_code && proc->jmp_func == jmp_func_code ) return TRUE;
	return FALSE;
}

PROCESS_THUNKS GetProcessThunks();
PROCESS_THUNKS GrowProcessThunks(PROCESS_THUNKS lastChunk);

#endif
