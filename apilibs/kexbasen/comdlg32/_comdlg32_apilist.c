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
#include "kexcoresdk.h"
#include "_comdlg32_apilist.h"

BOOL init_comdlg32()
{
	return TRUE;
}

static const apilib_named_api comdlg32_named_apis[] = 
{
/*** AUTOGENERATED APILIST NAMED EXPORTS BEGIN ***/
	DECL_API("ChooseColorW", ChooseColorW_fwd),
	DECL_API("ChooseFontW", ChooseFontW_fwd),
	DECL_API("FindTextW", FindTextW_fwd),
	DECL_API("GetFileTitleW", GetFileTitleW_fwd),
	DECL_API("GetOpenFileNameA", GetOpenFileNameA_fix),
	DECL_API("GetOpenFileNameW", GetOpenFileNameW_new),
	DECL_API("GetSaveFileNameA", GetSaveFileNameA_fix),
	DECL_API("GetSaveFileNameW", GetSaveFileNameW_new),
	DECL_API("PageSetupDlgW", PageSetupDlgW_fwd),
	DECL_API("PrintDlgW", PrintDlgW_fwd),
	DECL_API("ReplaceTextW", ReplaceTextW_fwd),
/*** AUTOGENERATED APILIST NAMED EXPORTS END ***/
};

#if 0
static const apilib_unnamed_api comdlg32_ordinal_apis[] =
{
/*** AUTOGENERATED APILIST ORDINAL EXPORTS BEGIN ***/
/*** AUTOGENERATED APILIST ORDINAL EXPORTS END ***/
};
#endif

const apilib_api_table apitable_comdlg32 = DECL_TAB("COMDLG32.DLL", comdlg32_named_apis, 0 /*comdlg32_ordinal_apis*/);
