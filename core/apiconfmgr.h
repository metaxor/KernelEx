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

#ifndef __APICONFMGR_H
#define __APICONFMGR_H

#include "apiconf.h"

class ApiConfigurationManager
{
public:
	ApiConfigurationManager();
	~ApiConfigurationManager();
	bool load_api_configurations();
	ApiConfiguration* get_default_configuration();
	bool are_extensions_disabled();
	ApiConfiguration* get_api_configuration(const char* conf_name);

#ifdef _DEBUG
	void dump_configurations();
#endif

protected:
	
private:
	char core_conf_file[MAX_PATH];
	ApiConfiguration** apiconf_ptrs;
	int apiconf_cnt;
	ApiConfiguration* default_apiconf;
	bool disable_extensions;

	bool join_apilibs(ApiConfiguration* apiconf);
	bool parse_overrides(ApiConfiguration* apiconf);
	bool add_apiconf(ApiConfiguration* ac);
};

inline ApiConfiguration* ApiConfigurationManager::get_default_configuration()
{
	return default_apiconf;
}

inline bool ApiConfigurationManager::are_extensions_disabled()
{
	return disable_extensions;
}

extern ApiConfigurationManager apiconfmgr;

#endif
