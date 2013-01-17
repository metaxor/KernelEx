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

/*
 * API logging DLL for KernelEx Core API hook infrastructure
 *
 * Make sure to disable API extensions for this module or else the DLL
 * will try to hooks its own imports.
 */

#include "apilog.h"
#include "apilog_params.h"
#include "writer.h"

extern "C" void * _AddressOfReturnAddress(void);
#pragma intrinsic(_AddressOfReturnAddress)

#define countof(x) (sizeof(x)/sizeof(x[0]))

#ifdef _DEBUG
#define DBGASSERT(e)  ((e) ? 0 : DebugBreak())
#else
#define DBGASSERT(e)  ((void)0)
#endif

extern "C"
int snprintf(char *buffer, size_t n, const char* format, ...);

int tlsindex = -1;
const bool apilog_enabled = true;

HINSTANCE hInst;

ThreadAddrStack::ThreadAddrStack()
{
	pos = 0;
}

void __stdcall ThreadAddrStack::push_ret_addr(DWORD addr)
{
	DBGASSERT(tlsindex != -1);
	ThreadAddrStack* tas = (ThreadAddrStack*) TlsGetValue(tlsindex);
	if (!tas)
	{
		tas = new ThreadAddrStack;
		TlsSetValue(tlsindex, tas);
	}
	tas->stack[tas->pos++] = addr;
	DBGASSERT(tas->pos < countof(tas->stack));
}

DWORD __stdcall ThreadAddrStack::pop_ret_addr()
{
	DBGASSERT(tlsindex != -1);
	ThreadAddrStack* tas = (ThreadAddrStack*) TlsGetValue(tlsindex);
	DBGASSERT(tas->pos > 0);
	return tas->stack[--tas->pos];
}

DWORD __stdcall ThreadAddrStack::get_level()
{
	DBGASSERT(tlsindex != -1);
	ThreadAddrStack* tas = (ThreadAddrStack*) TlsGetValue(tlsindex);
	return tas->pos;
}


log_stub::log_stub(const char* source, const char* target, const char* name, 
		unsigned long proc)
		: call_prelog(DWORD(pre_log)), call_postlog(DWORD(post_log)),
		call_orig(proc)
{
	c_pushad1 = c_pushad2 = 0x60;
	c_popad1 = c_popad2 = 0x61;
	c_ret = 0xc3;
	c_push1 = c_push2 = 0x68;
	v_lgd1 = &lgd;
	v_lgd2 = &lgd;
	c_push_eax = 0x50;
	c_add_esp = 0xc483;
	c_sub_esp = 0xec83;
	c_byte_4 = c_byte_4_1 = 4;
	lgd.source = source;
	lgd.target = target;
	lgd.api_name = name;
}

void __stdcall log_stub::pre_log(log_data* lgd)
{
	DWORD last_err;
	DWORD caller_addr;
	
	caller_addr = *((DWORD*) _AddressOfReturnAddress() + 10);
	last_err = GetLastError();
	
	ThreadAddrStack::push_ret_addr(caller_addr);
	
	if (apilog_enabled)
	{
		DWORD level;
		char msg[DEBUGMSG_MAXLEN];

		level = ThreadAddrStack::get_level();

		int z = snprintf(msg, sizeof(msg)-1, "%-2d|%x|%*s[%s]%08x:<%s>%s", 
				level,
				GetCurrentThreadId(),
				(level-1) * 2, "",
				lgd->source, caller_addr,
				lgd->target, lgd->api_name);

		va_list ap = va_list((DWORD*) &lgd + 10);
		int limit = sizeof(msg) - 1 - z;
		int zz = ApiLogParams::inst().decode_parameters(lgd->target, lgd->api_name, ap, msg + z, limit);
		if (zz > limit)
			z += limit;
		else
			z += zz;
		strcpy(msg + z, "\n");

		writer_fn(msg);
	}

	SetLastError(last_err);
}

void __stdcall log_stub::post_log(log_data* lgd, DWORD retval)
{
	DWORD last_err;
	DWORD& caller_addr = *((DWORD*) _AddressOfReturnAddress() + 11);
	
	last_err = GetLastError();
	
	caller_addr = ThreadAddrStack::pop_ret_addr();

	if (apilog_enabled)
	{
		DWORD level;
		char msg[DEBUGMSG_MAXLEN];

		level = ThreadAddrStack::get_level() + 1;

		snprintf(msg, sizeof(msg), "%-2d|%x|%*s[%s]%08x:<%s>%s|%x\n", 
				level,
				GetCurrentThreadId(),
				(level-1) * 2, "",
				lgd->source, caller_addr,
				lgd->target, lgd->api_name,
				retval);

		writer_fn(msg);
	}

	SetLastError(last_err);
}

