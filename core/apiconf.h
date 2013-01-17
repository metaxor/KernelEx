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

#ifndef __APICONF_H
#define __APICONF_H

#include "apilib.h"

class ApiConfiguration
{
	struct NamedApi
	{
		const char*     api_name;     /* function name */
		unsigned long   address;      /* function address */
		bool operator<(const NamedApi& a) const;
	};

	struct ModuleApi
	{
		unsigned short  named_apis_count;
		unsigned short  unnamed_apis_count;
		NamedApi*       named_apis;
		unsigned long*  unnamed_apis; /* function addresses */
	};

public:
	ApiConfiguration(const char* name);
	ApiConfiguration(const char* new_name, const ApiConfiguration& src);
	~ApiConfiguration();

	bool is_table_empty(unsigned short table_id);
	const char* get_name();

	bool merge(const ApiLibrary* apilib);
	bool merge(const char* module, unsigned short ordinal, 
			const ApiLibrary* apilib, int id);
	bool merge(const char* module, const char* api_name, 
			const ApiLibrary* apilib, int id);
	bool erase(const char* module, unsigned short ordinal);
	bool erase(const char* module, const char* api_name);
	unsigned long get(unsigned short table_id, unsigned short hint, const char* api_name);
	unsigned long get(unsigned short table_id, unsigned short api_ordinal);
	bool is_initialized();

#ifdef _DEBUG
	void dump();
#endif

	friend class ApiConfigurationManager;

protected:

private:
	ModuleApi* api_tables;
	unsigned short api_tables_count;
	char* conf_name;
	bool initialized;

	bool prepare(const char* name);
};

inline bool ApiConfiguration::NamedApi::operator<(const NamedApi& a) const
{
	return strcmp(this->api_name, a.api_name) < 0;
}

/** Returns true if selected api table is empty or invalid.
 * @return True if selected api table is empty or invalid, false otherwise.
 */
inline bool ApiConfiguration::is_table_empty(unsigned short table_id)
{
	if (table_id >= api_tables_count)
		return true;
	return api_tables[table_id].unnamed_apis_count ? false : true;
}

/** Retrieves api configuration name.
 * @return Api configuration name.
 */
inline const char* ApiConfiguration::get_name()
{
	return conf_name;
}

#endif
