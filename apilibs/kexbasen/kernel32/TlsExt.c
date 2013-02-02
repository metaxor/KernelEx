/*
 *  KernelEx
 *  Copyright (C) 2009, Xeno86
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
#include "k32ord.h"
#include "kexcoresdk.h"
#include "kstructs.h"

/*
 * Design note:
 * 
 * We use last TLS slot (79) specially, storing there
 * a pointer to extended TLS slot table.
 */

#define __ASM_IS_L33T__

#define TLS_SIZE             80  /* size of TDB98->TlsSlots */
#define TLS_BITMAP_SIZE      3   /* size of PDB98->tlsInUseBits */
#define EXT_TLS_SIZE         1024
#define EXT_TLS_BITMAP_SIZE  ((EXT_TLS_SIZE - 1) / (8 * sizeof(DWORD)) + 1)
#define TOTAL_TLS_SIZE       (TLS_SIZE-1 + EXT_TLS_SIZE)

/* Process extended TLS bitmap. */
static DWORD ExtTlsBitmap[EXT_TLS_BITMAP_SIZE];

/* Pointer to TlsLock in kernel32. */
static CRITICAL_SECTION* TlsLock;
static CRITICAL_SECTION* k32lock;
static DWORD lasterror_offs;

static LPVOID* AllocExtTlsSlots()
{
	LPVOID* p;
	p = (LPVOID*) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(LPVOID) * EXT_TLS_SIZE);
	if (!p) 
		SetLastError(ERROR_NOT_ENOUGH_MEMORY);
	return p;
}

static CRITICAL_SECTION* find_TlsLock()
{
	PROC pTlsAlloc = kexGetProcAddress(GetModuleHandle("KERNEL32"), "TlsAlloc");
	return *(CRITICAL_SECTION**)((DWORD) pTlsAlloc + 2);
}

/* initialization */
BOOL init_exttls(void)
{
	BOOL ret = FALSE;
	PDB98* pdb = get_pdb();
	DWORD GV = kexGetVersion();
	
	//offsets for GetLastError value differ 
	//between systems :(
	//we set them here
	if (GV == 0xc0000a04) //98
	{
		lasterror_offs = 0x60;
	}
	else if (GV == 0xc0005a04) //Me
	{
		lasterror_offs = 0x74;
	}
	else 
	{
		return FALSE;
	}

	//find TlsLock
	TlsLock = find_TlsLock();
	k32lock = (CRITICAL_SECTION*) kexGetK32Lock();

	if(TlsLock == NULL)
	{
		TRACE_OUT("TlsLock not found !\n");
		return FALSE;
	}

	if(k32lock == NULL)
	{
		TRACE_OUT("k32lock not found !\n");
		return FALSE;
	}

	_EnterSysLevel(TlsLock);

	//check if TLS index 79 free
	if (!(pdb->tlsInUseBits[2] & (1 << 15)))
	{
		//reserve TLS index 79
		pdb->tlsInUseBits[2] |= (1 << 15); 
		ret = TRUE;
	}

	_LeaveSysLevel(TlsLock);

	if (!ret)
		kexDebugPrint("init_exttls failed");

	return ret;
}

void detach_exttls(void)
{
	TDB98* tdb = get_tdb();
	LPVOID ext = tdb->TlsSlots[TLS_SIZE-1];
	if (ext)
	{
		HeapFree(GetProcessHeap(), 0, ext);
		tdb->TlsSlots[TLS_SIZE-1] = 0;
	}
}

/*  TlsAlloc_new=TlsAlloc */
DWORD WINAPI TlsAlloc_new(void)
{
	PDB98* pdb;
	DWORD index = 0;
	int i;

	pdb = get_pdb();

	_EnterSysLevel(TlsLock);

	for (i = 0 ; i < TLS_BITMAP_SIZE ; i++, index += 32)
	{
		if (pdb->tlsInUseBits[i] == 0xffffffff)
			continue;

		for (int j = 0, a = 1 ; j < 32, index < TLS_SIZE-1 ; j++, a <<= 1, index++)
		{
			if ((pdb->tlsInUseBits[i] & a) == 0)
			{
				pdb->tlsInUseBits[i] |= a;
				_LeaveSysLevel(TlsLock);
				return index;
			}
		}
	}

	for (i = 0 ; i < EXT_TLS_BITMAP_SIZE ; i++, index += 32)
	{
		if (ExtTlsBitmap[i] == 0xffffffff)
			continue;

		for (int j = 0, a = 1 ; j < 32, index < TOTAL_TLS_SIZE ; j++, a <<= 1, index++)
		{
			if ((ExtTlsBitmap[i] & a) == 0)
			{
				ExtTlsBitmap[i] |= a;
				_LeaveSysLevel(TlsLock);
				return index;
			}
		}
	}
	
	_LeaveSysLevel(TlsLock);

	SetLastError(ERROR_NO_MORE_ITEMS);
	return TLS_OUT_OF_INDEXES;
}

