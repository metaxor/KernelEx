/*
 *  KernelEx
 *  Copyright (C) 2008, Xeno86
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

static DWORD hexstr_to_dword(const char* hexstr)
{
	int i;
	DWORD res = 0;

	for (i = 0 ; i < 8 ; i++)
	{
		res <<= 4;
		if (hexstr[i] >= '0' && hexstr[i] <= '9')
			res += hexstr[i] - '0';
		else if (hexstr[i] >= 'a' && hexstr[i] <= 'f')
			res += hexstr[i] - 'a' + 0xa;
		else if (hexstr[i] >= 'A' && hexstr[i] <= 'F')
			res += hexstr[i] - 'A' + 0xA;
		else
			return (DWORD)-1;
	}
	return res;
}

static void dword_to_hexstr(DWORD val, void* _out, BOOL unicode)
{
	int i;
	char* out = (char*) _out;

	for (i = 0 ; i < 8 ; i++)
	{
		int v = (val & 0xf0000000) >> 28;
		val <<= 4;
		if (v <= 9)
			*out++ = v + '0';
		else
			*out++ = v - 0xa + 'A';
		if (unicode)
			*out++ = 0;
	}
	*out++ = 0;
	if (unicode)
		*out++ = 0;
}

static LANGID UILanguage_readreg()
{
	HKEY key;
	DWORD type;
	char data[10];
	LONG res;
	DWORD count = sizeof(data);

	if (RegOpenKey(HKEY_CURRENT_USER, "Control Panel\\desktop\\ResourceLocale", &key))
		return (LANGID) -1;

	res = RegQueryValueEx(key, NULL, NULL, &type, (BYTE*)data, &count);
	RegCloseKey(key);

	if (!res && type == REG_SZ && count == 9)
	{
		DWORD res = hexstr_to_dword(data);
		if (res < 0x10000)
			return (LANGID) res;
	}
	return (LANGID) -1;
}

/* MAKE_EXPORT GetUserDefaultUILanguage_new=GetUserDefaultUILanguage */
LANGID WINAPI GetUserDefaultUILanguage_new(void)
{
	LANGID res = UILanguage_readreg();
	if (res != (LANGID) -1)
		return res;
	return (LANGID) 0x409; /* default to English US */
}

/* MAKE_EXPORT GetSystemDefaultUILanguage_new=GetSystemDefaultUILanguage */
LANGID WINAPI GetSystemDefaultUILanguage_new(void)
{
	LANGID res = UILanguage_readreg();
	if (res != (LANGID) -1)
		return res;
	return (LANGID) 0x409; /* default to English US */
}

typedef long LONG_PTR;
typedef BOOL (CALLBACK *UILANGUAGE_ENUMPROCA)(LPSTR, LONG_PTR);
typedef BOOL (CALLBACK *UILANGUAGE_ENUMPROCW)(LPWSTR, LONG_PTR);

/* MAKE_EXPORT EnumUILanguagesA_new=EnumUILanguagesA */
BOOL WINAPI EnumUILanguagesA_new(UILANGUAGE_ENUMPROCA pUILangEnumProc, DWORD dwFlags, LONG_PTR lParam)
{
	char buf[10];
	LANGID lid;

	if (!pUILangEnumProc) 
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	if (dwFlags) 
	{
		SetLastError(ERROR_INVALID_FLAGS);
		return FALSE;
	}

	lid = GetSystemDefaultUILanguage_new();

	dword_to_hexstr(lid, buf, FALSE);

	pUILangEnumProc(buf, lParam);
	return TRUE;
}

/* MAKE_EXPORT EnumUILanguagesW_new=EnumUILanguagesW */
BOOL WINAPI EnumUILanguagesW_new(UILANGUAGE_ENUMPROCW pUILangEnumProc, DWORD dwFlags, LONG_PTR lParam)
{
	WCHAR buf[10];
	LANGID lid;

	if (!pUILangEnumProc) 
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	if (dwFlags) 
	{
		SetLastError(ERROR_INVALID_FLAGS);
		return FALSE;
	}

	lid = GetSystemDefaultUILanguage_new();

	dword_to_hexstr(lid, buf, TRUE);

	pUILangEnumProc(buf, lParam);
	return TRUE;
}

/* MAKE_EXPORT SetThreadUILanguage_new=SetThreadUILanguage */
LANGID WINAPI SetThreadUILanguage_new(LANGID LangId)
{
	return LangId;
}
