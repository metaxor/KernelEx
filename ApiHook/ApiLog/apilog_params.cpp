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

#include <malloc.h>
#include "apilog_params.h"

extern "C"
int vsnprintf(char *buffer, size_t n, const char *format, va_list ap);

//singleton instance
ApiLogParams* ApiLogParams::inst_;

ApiLogParams::ApiLogParams() : empty_signature("")
{
}

ApiLogParams::~ApiLogParams()
{
}

int ApiLogParams::decode_parameters(const char* lib, const char* fn, va_list ap, 
									 char* buf, int len)
{
	const sstring& sign = get_signature(lib, fn);

	if (sign.empty())
		return 0;

	int slen = sign.length();
	int extra = 0;
	for (int i = 0 ; i < slen ; i++)
	{
		if (sign[i] == 's' || sign[i] == 'S') extra += 2;
	}
	char* format = (char*) alloca(slen * 3 + 3);
	char* p = format;

	*p++ = '(';

	for (int i = 0 ; i < slen ; i++)
	{
		bool wide = false;
		char fc = sign[i];
		if (i != 0)
			*p++ = ' ';
		if (fc == 's' || fc == 'S') *p++ = '\"';
		*p++ = '%';
		*p++ = fc;
		if (fc == 's' || fc == 'S') *p++ = '\"';
	}

	*p++ = ')';
	*p++ = '\0';

	__try 
	{
		return vsnprintf(buf, len, format, ap);
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		//better output nothing than crash
		return 0;
	}
}

size_t ApiLogParams::readline(HANDLE file, char* buf, size_t size)
{
	char c;
	DWORD nr;
	size_t pos = 0;
	bool cmt = false;
	bool read_sth = false;

	//fill buffer with a line, skipping comments, trimming line to buffer size
	while (ReadFile(file, &c, 1, &nr, NULL) && nr == 1)
	{
		read_sth = true;
		if (c == '\r')
			continue;
		if (c == '\n')
			break;
		if (c == ';' || cmt)
		{
			cmt = true;
			continue;
		}
		if (pos < size-1)
			buf[pos++] = c;
	}
	if (read_sth)
		buf[pos++] = '\0';

	return pos;
}

bool ApiLogParams::load_signatures(const char* file_name)
{
	char buf[256];
	sstring lib;
	HANDLE file = CreateFile(file_name, GENERIC_READ, FILE_SHARE_READ, 
			NULL, OPEN_EXISTING, 0, NULL);
	if (file == INVALID_HANDLE_VALUE)
		return false;

	while (readline(file, buf, sizeof(buf)) != 0)
	{
		char* p;
		char* fun;
		char* sig;

		//library name
		if (buf[0] == '[')
		{
			p = strchr(buf, ']');
			if (!p) continue;
			*p = '\0';
			strupr(buf);
			lib = buf + 1;
		}
		//function definition
		else
		{
			fun = buf;
			p = strchr(buf, '=');
			if (!p) continue;
			*p++ = '\0';
			sig = p;

			if (!lib.empty())
				signatures[lib][fun] = sig;
		}
	}

	CloseHandle(file);
	return true;
}

const sstring& ApiLogParams::get_signature(const char* lib, const char* fn)
{
	std::map<sstring, std::map<sstring, sstring> >::iterator outer;
	std::map<sstring, sstring>::iterator inner;

	outer = signatures.find(lib);

	if (outer == signatures.end())
		return empty_signature;

	inner = outer->second.find(fn);

	if (inner == outer->second.end())
		return empty_signature;

	return inner->second;
}