/*  TlsFree_new=TlsFree */
BOOL WINAPI TlsFree_new(DWORD dwTlsIndex)
{
	int ret = 0;
	PDB98* pdb = get_pdb();

	_EnterSysLevel(k32lock);
	_EnterSysLevel(TlsLock);

	if (dwTlsIndex < TLS_SIZE-1)
	{
		int rem = dwTlsIndex % (sizeof(DWORD) * 8);
		int div = dwTlsIndex / (sizeof(DWORD) * 8);
		if (pdb->tlsInUseBits[div] & (1 << rem))
		{
			pdb->tlsInUseBits[div] &= ~(1 << rem);
			ret = 1;
		}
	}
	else if (dwTlsIndex < TOTAL_TLS_SIZE)
	{
		dwTlsIndex -= TLS_SIZE-1;
		int rem = dwTlsIndex % (sizeof(DWORD) * 8);
		int div = dwTlsIndex / (sizeof(DWORD) * 8);
		if (ExtTlsBitmap[div] & (1 << rem))
		{
			ExtTlsBitmap[div] &= ~(1 << rem);
			ret = 2;
		}
	}

	if (ret)
	{
		const NODE* thread;

		for (thread = pdb->ThreadList->firstNode ; thread != NULL ; thread = thread->next)
		{
			TDB98* tdb = (TDB98*) thread->data;
			if (ret == 1)
				tdb->TlsSlots[dwTlsIndex] = NULL;
			else
			{
				LPVOID* ext = (LPVOID*) tdb->TlsSlots[TLS_SIZE-1];
				if (ext)
					ext[dwTlsIndex] = 0;
			}
		}
	}

	_LeaveSysLevel(TlsLock);
	_LeaveSysLevel(k32lock);

	if (!ret)
		SetLastError(ERROR_INVALID_PARAMETER);
	return (ret != 0);
}

#ifdef __ASM_IS_L33T__

/*  TlsGetValue_new2=TlsGetValue */
__declspec(naked)
LPVOID WINAPI TlsGetValue_new2(DWORD dwTlsIndex)
{
__asm {
	mov     ecx, [esp+4] ;dwTlsIndex
	mov     edx, fs:18h // Thread Info Block
	cmp     ecx, TLS_SIZE-1
	jnb     __more
	mov     eax, [edx+ecx*4+88h]
__exit_no_error:
	mov     ecx, lasterror_offs
	mov     dword ptr [edx+ecx], NO_ERROR
__exit:
	retn    4
__more:
	cmp     ecx, TOTAL_TLS_SIZE
	jnb     __error
	mov     eax, [edx+(TLS_SIZE-1)*4+88h]
	test    eax, eax
	jz      __exit_no_error
	sub     ecx, TLS_SIZE-1
	mov     eax, [eax+ecx*4]
	jmp     __exit_no_error
__error:
	mov     ecx, lasterror_offs
	mov     dword ptr [edx+ecx], ERROR_INVALID_PARAMETER
	xor     eax, eax
	jmp     __exit
	}
}

