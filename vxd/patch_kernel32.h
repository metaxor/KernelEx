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

#ifndef __PATCH_KERNEL32_H
#define __PATCH_KERNEL32_H

#include "patch.h"
#include "interface.h"

class PEmanip;

class Patch_kernel32 : public Patch
{
public:
	Patch_kernel32(PEmanip& pem);
	bool apply();
	
	KernelEx_dataseg* stub_address;

private:
	bool find_signature();
	bool prepare_subsystem_check();
	bool find_resource_check1();
	bool find_resource_check2();
	bool disable_named_and_rcdata_resources_mirroring();
	bool mod_imte_alloc();
	bool mod_mr_alloc();
	bool mod_pdb_alloc();
	bool find_ExportFromX();
	bool find_IsKnownDLL();
	bool find_FLoadTreeNotify1();
	bool find_FLoadTreeNotify2();
	bool create_stubs();
	bool is_fixupc(DWORD addr);

	PEmanip& pefile;
	DWORD code_seg_start;
	DWORD code_seg_size;
	DWORD data_seg_start;
	DWORD data_seg_size;

	DWORD _GetOrdinal;
	DWORD _ExportFromOrdinal;
	DWORD _ExportFromName;
	DWORD _IsKnownDLL;
	DWORD _FLoadTreeNotify;
	DWORD _SubsysCheckPerform;
	DWORD GetOrdinal_call1;
	DWORD GetOrdinal_call2;
	DWORD gpa_ExportFromOrdinal_call;
	DWORD gpa_ExportFromName_call;
	DWORD EFN_EFO_call;
	DWORD IsKnownDLL_call;
	DWORD FLoadTreeNotify_call1;
	DWORD FLoadTreeNotify_call2;
	DWORD SubsysCheck_jmp;
};

#endif
