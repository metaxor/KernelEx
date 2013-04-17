/*
 *  KernelEx
 *  Copyright (C) 2013, Ley0k
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

UNIMPL_FUNC(NtCreateProcess, 8);
UNIMPL_FUNC(NtFsControlFile, 10);
UNIMPL_FUNC(NtOpenDirectoryObject, 3);
UNIMPL_FUNC(NtOpenSymbolicLinkObject, 3);
UNIMPL_FUNC(NtQueryDirectoryObject, 7);
UNIMPL_FUNC(NtQueryInformationFile, 5);
UNIMPL_FUNC(NtQuerySymbolicLinkObject, 3);
UNIMPL_FUNC(NtSetInformationFile, 5);

UNIMPL_FUNC(RtlAddAccessAllowedAce, 4);
UNIMPL_FUNC(RtlAddAce, 5);
UNIMPL_FUNC(RtlClearBits, 3);
UNIMPL_FUNC(RtlCreateAcl, 3);
UNIMPL_FUNC(RtlDecompressBuffer, 6);
UNIMPL_FUNC(RtlDeleteElementGenericTable, 2);
UNIMPL_FUNC(RtlEnumerateGenericTableWithoutSplaying, 2);
UNIMPL_FUNC(RtlFindMessage, 5);
UNIMPL_FUNC(RtlFindSetBits, 3);
UNIMPL_FUNC(RtlFormatMessage, 8);
UNIMPL_FUNC(RtlInitializeGenericTable, 5);
UNIMPL_FUNC(RtlInsertElementGenericTable, 4);
UNIMPL_FUNC(RtlLengthSecurityDescriptor, 1);
UNIMPL_FUNC(RtlLookupElementGenericTable, 2);
UNIMPL_FUNC(RtlNewSecurityObject, 6);
UNIMPL_FUNC(RtlNormalizeProcessParams, 1);
UNIMPL_FUNC(RtlNumberOfSetBits, 1);
UNIMPL_FUNC(RtlQueryInformationAcl, 4);
UNIMPL_FUNC(RtlQueryRegistryValues, 5);
UNIMPL_FUNC(RtlSetBits, 3);
UNIMPL_FUNC(RtlSetDaclSecurityDescriptor, 4);
UNIMPL_FUNC(RtlSetGroupSecurityDescriptor, 3);
UNIMPL_FUNC(RtlSetOwnerSecurityDescriptor, 3);
UNIMPL_FUNC(RtlTimeToTimeFields, 2);
UNIMPL_FUNC(RtlValidSecurityDescriptor, 1);
UNIMPL_FUNC(RtlWriteRegistryValue, 6);

UNIMPL_FUNC(ZwCreateProcess, 8);
UNIMPL_FUNC(ZwFsControlFile, 10);
UNIMPL_FUNC(ZwOpenDirectoryObject, 3);
UNIMPL_FUNC(ZwOpenSymbolicLinkObject, 3);
UNIMPL_FUNC(ZwQueryDirectoryObject, 7);
UNIMPL_FUNC(ZwQueryInformationFile, 5);
UNIMPL_FUNC(ZwQuerySymbolicLinkObject, 3);
UNIMPL_FUNC(ZwSetInformationFile, 5);