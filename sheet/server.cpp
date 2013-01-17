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

#include <windows.h>
#include <olectl.h>
#include "server.h"
#include "factory.h"
#include "sheet.h"

long g_LockCount;
HMODULE g_hModule;


// {2338519D-676A-4ff8-99B9-924252B43710}
static const GUID CLSID_KexShlExt = 
{ 0x2338519d, 0x676a, 0x4ff8, { 0x99, 0xb9, 0x92, 0x42, 0x52, 0xb4, 0x37, 0x10 } };
static const char sCLSID_KexShlExt[] = { "{2338519D-676A-4ff8-99B9-924252B43710}" };


STDAPI DllCanUnloadNow()
{
	return (g_LockCount > 0) ? S_FALSE : S_OK;
}


STDAPI DllRegisterServer()
{
	bool result = true;
	HKEY hkey, hkey2, keyCLSID;
	
	if (RegOpenKey(HKEY_CLASSES_ROOT, "CLSID", &keyCLSID) 
			!= ERROR_SUCCESS)
		result = false;
	
	if (result)
	{
		if (RegCreateKey(keyCLSID, sCLSID_KexShlExt, &hkey) != ERROR_SUCCESS)
			result = false;

		if (result)
		{
			RegSetValueEx(hkey, NULL, 0, REG_SZ, (LPBYTE)"KernelEx Shell Extension", 
					sizeof("KernelEx Shell Extension"));

			if (RegCreateKey(hkey, "InProcServer32", &hkey2) != ERROR_SUCCESS)
				result = false;

			if (result)
			{
				char filename[MAX_PATH];
				GetModuleFileName(g_hModule, filename, sizeof(filename));

				RegSetValueEx(hkey2, NULL, 0, REG_SZ, (LPBYTE)filename, strlen(filename) + 1);
				RegSetValueEx(hkey2, "ThreadingModel", 0, REG_SZ, (LPBYTE)"Apartment", 
						sizeof("Apartment"));

				RegCloseKey(hkey2);
			}

			RegCloseKey(hkey);
		}

		RegCloseKey(keyCLSID);
	}

	if (RegCreateKey(HKEY_CLASSES_ROOT, "*\\shellex\\PropertySheetHandlers", 
			&hkey) != ERROR_SUCCESS)
		result = false;

	if (result)
	{
		if (RegCreateKey(hkey, "KernelEx", &hkey2) != ERROR_SUCCESS)
			result = false;

		if (result)
		{
			RegSetValueEx(hkey2, NULL, 0, REG_SZ, (LPBYTE)sCLSID_KexShlExt, sizeof(sCLSID_KexShlExt));
			RegCloseKey(hkey2);
		}

		RegCloseKey(hkey);
	}

	return result ? S_OK : SELFREG_E_CLASS;
}


STDAPI DllUnregisterServer()
{
	bool result = true;
	HKEY hkey, keyCLSID;

	if (RegOpenKey(HKEY_CLASSES_ROOT, "CLSID", &keyCLSID) 
			!= ERROR_SUCCESS)
		result = false;

	if (result)
	{
		if (RegOpenKey(keyCLSID, sCLSID_KexShlExt, &hkey) != ERROR_SUCCESS)
			result = false;

		if (result)
		{
			RegDeleteKey(hkey, "InProcServer32");
			RegCloseKey(hkey);
		}

		RegDeleteKey(keyCLSID, sCLSID_KexShlExt);
		RegCloseKey(keyCLSID);
	}

	if (RegOpenKey(HKEY_CLASSES_ROOT, "*\\shellex\\PropertySheetHandlers", 
			&hkey) != ERROR_SUCCESS)
		result = false;

	if (result)
	{
		RegDeleteKey(hkey, "KernelEx");
		RegCloseKey(hkey);
	}

	return S_OK;
}
		
	
STDAPI DllGetClassObject(const CLSID& clsid, const IID& iid, void** ppv)
{
	HRESULT hr;

	if (clsid == CLSID_KexShlExt)
	{
		CFactory* factory = new CFactory;
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


BOOL APIENTRY DllMain(HINSTANCE hModule, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		g_hModule = hModule;
		DisableThreadLibraryCalls(hModule);
	}
	return TRUE;
}
