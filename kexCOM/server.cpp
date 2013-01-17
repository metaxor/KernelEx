/*
 *  KernelEx
 *  Copyright (C) 2009, Xeno86
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

#include <objbase.h>
#include <shlguid.h>
#include <shlwapi.h>
#include "server.h"
#include "factory.h"

long g_LockCount;
HMODULE g_hModule;
static HMODULE hShell32;
static LPFNGETCLASSOBJECT SHGetClassObject;


static bool is_shell32_v5()
{
	DLLGETVERSIONPROC DllGetVersion;
	DLLVERSIONINFO dvi;
	HRESULT hr;

	dvi.cbSize = sizeof(dvi);

	DllGetVersion = (DLLGETVERSIONPROC) GetProcAddress(hShell32, "DllGetVersion");
	if (!DllGetVersion)
		return false;

	hr = DllGetVersion(&dvi);
	if (SUCCEEDED(hr) && dvi.dwMajorVersion >= 5)
		return true;

	return false;
}


static bool is_failsafe_mode()
{
	static int s = -1;
	if (s == -1)
		s = GetSystemMetrics(SM_CLEANBOOT);
	return s != 0;
}


STDAPI DllCanUnloadNow()
{
	return (g_LockCount > 0) ? S_FALSE : S_OK;
}


STDAPI DllRegisterServer()
{
	LONG result;
	char filename[MAX_PATH];

	// SHELL32 v5.0 and above has ShellLinkW - kexCOM is not needed
	if (is_shell32_v5())
		return S_OK;

	GetModuleFileName(g_hModule, filename, sizeof(filename));
	result = RegSetValue(HKEY_CLASSES_ROOT, 
			"CLSID\\{00021401-0000-0000-C000-000000000046}\\InProcServer32", 
			REG_SZ, filename, strlen(filename));

	return (result == ERROR_SUCCESS) ? S_OK : E_FAIL;
}


STDAPI DllUnregisterServer()
{
	LONG result;
	const char szShell32[] = { "shell32.dll" };

	result = RegSetValue(HKEY_CLASSES_ROOT, 
			"CLSID\\{00021401-0000-0000-C000-000000000046}\\InProcServer32", 
			REG_SZ, szShell32, strlen(szShell32));

	return (result == ERROR_SUCCESS) ? S_OK : E_FAIL;
}
		
	
STDAPI DllGetClassObject(const CLSID& clsid, const IID& iid, void** ppv)
{
	HRESULT hr;

	if (is_failsafe_mode())
		return SHGetClassObject(clsid, iid, ppv);

	if (clsid == CLSID_ShellLink)
	{
		IClassFactory* pcf;

		hr = SHGetClassObject(CLSID_ShellLink, IID_IClassFactory, (void**) &pcf);
		if (FAILED(hr))
			return hr;

		CFactory* factory = new CFactory(pcf);
		if (!factory)
			return E_OUTOFMEMORY;

		hr = factory->QueryInterface(iid, ppv);
		factory->Release();
		return hr;
	}
    else
    {	
        *ppv = NULL;	
        return CLASS_E_CLASSNOTAVAILABLE;
    }
}


HMODULE GetCurrentModule()
{
	MEMORY_BASIC_INFORMATION mbi;
	static int dummy;
	VirtualQuery(&dummy, &mbi, sizeof(mbi));
	return (HMODULE)mbi.AllocationBase;
}


BOOL APIENTRY DllMain(HINSTANCE hModule, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		char szPath[MAX_PATH];

		/* LoadLibrary(self) trick to prevent premature unload in buggy programs
		 * such as Adobe Photoshop, Acrobat Reader,... */
		GetModuleFileName(GetCurrentModule(), szPath, sizeof(szPath));
		LoadLibrary(szPath);

		g_hModule = hModule;
		DisableThreadLibraryCalls(hModule);

	    hShell32 = LoadLibrary("SHELL32");
		SHGetClassObject = (LPFNGETCLASSOBJECT) GetProcAddress(hShell32, "DllGetClassObject");
	}
    else if (dwReason == DLL_PROCESS_DETACH)
    {
        FreeLibrary(hShell32);
    }

    return TRUE;
}
