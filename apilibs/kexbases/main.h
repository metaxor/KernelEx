/*
 *  KernelEx
 *  Copyright (C) 2013, Ley0k
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
 
#pragma once

BOOL ppi_init(PPDB98 Process);
BOOL pti_init(PTDB98 Thread);
BOOL thread_init(PTDB98 Thread);
BOOL process_init(PPDB98 Process);
VOID pti_uninit(PTDB98 Thread);
VOID ppi_uninit(PPDB98 Process);
VOID thread_uninit(PTDB98 Thread);
VOID process_uninit(PPDB98 Process);
