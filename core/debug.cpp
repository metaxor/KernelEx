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

#include <stdarg.h>
#include <windows.h>
#include "debug.h"

extern "C"
int vsnprintf(char *buffer, size_t n, const char *format, va_list ap);

void dbgvprintf(const char* format, void* _argp)
{
	char msg[DEBUGMSG_MAXLEN];
	va_list argp = (va_list) _argp;
	int cnt = vsnprintf(msg, sizeof(msg), format, argp);

	OutputDebugString(msg);
}

void dbgprintf(const char* format, ...)
{
	va_list argp;
	va_start(argp, format);
	dbgvprintf(format, argp);
	va_end(argp);
}

