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
#include "SettingsDB.h"
#include "apiconfmgr.h"
#include "internals.h"
#include "resolver.h"
#include "debug.h"
#include "wildcmp.h"

SettingsDB SettingsDB::instance;

SettingsDB::SettingsDB()
{
	InitializeCriticalSection(&cs);
	MakeCriticalSectionGlobal(&cs);
}

SettingsDB::~SettingsDB()
{
	DeleteCriticalSection(&cs);
}

void SettingsDB::clear()
{
	EnterCriticalSection(&cs);
	db.clear();
	db_wild.clear();
	appsetting as;
	as.flags = LDR_KEX_DISABLE;
	db.insert(pair<sstring,appsetting>(own_path, as));
	LeaveCriticalSection(&cs);
}

void SettingsDB::flush_all()
{
	EnterCriticalSection(&cs);
	clear();
	parse_configs();
	parse_flags();
	add_apilib_excludes();
	LeaveCriticalSection(&cs);
}

void SettingsDB::parse_configs()
{
	LONG result;
	HKEY key;
	DWORD index = 0;
	char path[MAX_PATH];
	char epath[MAX_PATH];
	DWORD path_len;
	char name[256];
	DWORD name_len;
	DWORD type;

	result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\KernelEx\\AppSettings\\Configs", 
			0, KEY_QUERY_VALUE, &key);
	if (result != ERROR_SUCCESS)
	{
		DBGPRINTF(("AppSettings\\Configs key missing\n"));
		return;
	}
	while (true)
	{
		appsetting as;
		map<sstring,appsetting>* pdb;

		path_len = sizeof(path);
		name_len = sizeof(name);
		result = RegEnumValue(key, index, path, &path_len, NULL, &type, (LPBYTE)name, &name_len);
		index++;
		if (result == ERROR_NO_MORE_ITEMS)
			break;
		if (result != ERROR_SUCCESS)
			break;
		if (type != REG_SZ)
			continue;
		name[sizeof(name) - 1] = '\0';
		as.conf = apiconfmgr.get_api_configuration(name);
		if (!as.conf)
			continue;
		
		int ret = ExpandEnvironmentStrings(path, epath, MAX_PATH);
		if (ret > MAX_PATH || ret == 0)
			continue;

		strupr(epath);
		if (strchr(epath, '*') || strchr(epath, '?'))
			pdb = &db_wild;
		else
			pdb = &db;
			
		pdb->insert(pair<sstring, appsetting>(epath, as));
	}
	RegCloseKey(key);
}

//note: call it after parse_configs()
void SettingsDB::parse_flags()
{
	LONG result;
	HKEY key;
	DWORD index = 0;
	char path[MAX_PATH];
	DWORD path_len;
	unsigned long flags;
	DWORD flags_len;
	DWORD type;

	result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\KernelEx\\AppSettings\\Flags", 
			0, KEY_QUERY_VALUE, &key);
	if (result != ERROR_SUCCESS)
	{
		DBGPRINTF(("AppSettings\\Flags key missing\n"));
		return;
	}
	while (true)
	{
		map<sstring,appsetting>* pdb;
		path_len = sizeof(path);
		flags_len = sizeof(flags);
		result = RegEnumValue(key, index, path, &path_len, NULL, &type, (LPBYTE)&flags, &flags_len);
		index++;
		if (result == ERROR_NO_MORE_ITEMS)
			break;
		if (result != ERROR_SUCCESS)
			break;
		if (type != REG_DWORD)
			continue;

		strupr(path);
		if (strchr(path, '*') || strchr(path, '?'))
			pdb = &db_wild;
		else
			pdb = &db;

		map<sstring,appsetting>::iterator it = pdb->find(path);
		if (it == pdb->end())
		{
			appsetting as;
			as.flags = flags;
			pdb->insert(pair<sstring,appsetting>(path, as));
		}
		else
		{
			it->second.flags = flags;
		}
	}
	RegCloseKey(key);
}