HMODULE GetCurrentModule()
{
	MEMORY_BASIC_INFORMATION mbi;
	static int dummy;
	VirtualQuery(&dummy, &mbi, sizeof(mbi));
	return (HMODULE)mbi.AllocationBase;
}

void load_signature()
{
	char signature_file[MAX_PATH];

	GetModuleFileName(hInst, signature_file, MAX_PATH);
	char* p = strrchr(signature_file, '\\');
	if (!p) p = signature_file;
	else p++;
	*p = '\0';
	strcat(signature_file, "signature.dat");
	OutputDebugString("Loading apilog signatures... ");
	bool res = ApiLogParams::inst().load_signatures(signature_file);
	OutputDebugString(res ? "done.\n" : "error.\n");
}

bool filter_out(const char* target, const char* api)
{
	static const char* filtered_apis[] = 
	{
		"HeapAlloc",
		"HeapFree",
		"HeapReAlloc",
		"HeapSize",
		"GetProcessHeap",
		"TlsGetValue",
		"TlsSetValue",
		"InitializeCriticalSection",
		"ReinitializeCriticalSection",
		"DeleteCriticalSection",
		"EnterCriticalSection",
		"LeaveCriticalSection",
		"InterlockedIncrement",
		"InterlockedDecrement",
		"InterlockedExchange",
		"InterlockedExchangeAdd",
		"InterlockedCompareExchange",
	};
	if (HIWORD(api) && !strcmp(target, "KERNEL32.DLL"))
	{
		for (int i = 0 ; i < countof(filtered_apis) ; i++)
		{
			if (!strcmp(api, filtered_apis[i]))
				return true;
		}
	}
	return false;
}

/*
 * This function is called before any call to _register and BEFORE DllMain
 * in order to let you prepare your API hooks.
 * 
 * Return:
 * 1 for success
 * 0 in case of error - this will trigger unloading of the DLL
 *
 * WARNING: This function is called inside Kernel32Lock
 * so you have to be VERY careful what you call here if you don't want
 * to crash the system.
 */
int kexApiHook_initonce(void)
{
	hInst = GetCurrentModule();
	DBGASSERT(hInst != NULL);
	load_signature();
	return 1;
}

/*
 * This function is called  by the resolver for each API imported by a module.
 * This function is called before DllMain.
 *
 * Parameters:
 * caller - the module which imports the API (full path)
 * target - the module from which API is imported (full path)
 * api - the name of the API or ordinal number of the API (if upper word is zero)
 * orig - address of the API being hooked, can be NULL if resolver found no API
 *
 * Return:
 * orig - if you don't want to hook the API
 * address of the hook - if you want to intercept the API
 * NULL - if you want to disable the API altogether
 *
 * WARNING: This function is called inside Kernel32Lock
 * so you have to be VERY careful what you call here if you don't want
 * to crash the system.
 */
PROC kexApiHook_register(const char* caller, const char* target, const char* api, PROC orig)
{
	char* new_api;
	if (orig == NULL)
		return orig;

	//extract DLL file names
	char* p;
	p = strrchr(caller, '\\');
	if (p) caller = p + 1;
	p = strrchr(target, '\\');
	if (p) target = p + 1;

	if (filter_out(target, api))
		return orig;

	if (HIWORD(api)) //named export
	{
		new_api = strdup(api);
	}
	else //ordinal export
	{
		char ord_name[32];
		snprintf(ord_name, sizeof(ord_name), "Ordinal:%u", (unsigned) api);
		new_api = strdup(ord_name);
	}
	return (PROC) new log_stub(caller, target, new_api, (unsigned long) orig);
}

void prologue()
{
	char path[MAX_PATH];
	GetModuleFileName(NULL, path, MAX_PATH);
	char msg[DEBUGMSG_MAXLEN];
	snprintf(msg, sizeof(msg) -1, "0 |KernelEx API Log start for %s\n", path);
	writer_fn(msg);
}

void epilogue()
{
	writer_fn("0 |End of API log\n");
}

/*
 * Called at DLL initialization/unload time, outside of Kernel32Lock.
 * All static variables are initialized at this point.
 * Also all DLLs on which this DLL depends are initialized here.
 * This DLL's module tree is initialized before DLLs belonging to the process
 * so be careful what you do.
 */
BOOL APIENTRY DllMain(HINSTANCE hModule, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		tlsindex = TlsAlloc();
		if (tlsindex == -1)
			return 0;

		DisableThreadLibraryCalls(hModule);
		if (!init_writer())
			return FALSE;

		prologue();
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		epilogue();
	}
	return TRUE;
}
