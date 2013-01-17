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

#include <windows.h>
#include <stdio.h>
#include "pemanip.h"

DWORD find_unique_pattern(void* loc, int size, const short* pattern, int pat_len, const char* pat_name)
{
	unsigned char* pos = (unsigned char*) loc;
	unsigned char* end_pos = pos + size - pat_len;
	unsigned char* found_loc = NULL;
	int found = 0;

	while (pos <= end_pos)
	{
		if (pattern[0] < 0 || *pos == pattern[0])
		{
			int j;
			for (j = 1 ; j < pat_len ; j++)
			{
				if (pattern[j] >= 0 && pos[j] != pattern[j])
					break;
			}
			if (j == pat_len) //pattern found
			{
				found++;
				if (!found_loc)
					found_loc = pos;
			}
		}
		pos++;
	}

	if (found != 1)
	{
		if (!found)
			printf("%s: pattern not found\n", pat_name);
		else
			printf("%s: pattern not unique, found occurrences: %d\n", pat_name, found);
		return 0;
	}

	for (int i = 0 ; i < pat_len ; i++)
		if (pattern[i] == -2)
			return *(DWORD*) &found_loc[i];

	printf("%s: invalid pattern\n", pat_name);
	return 0;
}

void eparse()
{
	printf("error parsing string\n");
	exit(1);
}

int main(int argc, char** argv)
{
	char buf[512];
	int debug = 0;
	
	if (argc < 2 || argc > 3)
	{
		printf("invalid parameters\n");
		return 1;
	}
	
	PEmanip pe(argv[1]);
	if (argc == 3 && !strcmp(argv[2], "-d"))
		debug = 1;
	
	FILE* f = fopen("patterns.txt", "rt");
	if (!f)
	{
		printf("failed to open pattern list\n");
		return 1;
	}
	
	while (fgets(buf, sizeof(buf), f))
	{
		short pat[128];
		int patlen = 0;
		char* p, *p2;
		char* pat_name;
		char* sec_name;
		
		pat_name = buf;
		p = strchr(buf, ':');
		if (!p) eparse();
		*p++ = 0;
		sec_name = p;
		p = strchr(p, '{');
		if (!p) eparse();
		*p++ = 0;
		p2 = strchr(p, '}');
		if (!p2) eparse();
		*p2 = 0;
		
		while (p < p2)
		{
			char* p3;
			short i;
			i = strtol(p, &p3, 0);
			if (p == p3) eparse();
			pat[patlen++] = i;
			p = p3 + 1;
		}
		//DWORD start = GetTickCount();
		DWORD ret = find_unique_pattern(pe.GetSectionByName(sec_name), pe.GetSectionSize(sec_name), pat, patlen, pat_name);
		//printf("algo completed in %dms\n", GetTickCount() - start);
		if (debug)
		{
			printf("processing %s in %s: {", pat_name, sec_name);
			for (int i = 0 ; i < patlen ; i++) if (pat[i] < 0) printf("%d,", pat[i]); else printf("0x%02x,", pat[i]);
			printf("\b}=0x%08x\n", ret);
		}
	}
	fclose(f);
	
	return 0;
}
