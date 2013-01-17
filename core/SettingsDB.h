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

#ifndef __SETTINGSDB_H
#define __SETTINGSDB_H

#include "sstring.hpp"
#include <map>
#include "resolver.h"
#include "apiconf.h"

using namespace std;

class SettingsDB
{
public:
	static SettingsDB instance;

	~SettingsDB();
	void clear();
	void flush_all();
	appsetting get_appsetting(const char* path);
	void write_single(const char* path, const char* conf_name, BYTE flags);
	void reset_single(const char* path);
#ifdef _DEBUG
	void dump_db();
#endif

private:
	map<sstring, appsetting> db;
	map<sstring, appsetting> db_wild;
	CRITICAL_SECTION cs;

	SettingsDB();
	void parse_configs();
	void parse_flags();
	void add_apilib_excludes();
};

#endif
