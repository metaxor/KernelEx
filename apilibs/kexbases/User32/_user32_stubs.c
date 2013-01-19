/*
 *  KernelEx
 *  Copyright (C) 2008-2010, Xeno86, Tihiy, Ley0k
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

#include "common.h"

UNIMPL_FUNC(InitSharedTable, 0); // InitSharedTable... An application who call this would make the system badly crash
UNIMPL_FUNC(PrintWindow, 3);
UNIMPL_FUNC(RegisterServicesProcess, 1);
UNIMPL_FUNC(SetLayeredWindowAttributes, 4);
