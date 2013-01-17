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

#ifndef __SSTRING_HPP
#define __SSTRING_HPP

#include <string.h>

/** Simple string container class. */
class sstring
{
public:
	sstring()
	{
		len = 0;
		storage = new char[1];
		storage[0] = '\0';
	}

	sstring(const char* src)
	{
		len = strlen(src);
		storage = new char[len + 1];
		strcpy(storage, src);
	}
	
	sstring(const sstring& src)
	{
		len = src.len;
		storage = new char[len + 1];
		strcpy(storage, src.storage);
	}
	
	~sstring()
	{
		delete [] storage;
	}

	sstring& operator=(const sstring& src)
	{
		len = src.len;
		delete [] storage;
		storage = new char[len + 1];
		strcpy(storage, src.storage);
		return *this;
	}
	
	sstring& operator=(const char* src)
	{
		len = strlen(src);
		delete [] storage;
		storage = new char[len + 1];
		strcpy(storage, src);
		return *this;
	}

	bool operator<(const sstring& a) const
	{
		return strcmp(storage, a.storage) < 0;
	}
	
	operator const char*() const
	{
		return storage;
	}
	
	int length() const
	{
		return len;
	}

	bool empty() const
	{
		return len == 0;
	}
	
private:
	int len;
	char* storage;
};

#endif
