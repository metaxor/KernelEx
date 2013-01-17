/*
 *  KernelEx
 *  Copyright (C) 2008-2011, Xeno86
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

#ifndef __INTERFACE_H
#define __INTERFACE_H

/* Update this whenever patching functions are changed. */
#define KEX_STUB_VER 11

#define KEX_SIGNATURE "KrnlEx"

enum
{
	JTAB_EFO_DYN,
	JTAB_EFO_STA,
	JTAB_EFN_DYN,
	JTAB_EFN_STA,
	JTAB_KNO_DLL,
	JTAB_FLD_TRN,
	JTAB_SYS_CHK,
	JTAB_RES_CHK,
	JTAB_SIZE
};

#include <pshpack1.h>
struct KernelEx_codeseg
{
	char signature[6];   /* "KrnlEx" */
	unsigned short version;
	struct
	{
		WORD opcode; /* 0xFF 0x25 */
		DWORD addr;   /* address to jtab */
		WORD nop;    /* dword align */
	} jmp_stub[JTAB_SIZE];
};

struct KernelEx_dataseg
{
	char signature[6];   /* "KrnlEx" */
	unsigned short version;
	DWORD jtab[JTAB_SIZE];
};
#include <poppack.h>

/*
 * IOCTL Support	
 */

#define IOCTL_CONNECT		1

struct ioctl_connect_params
{
	WORD vxd_version;
	bool status;
	KernelEx_dataseg* stub_ptr;
};

#endif
