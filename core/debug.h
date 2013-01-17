/*
 *  KernelEx
 *  Copyright (C) 2008-2009, Xeno86
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

#ifndef __DEBUG_H
#define __DEBUG_H

#define DEBUGMSG_MAXLEN 256

#ifndef _DEBUG

#define DBGPRINTF(x) do { } while (0)
#define DBGASSERT(x) do { } while (0)

#else

#define DBGPRINTF(x) dbgprintf x
#define DBGASSERT(x) \
		if (!(x)) \
		do \
		{ \
			dbgprintf("ASSERTION FAILED: file: %s, line: %d\n", __FILE__, __LINE__); \
			DebugBreak(); \
		} \
		while (0)

#endif


void dbgvprintf(const char* format, void* _argp);
void dbgprintf(const char* format, ...);

#endif
