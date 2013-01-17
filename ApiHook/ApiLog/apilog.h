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

#ifndef _APILOG_APILOG_H
#define _APILOG_APILOG_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

class ThreadAddrStack
{
public:
	ThreadAddrStack();
	static void __stdcall push_ret_addr(DWORD addr);
	static DWORD __stdcall pop_ret_addr();
	static DWORD __stdcall get_level();

private:
	int pos;
	DWORD stack[1023];
};

#pragma pack(push,1)


/* Creates a stub that calls address specified in the constructor. */
class redir_stub
{
public:
	redir_stub(unsigned long target, bool make_call = true)
	{
		op = make_call ? 0xe8 : 0xe9;
		addr = target - ((unsigned long)(this) + 5);
	}

private:
	unsigned char op;
	unsigned long addr;
};



/* Creates a stub for api logging. */
class log_stub
{
public:
	log_stub(const char* source, const char* target, const char* name, 
			unsigned long proc);

private:
	struct log_data
	{
		const char* source;
		const char* target;
		const char* api_name;
	};

	static void __stdcall pre_log(log_data* lgd);
	static void __stdcall post_log(log_data* lgd, DWORD retval);

/*
	pushad
	push  lgd
	call  pre_log@4
	popad

	add   esp, 4
	call  orig
	sub   esp, 4

	pushad
	push  eax
	push  lgd
	call  post_log@8
	popad
	ret
*/

	BYTE c_pushad1;
	BYTE c_push1;
	log_data* v_lgd1;
	redir_stub call_prelog;
	BYTE c_popad1;

	WORD c_add_esp;
	BYTE c_byte_4;
	redir_stub call_orig;
	WORD c_sub_esp;
	BYTE c_byte_4_1;
	
	BYTE c_pushad2;
	BYTE c_push_eax;
	BYTE c_push2;
	log_data* v_lgd2;
	redir_stub call_postlog;
	BYTE c_popad2;
	BYTE c_ret;

	log_data lgd;
};

#pragma pack(pop)

#endif
