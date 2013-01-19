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

#include "common.h"
#include "kexcoresdk.h"
#include "_ntdll_apilist.h"

BOOL init_ntdll()
{
	return TRUE;
}

#if 0
static const apilib_named_api ntdll_named_apis[] = 
{
/*** AUTOGENERATED APILIST NAMED EXPORTS BEGIN ***/
/*** AUTOGENERATED APILIST NAMED EXPORTS END ***/
};
#endif

#if 0
static const apilib_unnamed_api ntdll_ordinal_apis[] =
{
/*** AUTOGENERATED APILIST ORDINAL EXPORTS BEGIN ***/
/*** AUTOGENERATED APILIST ORDINAL EXPORTS END ***/
};
#endif

const apilib_api_table apitable_ntdll = DECL_TAB("NTDLL.DLL", 0 /*ntdll_named_apis*/, 0 /*ntdll_ordinal_apis*/);
