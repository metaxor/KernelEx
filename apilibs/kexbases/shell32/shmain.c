/*
 *  KernelEx
 *  Copyright (C) 2007, Xeno86
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

#include "_shell32_apilist.h"
#include <olectl.h>

/* MAKE_EXPORT DllInstall=DllInstall */
STDAPI DllInstall(BOOL bInstall,
	LPCWSTR pszCmdLine
)
{
	FIXME("%s %s: stub\n", bInstall ? "TRUE":"FALSE", debugstr_w(pszCmdLine));
	return S_OK;
}

/* MAKE_EXPORT DllRegisterServer=DllRegisterServer */
STDAPI DllRegisterServer(void)
{
	DWORD dwVer = kexGetVersion();

	/* Windows ME */
	if(dwVer == 0xc0005a04)
	{
		HINSTANCE hInstance = LoadLibrary("shell32.dll");
		FARPROC _DllRegisterServer = NULL;
		HRESULT Result = 0;

		if(hInstance == NULL)
			return E_NOTIMPL;

		_DllRegisterServer = GetProcAddress(hInstance, "DllRegisterServer");

		if(_DllRegisterServer == NULL)
			return E_NOTIMPL;

		Result = _DllRegisterServer();

		FreeLibrary(hInstance);

		return Result;
	}

	FIXME_OUT("Update the registry from resource\n");
	FIXME_OUT("Register shell folders\n");
	return S_OK;
}

/* MAKE_EXPORT DllUnregisterServer=DllUnregisterServer */
STDAPI DllUnregisterServer(void)
{
	DWORD dwVer = kexGetVersion();

	/* Windows ME */
	if(dwVer == 0xc0005a04)
	{
		HINSTANCE hInstance = LoadLibrary("shell32.dll");
		FARPROC _DllUnregisterServer = NULL;
		HRESULT Result = 0;

		if(hInstance == NULL)
			return E_NOTIMPL;

		_DllUnregisterServer = GetProcAddress(hInstance, "DllUnregisterServer");

		if(_DllUnregisterServer == NULL)
			return E_NOTIMPL;

		Result = _DllUnregisterServer();

		FreeLibrary(hInstance);

		return Result;
	}

	FIXME_OUT("Update the registry from resource\n");
	return S_OK;
}