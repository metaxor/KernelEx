/*
 *  KernelEx
 *  Copyright (C) 2010, Xeno86
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

#define _WIN32_WINNT 0x0501
#define _WIN32_IE 0x0500
#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include "resource.h"

/* Program used to verify KernelEx installation.
 * If it annoys you add DWORD value "NoVerify" 
 * under HKLM\Software\KernelEx and set it to 1.
 */

HINSTANCE hInstance;
unsigned long core_version;

typedef unsigned long (*kexVersion_t)();

int snprintf(char*, size_t, const char*, ...);

void install_failure()
{
	char msg[256];
	char title[100];

	LoadString(hInstance, IDS_FAILURETITLE, title, sizeof(title));
	
	LoadString(hInstance, IDS_FAILURE, msg, sizeof(msg));
	MessageBox(NULL, msg, title, MB_OK | MB_ICONERROR);
}

void erase_from_startup()
{
	HKEY key;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\Run",
			0, KEY_ALL_ACCESS, &key) != ERROR_SUCCESS)
		return;

	RegDeleteValue(key, "KexVerify");
	RegCloseKey(key);
}

int is_disable_verify()
{
	HKEY key;
	DWORD type;
	DWORD data;
	LONG ret;
	DWORD size = sizeof(data);

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\KernelEx",
			0, KEY_QUERY_VALUE, &key) != ERROR_SUCCESS)
		return 0;

	ret = RegQueryValueEx(key, "NoVerify", NULL, &type, (BYTE*) &data, &size);

	RegCloseKey(key);

	if (ret != ERROR_SUCCESS || type != REG_DWORD || size != sizeof(DWORD) || data == 0)
		return 0;
	return 1;
}

int test_core()
{
	HMODULE h = GetModuleHandle("KERNELEX.DLL");
	kexVersion_t getver = (kexVersion_t)GetProcAddress(h, "kexGetKEXVersion");
	
	if (!getver)
		return 0;

	core_version = getver();
	return 1;
}

int test_apis()
{
	HMODULE h = GetModuleHandle("KERNEL32.DLL");
	PROC proc = GetProcAddress(h, "GetSystemWindowsDirectoryA");
	if (!proc)
		return 0;

	h = LoadLibrary("GDI32.DLL");
	proc = GetProcAddress(h, "GetGlyphIndicesW");
	FreeLibrary(h);

	if (!proc)
		return 0;

	return 1;
}

BOOL CALLBACK DlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_INITDIALOG:
	{
		char ver_s[32];
		snprintf(ver_s, sizeof(ver_s), "v%d.%d.%d", 
				core_version>>24, (core_version>>16) & 0xff, core_version & 0xffff);
		SendMessage(GetDlgItem(hwnd, IDC_VERSION), WM_SETTEXT, 0, (LPARAM) ver_s);
		return TRUE;
	}
	case WM_COMMAND:
	case WM_CLOSE:
		EndDialog(hwnd, 0);
		return TRUE;
	case WM_NOTIFY:
		switch (((LPNMHDR)lParam)->code)
		{
		case NM_CLICK:
		case NM_RETURN:
        {
            PNMLINK pNMLink = (PNMLINK)lParam;
            LITEM item = pNMLink->item;
            if (((LPNMHDR)lParam)->idFrom == IDC_LINK)
            {
                ShellExecuteW(NULL, L"open", item.szUrl, NULL, NULL, SW_SHOW);
				return TRUE;
            }
			break;
        }
	    }
		break;
	}
	return FALSE;
}

int are_extensions_enabled()
{
	HKEY key;
	DWORD type, data, size = sizeof(data);
	LONG res;
	res = RegOpenKey(HKEY_LOCAL_MACHINE, "Software\\KernelEx", &key);
	if (res != ERROR_SUCCESS)
		return 1;
	res = RegQueryValueEx(key, "DisableExtensions", NULL, &type, (char*)&data, &size);
	if (res == ERROR_SUCCESS && type == REG_DWORD && size == sizeof(data) && data == 1)
		return 0;
	RegCloseKey(key);
	return 1;
}

__declspec(noreturn)
void entrypoint()
{
	INITCOMMONCONTROLSEX iccex;

	hInstance = GetModuleHandle(NULL);

	erase_from_startup();

	if (!is_disable_verify())
	{
		int okay = 0;

		if (test_core() && test_apis())
		{
			iccex.dwSize = sizeof(iccex);
			iccex.dwICC = ICC_LINK_CLASS;
			if (InitCommonControlsEx(&iccex))
			{
				int dialog;
				if (are_extensions_enabled())
					dialog = IDD_DIALOG1;
				else
					dialog = IDD_DIALOG2;
				if (!DialogBoxParam(hInstance, MAKEINTRESOURCE(dialog), NULL, DlgProc, 0))
					okay = 1;
			}
		}

		if (!okay)
			install_failure();
	}


	ExitProcess(0);
}
