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

/** @file
 * This file contains workarounds for compiler bugs.
 * This file is included in every file MSVC compiles via forced includes /FI flag.
 */

#ifndef __MSVC_QUIRKS_H
#define __MSVC_QUIRKS_H

/* MSVC 6.0 quirks */
#if defined(_MSC_VER) && _MSC_VER < 1201

/* MSVC 6.0 for-loop workaround. */
#define for if (0); else for

/* disable "identifier was truncated to '255' characters in the debug information" */
#pragma warning(disable:4786)

#endif /* MSVC 6.0 quirks */

#endif
