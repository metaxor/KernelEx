/*
 *  KernelEx
 *  Copyright (C) 2008-2010, Xeno86
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

#ifndef __THUNKS_H
#define __THUNKS_H

PROC ExportFromOrdinalStatic_thunk(IMAGE_NT_HEADERS* PEh, WORD ordinal);
PROC ExportFromNameStatic_thunk(IMAGE_NT_HEADERS* PEh, WORD hint, LPCSTR name);
PROC ExportFromOrdinalDynamic_thunk(IMAGE_NT_HEADERS* PEh, WORD ordinal);
PROC ExportFromNameDynamic_thunk(IMAGE_NT_HEADERS* PEh, WORD hint, LPCSTR name);
void SubSysCheck();
extern BYTE mod_ext_ena;

#endif
