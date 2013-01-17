/*
 * Copyright 2009 Xeno86
 * Copyright 1995,1997 Morten Welinder
 * Copyright 1997-1998 Marcus Meissner
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

/* Processor feature flags.  */
#define PF_FLOATING_POINT_PRECISION_ERRATA      0
#define PF_FLOATING_POINT_EMULATED              1
#define PF_COMPARE_EXCHANGE_DOUBLE              2
#define PF_MMX_INSTRUCTIONS_AVAILABLE           3
#define PF_PPC_MOVEMEM_64BIT_OK                 4
#define PF_ALPHA_BYTE_INSTRUCTIONS              5
#define PF_XMMI_INSTRUCTIONS_AVAILABLE          6
#define PF_3DNOW_INSTRUCTIONS_AVAILABLE         7
#define PF_RDTSC_INSTRUCTION_AVAILABLE          8
#define PF_PAE_ENABLED                          9
#define PF_XMMI64_INSTRUCTIONS_AVAILABLE        10
#define PF_SSE_DAZ_MODE_AVAILABLE               11
#define PF_NX_ENABLED                           12
#define PF_SSE3_INSTRUCTIONS_AVAILABLE          13
#define PF_COMPARE_EXCHANGE128                  14
#define PF_COMPARE64_EXCHANGE128                15
#define PF_CHANNELS_ENABLED                     16

#define AUTH    0x68747541      /* "Auth" */
#define ENTI    0x69746e65      /* "enti" */
#define CAMD    0x444d4163      /* "cAMD" */

static BYTE PF[17] = {0,};

static void do_cpuid(unsigned int _eax, unsigned int *p)
{
#ifdef __GNUC__
	__asm__("cpuid"
			: "=a" (p[0]), "=b" (p[1]), "=c" (p[2]), "=d" (p[3])
			:  "a" (_eax));
#else
	__asm {
		mov eax, _eax
		mov edi, p
		cpuid
		mov [edi], eax
		mov [edi+4], ebx
		mov [edi+8], ecx
		mov [edi+12], edx
	}
#endif
}

void get_cpuinfo()
{
	unsigned int regs[4], regs2[4];

	do_cpuid(0x00000000, regs);  /* get standard cpuid level and vendor name */
	if (regs[0]>=0x00000001)   /* Check for supported cpuid version */
	{
		do_cpuid(0x00000001, regs2); /* get cpu features */
		PF[PF_FLOATING_POINT_EMULATED]     = !(regs2[3] & 1);
		PF[PF_RDTSC_INSTRUCTION_AVAILABLE] = (regs2[3] & (1 << 4 )) >> 4;
		PF[PF_COMPARE_EXCHANGE_DOUBLE]     = (regs2[3] & (1 << 8 )) >> 8;
		PF[PF_MMX_INSTRUCTIONS_AVAILABLE]  = (regs2[3] & (1 << 23)) >> 23;
		PF[PF_XMMI_INSTRUCTIONS_AVAILABLE] = (regs2[3] & (1 << 25)) >> 25;
		PF[PF_XMMI64_INSTRUCTIONS_AVAILABLE] = (regs2[3] & (1 << 26)) >> 26;
		PF[PF_SSE3_INSTRUCTIONS_AVAILABLE] = (regs2[2] & 1);

		if (regs[1] == AUTH &&
			regs[3] == ENTI &&
			regs[2] == CAMD)
		{
			do_cpuid(0x80000000, regs);  /* get vendor cpuid level */
			if (regs[0]>=0x80000001)
			{
				do_cpuid(0x80000001, regs2);  /* get vendor features */
				PF[PF_3DNOW_INSTRUCTIONS_AVAILABLE] = (regs2[3] & (1 << 31)) >> 31;
			}
		}
	}
}

/* MAKE_EXPORT IsProcessorFeaturePresent_new=IsProcessorFeaturePresent */
BOOL WINAPI IsProcessorFeaturePresent_new(DWORD feature) 
{	
	if (feature < 17)
		return PF[feature];
	else
		return FALSE;
}
