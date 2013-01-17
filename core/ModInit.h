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

#ifndef __MODINIT_H
#define __MODINIT_H

#include "kstructs.h"

bool ModuleInitializer_init();

class ModuleInitializer
{
public:
	void add_module(MODREF* mr);
	DWORD get_handle_for_index(WORD idx);
	bool has_initialize();
	bool initialize_modules();
	void destroy();
	
	static ModuleInitializer* get_instance(bool alloc);

private:
	int size;
	int init;
	MODREF* data[64];

	ModuleInitializer();
	~ModuleInitializer();
};

inline bool ModuleInitializer::has_initialize()
{
	return init < size;
}

#endif