void SettingsDB::add_apilib_excludes()
{
	ApiLibrary* lib;
	int i = 1;
	appsetting as;
	as.flags = LDR_KEX_DISABLE;
	while ((lib = apilibmgr.get_apilib_by_index(i++)) != NULL)
	{
		char path[MAX_PATH];
		lib->get_dll_path(path);
		db[path] = as;
	}
}

appsetting SettingsDB::get_appsetting(const char* path)
{
	map<sstring,appsetting>::const_iterator it;

	EnterCriticalSection(&cs);
	//try full path first
	it = db.find(path);
	if (it == db.end())
	{
		//then try wildcard matching
		for (it = db_wild.begin() ; it != db_wild.end() ; it++)
		{
			if (wildcmp(it->first, path))
				break;
		}
	}

	bool atend = it == db.end() || it == db_wild.end();
	LeaveCriticalSection(&cs);
	if (!atend)
	{
		appsetting as = it->second;
		as.flags |= LDR_VALID_FLAG;
		if (!as.conf && !(as.flags & LDR_KEX_DISABLE))
			as.conf = apiconfmgr.get_default_configuration();
		return as;
	}
	else
		return appsetting();
}

void SettingsDB::write_single(const char* path, const char* conf_name, BYTE flags)
{
	LONG result;
	HKEY key;
	appsetting as;
	
	//convert path to uppercase
	char path2[MAX_PATH];
	strncpy(path2, path, sizeof(path2));
	strupr(path2);
	path = path2;

	//check if configuration name is valid
	as.conf = apiconfmgr.get_api_configuration(conf_name);
	as.flags = flags;

	//write config
	result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
			"Software\\KernelEx\\AppSettings\\Configs", 0, KEY_WRITE, &key);
	if (result == ERROR_SUCCESS)
	{
		if (!as.conf)
			RegDeleteValue(key, path);
		else
			RegSetValueEx(key, path, 0, REG_SZ, (const BYTE*) conf_name, 
					strlen(conf_name) + 1);
		RegCloseKey(key);
	}

	//write flags
	result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
			"Software\\KernelEx\\AppSettings\\Flags", 0, KEY_WRITE, &key);
	if (result == ERROR_SUCCESS)
	{
		RegSetValueEx(key, path, 0, REG_DWORD, (BYTE*) &as.flags, sizeof(as.flags));
		RegCloseKey(key);
	}

	//add to DB
	EnterCriticalSection(&cs);
	db.erase(path);
	db.insert(pair<sstring,appsetting>(path, as));
	LeaveCriticalSection(&cs);
}

void SettingsDB::reset_single(const char* path)
{
	LONG result;
	HKEY key;

	//convert path to uppercase
	char path2[MAX_PATH];
	strncpy(path2, path, sizeof(path2));
	strupr(path2);
	path = path2;

	//erase config
	result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
			"Software\\KernelEx\\AppSettings\\Configs", 0, KEY_WRITE, &key);
	if (result == ERROR_SUCCESS)
	{
		RegDeleteValue(key, path);
		RegCloseKey(key);
	}

	//erase flags
	result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
			"Software\\KernelEx\\AppSettings\\Flags", 0, KEY_WRITE, &key);
	if (result == ERROR_SUCCESS)
	{
		RegDeleteValue(key, path);
		RegCloseKey(key);
	}

	//erase from DB
	EnterCriticalSection(&cs);
	db.erase(path);
	LeaveCriticalSection(&cs);
}

#ifdef _DEBUG

void SettingsDB::dump_db()
{
	map<sstring, appsetting>::const_iterator it;
	
	printf("User settings:\n");
	for (it = db.begin() ; it != db.end() ; it++)
	{
		ApiConfiguration* conf = it->second.conf;
		printf("%-40s %-10s %02x\n", static_cast<const char*>(it->first), 
				conf ? conf->get_name() : "unknown", it->second.flags);
	}

	printf("\nPredefined settings:\n");
	for (it = db_wild.begin() ; it != db_wild.end() ; it++)
	{
		ApiConfiguration* conf = it->second.conf;
		printf("%-40s %-10s %02x\n", static_cast<const char*>(it->first), 
				conf ? conf->get_name() : "unknown", it->second.flags);
	}
}

#endif
