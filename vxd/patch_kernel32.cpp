/*
 *  KernelEx
 *  Copyright (C) 2011, Xeno86
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

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "pemanip.h"
#include "patch_kernel32.h"
#include "interface.h"
#include "debug.h"

#define CODE_SEG ".text"
#define DATA_SEG ".data"
#define INIT_SEG "_INIT"

Patch_kernel32::Patch_kernel32(PEmanip& pem) : pefile(pem)
{
	stub_address = 0;

	code_seg_start = (DWORD) pefile.GetSectionByName(CODE_SEG);
	code_seg_size = pefile.GetSectionSize(CODE_SEG);

	data_seg_start = (DWORD) pefile.GetSectionByName(DATA_SEG);
	data_seg_size = pefile.GetSectionSize(DATA_SEG);
}

bool Patch_kernel32::apply()
{
	if (find_signature())
	{
		DBGPRINTF(("KERNEL32 already patched by previous KernelEx version"));
		DBGPRINTF(("Make sure pure KERNEL32 is installed before proceeding"));
		return false;
	}

	DBGPRINTF(("Preparing KERNEL32 for KernelEx... "));

	if (!find_ExportFromX())
		return false;
	if (!find_IsKnownDLL())
		return false;
	if (!find_FLoadTreeNotify1())
		return false;
	if (!find_FLoadTreeNotify2())
		return false;
	if (!prepare_subsystem_check())
		return false;
	if (!find_resource_check1())
		return false;
	if (!find_resource_check2())
		return false;
	if (!disable_named_and_rcdata_resources_mirroring())
		return false;
	if (!mod_imte_alloc())
		return false;
	if (!mod_mr_alloc())
		return false;
	if (!mod_pdb_alloc())
		return false;
	if (!create_stubs())
		return false;

	DBGPRINTF(("... all done!"));

	return true;
}

bool Patch_kernel32::find_signature()
{
	DWORD ptr;
	KernelEx_dataseg* dseg = NULL;
	DWORD sign_len = sizeof(KEX_SIGNATURE) - 1;

	ptr = data_seg_start + data_seg_size - sign_len;
	ptr = PEmanip::ALIGN(ptr, sizeof(DWORD));

	while (ptr >= data_seg_start)
	{
		if (!memcmp((void*) ptr, KEX_SIGNATURE, sign_len))
		{
			dseg = (KernelEx_dataseg*) ptr;
			break;
		}
		ptr -= sizeof(DWORD);
	}

	return dseg != NULL;
}

bool Patch_kernel32::prepare_subsystem_check()
{
	static const short pattern[] = {
		0x66,0x8b,0x46,0x48,0x66,0x3d,0x04,0x00,0x0f,0x87,0x9c,0x01,0x00,0x00,
		0x75,0x0b,0x66,0x83,0x7e,0x4a,0x0a,0x0f,0x87,0x8f,0x01,0x00,0x00,0x66,
		0x81,0x7e,0x04,0x4c,0x01,
	};
	static const short after[] = {
		0x66,0x83,0x7E,0x48,0x04,0x75,0x05,0x66,0x83,0x7E,0x4A,0x0A,0xE9,0x00,
		0x00,0x00,0x00,0x0F,0x87,0x93,0x01,0x00,0x00,0x90,0x90,0x90,0x90,0x66,
		0x81,0x7e,0x04,0x4c,0x01,
	};

	int length = sizeof(pattern) / sizeof(short);
	DWORD found_loc;
	int found = find_pattern(code_seg_start, code_seg_size, pattern, length, &found_loc);
	if (found != 1)
	{
		if (!found) ShowError(IDS_NOPAT, "subsystem_check");
		else ShowError(IDS_MULPAT, "subsystem_check");
		return false;
	}
	DBGPRINTF(("%s: pattern found @ 0x%08x", "subsystem_check", found_loc));
	set_pattern(found_loc, after, length);
	
	SubsysCheck_jmp = found_loc + 12;
	_SubsysCheckPerform = decode_jmp(SubsysCheck_jmp, 5);
	return true;
}

bool Patch_kernel32::find_resource_check1()
{
	static const short pattern[] = {
		0x6A,0x00,0x8D,0x45,0xE0,0x50,0xFF,0x75,0xA4,0xFF,0x75,0xD0,0x8D,0x45,
		0xCC,0x50,0xFF,0x75,0x08,0xE8,-2,-2,-2,-2,0x85,0xC0
	};

	int length = sizeof(pattern) / sizeof(short);
	DWORD found_loc;
	int found = find_pattern(code_seg_start, code_seg_size, pattern, length, &found_loc);
	if (found != 1)
	{
		if (!found) ShowError(IDS_NOPAT, "resource_check1");
		else ShowError(IDS_MULPAT, "resource_check1");
		return false;
	}
	DBGPRINTF(("%s: pattern found @ 0x%08x", "resource_check1", found_loc));
	GetOrdinal_call1 = found_loc + 19;
	_GetOrdinal = decode_call(GetOrdinal_call1, 5);
	return true;
}

bool Patch_kernel32::find_resource_check2()
{
	static const short pattern[] = {
		0x66,0x8B,0x45,0xE0,0x66,0x2D,0x06,0x80,0x66,0x3D,0x01,0x00,0x1B,0xC0,
		0xF7,0xD8,0x50,0x8D,0x45,0xDE,0x50,0xFF,0x75,  -1,0xFF,0x75,0xD0,0x8D,
		0x45,0xCC,0x50,0xFF,0x75,0x08,0xE8,  -2,  -2,  -2,  -2,0x85,0xC0
	};

	int length = sizeof(pattern) / sizeof(short);
	DWORD found_loc;
	int found = find_pattern(code_seg_start, code_seg_size, pattern, length, &found_loc);
	if (found != 1)
	{
		if (!found) ShowError(IDS_NOPAT, "resource_check2");
		else ShowError(IDS_MULPAT, "resource_check2");
		return false;
	}
	DBGPRINTF(("%s: pattern found @ 0x%08x", "resource_check2", found_loc));
	GetOrdinal_call2 = found_loc + 34;
	return true;
}

//no named/rcdata resource types mirroring
bool Patch_kernel32::disable_named_and_rcdata_resources_mirroring()
{
	static const short pattern[] = {
		0x66,0x8B,0x40,0x0E,0x66,0x89,0x45,0xDA,0x8B,0x45,  -1,0x66,0x8B,0x48,
		0x0C,0x66,0x89,0x4D,0xD8,0x66,0x8B,0x45,0xE0,0x8B,0x4D,0xD4,0x66,0x89,
		0x01,0x83,0x45,0xD4,0x02,0x66,0x8B,0x45,0xDA,0x66,0x03,0x45,0xD8,0x66,
		0x89,0x45,0x8C,0x8B,0x4D,0xD4,0x66,0x89,0x01,0x83,0x45,0xD4,0x02
	};
	static const short after[] = {
		0x66,0x8B,0x48,0x0E,0x66,0x03,0x48,0x0C,0x66,0x89,0x4D,0x8C,0x8B,0x45,
		0xA4,0x83,0x38,0x0A,0x74,0x40,0x83,0x38,0x00,0x79,0x04,0x3B,0xC0,0xEB,
		0x37,0x66,0x8B,0x45,0xE0,0x8B,0x4D,0xD4,0x66,0x89,0x01,0x83,0xC1,0x02,
		0x66,0x8B,0x45,0x8C,0x66,0x89,0x01,0x83,0xC1,0x02,0x89,0x4D,0xD4
	};

	int length = sizeof(pattern) / sizeof(short);
	DWORD found_loc;
	int found = find_pattern(code_seg_start, code_seg_size, pattern, length, &found_loc);
	if (found != 1)
	{
		if (!found) ShowError(IDS_NOPAT, "disable_named_and_rcdata_resources_mirroring");
		else ShowError(IDS_MULPAT, "disable_named_and_rcdata_resources_mirroring");
		return false;
	}
	DBGPRINTF(("%s: pattern found @ 0x%08x", "disable_named_and_rcdata_resources_mirroring", found_loc));
	set_pattern(found_loc, after, length);
	return true;
}

bool Patch_kernel32::mod_imte_alloc()
{
	//VA BFF8745C, RVA 1745C, file 15A5C, sec E45C
	static const short pattern[] = {
		0x66,0xff,0x05,-1,-1,-1,0xbf,0x6a,0x3c,0xe8,
	};
	static const short after[] = {
		0x66,0xff,0x05,-1,-1,-1,0xbf,0x6a,0x40,0xe8,
	};

	int length = sizeof(pattern) / sizeof(short);
	DWORD found_loc;
	int found = find_pattern(code_seg_start, code_seg_size, pattern, length, &found_loc);
	if (found != 1)
	{
		if (!found) ShowError(IDS_NOPAT, "mod_imte_alloc");
		else ShowError(IDS_MULPAT, "mod_imte_alloc");
		return false;
	}
	DBGPRINTF(("%s: pattern found @ 0x%08x", "mod_imte_alloc", found_loc));
	set_pattern(found_loc, after, length);
	return true;
}

bool Patch_kernel32::mod_mr_alloc()
{
	static const short pattern[] = {
		0x75,0xF6,0x8D,0x04,-1,0x1C,0x00,0x00,0x00,0x50
	};
	static const short after[] = {
		0x75,0xF6,0x8D,0x04,-1,0x24,0x00,0x00,0x00,0x50
	};

	int length = sizeof(pattern) / sizeof(short);
	DWORD found_loc;
	int found = find_pattern(code_seg_start, code_seg_size, pattern, length, &found_loc);
	if (found != 1)
	{
		if (!found) ShowError(IDS_NOPAT, "mod_mr_alloc");
		else ShowError(IDS_MULPAT, "mod_mr_alloc");
		return false;
	}
	DBGPRINTF(("%s: pattern found @ 0x%08x", "mod_mr_alloc", found_loc));
	set_pattern(found_loc, after, length);
	return true;
}

bool Patch_kernel32::mod_pdb_alloc()
{
	static const short pattern[] = {
		0x53,0x56,0x57,0x6A,0x06,0x68,0xC4,0x00,0x00,0x00,0xE8
	};
	static const short after[] = {
		0x53,0x56,0x57,0x6A,0x06,0x68,0xC8,0x00,0x00,0x00,0xE8
	};

	int length = sizeof(pattern) / sizeof(short);
	DWORD found_loc;
	int found = find_pattern(code_seg_start, code_seg_size, pattern, length, &found_loc);
	if (found != 1)
	{
		if (!found) ShowError(IDS_NOPAT, "mod_pdb_alloc");
		else ShowError(IDS_MULPAT, "mod_pdb_alloc");
		return false;
	}
	DBGPRINTF(("%s: pattern found @ 0x%08x", "mod_pdb_alloc", found_loc));
	set_pattern(found_loc, after, length);
	return true;
}

bool Patch_kernel32::find_ExportFromX()
{
	static const short pattern[] = {
		0x8B,0x0D,-1,-1,-1,-1,0x0F,0xBF,0x40,0x10,0x8B,0x14,0x81,0x8B,0x44,
		0x24,0x14,0x3D,0x00,0x00,0x01,0x00,0x8B,0x4A,0x04,0x73,0x15,-1,0x1D,
		-1,-1,-1,-1,0x75,0x04,0x6A,0x32,0xEB,0x3E,0x50,0x51,0xE8,-2,-2,-2,-2,
		0xEB,0x0C,0xFF,0x74,0x24,0x14,0x6A,0x00,0x51,0xE8,-2,-2,-2,-2,0x83,
		0x7F,0x54,0x00,0x8B,0xF0
	};

	int length = sizeof(pattern) / sizeof(short);
	DWORD found_loc;
	int found = find_pattern(code_seg_start, code_seg_size, pattern, length, &found_loc);
	if (found != 1)
	{
		if (!found) ShowError(IDS_NOPAT, "exportfromX");
		else ShowError(IDS_MULPAT, "exportfromX");
		return false;
	}
	DBGPRINTF(("%s: pattern found @ 0x%08x", "exportfromX", found_loc));
	gpa_ExportFromOrdinal_call = found_loc + 0x29;
	gpa_ExportFromName_call = found_loc + 0x37;
	_ExportFromOrdinal = decode_call(gpa_ExportFromOrdinal_call, 5);
	_ExportFromName = decode_call(gpa_ExportFromName_call, 5);
	DWORD start = _ExportFromName;
	for (DWORD a = start ; a < start + 0x100 ; a++)
	{
		if (is_call_ref(a, _ExportFromOrdinal))
		{
			EFN_EFO_call = a;
			return true;
		}
	}
	DBGPRINTF(("EFN_EFO_call not found"));
	return false;
}

bool Patch_kernel32::find_IsKnownDLL()
{
	static const short pattern[] = {
		0xFF,0x75,0xFC,0x8D,0x8D,0xF0,0xFE,0xFF,0xFF,0x51,0xE8,-2,-2,-2,-2,
		0x85,0xC0,0x75,0x1E,0x8D,0x85,0xE8,0xFD,0xFF,0xFF,
		0x8D,0x8D,0xF0,0xFE,0xFF,0xFF,0x50,0x51
	};

	int length = sizeof(pattern) / sizeof(short);
	DWORD found_loc;
	int found = find_pattern(code_seg_start, code_seg_size, pattern, length, &found_loc);
	if (found != 1)
	{
		if (!found) ShowError(IDS_NOPAT, "IsKnownDLL");
		else ShowError(IDS_MULPAT, "IsKnownDLL");
		return false;
	}
	DBGPRINTF(("%s: pattern found @ 0x%08x", "IsKnownDLL", found_loc));
	IsKnownDLL_call = found_loc + 10;
	_IsKnownDLL = decode_call(IsKnownDLL_call, 5);
	return true;
}

bool Patch_kernel32::find_FLoadTreeNotify1()
{
	static const short pattern[] = {
		0x56,0xA1,-1,-1,-1,-1,0x6A,0x01,0x8B,0x08,0xFF,0xB1,0x98,0x00,0x00,
		0x00,0xE8,-2,-2,-2,-2,0x83,0xF8,0x01,0x1B,0xF6,0xF7,0xDE
	};

	int length = sizeof(pattern) / sizeof(short);
	DWORD found_loc;
	int found = find_pattern(code_seg_start, code_seg_size, pattern, length, &found_loc);
	if (found != 1)
	{
		if (!found) ShowError(IDS_NOPAT, "FLoadTreeNotify1");
		else ShowError(IDS_MULPAT, "FLoadTreeNotify1");
		return false;
	}
	DBGPRINTF(("%s: pattern found @ 0x%08x", "FLoadTreeNotify1", found_loc));
	FLoadTreeNotify_call1 = found_loc + 16;
	_FLoadTreeNotify = decode_call(FLoadTreeNotify_call1, 5);
	return true;
}

bool Patch_kernel32::find_FLoadTreeNotify2()
{
	static const short pattern[] = {
		0x6A,0x00,0x57,0xE8,-1,-1,-1,-1,0x6A,0x00,0x56,0xE8,-2,-2,-2,-2,
		0x85,0xC0,0x74,0x12,0x56,0xE8,-1,-1,-1,-1,0x68,0x5A,0x04,0x00,0x00,
		0x33,0xF6,0xE8,-1,-1,-1,-1
	};

	int length = sizeof(pattern) / sizeof(short);
	DWORD found_loc;
	int found = find_pattern(code_seg_start, code_seg_size, pattern, length, &found_loc);
	if (found != 1)
	{
		if (!found) ShowError(IDS_NOPAT, "FLoadTreeNotify2");
		else ShowError(IDS_MULPAT, "FLoadTreeNotify2");
		return false;
	}
	DBGPRINTF(("%s: pattern found @ 0x%08x", "FLoadTreeNotify2", found_loc));
	FLoadTreeNotify_call2 = found_loc + 11;
	return true;
}

bool Patch_kernel32::create_stubs()
{
	KernelEx_codeseg* cseg;
	KernelEx_dataseg* dseg;
	if (!pefile.AllocSectionSpace(CODE_SEG, 
			sizeof(KernelEx_codeseg), (void**) &cseg, sizeof(DWORD)))
	{
		ShowError(IDS_FAILSEC, CODE_SEG);
		return false;
	}
	if (!pefile.AllocSectionSpace(DATA_SEG,
			sizeof(KernelEx_dataseg), (void**) &dseg, sizeof(DWORD)))
	{
		ShowError(IDS_FAILSEC, DATA_SEG);
		return false;
	}

	pagelock(pefile.GetImageBase(), 1);
	pagelock((DWORD) cseg, sizeof(KernelEx_codeseg));
	pagelock((DWORD) dseg, sizeof(KernelEx_dataseg));

	stub_address = dseg;

	memcpy(cseg->signature, "KrnlEx", 6);
	cseg->version = KEX_STUB_VER;
	for (int i = 0 ; i < JTAB_SIZE ; i++)
	{
		cseg->jmp_stub[i].opcode = 0x25ff;
		cseg->jmp_stub[i].addr = (DWORD) &dseg->jtab[i];
		cseg->jmp_stub[i].nop = 0x9090;
	}

	memcpy(dseg->signature, "KrnlEx", 6);
	dseg->version = KEX_STUB_VER;
	dseg->jtab[JTAB_EFO_DYN] = _ExportFromOrdinal;
	dseg->jtab[JTAB_EFO_STA] = _ExportFromOrdinal;
	dseg->jtab[JTAB_EFN_DYN] = _ExportFromName;
	dseg->jtab[JTAB_EFN_STA] = _ExportFromName;
	dseg->jtab[JTAB_KNO_DLL] = _IsKnownDLL;
	dseg->jtab[JTAB_FLD_TRN] = _FLoadTreeNotify;
	dseg->jtab[JTAB_SYS_CHK] = _SubsysCheckPerform;
	dseg->jtab[JTAB_RES_CHK] = _GetOrdinal;

	//exportfromx patch
	int efo_cnt = 0;
	int efn_cnt = 0;

	for (DWORD a = code_seg_start ; a < code_seg_start + code_seg_size ; a++)
	{
		if (is_call_ref(a, _ExportFromOrdinal))
		{
			DWORD file_loc = a;
			if (a == EFN_EFO_call) 
				continue;
			if (a == gpa_ExportFromOrdinal_call)
				set_call_ref(file_loc, (DWORD) &cseg->jmp_stub[JTAB_EFO_DYN]);
			else
				set_call_ref(file_loc, (DWORD) &cseg->jmp_stub[JTAB_EFO_STA]);
			DBGPRINTF(("%s: address 0x%08x", "EFO", a));
			efo_cnt++;
		}
		else if (is_call_ref(a, _ExportFromName))
		{
			DWORD file_loc = a;
			if (is_fixupc(a))
				continue;
			if (a == gpa_ExportFromName_call)
				set_call_ref(file_loc, (DWORD) &cseg->jmp_stub[JTAB_EFN_DYN]);
			else
				set_call_ref(file_loc, (DWORD) &cseg->jmp_stub[JTAB_EFN_STA]);
			DBGPRINTF(("%s: address 0x%08x", "EFN", a));
			efn_cnt++;
		}
	}

	if (efo_cnt != 2 || efn_cnt != 2)
	{
		ShowError(IDS_ERRCHECK);
		return false;
	}

	//isknowndll patch
	set_call_ref(IsKnownDLL_call, (DWORD) &cseg->jmp_stub[JTAB_KNO_DLL]);
	DBGPRINTF(("%s: address 0x%08x", "KNO_DLL", IsKnownDLL_call));

	//FLoadTreeNotify patch
	set_call_ref(FLoadTreeNotify_call1, (DWORD) &cseg->jmp_stub[JTAB_FLD_TRN]);
	DBGPRINTF(("%s: address 0x%08x", "FLD_TRN1", FLoadTreeNotify_call1));
	set_call_ref(FLoadTreeNotify_call2, (DWORD) &cseg->jmp_stub[JTAB_FLD_TRN]);
	DBGPRINTF(("%s: address 0x%08x", "FLD_TRN2", FLoadTreeNotify_call2));

	//subsys check patch
	set_jmp_ref(SubsysCheck_jmp, (DWORD) &cseg->jmp_stub[JTAB_SYS_CHK]);
	DBGPRINTF(("%s: address 0x%08x", "SYS_CHK", SubsysCheck_jmp));

	//resource check patch
	set_call_ref(GetOrdinal_call1, (DWORD) &cseg->jmp_stub[JTAB_RES_CHK]);
	DBGPRINTF(("%s: address 0x%08x", "RES_CHK1", GetOrdinal_call1));
	set_call_ref(GetOrdinal_call2, (DWORD) &cseg->jmp_stub[JTAB_RES_CHK]);
	DBGPRINTF(("%s: address 0x%08x", "RES_CHK2", GetOrdinal_call2));

	return true;
}

bool Patch_kernel32::is_fixupc(DWORD addr)
{
	if (*(DWORD*)(addr - 5) == 0xDAC)
		return true;
	return false;
}
