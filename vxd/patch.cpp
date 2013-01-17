/*
 *  KernelEx
 *  Copyright (C) 2010-2011, Xeno86
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
#include <cassert>
#include "patch.h"
#include "debug.h"

extern "C" {
#define WANTVXDWRAPS
#include <vmm.h>
#include <vxdwraps.h>
};

#ifdef _DEBUG

const struct
{
	MessageID id;
	const char* desc;
}
MessageText[] =
{
    IDS_NOPAT,               "ERROR: %s: pattern not found",
    IDS_MULPAT,              "ERROR: %s: multiple pattern matches",
    IDS_FAILSEC,             "ERROR: Failed to allocate %s section memory",
    IDS_ERRCHECK,            "ERROR: Checksum error",
    IDS_WINVER,              "Incompatible Windows version",
    IDS_DOWNGRADE,           "Can't downgrade. Please uninstall currently installed version of KernelEx before continuing.",
};

#endif

void Patch::ShowError(MessageID id, ...)
{
#ifdef _DEBUG
	const char* format = NULL;
	char out[512];
	va_list vargs;
	
	va_start(vargs, id);
	for (int i = 0 ; i < sizeof(MessageText) / sizeof(MessageText[0]) ; i++)
	{
		if (MessageText[i].id == id)
		{
			format = MessageText[i].desc;
			break;
		}
	}

	assert(format != NULL);

	_Vsprintf(out, const_cast<char*>(format), vargs);
	va_end(vargs);
	strcat(out, "\n");
	debug_printf(out);
#endif
	abort();
}

int Patch::find_pattern(DWORD offset, int size, const short* pattern, 
						int pat_len, DWORD* found_loc)
{
	DWORD end_offset = offset + size - pat_len;
	int found = 0;

	for (DWORD i = offset ; i <= end_offset ; i++)
	{
		if (pattern[0] < 0 || *(unsigned char*)i == pattern[0])
		{
			int j;
			for (j = 1 ; j < pat_len ; j++)
			{
				if (pattern[j] >= 0 && *(unsigned char*)(i + j) != pattern[j]) 
					break;
			}
			if (j == pat_len) //pattern found
			{
				*found_loc = i;
				found++;
			}
		}
	}
	return found;
}

void Patch::set_pattern(DWORD loc, const short* new_pattern, int pat_len)
{
	unsigned char* offset = (unsigned char*) loc;
	const short* pat_ptr = new_pattern;

	while (pat_len--)
	{
		if (*pat_ptr != -1)
			*offset = *pat_ptr & 0xff;
		offset++;
		pat_ptr++;
	}

	pagelock(loc, pat_len);
}

DWORD Patch::decode_call(DWORD addr, int len)
{
	unsigned char* code = (unsigned char*)addr;

	/* CALL rel32 */
	if (code[0] == 0xe8)
	{
		if (len && len == 5 || !len)
		return (DWORD)((DWORD)code + 5 + *(INT32*)(code + 1));
	}
	/* CALL imm32 */
	else if (code[0] == 0xff && code[1] == 0x15)
	{
		if (len && len == 6 || !len)
		return *(DWORD*)(code + 2);
	}
	/* unmatched */
	return 0;
}

DWORD Patch::decode_jmp(DWORD addr, int len)
{
	unsigned char* code = (unsigned char*)addr;

	/* JMP rel8 */
	if (code[0] == 0xeb)
	{
		if (len && len == 2 || !len)
		return (DWORD)((DWORD)code + 2 + *(INT8*)(code + 1));
	}
	/* JMP rel32 */
	else if (code[0] == 0xe9)
	{
		if (len && len == 5 || !len)
		return (DWORD)((DWORD)code + 5 + *(INT32*)(code + 1));
	}
	/* JMP imm32 */
	else if (code[0] == 0xff && code[1] == 0x25)
	{
		if (len && len == 6 || !len)
		return *(DWORD*)(code + 2);
	}
	/* Jxx rel8 */
	else if (code[0] >= 0x70 && code[0] < 0x7f || code[0] == 0xe3)
	{
		if (len && len == 2 || !len)
		return (DWORD)((DWORD)code + 2 + *(INT8*)(code + 1));
	}
	/* Jxx rel32 */
	else if (code[0] == 0x0f && code[1] >= 0x80 && code[1] <= 0x8f)
	{
		if (len && len == 6 || !len)
		return (DWORD)((DWORD)code + 6 + *(INT32*)(code + 2));
	}
	/* unmatched */
	return 0;
}

bool Patch::is_call_ref(DWORD loc, DWORD target)
{
	DWORD addr = decode_call(loc, 5); // size of call imm32 opcode
	if (addr == target)
		return true;
	return false;
}

// Both addresses have to be from the same section!
void Patch::set_call_ref(DWORD loc, DWORD target)
{
	DWORD rel;

	assert(*(BYTE*) loc == 0xe8);

	rel = target - (loc + 5);
	*(DWORD*)(loc + 1) = rel;
	pagelock(loc + 1, sizeof(DWORD));
}

// Both addresses have to be from the same section!
void Patch::set_jmp_ref(DWORD loc, DWORD target)
{
	DWORD rel;
	unsigned char* code = (unsigned char*)loc;

	if (code[0] == 0xe9)
	{
		rel = target - (loc + 5);
		*(DWORD*)(loc + 1) = rel;
		pagelock(loc + 1, sizeof(DWORD));
	}
	else if (code[0] == 0x0f && code[1] >= 0x80 && code[1] <= 0x8f)
	{
		rel = target - (loc + 6);
		*(DWORD*)(loc + 2) = rel;
		pagelock(loc + 2, sizeof(DWORD));
	}
	else assert(false);
}

void Patch::pagelock(DWORD addr, DWORD count)
{
	_LinPageLock(PAGE(addr), NPAGES((addr & PAGEMASK) + count), 0);
}
