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

#ifndef __PATCH_H
#define __PATCH_H

enum MessageID
{
	IDS_NOPAT,
	IDS_MULPAT,
	IDS_FAILSEC,
	IDS_ERRCHECK,
	IDS_WINVER,
	IDS_DOWNGRADE,
};

class Patch
{
public:
	Patch() {}
	virtual bool apply() = 0;
	
protected:
	void ShowError(MessageID id, ...);
	static int find_pattern(DWORD offset, int size, const short* pattern, int pat_len, DWORD* found_loc);
	void set_pattern(DWORD loc, const short* new_pattern, int pat_len);
	DWORD decode_call(DWORD addr, int len = 0);
	DWORD decode_jmp(DWORD addr, int len = 0);
	bool is_call_ref(DWORD loc, DWORD target);
	void set_call_ref(DWORD loc, DWORD target);
	void set_jmp_ref(DWORD loc, DWORD target);
	void pagelock(DWORD addr, DWORD count);
};

#endif
