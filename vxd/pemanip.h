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

#ifndef _SETUP_PEMANIP_H
#define _SETUP_PEMANIP_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winnt.h>

class PEmanip
{
public:
	PEmanip();
	PEmanip(void* memory);
	bool HasTarget();
	bool OpenMemory(void* memory);
	void Close();
	IMAGE_NT_HEADERS* GetPEHeader();
	IMAGE_SECTION_HEADER* RvaToSection(DWORD Rva);
	void* RvaToPointer(DWORD Rva);
	DWORD PointerToRva(void* pointer);
	void* GetSectionByName(const char* name);
	DWORD GetSectionSize(const char* name);
	bool AllocSectionSpace(const char* name, int needed_space, void** ptr, int align = 1);
	DWORD GetExportedAPI(const char* func);
	DWORD GetImageBase();
	void* CreateBackup();
	static unsigned long ALIGN(unsigned long x, unsigned long y);

private:
	void ZeroInit();
	IMAGE_SECTION_HEADER* FindSectionByName(const char* name);

	bool has_target;
	int target_len;
	IMAGE_DOS_HEADER* MZh;
	IMAGE_NT_HEADERS* PEh;
	IMAGE_SECTION_HEADER* section_hdrs;
	DWORD image_base;
};

inline bool PEmanip::HasTarget()
{
	return has_target;
}

inline IMAGE_NT_HEADERS* PEmanip::GetPEHeader()
{
	return PEh;
}

inline unsigned long PEmanip::ALIGN(unsigned long x, unsigned long y)
{
	return (x + y - 1) & (~(y - 1));
}

inline DWORD PEmanip::GetImageBase()
{
	return image_base;
}

inline void* PEmanip::RvaToPointer(DWORD Rva)
{
	return (void*)(Rva + image_base);
}

inline DWORD PEmanip::PointerToRva(void* pointer)
{
	return (DWORD) pointer - image_base;
}

#endif
