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

#ifdef _ENABLE_APIHOOK

#include "apihook.h"
#include "resolver.h"
#include "internals.h"
#include "ModInit.h"
#include "ProcessStorage.h"
#include "debug.h"

extern "C" int snprintf(char*, size_t, const char*, ...);

PROC apihook::prepare(BOOL is_static)
{
	typedef int (*init_once_t)(void);

	HMODULE hApiHookDll;
	MODREF* mr;
	PROC ah_reg;
	init_once_t init_once;

	DBGPRINTF(("Preparing API HOOK DLL\n"));

	_EnterSysLevel(krnl32lock);
	mr = MRLoadTree("kexApiHook.dll");
	if (!mr)
	{
		char path[MAX_PATH];
		FreeLibRemove();
		get_default_dll_path(path, sizeof(path));
		mr = MRLoadTree(path);
	}
	if (mr)
	{
		IMTE** pmteModTable = *ppmteModTable;
		IMTE_KEX* imte = (IMTE_KEX*) pmteModTable[mr->mteIndex];
		hApiHookDll = (HMODULE) imte->pNTHdr->OptionalHeader.ImageBase;
	}
	else
	{
		FreeLibRemove();
		hApiHookDll = NULL;
	}
	_LeaveSysLevel(krnl32lock);

	//this will also create heap for us if one isn't there
	ModuleInitializer* mi = ModuleInitializer::get_instance(true);

	if (hApiHookDll)
	{
		init_once = (init_once_t) iGetProcAddress(hApiHookDll, "kexApiHook_initonce");
		__try
		{
			if (!init_once || !init_once())
				hApiHookDll = NULL;
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			OutputDebugString("KernelEx: API HOOK DLL crashed during init");
			hApiHookDll = NULL;
		}
	}

	if (hApiHookDll && !is_static)
	{
		DBGPRINTF(("Explicitly initializing APIHOOK [PID=%08x]\n", 
				GetCurrentProcessId()));
		if (FLoadTreeNotify(mr, FALSE))
		{
			FreeLibTree(mr);
			hApiHookDll = NULL;
		}
	}

	if (hApiHookDll)
		ah_reg = iGetProcAddress(hApiHookDll, "kexApiHook_register");

	if (!hApiHookDll || !init_once || !ah_reg)
	{
		if (mr)
		{
			_EnterSysLevel(krnl32lock);
			FreeLibTree(mr);
			_LeaveSysLevel(krnl32lock);
		}
		return NULL;
	}

	mi->add_module(mr);

	return ah_reg;
}

VOID apihook::get_default_dll_path(LPSTR buf, DWORD len)
{
	HKEY key;
	DWORD type;
	LONG result;
	DWORD len2 = len;

	result = RegOpenKey(HKEY_LOCAL_MACHINE, "Software\\KernelEx", &key);
	if (result == ERROR_SUCCESS)
	{
		result = RegQueryValueEx(key, "ApiHookPath", NULL, &type, (BYTE*)buf, &len2);
		RegCloseKey(key);
	}
	if (result != ERROR_SUCCESS || type != REG_SZ)
	{
		snprintf(buf, len, "%s%s", (LPCSTR) kernelex_dir, "kexApiHook.dll");
	}
}

PROC apihook::hook(BOOL is_static, LPCSTR caller, LPCSTR target, LPCSTR api, PROC orig)
{
	typedef PROC (*apihook_register_t)(LPCSTR, LPCSTR, LPCSTR, PROC);
	static const int psidx = ProcessStorage::allocate();

	ProcessStorage* ps = ProcessStorage::get_instance();
	apihook_register_t ah_reg = (apihook_register_t) ps->get(psidx);

	if (!ah_reg)
	{
		//to avoid recursion in hook dll, disable the hook first
		ps->set(psidx, (void*) -1);
		ah_reg = (apihook_register_t) prepare(is_static);
		if (!ah_reg)
		{
			OutputDebugString("KernelEx: Couldn't locate API HOOK DLL or DLL failed to init.");
			ah_reg = (apihook_register_t)-1;
		}
		else
		{
			ps->set(psidx, (void*) ah_reg);
		}
	}
	if ((int)ah_reg == -1)
	{
		//failed once - so run normally as promised
		return orig;
	}

	__try
	{
		return ah_reg(caller, target, api, orig);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		OutputDebugString("KernelEx: API HOOK DLL crashed. Destroying process");
		return NULL;
	}
}

#endif