/*  TlsSetValue_new2=TlsSetValue */
__declspec(naked)
BOOL WINAPI TlsSetValue_new2(DWORD dwTlsIndex, LPVOID lpTlsValue)
{
__asm {
	mov     ecx, [esp+4] ;dwTlsIndex
	mov     edx, fs:18h // Thread Info Block
	cmp     ecx, TLS_SIZE-1
	jnb     __more
	mov     eax, [esp+8] ;lpTlsValue
	mov     [edx+ecx*4+88h], eax
__exit_no_error:
	mov     eax, 1
__exit:
	retn    8
__more:
	cmp     ecx, TOTAL_TLS_SIZE
	jnb     __error
	mov     eax, [edx+(TLS_SIZE-1)*4+88h]
	test    eax, eax
	jz      __alloc
__no_alloc:
	mov     edx, [esp+8] ;lpTlsValue
	sub     ecx, TLS_SIZE-1
	mov     [eax+ecx*4], edx
	jmp     __exit_no_error
__alloc:
	call    AllocExtTlsSlots
	test    eax, eax
	jz      __exit
	mov     ecx, fs:18h
	lea     edx, [ecx+(TLS_SIZE-1)*4+88h]
	mov     [edx], eax
	mov     ecx, [esp+4] ;dwTlsIndex
	jmp     __no_alloc
__error:
	mov     ecx, lasterror_offs
	mov     dword ptr [edx+ecx], ERROR_INVALID_PARAMETER
	xor     eax, eax
	jmp     __exit
	}
}

#else //__ASM_IS_L33T__

static inline void SetLastError_fast(TDB98* tdb, DWORD error)
{
	*(DWORD*) ((DWORD) tdb + lasterror_offs + 8) = error;
}

LPVOID WINAPI TlsGetValue_new2(DWORD dwTlsIndex)
{
	TDB98* tdb = get_tdb();

	if (dwTlsIndex < TLS_SIZE-1)
	{
		SetLastError_fast(tdb, NO_ERROR);
		return tdb->TlsSlots[dwTlsIndex];
	}
	else if (dwTlsIndex < TOTAL_TLS_SIZE)
	{
		LPVOID* ext;
		ext = (LPVOID*) tdb->TlsSlots[TLS_SIZE-1];
		if (ext)
			return ext[dwTlsIndex-TLS_SIZE-1];
		else
			return NULL;
	}
	else
	{
		SetLastError_fast(tdb, ERROR_INVALID_PARAMETER);
		return NULL;
	}
}

BOOL WINAPI TlsSetValue_new2(DWORD dwTlsIndex, LPVOID lpTlsValue)
{
	TDB98* tdb = get_tdb();

	if (dwTlsIndex < TLS_SIZE-1)
	{
		tdb->TlsSlots[dwTlsIndex] = lpTlsValue;
		return TRUE;
	}
	else if (dwTlsIndex < TOTAL_TLS_SIZE)
	{
		LPVOID* ext = (LPVOID*) tdb->TlsSlots[TLS_SIZE-1];
		if (!ext)
		{
			ext = AllocExtTlsSlots();
			if (!ext)
				return FALSE;
			tdb->TlsSlots[TLS_SIZE-1] = ext;
		}
		ext[dwTlsIndex-TLS_SIZE-1] = lpTlsValue;
		return TRUE;
	}
	else
	{
		SetLastError_fast(tdb, ERROR_INVALID_PARAMETER);
		return FALSE;
	}
}

#endif

/* ORIGINAL UNALTERED FUNCTION AS REFERENCE
__declspec(naked)
LPVOID WINAPI TlsGetValue_new(DWORD dwTlsIndex)
{
__asm {
	mov     ecx, dwTlsIndex
	mov     edx, fs:18h // Thread Info Block
	cmp     ecx, 50h
	jnb     __error
	mov     eax, [edx+ecx*4+88h]
	mov     dword ptr [edx+60h], NO_ERROR
__exit:
	retn    4
__error:
	mov     dword ptr [edx+60h], ERROR_INVALID_PARAMETER //Me:[edx+74h]
	xor     eax, eax
	jmp     __exit
	}
}
*/

/* ORIGINAL UNALTERED FUNCTION AS REFERENCE
__declspec(naked)
BOOL WINAPI TlsSetValue_new(DWORD dwTlsIndex, LPVOID lpTlsValue)
{
__asm {
	mov     ecx, dwTlsIndex
	mov     edx, fs:18h // Thread Info Block
	cmp     ecx, 50h
	jnb     __error
	mov     eax, lpTlsValue
	mov     [edx+ecx*4+88h], eax
	mov     eax, 1
__exit:
	retn    8
__error:
	mov     dword ptr [edx+60h], ERROR_INVALID_PARAMETER //Me:[edx+74h]
	xor     eax, eax
	jmp     __exit
	}
}
*/
