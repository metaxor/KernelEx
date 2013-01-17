/*
 *  KernelEx
 *  Copyright (C) 2011, Xeno86
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

#include <basedef.h>
#include <vmm.h>

void __cdecl abort(void)
{
	__asm int 3
}

int __cdecl _purecall (void)
{
	abort();
	return 0;
}

void __cdecl _assert(const char* expr, const char* file, unsigned line)
{
	_Debug_Printf_Service("Assertion failed: '%s' in %s line %d", expr, file, line);
	abort();
}

void __declspec(naked) _stdcall RtlUnwind(
	PVOID TargetFrame,
	PVOID TargetIp,
	PVOID ExceptionRecord,
	PVOID ReturnValue
)
{
	VMMJmp(ObsoleteRtlUnwind);
}
