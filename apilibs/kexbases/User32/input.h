/*
 *  KernelEx
 *	Copyright (C) 2013, Ley0k
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

typedef struct _INPUTDATA
{
	BYTE		un1[0x5];						// 000
	BYTE		pKey[256 * 2 / 8];				// 006 - 2 bits per key
	BYTE		pKeyRecentDown[256 / 8];		// 046 - 1 bit per key
	BYTE		un3[0x10];						// 065
	WORD		MouseX;							// 076
	WORD		MouseY;							// 078
} INPUTDATA, *PINPUTDATA;

extern PINPUTDATA pInputData;

BOOL FASTCALL InitInputSegment(void);