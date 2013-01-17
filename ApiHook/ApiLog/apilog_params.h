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

#ifndef __APILOGPARAMS_H
#define __APILOGPARAMS_H

#include <map>
#include <stdarg.h>
#include <windows.h>
#include "sstring.hpp"

class ApiLogParams
{
public:
	~ApiLogParams();
	int decode_parameters(const char* lib, const char* fn, va_list ap, char* buf, int len);
	bool load_signatures(const char* file_name);
	static ApiLogParams& inst();

private:
	ApiLogParams();
	const sstring& get_signature(const char* lib, const char* fn);
	size_t readline(HANDLE file, char* buf, size_t size);

    static ApiLogParams* inst_;
	const sstring empty_signature;
	std::map<sstring, std::map<sstring, sstring> > signatures;
};

inline ApiLogParams& ApiLogParams::inst()
{
	if (!inst_) inst_ = new ApiLogParams;
	return *inst_;
}

#endif // __APILOGPARAMS_H
