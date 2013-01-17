/*
 *  KernelEx
 *  Copyright (C) 2008-2011, Xeno86
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

#ifndef __RESOLVER_H
#define __RESOLVER_H

#include "kstructs.h"
#include "apilib.h"

/***** loader flags *****/
#define LDR_KEX_DISABLE          1  /* disable KernelEx API extensions for this module */
#define LDR_OVERRIDE_PROC_MOD    2  /* use same configuration and flags for all modules in a process */
#define LDR_HOOK_APIS            8  /* enable API tracing */
#define LDR_NO_INHERIT          16  /* don't inherit configuration and flags to child processes */
#define LDR_VALID_FLAG         128  /* denotes that flags field is valid */

#pragma pack(push,1)

class ApiConfiguration;

struct IMTE_KEX : public IMTE
{
	WORD            mod_index;  /* this value minus 1 is index into MODAPI table in API configurations
								 * 0xff00-0xfffe - api libraries
	                             * 0 - not checked, 0xffff - not an overridden module */
};

struct appsetting
{
	appsetting() : conf(NULL), flags(0) {}
	ApiConfiguration* conf;     /* pointer to API configuration used by the module */
	unsigned long flags;        /* resolver flags */
};


struct MODREF_KEX
{
	MODREF_KEX(PMODREF pmr) : mr(*pmr), 
		as(*(appsetting*)(mr.ImplicitImports + mr.cImportedModules)) {}
	MODREF& mr;
	appsetting& as;
};

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

struct config_params
{
	ApiConfiguration* apiconf;
#ifdef _ENABLE_APIHOOK
	bool hook_apis;
#endif
};

#pragma pack(pop)

extern LONG old_jtab[];

bool are_extensions_enabled();
bool are_extensions_enabled_module(const char* path);
DWORD encode_address(DWORD addr, const ApiLibrary* apilib);
PROC WINAPI GetK32OrdinalAddress(WORD wOrd);
PROC WINAPI iGetProcAddress(HMODULE hModule, LPCSTR lpProcName);
PROC WINAPI ExportFromOrdinal(IMTE_KEX* target, MODREF* caller, BOOL is_static, WORD ordinal);
PROC WINAPI ExportFromName(IMTE_KEX* target, MODREF* caller, BOOL is_static, WORD hint, LPCSTR name);

#ifdef _DEBUG
void dump_imtes(void);
#endif

#endif
