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

#include "pemanip.h"
#include "debug.h"
#include <cstring>

PEmanip::PEmanip()
{
	ZeroInit();
}

PEmanip::PEmanip(const char* file_path, int add_size)
{
	ZeroInit();
	OpenFile(file_path, add_size);
}

PEmanip::PEmanip(void* memory)
{
	ZeroInit();
	OpenMemory(memory);
}

PEmanip::~PEmanip()
{
	if (file_buf)
		HeapFree(GetProcessHeap(), 0, file_buf);
	if (file_path)
		HeapFree(GetProcessHeap(), 0, file_path);
}

void PEmanip::ZeroInit()
{
	file_buf = NULL;
	file_path = NULL;
	MZh = NULL;
	PEh = NULL;
	section_hdrs = NULL;
	target_len = 0;
	max_target_len = 0;
	image_base = 0;
	has_target = false;
}

bool PEmanip::HasTarget()
{
	return has_target;
}

IMAGE_NT_HEADERS* PEmanip::GetPEHeader()
{
	return PEh;
}

int PEmanip::GetFileSize(const char* file_path)
{
	HANDLE file = CreateFile(file_path, GENERIC_READ, FILE_SHARE_READ, 
			NULL, OPEN_EXISTING, 0, NULL);
    int file_size = -1;
    if (file != INVALID_HANDLE_VALUE)
    {
		file_size = ::GetFileSize(file, NULL);
        CloseHandle(file);
    }
    return file_size;
}

bool PEmanip::LoadFile(const char* file_path, int add_size)
{
	void* buf = NULL;
	int buf_len;
	HANDLE file;
	int bytes_read;
	bool ret = false;

    if (!file_path || add_size < 0)
        return false;

    buf_len = GetFileSize(file_path);
    if (buf_len > 0)
    {
        buf = HeapAlloc(GetProcessHeap(), 0, buf_len + add_size);
        if (buf)
        {
			file = CreateFile(file_path, GENERIC_READ, FILE_SHARE_READ, 
					NULL, OPEN_EXISTING, 0, NULL);
			if (file != INVALID_HANDLE_VALUE)
			{
				if (ReadFile(file, buf, buf_len, (DWORD*) &bytes_read, NULL) 
						&& bytes_read == buf_len)
				{
					this->file_path = (char*) HeapAlloc(GetProcessHeap(), 
							0, strlen(file_path) + 1);
					this->target_len = buf_len;
					this->file_buf = buf;
					this->max_target_len = buf_len + add_size;
					if (this->file_path)
					{
						strcpy(this->file_path, file_path);
						ret = true;
					}
				}
				CloseHandle(file);
			}
        }
    }

	if (!ret && buf)
		HeapFree(GetProcessHeap(), 0, buf);

    return ret;
}

bool PEmanip::SaveFile(const char* file_path)
{
	HANDLE file;
	int bytes_written;
    bool ret = false;

    if (!file_path || !file_buf)
        return false;

	file = CreateFile(file_path, GENERIC_WRITE, 0, NULL, 
			CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file != INVALID_HANDLE_VALUE)
    {
		if (WriteFile(file, file_buf, target_len, (DWORD*) &bytes_written, NULL) 
				&& bytes_written == target_len)
        {
            ret = true;
        }
        CloseHandle(file);
    }

    return ret;
}

void PEmanip::Close()
{
	if (file_buf)
		HeapFree(GetProcessHeap(), 0, file_buf);
	if (file_path)
		HeapFree(GetProcessHeap(), 0, file_path);

	ZeroInit();
}

int PEmanip::GetBufferLen()
{
    return target_len;
}

bool PEmanip::SetBufferLen(int new_size)
{
    if (new_size >= max_target_len)
    	return false;

	if (new_size < target_len)
		return false;

	target_len = new_size;
	return true;
}

unsigned long PEmanip::ALIGN(unsigned long x, unsigned long y)
{
	return (x + y - 1) & (~(y - 1));
}

bool PEmanip::OpenFile(const char* file_path, int add_size)
{
	if (has_target)
		return false;

	if (LoadFile(file_path, add_size))
	{
		MZh = (IMAGE_DOS_HEADER*) file_buf;

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
				image_base = PEh->OptionalHeader.ImageBase;
				has_target = true;
				return true;
			}
		}
	}
	return false;
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
			max_target_len = target_len;
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

void* PEmanip::RvaToPointer(DWORD Rva)
{
	if (!file_buf)
		return (void*)(Rva + image_base);

	IMAGE_SECTION_HEADER* section = section_hdrs;

	for (int i = 0 ; i < PEh->FileHeader.NumberOfSections ; i++)
	{
		if ((Rva >= section->VirtualAddress) 
			&& (Rva < (section->VirtualAddress + section->Misc.VirtualSize)))
		{
			return (void*) (Rva - section->VirtualAddress 
				+ section->PointerToRawData + (DWORD) MZh);
		}
		section++;
	}
	return 0;
}

DWORD PEmanip::PointerToRva(void* pointer)
{
	if (!file_buf)
		return (DWORD) pointer - image_base;

	IMAGE_SECTION_HEADER* section = section_hdrs;
	DWORD offset;

	if (pointer < MZh)
		return 0;

	offset = (DWORD) pointer - (DWORD) MZh;

	for (int i = 0 ; i < PEh->FileHeader.NumberOfSections ; i++)
	{
		if ((offset >= section->PointerToRawData) 
			&& (offset < (section->PointerToRawData + section->SizeOfRawData)))
		{
			return offset - section->PointerToRawData 
				+ section->VirtualAddress;
		}
		section++;
	}
	return 0;
}

IMAGE_SECTION_HEADER* PEmanip::FindSectionByName(const char* name)
{
	IMAGE_SECTION_HEADER* section = section_hdrs;

	for (int i = 0 ; i < PEh->FileHeader.NumberOfSections ; i++)
	{
		if (!strnicmp((char*) section->Name, name, sizeof(section->Name)))
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

int PEmanip::GetSectionSize(const char* name)
{
	IMAGE_SECTION_HEADER* section = FindSectionByName(name);

	if (section)
		if (file_buf)
			return section->SizeOfRawData;
		else
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

	DBGPRINTF(("Bytes available: %d, ", available_space));
	DBGPRINTF(("Bytes needed: %d\n", needed_space));

	if (available_space < needed_space)
	{
		DBGPRINTF(("Not enough space in section!\n"));
		return false;
	}

	if (new_size > Section->SizeOfRawData)
	{
		int offset = ALIGN(new_size - Section->SizeOfRawData, 
				PEh->OptionalHeader.FileAlignment);
		
		DBGPRINTF(("Applying offset of %d\n", offset));

		void* block_start = RvaToPointer((Section + 1)->VirtualAddress);
		int block_size = GetBufferLen() - (Section + 1)->PointerToRawData;

		char* tmp = (char*)HeapAlloc(GetProcessHeap(), 0, block_size);
		memcpy(tmp, block_start, block_size);
		memcpy((void*)((int)block_start + offset), tmp, block_size);
		HeapFree(GetProcessHeap(), 0, tmp);
		memset(block_start, 0, offset);

		// update offsets
		for (int i = 0 ; i < PEh->FileHeader.NumberOfSections ; i++)
		{
			if (&section_hdrs[i] <= Section) 
				continue;
			section_hdrs[i].PointerToRawData += offset;
		}
		Section->SizeOfRawData += offset;
		if (!SetBufferLen(GetBufferLen() + offset)) 
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

DWORD PEmanip::GetImageBase()
{
	return image_base;
}
