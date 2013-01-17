/*
 *  KernelEx
 *  Copyright (C) 2009, Xeno86
 *  Copyright (C) 2010, Tihiy
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

#ifndef __WSTRING_HPP
#define __WSTRING_HPP

#include <string.h>

/** Simple string container class. */
class wstring
{
public:
	wstring(const WCHAR* src)
	{
		len = lstrlenW(src);
		storage = new WCHAR[len + 1];
		lstrcpyW(storage, src);
	}
	
	wstring(const wstring& src)
	{
		len = src.len;
		storage = new WCHAR[len + 1];
		lstrcpyW(storage, src.storage);
	}
	
	~wstring()
	{
		delete [] storage;
	}

	wstring& operator=(const wstring& src)
	{
		len = src.len;
		delete [] storage;
		storage = new WCHAR[len + 1];
		lstrcpyW(storage, src.storage);
		return *this;
	}
	
	wstring& operator=(const WCHAR* src)
	{
		len = lstrlenW(src);
		delete [] storage;
		storage = new WCHAR[len + 1];
		lstrcpyW(storage, src);
		return *this;
	}

	bool operator<(const wstring& a) const
	{
		return lstrcmpiW(storage, a.storage) < 0;
	}
	
	operator const WCHAR*() const
	{
		return storage;
	}
	
	int length() const
	{
		return len;
	}
	
private:
	int len;
	WCHAR* storage;
};

#endif
