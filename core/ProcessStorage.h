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

#ifndef __PROCESSSTORAGE_H
#define __PROCESSSTORAGE_H

#define PS_MAX_INDEX 64

class ProcessStorage
{
public:
	static ProcessStorage* get_instance();
	void* get(int index);
	bool set(int index, void* value);
	static int allocate();

private:
	static long count;
	void* data[PS_MAX_INDEX];

	static ProcessStorage* create_instance();
	ProcessStorage();
	~ProcessStorage();
};

#endif
