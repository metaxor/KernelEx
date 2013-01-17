/*
 *  KernelEx
 *  Copyright (C) 2008-2009, 2011, Xeno86
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

#include "pemanip.h"
#include "debug.h"
#include <cstring>

extern "C" {
#define WANTVXDWRAPS
#include <vmm.h>
#include <vxdwraps.h>
};

PEmanip::PEmanip()
{
	ZeroInit();
}

PEmanip::PEmanip(void* memory)
{
	ZeroInit();
	OpenMemory(memory);
}

void PEmanip::ZeroInit()
{
	MZh = NULL;
	PEh = NULL;
	section_hdrs = NULL;
	target_len = 0;
	image_base = 0;
	has_target = false;
}

void PEmanip::Close()
{
	ZeroInit();
}

bool PEmanip::OpenMemory(void* memory)
{
	if (has_target)
		return false;

	MZh = (IMAGE_DOS_HEADER*) memory;

	if (MZh->e_magic == IMAGE_DOS_SIGNATURE)
	{
		PEh = (IMAGE_NT_HEADERS*) ((DWORD) MZh + MZh->e_lfanew);
		if ((PEh->Signature == IMAGE_NT_SIGNATURE)
			&& (PEh->FileHeader.Machine == IMAGE_FILE_MACHINE_I386)
			&& (PEh->OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR_MAGIC))
		{
			section_hdrs = (IMAGE_SECTION_HEADER*)
				((DWORD) PEh
				+ PEh->FileHeader.SizeOfOptionalHeader
				+ sizeof(IMAGE_FILE_HEADER)
				+ sizeof(DWORD));
			image_base = (DWORD)memory;
			has_target = true;
			target_len = PEh->OptionalHeader.SizeOfImage;
			return true;
		}
	}
	return false;
}

IMAGE_SECTION_HEADER* PEmanip::RvaToSection(DWORD Rva)
{
	IMAGE_SECTION_HEADER* section = section_hdrs;

	for (int i = 0 ; i < PEh->FileHeader.NumberOfSections ; i++)
	{
		if ((Rva >= section->VirtualAddress) 
			&& (Rva < (section->VirtualAddress + section->Misc.VirtualSize)))
		{
			return section;
		}
		section++;
	}
	return NULL;
}

IMAGE_SECTION_HEADER* PEmanip::FindSectionByName(const char* name)
{
	IMAGE_SECTION_HEADER* section = section_hdrs;

	for (int i = 0 ; i < PEh->FileHeader.NumberOfSections ; i++)
	{
		if (!strncmp((char*) section->Name, name, sizeof(section->Name)))
			return section;
		section++;
	}
	return NULL;
}

void* PEmanip::GetSectionByName(const char* name)
{
	IMAGE_SECTION_HEADER* section = FindSectionByName(name);
	
	if (section)
		return RvaToPointer(section->VirtualAddress);
	return NULL;
}

DWORD PEmanip::GetSectionSize(const char* name)
{
	IMAGE_SECTION_HEADER* section = FindSectionByName(name);

	if (section)
		return section->Misc.VirtualSize;
	return 0;
}

bool PEmanip::AllocSectionSpace(const char* name, int needed_space, void** ptr, int align)
{
	IMAGE_SECTION_HEADER* Section = FindSectionByName(name);

	if (!Section || align < 1 || needed_space < 0)
		return false;

	if (!needed_space)
		return true;

	DWORD current_size = ALIGN(Section->Misc.VirtualSize, align);
	DWORD new_size = current_size + needed_space;

	DWORD max_size = (Section + 1)->VirtualAddress - Section->VirtualAddress;
	int available_space = max_size - current_size;

	DBGPRINTF(("Allocating space in section '%s' avail: %d  needed: %d",
			name, available_space, needed_space));

	if (available_space < needed_space)
	{
		DBGPRINTF(("Not enough space in section!"));
		return false;
	}

	Section->Misc.VirtualSize = new_size;

	if (ptr)
		*ptr = RvaToPointer(Section->VirtualAddress + current_size);

	return true;
}

DWORD PEmanip::GetExportedAPI(const char* func)
{
	IMAGE_EXPORT_DIRECTORY* Exports = (IMAGE_EXPORT_DIRECTORY*) 
		RvaToPointer(PEh->OptionalHeader
		.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);

	if (Exports)
	{
		DWORD* Names = (DWORD*) RvaToPointer(Exports->AddressOfNames);

		for (DWORD i = 0 ; i < Exports->NumberOfNames ; i++)
		{
			if (!strcmp(func, (char*) RvaToPointer(*Names)))
			{
				WORD* OrdinalTable = (WORD*) RvaToPointer(Exports->AddressOfNameOrdinals);
				WORD Index = OrdinalTable[i];
				DWORD* FunctionTable = (DWORD*) RvaToPointer(Exports->AddressOfFunctions);
				DWORD Address = (DWORD) FunctionTable[Index];
				return Address;
			}
			Names++;
		}
	}

	return 0;
}

void* PEmanip::CreateBackup()
{
	if (!has_target)
		return NULL;
	void* mem = _HeapAllocate(target_len, HEAPSWAP);
	memcpy(mem, MZh, target_len);
	return mem;
}
