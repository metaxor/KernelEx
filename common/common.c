/*
 *  KernelEx
 *  Copyright (C) 2008, Xeno86
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

int acp_mcs;

static int GetMaxCharSize(UINT CodePage)
{
	CPINFO cpi;
	if (!GetCPInfo(CodePage, &cpi))
		return 2;
	return cpi.MaxCharSize;
}

BOOL common_init(void)
{
	acp_mcs = GetMaxCharSize(CP_ACP);
	return TRUE;
}

char* file_fixWprefix(char* in)
{
	if (*(int *)in == 0x5c3f5c5c) //if (!strncmp(in, "\\?\", 4))
	{
		in += 4;
		if (*(int *)in == 0x5c434e55) //if (!strncmp(in, "UNC\", 4))
		{
			in += 2;
			*in = '\\';
		}
	}
	return in;
}

void fatal_error(const char* msg)
{
	MessageBox(NULL, msg, "KernelEx error", MB_OK | MB_ICONERROR);
	ExitProcess(1);
}

size_t lstrlenAnull(LPCSTR s)
{
	__try
	{
		LPCSTR ss = s;
		while (*ss) ss++;
		return ss - s + 1;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return 0;
	}
}

size_t lstrlenWnull(LPCWSTR s)
{
	__try
	{
		LPCWSTR ss = s;
		while (*ss) ss++;
		return ss - s + 1;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return 0;
	}
}
