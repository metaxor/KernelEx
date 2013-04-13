/*
 *  KernelEx
 *  Copyright (C) 2013 Ley0k
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

#define K32OBJ_PIPE WIN98_K32OBJ_PIPE

typedef struct tagNAMEDPIPE {
	WORD		Type;				// 00 K32OBJ_PIPE
	WORD		cReferences;		// 02

	HANDLE		hWaitEvent;			// Used by WaitNamedPipe (global event)

	LPSTR		lpName;				// Name

	DWORD		dwOpenMode;			// Open mode
	DWORD		dwPipeMode;			// Pipe mode

	WORD		nMaxInstances;		// Max instances from 1 to PIPE_UNLIMITED_INSTANCES (255)
	DWORD		nOutBufferSize;		// Max size of the output buffer
	DWORD		nInBufferSize;		// Max size of the input buffer
	DWORD		nTimeout;			// Default timeout value

	LPSTR		lpBuffer;			// Data buffer for the pipe
} NAMEDPIPE, *PNAMEDPIPE;