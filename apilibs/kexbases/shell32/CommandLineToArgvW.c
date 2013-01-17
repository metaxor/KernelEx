/*
 *                 Shell basics
 *
 * Copyright 1998 Marcus Meissner
 * Copyright 1998 Juergen Schmied (jsch)  *  <juergen.schmied@metronet.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

#include <windows.h>
#include "common.h"
#include "../kernel32/_kernel32_apilist.h"

/*************************************************************************
 * CommandLineToArgvW            [SHELL32.@]
 *
 * We must interpret the quotes in the command line to rebuild the argv
 * array correctly:
 * - arguments are separated by spaces or tabs
 * - quotes serve as optional argument delimiters
 *   '"a b"'   -> 'a b'
 * - escaped quotes must be converted back to '"'
 *   '\"'      -> '"'
 * - an odd number of '\'s followed by '"' correspond to half that number
 *   of '\' followed by a '"' (extension of the above)
 *   '\\\"'    -> '\"'
 *   '\\\\\"'  -> '\\"'
 * - an even number of '\'s followed by a '"' correspond to half that number
 *   of '\', plus a regular quote serving as an argument delimiter (which
 *   means it does not appear in the result)
 *   'a\\"b c"'   -> 'a\b c'
 *   'a\\\\"b c"' -> 'a\\b c'
 * - '\' that are not followed by a '"' are copied literally
 *   'a\b'     -> 'a\b'
 *   'a\\b'    -> 'a\\b'
 *
 * Note:
 * '\t' == 0x0009
 * ' '  == 0x0020
 * '"'  == 0x0022
 * '\\' == 0x005c
 */
 
/* MAKE_EXPORT CommandLineToArgvW_new=CommandLineToArgvW */
LPWSTR* WINAPI CommandLineToArgvW_new(LPCWSTR lpCmdline, int* numargs)
{
	DWORD argc;
	LPWSTR  *argv;
	LPCWSTR cs;
	LPWSTR arg,s,d;
	LPWSTR cmdline;
	int in_quotes,bcount;

	if (*lpCmdline==0)
	{
		/* Return the path to the executable */
		DWORD len, size=16;

		argv=(LPWSTR*)LocalAlloc(LMEM_FIXED, size);
		for (;;)
		{
			len = GetModuleFileNameW_new(0, (LPWSTR)(argv+1), (size-sizeof(LPWSTR))/sizeof(WCHAR));
			if (!len)
			{
				LocalFree(argv);
				return NULL;
			}
			if (len < size) break;
			size*=2;
			argv=(LPWSTR*)LocalReAlloc(argv, size, 0);
		}
		argv[0]=(LPWSTR)(argv+1);
		if (numargs)
			*numargs=2;

		return argv;
	}

	/* to get a writable copy */
	argc=0;
	bcount=0;
	in_quotes=0;
	cs=lpCmdline;
	while (1)
	{
		if (*cs==0 || ((*cs==0x0009 || *cs==0x0020) && !in_quotes))
		{
			/* space */
			argc++;
			/* skip the remaining spaces */
			while (*cs==0x0009 || *cs==0x0020) {
				cs++;
			}
			if (*cs==0)
				break;
			bcount=0;
			continue;
		}
		else if (*cs==0x005c)
		{
			/* '\', count them */
			bcount++;
		}
		else if ((*cs==0x0022) && ((bcount & 1)==0))
		{
			/* unescaped '"' */
			in_quotes=!in_quotes;
			bcount=0;
		}
		else
		{
			/* a regular character */
			bcount=0;
		}
		cs++;
	}
	/* Allocate in a single lump, the string array, and the strings that go with it.
	 * This way the caller can make a single GlobalFree call to free both, as per MSDN.
	 */
	argv=(LPWSTR*)LocalAlloc(LMEM_FIXED, argc*sizeof(LPWSTR)+(lstrlenW(lpCmdline)+1)*sizeof(WCHAR));
	if (!argv)
		return NULL;
	cmdline=(LPWSTR)(argv+argc);
	lstrcpyW(cmdline, lpCmdline);

	argc=0;
	bcount=0;
	in_quotes=0;
	arg=d=s=cmdline;
	while (*s)
	{
		if ((*s==0x0009 || *s==0x0020) && !in_quotes)
		{
			/* Close the argument and copy it */
			*d=0;
			argv[argc++]=arg;

			/* skip the remaining spaces */
			do {
				s++;
			} while (*s==0x0009 || *s==0x0020);

			/* Start with a new argument */
			arg=d=s;
			bcount=0;
		}
		else if (*s==0x005c)
		{
			/* '\\' */
			*d++=*s++;
			bcount++;
		}
		else if (*s==0x0022)
		{
			/* '"' */
			if ((bcount & 1)==0)
			{
				/* Preceded by an even number of '\', this is half that
				 * number of '\', plus a quote which we erase.
				 */
				d-=bcount/2;
				in_quotes=!in_quotes;
				s++;
			}
			else
			{
				/* Preceded by an odd number of '\', this is half that
				 * number of '\' followed by a '"'
				 */
				d=d-bcount/2-1;
				*d++='"';
				s++;
			}
			bcount=0;
		}
		else
		{
			/* a regular character */
			*d++=*s++;
			bcount=0;
		}
	}
	if (*arg)
	{
		*d='\0';
		argv[argc++]=arg;
	}
	if (numargs)
		*numargs=argc;

	return argv;
}
