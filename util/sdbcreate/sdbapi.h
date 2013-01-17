/*
 *  KernelEx
 *  Copyright (C) 2010, Tihiy
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

#ifndef __SDBAPI_H
#define __SDBAPI_H

#include <windows.h>

#ifdef SDBAPI_IMPLIB

#ifdef _MSC_VER
#pragma warning(disable:4716)
#endif
#define MAKE_HEADER(decl) \
	__declspec(dllexport) decl {}

#else

#define MAKE_HEADER(decl) \
	__declspec(dllimport) decl ;

#endif

#ifdef __cplusplus
extern "C" {
#endif

//types
typedef PVOID PDB;
typedef DWORD INDEXID;
typedef DWORD TAGID;
typedef DWORD TAG;

//enums
typedef enum _PATH_TYPE {
  DOS_PATH,
  NT_PATH 
} PATH_TYPE;

//tags
#define TAG_DWORD				0x4000
#define TAG_MSI_TRANSFORM_TAGID	0x401B

#define TAG_STRING				0x6000
#define TAG_NAME				0x6001
#define TAG_APP_NAME			0x6006
#define TAG_MSI_TRANSFORM_FILE	0x601F
#define TAG_COMPILER_VERSION	0x6022

#define TAG_LIST				0x7000
#define TAG_DATABASE			0x7001
#define TAG_LIBRARY				0x7002
#define TAG_FILE				0x700C
#define TAG_MSI_TRANSFORM		0x7010
#define TAG_MSI_TRANSFORM_REF	0x7011
#define TAG_MSI_PACKAGE			0x7012

#define TAG_BINARY				0x9000
#define TAG_FILE_BITS			0x9003
#define TAG_EXE_ID				0x9004
#define TAG_MSI_PACKAGE_ID		0x9006
#define TAG_DATABASE_ID			0x9007




//functions
MAKE_HEADER(PDB WINAPI SdbCreateDatabase(LPCWSTR pwszPath, PATH_TYPE eType))
MAKE_HEADER(void WINAPI SdbCloseDatabase(PDB pdb))
MAKE_HEADER(void WINAPI SdbCloseDatabaseWrite(PDB pdb))
MAKE_HEADER(BOOL WINAPI SdbDeclareIndex(PDB pdb,TAG tWhich,TAG tKey,DWORD dwEntries,BOOL bUniqueKey,INDEXID *piiIndex))
MAKE_HEADER(BOOL WINAPI SdbCommitIndexes(PDB pdb))
MAKE_HEADER(BOOL WINAPI SdbStartIndexing(PDB pdb,INDEXID iiWhich))
MAKE_HEADER(BOOL WINAPI SdbStopIndexing(PDB pdb,INDEXID iiWhich))
MAKE_HEADER(TAGID WINAPI SdbBeginWriteListTag(PDB pdb,TAG tTag))
MAKE_HEADER(BOOL WINAPI SdbEndWriteListTag(PDB pdb,TAGID tiList))
MAKE_HEADER(BOOL WINAPI SdbWriteBinaryTag(PDB pdb,TAG tTag,PBYTE pBuffer,DWORD dwSize))
MAKE_HEADER(BOOL WINAPI SdbWriteBinaryTagFromFile(PDB pdb,TAG tTag,LPCWSTR pwszPath))
MAKE_HEADER(BOOL WINAPI SdbWriteStringTag(PDB pdb,TAG tTag,LPCWSTR pwszData))
MAKE_HEADER(BOOL WINAPI SdbWriteDWORDTag(PDB pdb,TAG tTag,DWORD dwData))
MAKE_HEADER(BOOL WINAPI SdbGUIDFromString(LPWSTR str, GUID* guid))
/*
MAKE_HEADER(
MAKE_HEADER(
MAKE_HEADER(
MAKE_HEADER(
MAKE_HEADER(
MAKE_HEADER(*/


#ifdef __cplusplus
}
#endif

#endif
