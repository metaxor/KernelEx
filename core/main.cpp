/*
 *  KernelEx
 *  Copyright (C) 2008-2010, Xeno86
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

#include <windows.h>
#include "version.h"
#include "kexcoresdk.h"
#include "debug.h"
#include "apiconfmgr.h"
#include "internals.h"

extern int internals_init();
extern void internals_uninit();
extern int resolver_init();
extern void resolver_uninit();
extern void resolver_hook();
extern void resolver_unhook();

extern BOOL resolver_process_attach();

static int init_count = 0;

//these should be visible externally only in debug builds
#ifdef _DEBUG
extern "C" _KEXCOREIMP
#endif
int kexInit()
{
	if (init_count > 0)
		return ++init_count;

	DBGPRINTF(("KernelEx Core v%s by Xeno86\n", VERSION_STR));
	DBGPRINTF(("Initializing...\n"));

	if (!internals_init())
		goto __error1;

	if (!resolver_init())
		goto __error2;

	if (!apiconfmgr.load_api_configurations())
		goto __error3;

	resolver_hook();
	
	DBGPRINTF(("Initialized successfully\n"));
	return ++init_count;

__error3:
	resolver_uninit();
__error2:
	internals_uninit();
__error1:
	DBGPRINTF(("Initialization failure\n"));
	return 0;
}

//these should be visible externally only in debug builds
#ifdef _DEBUG
extern "C" _KEXCOREIMP
#endif
int kexUninit()
{
	if (init_count == 0)
		return 0;
	if (init_count > 1)
		return --init_count;

	DBGPRINTF(("Uninitializing\n"));
	resolver_unhook();
	resolver_uninit();
	internals_uninit();
	return --init_count;
}

extern "C" _KEXCOREIMP
DWORD WINAPI MprStart(LPVOID)
{
	return 0;
}

void load_MPRServices()
{    
    char subkey[200];
    char dllname[MAX_PATH];
    HKEY hk_serv;
    HKEY hk_this;
    DWORD size;
    DWORD index = 0;
    
    if (RegOpenKey(HKEY_LOCAL_MACHINE, "System\\CurrentControlSet\\Control\\MPRServices",
            &hk_serv) != ERROR_SUCCESS)
        return;
    while (RegEnumKey(hk_serv, index, subkey, sizeof(subkey)) == ERROR_SUCCESS)
    {
        RegOpenKey(hk_serv, subkey, &hk_this);
        size = sizeof(dllname);
        if (RegQueryValueEx(hk_this, "DllName", NULL, NULL, (BYTE*)dllname, &size) 
                == ERROR_SUCCESS)
        {         
            LoadLibrary(dllname);
        }
        RegCloseKey(hk_this);
        index++;
    }
    RegCloseKey(hk_serv);
}

/** Check if loaded into shared memory area.
 * @param addr Address to which loaded.
 * @return TRUE if loaded to shared memory, FALSE otherwise.
 */
static bool ensure_shared_memory(DWORD addr)
{
	if (!IS_SHARED(addr))
	{
		MessageBox(NULL, "KernelEx not loaded into shared memory!", 
				"Critical Error", MB_ICONERROR | MB_OK);
		return false;
	}
	return true;
}

static bool is_failsafe_mode()
{
	return GetSystemMetrics(SM_CLEANBOOT) != 0;
}

static int load_count(DWORD addr)
{
	if (!ppmteModTable)
		return 1;
	IMTE** pmteModTable = *ppmteModTable;
	return pmteModTable[MRfromCallerAddr(addr)->mteIndex]->cUsage;
}

static void get_own_path()
{
	char path[MAX_PATH];
	GetModuleFileName(hInstance, path, sizeof(path));
	own_path = path;
}

static bool detect_old_version()
{
	typedef void (WINAPI *KV_t)(char *);
	KV_t h_KVersion;
	char buf[32];
	
	h_KVersion = (KV_t)GetProcAddress(GetModuleHandle("kernel32"), "KUPVersion");
	if (!h_KVersion)
		h_KVersion = (KV_t)GetProcAddress(GetModuleHandle("kernel32"), "KEXVersion");
	if (!h_KVersion)
		return false;
	h_KVersion(buf);
	ShowError(IDS_OLDVER, buf);
	return true;
}

/** CRT startup routine declaration. */
extern "C"
BOOL WINAPI _DllMainCRTStartup(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved);

/** Pre-init code executed before CRT startup code. */
extern "C"
BOOL APIENTRY PreDllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
	BOOL ret = TRUE;

	if (reason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(instance);

		if (load_count((DWORD) instance) == 1)
		{
			//first reference => do init
			hInstance = instance;

			if (!ensure_shared_memory((DWORD)instance))
				return FALSE;

			if (is_failsafe_mode())
				return FALSE;

			if (detect_old_version())
				return FALSE;

            //very important - without this memory allocation won't work
			if (!create_shared_heap())
				return FALSE;

			//we are in shared memory so CRT init code should be called only once 
			//globally (by default it would be called on every process attach)
			ret = _DllMainCRTStartup(instance, reason, reserved);

			get_own_path();
		}
		else
		{
			//referenced by other module => call resolver
			ret = resolver_process_attach();
		}
	}
	else if (reason == DLL_PROCESS_DETACH)
	{
		if (load_count((DWORD) instance) == 1)
		{
			ret = _DllMainCRTStartup(instance, reason, reserved);
			destroy_shared_heap();
		}
	}

	return ret;
}

/** Entry point. Called by CRT startup code. */
extern "C"
BOOL APIENTRY DllMain(HINSTANCE instance, DWORD reason, BOOL load_static)
{
	switch(reason)
	{
		case DLL_PROCESS_ATTACH:

			if(GetModuleHandle("MPREXE.EXE"))
			{
				//auto start if loaded by MPREXE

				//load all other MPR services before further execution
				load_MPRServices();

				//initialize
				if (!kexInit())
					return FALSE;

				return TRUE;
			}
			break;
		case DLL_PROCESS_DETACH:
			//for additional safety - auto uninit on core unload
			if (load_count((DWORD) instance) == 1 && init_count > 0)
			{
				init_count = 1;
				kexUninit();
			}
			break;
	}

	return TRUE;
}

