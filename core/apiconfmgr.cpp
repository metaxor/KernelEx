/*
 *  KernelEx
 *  Copyright (C) 2008-2009, Xeno86
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
#include <stdio.h>
#include <malloc.h>
#include "debug.h"
#include "SettingsDB.h"
#include "internals.h"
#include "resolver.h"
#include "apiconfmgr.h"

#define ALLOC_CAPACITY 10

ApiConfigurationManager apiconfmgr;

ApiConfigurationManager::ApiConfigurationManager()
{
	apiconf_ptrs = NULL;
	apiconf_cnt = 0;
	default_apiconf = NULL;
	disable_extensions = false;
}

ApiConfigurationManager::~ApiConfigurationManager()
{
	for (int i = 0 ; i < apiconf_cnt ; i++)
		delete apiconf_ptrs[i];
	if (apiconf_ptrs)
		free(apiconf_ptrs);
}

bool ApiConfigurationManager::add_apiconf(ApiConfiguration* ac)
{
	//allocate space for new ApiConfigurations
	if (apiconf_cnt % ALLOC_CAPACITY == 0)
	{
		void* new_block = realloc(apiconf_ptrs, 
				(apiconf_cnt + ALLOC_CAPACITY) * sizeof(ApiConfiguration*));
		
		if (!new_block)
		{
			DBGPRINTF(("Failed to add api configuration to list of new ACs\n"));
			return false;
		}

		apiconf_ptrs = (ApiConfiguration**) new_block;
	}

	//add to table of new ApiConfigurations
	apiconf_ptrs[apiconf_cnt++] = ac;
	return true;
}

ApiConfiguration* ApiConfigurationManager::get_api_configuration(const char* conf_name)
{
	for (int i = 0 ; i < apiconf_cnt ; i++)
		if (!strcmp(apiconf_ptrs[i]->conf_name, conf_name))
			return apiconf_ptrs[i];
	return NULL;
}

bool ApiConfigurationManager::load_api_configurations()
{
	strcpy(core_conf_file, kernelex_dir);
	strcat(core_conf_file, "core.ini");

	DBGPRINTF(("Loading api configurations\n"));

	HKEY key;
	LONG result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
			"Software\\KernelEx", 0, KEY_WRITE, &key);
	if (result == ERROR_SUCCESS)
	{
		DWORD type, data, size = sizeof(data);
		RegQueryValueEx(key, "DisableExtensions", NULL, &type, (BYTE*) &data, &size);
		if (result == ERROR_SUCCESS && type == REG_DWORD && size == sizeof(data) && data == 1)
			disable_extensions = true;
		RegCloseKey(key);
	}
	
	int default_apiconf_index = GetPrivateProfileInt(
			"ApiConfigurations", "default", 0, core_conf_file);

	for (int i = 0 ; i < 65536 ; i++)
	{
		char num[6];
		char apiconf_name[256];
		ApiConfiguration* apiconf = NULL;

		sprintf(num, "%d", i);
		if (!GetPrivateProfileString("ApiConfigurations", num, "", 
				apiconf_name, sizeof(apiconf_name), core_conf_file))
		{
			//no more entries
			if (i <= default_apiconf_index)
			{
				DBGPRINTF(("Failed to load default api configuration - aborting\n"));
				return false;
			}
			break;
		}
		DBGPRINTF(("Processing api configuration #%s: %s\n", num, apiconf_name));

		char buf[256];

		if (!GetPrivateProfileString(apiconf_name, "inherit", "", buf, 
				sizeof(buf), core_conf_file) || !strcmpi(buf, "none"))
		{
			//no inherit
			apiconf = new ApiConfiguration(apiconf_name);
		}
		else
		{
			ApiConfiguration* src_conf = get_api_configuration(buf);
			if (!src_conf)
			{
				DBGPRINTF(("Cannot inherit: %s. Configuration not found\n", buf));
				goto __error;
			}

			apiconf = new ApiConfiguration(apiconf_name, *src_conf);
		}

		if (!apiconf || !apiconf->is_initialized())
		{
			DBGPRINTF(("Failed to create new api configuration object\n"));
			goto __error;
		}

		if (!join_apilibs(apiconf) || !parse_overrides(apiconf) || !add_apiconf(apiconf))
			goto __error;

		continue;

__error:
		if (apiconf)
			delete apiconf;
		if (i == default_apiconf_index)
		{
			DBGPRINTF(("Failed to load default api configuration - aborting\n"));
			return false;
		}
	} //for loop

	DBGPRINTF(("No more api configurations\n"));

	//set default apiconf
	if (default_apiconf_index >= 0)
		default_apiconf = apiconf_ptrs[default_apiconf_index];
	else
	{
		DBGPRINTF(("Failed to load default api configuration - aborting\n"));
		return false;
	}

	DBGPRINTF(("Default api configuration is: %s\n", default_apiconf->get_name()));
	DBGPRINTF(("API extensions are by default: %s\n", 
			disable_extensions ? "disabled" : "enabled"));

	SettingsDB::instance.flush_all();

	return true;
}

bool ApiConfigurationManager::join_apilibs(ApiConfiguration* apiconf)
{
	char buf[256];

	if (GetPrivateProfileString(apiconf->get_name(), "contents", "", 
			buf, sizeof(buf), core_conf_file) && strcmpi(buf, "none") != 0)
	{
		char buf2[256];

		strupr(buf);
		strcpy(buf2, buf);
		char* lib = strtok(buf, ",");
		while (lib)
		{
			if (!apilibmgr.load_apilib(lib))
			{
				DBGPRINTF(("Failed to load api library: %s\n", lib));
				return false;
			}
			lib = strtok(NULL, ",");
		}

		//merge has to be done in a separate loop because STD
		//api library is based on other loaded api libraries
		lib = strtok(buf2, ",");
		while (lib)
		{
			if (!apiconf->merge(apilibmgr.get_apilib(lib)))
			{
				DBGPRINTF(("Failed to merge api library: %s\n", lib));
				return false;
			}
			lib = strtok(NULL, ",");
		}
	}
	return true;
}

bool ApiConfigurationManager::parse_overrides(ApiConfiguration* apiconf)
{
	//parse names and ordinal overrides
	char buf[MAX_PATH];
	char section_data[32767];
	DWORD res;

	//first try [conf.names.xx]
	strcpy(buf, apiconf->get_name());
	strcat(buf, ".names");
	if (isWinMe())
		strcat(buf, ".me");
	else
		strcat(buf, ".98");

	res = GetPrivateProfileSection(buf, section_data, sizeof(section_data), 
			core_conf_file);

	//else try [conf.names]
	if (!res)
	{
		strcpy(buf, apiconf->get_name());
		strcat(buf, ".names");
		res = GetPrivateProfileSection(buf, section_data, sizeof(section_data), 
				core_conf_file);
	}

	if (res)
	{
		DBGPRINTF(("Parsing named api overrides\n"));

		char* line = section_data;
		while (*line)
		{
			char* module;
			char* api_name;
			char* apilib_name;
			int id;

			char* p = line;
			char* oldline = line;
			line += strlen(line) + 1;

			module = p;
			p = strchr(p, '.');
			if (!p)
			{
				DBGPRINTF(("Error parsing line: %s\n", oldline));
				continue;
			}
			*p++ = '\0';
			strcpy(buf, module);
			strcat(buf, ".DLL");
			module = buf;
			api_name = p;
			p = strchr(p, '=');
			if (!p)
			{
				DBGPRINTF(("Error parsing line: %s\n", oldline));
				continue;
			}
			*p++ = '\0';
			apilib_name = p;
			p = strchr(p, '.');
			if (!p)
				id = 0;
			else
			{
				*p++ = '\0';
				id = atoi(p);
				if (id < 0)
				{
					DBGPRINTF(("Error parsing line: %s\n", oldline));
					continue;
				}
			}
			strupr(apilib_name);
			if (!strcmp(apilib_name, "NONE"))
			{
				if (!apiconf->erase(module, api_name))
				{
					DBGPRINTF(("Failed to merge named api overrides\n"));
					return false;
				}
			}
			else
			{
				if (!apiconf->merge(module, api_name, 
						apilibmgr.get_apilib(apilib_name), id))
				{
					DBGPRINTF(("Failed to merge named api overrides\n"));
					return false;
				}
			}
		}
	}

	//first try [conf.ordinals.xx]
	strcpy(buf, apiconf->get_name());
	strcat(buf, ".ordinals");

	if (isWinMe())
		strcat(buf, ".me");
	else
		strcat(buf, ".98");

	//else try [conf.ordinals]
	res = GetPrivateProfileSection(buf, section_data, sizeof(section_data), 
			core_conf_file);
	
	if (!res)
	{
		strcpy(buf, apiconf->get_name());
		strcat(buf, ".ordinals");
		res = GetPrivateProfileSection(buf, section_data, sizeof(section_data), 
				core_conf_file);		
	}

	if (res)
	{
		DBGPRINTF(("Parsing ordinal api overrides\n"));

		char* line = section_data;
		while (*line)
		{
			char* module;
			char* ordinal_s;
			int ordinal;
			char* apilib_name;
			int id;

			char* p = line;
			char* oldline = line;
			line += strlen(line) + 1;

			module = p;
			p = strchr(p, '.');
			if (!p)
			{
				DBGPRINTF(("Error parsing line: %s\n", oldline));
				continue;
			}
			*p++ = '\0';
			strcpy(buf, module);
			strcat(buf, ".DLL");
			module = buf;
			ordinal_s = p;
			p = strchr(p, '=');
			if (!p)
			{
				DBGPRINTF(("Error parsing line: %s\n", oldline));
				continue;
			}
			*p++ = '\0';
			ordinal = atoi(ordinal_s);
			if (ordinal < 1 || ordinal > 65535)
			{
				DBGPRINTF(("Error parsing line: %s\n", oldline));
				continue;
			}
			apilib_name = p;
			p = strchr(p, '.');
			if (!p)
				id = 0;
			else
			{
				*p++ = '\0';
				id = atoi(p);
				if (id < 0)
				{
					DBGPRINTF(("Error parsing line: %s\n", oldline));
					continue;
				}
			}
			strupr(apilib_name);
			if (!strcmp(apilib_name, "NONE"))
			{
				if (!apiconf->erase(module, (WORD) ordinal))
				{
					DBGPRINTF(("Failed to merge ordinal api overrides\n"));
					return false;
				}
			}
			else 
			{
				if (!apiconf->merge(module, (WORD) ordinal, 
					apilibmgr.get_apilib(apilib_name), id))
				{
					DBGPRINTF(("Failed to merge ordinal api overrides\n"));
					return false;
				}
			}
		}
	}
	return true;
}

#ifdef _DEBUG

void ApiConfigurationManager::dump_configurations()
{
	printf("Dumping all api configurations (count = %d) ...\n\n", apiconf_cnt);
	for (int i = 0 ; i < apiconf_cnt ; i++)
	{
		apiconf_ptrs[i]->dump();
	}
	printf("End dump\n\n");
}

#endif
