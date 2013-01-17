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

#ifndef _SETUP_PEMANIP_H
#define _SETUP_PEMANIP_H

#include <windows.h>
#include <winnt.h>

class PEmanip
{
private:
	bool has_target;
	char* file_path;
	void* file_buf;
	int target_len;
	int max_target_len;
	IMAGE_DOS_HEADER* MZh;
	IMAGE_NT_HEADERS* PEh;
	IMAGE_SECTION_HEADER* section_hdrs;
	DWORD image_base;

	void ZeroInit();
	int GetFileSize(const char* file_path);
	bool LoadFile(const char* file_path, int add_size);
	int GetBufferLen();
	bool SetBufferLen(int new_size);
	static unsigned long ALIGN(unsigned long x, unsigned long y);
	IMAGE_SECTION_HEADER* FindSectionByName(const char* name);

public:
	PEmanip();
	PEmanip(const char* file_path, int add_size = 0);
	PEmanip(void* memory);
	~PEmanip();
	bool HasTarget();
	bool OpenFile(const char* file_path, int add_size = 0);
	bool OpenMemory(void* memory);
	bool SaveFile(const char* file_path);
	void Close();
	IMAGE_NT_HEADERS* GetPEHeader();
	IMAGE_SECTION_HEADER* RvaToSection(DWORD Rva);
	void* RvaToPointer(DWORD Rva);
	DWORD PointerToRva(void* pointer);
	void* GetSectionByName(const char* name);
	int GetSectionSize(const char* name);
	bool AllocSectionSpace(const char* name, int needed_space, void** ptr, int align = 1);
	DWORD GetExportedAPI(const char* func);
	DWORD GetImageBase();
};

#endif
