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
#include "KexLinkage.h"


KexLinkage KexLinkage::instance;


KexLinkage::KexLinkage()
{
	disable_extensions = false;
	m_ready = Prepare();
}


KexLinkage::~KexLinkage()
{
	if (hKernelEx)
		FreeLibrary(hKernelEx);
}


bool KexLinkage::Prepare()
{
	char core_conf_file[MAX_PATH];
	HKEY key;
	DWORD type;
	DWORD len;
	DWORD data;

	hKernelEx = LoadLibrary("KERNELEX.DLL");
	if (!hKernelEx)
		return false;

	m_kexGetModuleSettings = (kexGetModuleSettings_t) GetProcAddress(hKernelEx,
			"kexGetModuleSettings");
	m_kexSetModuleSettings = (kexSetModuleSettings_t) GetProcAddress(hKernelEx,
			"kexSetModuleSettings");
	m_kexResetModuleSettings = (kexResetModuleSettings_t) GetProcAddress(hKernelEx,
			"kexResetModuleSettings");
	m_kexGetKEXVersion = (kexGetKEXVersion_t) GetProcAddress(hKernelEx,
			"kexGetKEXVersion");
	m_kexGetCoreCaps = (kexGetCoreCaps_t) GetProcAddress(hKernelEx,
			"kexGetCoreCaps");

	if (!m_kexGetModuleSettings || !m_kexSetModuleSettings 
			|| !m_kexResetModuleSettings
			|| !m_kexGetKEXVersion || !m_kexGetCoreCaps)
		return false;

	//read config file location from registry
	long result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\KernelEx", 
			0, KEY_QUERY_VALUE, &key);
	if (result != ERROR_SUCCESS)
		return false;

	len = sizeof(data);
	result = RegQueryValueEx(key, "DisableExtensions", NULL, &type, 
			(BYTE*) &data, &len);
	if (result == ERROR_SUCCESS && type == REG_DWORD && len == sizeof(data) && data == 1)
		disable_extensions = true;

	len = sizeof(core_conf_file);
	result = RegQueryValueEx(key, "InstallDir", NULL, &type, 
			(BYTE*)core_conf_file, &len);

	RegCloseKey(key);
	if (result != ERROR_SUCCESS || type != REG_SZ || len == 0)
		return false;

	strcat(core_conf_file, "\\core.ini");
	if (GetFileAttributes(core_conf_file) == 0xffffffff)
		return false;

	//parse config file
	default_index = GetPrivateProfileInt("ApiConfigurations", 
			"default", 0, core_conf_file);

	for (int i = 0 ; i < 65536 ; i++)
	{
		char num[6];
		char conf_name[256];
		char conf_desc[256];

		sprintf(num, "%d", i);
		
		if (!GetPrivateProfileString("ApiConfigurations", num, "", 
				conf_name, sizeof(conf_name), core_conf_file))
			break;

		if (GetPrivateProfileInt(conf_name, "noshow", 0, core_conf_file))
			continue;

		GetPrivateProfileString(conf_name, "desc", conf_name,
				conf_desc, sizeof(conf_desc), core_conf_file);

		conf c(conf_name, conf_desc);
		confs.push_back(c);
	}

	return true;
}


bool KexLinkage::IsReady()
{
	return m_ready;
}
