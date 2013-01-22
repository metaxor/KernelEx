/*
 * Copyright 1996-1998 Marcus Meissner
 * Copyright 1999, 2000 Juergen Schmied <juergen.schmied@debitel.net>
 * Copyright 2003 CodeWeavers Inc. (Ulrich Czekalla)
 * Copyright 2007 Xeno86
 * Copyright 2009 Tihiy
 * Copyright 2013 Ley0k
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* 
 * The purpose of this code is NOT to add additional security 
 * and access rights but to satisfy some of the installers which
 * check those APIs and fail with "You must have administrator rights"
 * messages when those APIs are not implemented.
 * The code has been borrowed from Wine project.
 */

#include "security.h"

static void dumpLsaAttributes( PLSA_OBJECT_ATTRIBUTES oa )
{
        if (oa)
        {
          TRACE("\n\tlength=%lu, rootdir=0x%08x, objectname=%s\n\tattr=0x%08lx, sid=%p qos=%p\n",
                oa->Length, oa->RootDirectory,
                oa->ObjectName?debugstr_w(oa->ObjectName->Buffer):"null",
                oa->Attributes, oa->SecurityDescriptor, oa->SecurityQualityOfService);
        }
}

static SID const sidWorld = { SID_REVISION, 1, { SECURITY_WORLD_SID_AUTHORITY} , { SECURITY_WORLD_RID } };

#define WINE_SIZE_OF_WORLD_ACCESS_ACL   (sizeof(ACL) + sizeof(ACCESS_ALLOWED_ACE) + sizeof(sidWorld) - sizeof(DWORD))

static void GetWorldAccessACL(PACL pACL)
{
    PACCESS_ALLOWED_ACE pACE = (PACCESS_ALLOWED_ACE) (pACL + 1);

    pACL->AclRevision = ACL_REVISION;
    pACL->Sbz1 = 0;
    pACL->AclSize = WINE_SIZE_OF_WORLD_ACCESS_ACL;
    pACL->AceCount = 1;
    pACL->Sbz2 = 0;

    pACE->Header.AceType = ACCESS_ALLOWED_ACE_TYPE;
    pACE->Header.AceFlags = CONTAINER_INHERIT_ACE;
    pACE->Header.AceSize = sizeof(ACCESS_ALLOWED_ACE) + sizeof(sidWorld) - sizeof(DWORD);
    pACE->Mask = 0xf3ffffff; /* Everything except reserved bits */
    memcpy(&pACE->SidStart, &sidWorld, sizeof(sidWorld));
}

/*      ##############################
        ######  TOKEN FUNCTIONS ######
        ##############################
*/

/******************************************************************************
 * OpenProcessToken                     [ADVAPI32.@]
 * Opens the access token associated with a process
 *
 * PARAMS
 *   ProcessHandle [I] Handle to process
 *   DesiredAccess [I] Desired access to process
 *   TokenHandle   [O] Pointer to handle of open access token
 *
 * RETURNS STD
 */
/* MAKE_EXPORT OpenProcessToken_new=OpenProcessToken */
BOOL WINAPI
OpenProcessToken_new( HANDLE ProcessHandle, DWORD DesiredAccess, 
                  HANDLE *TokenHandle )
{
	FIXME("OpenProcessToken(0x%08x,0x%08lx,%p): stub\n",
		ProcessHandle,DesiredAccess, TokenHandle);
	*(int*)TokenHandle = 0xcafe;
	return TRUE;
}

/******************************************************************************
 * OpenThreadToken [ADVAPI32.@]
 *
 * PARAMS
 *   thread        []
 *   desiredaccess []
 *   openasself    []
 *   thandle       []
 */
/* MAKE_EXPORT OpenThreadToken_new=OpenThreadToken */
BOOL WINAPI
OpenThreadToken_new( HANDLE ThreadHandle, DWORD DesiredAccess, 
                 BOOL OpenAsSelf, HANDLE *TokenHandle)
{
    FIXME("OpenThreadToken(0x%08x,0x08lx,0x%08x,%p): stub\n",
        ThreadHandle,DesiredAccess, OpenAsSelf, TokenHandle);
	*(int*)TokenHandle = 0xcafe;
	return TRUE;
}

/******************************************************************************
 * DuplicateTokenEx [ADVAPI32.@]
 */
/* MAKE_EXPORT DuplicateTokenEx_new=DuplicateTokenEx */
BOOL WINAPI DuplicateTokenEx_new(
        HANDLE ExistingTokenHandle, DWORD dwDesiredAccess,
        LPSECURITY_ATTRIBUTES lpTokenAttributes,
        SECURITY_IMPERSONATION_LEVEL ImpersonationLevel,
        TOKEN_TYPE TokenType,
        PHANDLE DuplicateTokenHandle )
{
    FIXME("DuplicateTokenEx(0x%08x,0x%08lx,0x%08x,0x%08x,%p): stub\n", ExistingTokenHandle, dwDesiredAccess,
          ImpersonationLevel, TokenType, DuplicateTokenHandle);
    *DuplicateTokenHandle = ExistingTokenHandle;

    return TRUE;
}

/******************************************************************************
 * DuplicateToken [ADVAPI32.@]
 */
/* MAKE_EXPORT DuplicateToken_new=DuplicateToken */
BOOL WINAPI DuplicateToken_new(
        HANDLE ExistingTokenHandle,
        SECURITY_IMPERSONATION_LEVEL ImpersonationLevel,
        PHANDLE DuplicateTokenHandle )
{
    return DuplicateTokenEx_new( ExistingTokenHandle, 0, NULL, ImpersonationLevel,
                             TokenImpersonation, DuplicateTokenHandle );
}

/******************************************************************************
 * AdjustTokenPrivileges [ADVAPI32.@]
 *
 * PARAMS
 *   TokenHandle          []
 *   DisableAllPrivileges []
 *   NewState             []
 *   BufferLength         []
 *   PreviousState        []
 *   ReturnLength         []
 */
/* MAKE_EXPORT AdjustTokenPrivileges_new=AdjustTokenPrivileges */
BOOL WINAPI
AdjustTokenPrivileges_new( HANDLE TokenHandle, BOOL DisableAllPrivileges,
                       PTOKEN_PRIVILEGES NewState, DWORD BufferLength, 
                       PTOKEN_PRIVILEGES PreviousState, LPDWORD ReturnLength )
{
    FIXME("AdjustTokenPrivileges(0x%08x,0x%08x,%p,0x%08lx,%p,%p),stub!\n",
        TokenHandle, DisableAllPrivileges, NewState, BufferLength, PreviousState, ReturnLength);
	return TRUE;
}

/******************************************************************************
 * CheckTokenMembership [ADVAPI32.@]
 *
 * PARAMS
 *   TokenHandle []
 *   SidToCheck  []
 *   IsMember    []
 */
/* MAKE_EXPORT CheckTokenMembership_new=CheckTokenMembership */
BOOL WINAPI
CheckTokenMembership_new( HANDLE TokenHandle, PSID SidToCheck,
                      PBOOL IsMember )
{
	FIXME("CheckTokenMembership(0x%08x %p %p) stub!\n", TokenHandle, SidToCheck, IsMember);

	*IsMember = TRUE;
	return TRUE;
}

/******************************************************************************
 * GetTokenInformation [ADVAPI32.@]
 *
 * PARAMS
 *   token           []
 *   tokeninfoclass  []
 *   tokeninfo       []
 *   tokeninfolength []
 *   retlen          []
 *
 */
/* MAKE_EXPORT GetTokenInformation_new=GetTokenInformation */
BOOL WINAPI
GetTokenInformation_new( HANDLE token, TOKEN_INFORMATION_CLASS tokeninfoclass, 
                     LPVOID tokeninfo, DWORD tokeninfolength, LPDWORD retlen )
{
        unsigned int len = 0;

    FIXME("GetTokenInformation(%08x,%ld,%p,%ld,%p): stub\n",
          token,tokeninfoclass,tokeninfo,tokeninfolength,retlen);

    switch (tokeninfoclass)
    {
    case TokenUser:
        len = sizeof(TOKEN_USER) + sizeof(SID);
        break;
    case TokenGroups:
        len = sizeof(TOKEN_GROUPS);
        break;
    case TokenOwner:
        len = sizeof(TOKEN_OWNER) + sizeof(SID);
        break;
    case TokenPrimaryGroup:
        len = sizeof(TOKEN_PRIMARY_GROUP);
        break;
    case TokenDefaultDacl:
        len = sizeof(TOKEN_DEFAULT_DACL);
        break;
    case TokenSource:
        len = sizeof(TOKEN_SOURCE);
        break;
    case TokenType:
        len = sizeof (TOKEN_TYPE);
        break;
#if 0
	case TokenImpersonationLevel:
    case TokenStatistics:
#endif /* 0 */
    }

    if (retlen) *retlen = len;

    if (tokeninfolength < len)
	{
		SetLastError(ERROR_INSUFFICIENT_BUFFER);
        return FALSE;
	}

    switch (tokeninfoclass)
    {
    case TokenUser:
        if( tokeninfo )
        {
            TOKEN_USER * tuser = (TOKEN_USER*) tokeninfo;
            PSID sid = (PSID) (tuser + 1);
            SID_IDENTIFIER_AUTHORITY localSidAuthority = {SECURITY_NT_AUTHORITY};
            InitializeSid_new(sid, &localSidAuthority, 1);
            *(GetSidSubAuthority_new(sid, 0)) = SECURITY_INTERACTIVE_RID;
            tuser->User.Sid = sid;
        }
        break;
    case TokenGroups:
        if (tokeninfo)
        {
            TOKEN_GROUPS *tgroups = (TOKEN_GROUPS*) tokeninfo;
            SID_IDENTIFIER_AUTHORITY sid = {SECURITY_NT_AUTHORITY};

            /* we need to show admin privileges ! */
            tgroups->GroupCount = 1;
            tgroups->Groups->Attributes = SE_GROUP_ENABLED;
            AllocateAndInitializeSid_new(&sid,
									 2,
									 SECURITY_BUILTIN_DOMAIN_RID,
									 DOMAIN_ALIAS_RID_ADMINS,
									 0, 0, 0, 0, 0, 0,
									 &(tgroups->Groups->Sid));
        }
        break;
    case TokenPrimaryGroup:
        if (tokeninfo)
        {
            TOKEN_PRIMARY_GROUP *tgroup = (TOKEN_PRIMARY_GROUP*) tokeninfo;
            SID_IDENTIFIER_AUTHORITY sid = {SECURITY_NT_AUTHORITY};
            AllocateAndInitializeSid_new( &sid,
                                         2,
                                         SECURITY_BUILTIN_DOMAIN_RID,
                                         DOMAIN_ALIAS_RID_ADMINS,
                                         0, 0, 0, 0, 0, 0,
                                         &(tgroup->PrimaryGroup));
        }
        break;
    case TokenPrivileges:
		{
			TOKEN_PRIVILEGES *tpriv = (TOKEN_PRIVILEGES*) tokeninfo;
			
			len = FIELD_OFFSET( TOKEN_PRIVILEGES, Privileges) + 0 * sizeof(LUID_AND_ATTRIBUTES);
			if (retlen) *retlen = len;
			if (tokeninfolength < len)
			{
				SetLastError(ERROR_INSUFFICIENT_BUFFER);
				return FALSE;
			}
			if (tpriv) tpriv->PrivilegeCount = 0;
		}
        break;
    case TokenOwner:
        if (tokeninfo)
        {
            TOKEN_OWNER *owner = (TOKEN_OWNER*) tokeninfo;
            PSID sid = (PSID) (owner + 1);
            SID_IDENTIFIER_AUTHORITY localSidAuthority = {SECURITY_NT_AUTHORITY};
            InitializeSid_new(sid, &localSidAuthority, 1);
            *(GetSidSubAuthority_new(sid, 0)) = SECURITY_INTERACTIVE_RID;
            owner->Owner = sid;
        }
        break;
    default:
        {
            SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
            return FALSE;
        }
    }
    return TRUE;
}

/*************************************************************************
 * SetThreadToken [ADVAPI32.@]
 *
 * Assigns an "impersonation token" to a thread so it can assume the
 * security privledges of another thread or process.  Can also remove
 * a previously assigned token.  Only supported on NT - it's a stub 
 * exactly like this one on Win9X.
 *
 */
/* MAKE_EXPORT SetThreadToken_new=SetThreadToken */
BOOL WINAPI SetThreadToken_new(PHANDLE thread, HANDLE token)
{
    FIXME("SetThreadToken(%p,%08x): stub\n", thread, token);
    return TRUE;
}

/*      ##############################
        ######  SID FUNCTIONS   ######
        ##############################
*/

/******************************************************************************
 * AllocateAndInitializeSid [ADVAPI32.@]
 *
 * PARAMS
 *   pIdentifierAuthority []
 *   nSubAuthorityCount   []
 *   nSubAuthority0       []
 *   nSubAuthority1       []
 *   nSubAuthority2       []
 *   nSubAuthority3       []
 *   nSubAuthority4       []
 *   nSubAuthority5       []
 *   nSubAuthority6       []
 *   nSubAuthority7       []
 *   pSid                 []
 */
/* MAKE_EXPORT AllocateAndInitializeSid_new=AllocateAndInitializeSid */
BOOL WINAPI
AllocateAndInitializeSid_new( PSID_IDENTIFIER_AUTHORITY pIdentifierAuthority,
                          BYTE nSubAuthorityCount,
                          DWORD nSubAuthority0, DWORD nSubAuthority1,
                          DWORD nSubAuthority2, DWORD nSubAuthority3,
                          DWORD nSubAuthority4, DWORD nSubAuthority5,
                          DWORD nSubAuthority6, DWORD nSubAuthority7,
                          PSID *pSid )
{
    SID *tmp_sid;

    TRACE("AllocateAndInitializeSid(%p, 0x%04x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,%p)\n",
                pIdentifierAuthority,nSubAuthorityCount,
                nSubAuthority0, nSubAuthority1, nSubAuthority2, nSubAuthority3,
                nSubAuthority4, nSubAuthority5, nSubAuthority6, nSubAuthority7, pSid);

    if (nSubAuthorityCount > 8)
	{
		SetLastError(ERROR_INVALID_SID);
		return FALSE;
	}

    if (!(tmp_sid= (SID*) HeapAlloc( GetProcessHeap(), 0,
                                    GetSidLengthRequired_new(nSubAuthorityCount))))
    {
		SetLastError(ERROR_NOT_ENOUGH_MEMORY);
		return FALSE;
	}

    tmp_sid->Revision = SID_REVISION;

    if (pIdentifierAuthority)
        memcpy(&tmp_sid->IdentifierAuthority, pIdentifierAuthority, sizeof(SID_IDENTIFIER_AUTHORITY));
    tmp_sid->SubAuthorityCount = nSubAuthorityCount;

    switch( nSubAuthorityCount )
    {
        case 8: tmp_sid->SubAuthority[7]= nSubAuthority7;
        case 7: tmp_sid->SubAuthority[6]= nSubAuthority6;
        case 6: tmp_sid->SubAuthority[5]= nSubAuthority5;
        case 5: tmp_sid->SubAuthority[4]= nSubAuthority4;
        case 4: tmp_sid->SubAuthority[3]= nSubAuthority3;
        case 3: tmp_sid->SubAuthority[2]= nSubAuthority2;
        case 2: tmp_sid->SubAuthority[1]= nSubAuthority1;
        case 1: tmp_sid->SubAuthority[0]= nSubAuthority0;
        break;
    }
    *pSid = tmp_sid;
    return TRUE;
}

/******************************************************************************
 * FreeSid [ADVAPI32.@]
 *
 * PARAMS
 *   pSid []
 */
/* MAKE_EXPORT FreeSid_new=FreeSid */
PVOID WINAPI
FreeSid_new( PSID pSid )
{
	TRACE("FreeSid(%p)\n", pSid);
	HeapFree( GetProcessHeap(), 0, pSid );
	return NULL; /* is documented like this */
}

/******************************************************************************
 * CopySid [ADVAPI32.@]
 *
 * PARAMS
 *   nDestinationSidLength []
 *   pDestinationSid       []
 *   pSourceSid            []
 */
/* MAKE_EXPORT CopySid_new=CopySid */
BOOL WINAPI
CopySid_new( DWORD nDestinationSidLength, PSID pDestinationSid, PSID pSourceSid )
{
	if (!IsValidSid_new(pSourceSid) ||
		(nDestinationSidLength < GetLengthSid_new(pSourceSid)))
	  return FALSE;

	if (nDestinationSidLength < (((SID*)pSourceSid)->SubAuthorityCount*4u+8))
	  return FALSE;

	memmove(pDestinationSid, pSourceSid, ((SID*)pSourceSid)->SubAuthorityCount*4+8);
	return TRUE;
}

/******************************************************************************
 * IsValidSid [ADVAPI32.@]
 *
 * PARAMS
 *   pSid []
 */
/* MAKE_EXPORT IsValidSid_new=IsValidSid */
BOOL WINAPI
IsValidSid_new( PSID pSid )
{
	if (!pSid || IsBadWritePtr(pSid, sizeof(SID)) || ((SID*)pSid)->Revision != SID_REVISION ||
		((SID*)pSid)->SubAuthorityCount > SID_MAX_SUB_AUTHORITIES)
	{
		return FALSE;
	}
    return TRUE;
}

/******************************************************************************
 * EqualSid [ADVAPI32.@]
 *
 * PARAMS
 *   pSid1 []
 *   pSid2 []
 */
/* MAKE_EXPORT EqualSid_new=EqualSid */
BOOL WINAPI
EqualSid_new( PSID pSid1, PSID pSid2 )
{
    if (!IsValidSid_new(pSid1) || !IsValidSid_new(pSid2))
        return FALSE;

    if (*GetSidSubAuthorityCount_new(pSid1) != *GetSidSubAuthorityCount_new(pSid2))
        return FALSE;

    if (memcmp(pSid1, pSid2, GetLengthSid_new(pSid1)) != 0)
        return FALSE;

    return TRUE;
}

/******************************************************************************
 * EqualPrefixSid [ADVAPI32.@]
 */
/* MAKE_EXPORT EqualPrefixSid_new=EqualPrefixSid */
BOOL WINAPI EqualPrefixSid_new (PSID pSid1, PSID pSid2) 
{
    if (!IsValidSid_new(pSid1) || !IsValidSid_new(pSid2))
        return FALSE;

    if (*GetSidSubAuthorityCount_new(pSid1) != *GetSidSubAuthorityCount_new(pSid2))
        return FALSE;

    if (memcmp(pSid1, pSid2, GetSidLengthRequired_new(((SID*)pSid1)->SubAuthorityCount - 1)) != 0)
        return FALSE;

    return TRUE;
}

/******************************************************************************
 * GetSidLengthRequired [ADVAPI32.@]
 *
 * PARAMS
 *   nSubAuthorityCount []
 */
/* MAKE_EXPORT GetSidLengthRequired_new=GetSidLengthRequired */
DWORD WINAPI
GetSidLengthRequired_new( BYTE nSubAuthorityCount )
{
    return (nSubAuthorityCount-1)*sizeof(DWORD) + sizeof(SID);
}

/******************************************************************************
 * InitializeSid [ADVAPI32.@]
 *
 * PARAMS
 *   pIdentifierAuthority []
 */
/* MAKE_EXPORT InitializeSid_new=InitializeSid */
BOOL WINAPI
InitializeSid_new (
        PSID pSid,
        PSID_IDENTIFIER_AUTHORITY pIdentifierAuthority,
        BYTE nSubAuthorityCount)
{
	int i;
	SID* pisid=(SID*)pSid;

	if (!pSid || nSubAuthorityCount >= SID_MAX_SUB_AUTHORITIES)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	pisid->Revision = SID_REVISION;
	pisid->SubAuthorityCount = nSubAuthorityCount;
	if (pIdentifierAuthority)
	  memcpy(&pisid->IdentifierAuthority, pIdentifierAuthority, sizeof (SID_IDENTIFIER_AUTHORITY));

	for (i = 0; i < nSubAuthorityCount; i++)
	  *GetSidSubAuthority_new(pSid, i) = 0;

	return TRUE;
}

/******************************************************************************
 * GetSidIdentifierAuthority [ADVAPI32.@]
 *
 * PARAMS
 *   pSid []
 */
/* MAKE_EXPORT GetSidIdentifierAuthority_new=GetSidIdentifierAuthority */
PSID_IDENTIFIER_AUTHORITY WINAPI
GetSidIdentifierAuthority_new( PSID pSid )
{
    return &(((SID*)pSid)->IdentifierAuthority);
}

/******************************************************************************
 * GetSidSubAuthority [ADVAPI32.@]
 *
 * PARAMS
 *   pSid          []
 *   nSubAuthority []
 */
/* MAKE_EXPORT GetSidSubAuthority_new=GetSidSubAuthority */
PDWORD WINAPI
GetSidSubAuthority_new( PSID pSid, DWORD nSubAuthority )
{
    return &(((SID*)pSid)->SubAuthority[nSubAuthority]);
}

/******************************************************************************
 * GetSidSubAuthorityCount [ADVAPI32.@]
 *
 * PARAMS
 *   pSid []
 */
/* MAKE_EXPORT GetSidSubAuthorityCount_new=GetSidSubAuthorityCount */
PUCHAR WINAPI
GetSidSubAuthorityCount_new (PSID pSid)
{
    return &(((SID*)pSid)->SubAuthorityCount);
}

/******************************************************************************
 * GetLengthSid [ADVAPI32.@]
 *
 * PARAMS
 *   pSid []
 */
/* MAKE_EXPORT GetLengthSid_new=GetLengthSid */
DWORD WINAPI
GetLengthSid_new (PSID pSid)
{
	TRACE("sid=%p\n",pSid);
	if (!pSid) return 0;
	return GetSidLengthRequired_new(*GetSidSubAuthorityCount_new(pSid));
}

/*      ##############################################
        ######  SECURITY DESCRIPTOR FUNCTIONS   ######
        ##############################################
*/
        
/******************************************************************************
 * InitializeSecurityDescriptor [ADVAPI32.@]
 *
 * PARAMS
 *   pDescr   []
 *   revision []
 */
/* MAKE_EXPORT InitializeSecurityDescriptor_new=InitializeSecurityDescriptor */
BOOL WINAPI
InitializeSecurityDescriptor_new( SECURITY_DESCRIPTOR *pDescr, DWORD revision )
{
	if (revision!=SECURITY_DESCRIPTOR_REVISION)
	{
		SetLastError(ERROR_UNKNOWN_REVISION);
		return FALSE;
	}
	memset(pDescr,'\0',sizeof(*pDescr));
	pDescr->Revision = SECURITY_DESCRIPTOR_REVISION;
	return TRUE;
}

/******************************************************************************
 * GetSecurityDescriptorLength [ADVAPI32.@]
 */
/* MAKE_EXPORT GetSecurityDescriptorLength_new=GetSecurityDescriptorLength */
DWORD WINAPI GetSecurityDescriptorLength_new( SECURITY_DESCRIPTOR *pDescr)
{
	SECURITY_DESCRIPTOR* lpsd=pDescr;
	ULONG offset = 0;
	ULONG Size = SECURITY_DESCRIPTOR_MIN_LENGTH;

	if ( lpsd == NULL )
			return 0;

	if ( lpsd->Control & SE_SELF_RELATIVE)
			offset = (ULONG) lpsd;

	if ( lpsd->Owner != NULL )
			Size += GetLengthSid_new((PSID)((LPBYTE)lpsd->Owner + offset));

	if ( lpsd->Group != NULL )
			Size += GetLengthSid_new((PSID)((LPBYTE)lpsd->Group + offset));

	if ( (lpsd->Control & SE_SACL_PRESENT) &&
		  lpsd->Sacl != NULL )
			Size += ((PACL)((LPBYTE)lpsd->Sacl + offset))->AclSize;

	if ( (lpsd->Control & SE_DACL_PRESENT) &&
		  lpsd->Dacl != NULL )
			Size += ((PACL)((LPBYTE)lpsd->Dacl + offset))->AclSize;

	return Size;
}

/******************************************************************************
 * GetSecurityDescriptorOwner [ADVAPI32.@]
 *
 * PARAMS
 *   pOwner            []
 *   lpbOwnerDefaulted []
 */
/* MAKE_EXPORT GetSecurityDescriptorOwner_new=GetSecurityDescriptorOwner */
BOOL WINAPI
GetSecurityDescriptorOwner_new( SECURITY_DESCRIPTOR *pDescr, PSID *Owner,
                            LPBOOL OwnerDefaulted )
{
	SECURITY_DESCRIPTOR* lpsd=pDescr;
	
	TRACE("GetSecurityDescriptorOwner(%p,%p,%p)\n", pDescr, Owner, OwnerDefaulted);

	if ( !lpsd  || !Owner || !OwnerDefaulted )
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	if (lpsd->Owner != NULL)
	{
		if (lpsd->Control & SE_SELF_RELATIVE)
			*Owner = (PSID)((LPBYTE)lpsd +
							(ULONG)lpsd->Owner);
		else
			*Owner = lpsd->Owner;

		if ( lpsd->Control & SE_OWNER_DEFAULTED )
			*OwnerDefaulted = TRUE;
		else
			*OwnerDefaulted = FALSE;
	}
	else
		*Owner = NULL;

	return TRUE;
}

/******************************************************************************
 * SetSecurityDescriptorOwner [ADVAPI32.@]
 *
 * PARAMS
 */
/* MAKE_EXPORT SetSecurityDescriptorOwner_new=SetSecurityDescriptorOwner */
BOOL WINAPI SetSecurityDescriptorOwner_new( PSECURITY_DESCRIPTOR pSecurityDescriptor, 
                                   PSID owner, BOOL ownerdefaulted)
{
	SECURITY_DESCRIPTOR* lpsd=(SECURITY_DESCRIPTOR*)pSecurityDescriptor;

	if (lpsd->Revision!=SECURITY_DESCRIPTOR_REVISION)
	{
		SetLastError(ERROR_UNKNOWN_REVISION);
		return FALSE;
	}
	if (lpsd->Control & SE_SELF_RELATIVE)
	{
		SetLastError(ERROR_INVALID_SECURITY_DESCR);
		return FALSE;
	}

	lpsd->Owner = owner;
	if (ownerdefaulted)
		lpsd->Control |= SE_OWNER_DEFAULTED;
	else
		lpsd->Control &= ~SE_OWNER_DEFAULTED;
	return TRUE;
}
/******************************************************************************
 * GetSecurityDescriptorGroup                   [ADVAPI32.@]
 */
/* MAKE_EXPORT GetSecurityDescriptorGroup_new=GetSecurityDescriptorGroup */
BOOL WINAPI GetSecurityDescriptorGroup_new(
        PSECURITY_DESCRIPTOR SecurityDescriptor,
        PSID *Group,
        LPBOOL GroupDefaulted)
{
	SECURITY_DESCRIPTOR* lpsd=(SECURITY_DESCRIPTOR*)SecurityDescriptor;
	
	TRACE("GetSecurityDescriptorGroup(%p,%p,%p)\n", SecurityDescriptor, Group, GroupDefaulted);

	if ( !lpsd || !Group || !GroupDefaulted )
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	if (lpsd->Group != NULL)
	{
		if (lpsd->Control & SE_SELF_RELATIVE)
			*Group = (PSID)((LPBYTE)lpsd +
							(ULONG)lpsd->Group);
		else
			*Group = lpsd->Group;

		if ( lpsd->Control & SE_GROUP_DEFAULTED )
			*GroupDefaulted = TRUE;
		else
			*GroupDefaulted = FALSE;
	}
	else
		*Group = NULL;

	return TRUE;
}       
/******************************************************************************
 * SetSecurityDescriptorGroup [ADVAPI32.@]
 */
/* MAKE_EXPORT SetSecurityDescriptorGroup_new=SetSecurityDescriptorGroup */
BOOL WINAPI SetSecurityDescriptorGroup_new ( PSECURITY_DESCRIPTOR pSecurityDescriptor,
                                           PSID group, BOOL groupdefaulted)
{
	SECURITY_DESCRIPTOR* lpsd=(SECURITY_DESCRIPTOR*)pSecurityDescriptor;

	if (lpsd->Revision!=SECURITY_DESCRIPTOR_REVISION)
	{
		SetLastError(ERROR_UNKNOWN_REVISION);
		return FALSE;
	}
	if (lpsd->Control & SE_SELF_RELATIVE)
	{
		SetLastError(ERROR_INVALID_SECURITY_DESCR);
		return FALSE;
	}

	lpsd->Group = group;
	if (groupdefaulted)
		lpsd->Control |= SE_GROUP_DEFAULTED;
	else
		lpsd->Control &= ~SE_GROUP_DEFAULTED;
	return TRUE;
}

/******************************************************************************
 * IsValidSecurityDescriptor [ADVAPI32.@]
 *
 * PARAMS
 *   lpsecdesc []
 */
/* MAKE_EXPORT IsValidSecurityDescriptor_new=IsValidSecurityDescriptor */
BOOL WINAPI
IsValidSecurityDescriptor_new( PSECURITY_DESCRIPTOR pSecurityDescriptor )
{
	SECURITY_DESCRIPTOR* lpsd=(SECURITY_DESCRIPTOR*)pSecurityDescriptor;

	if ( ! pSecurityDescriptor )
	{
		SetLastError(ERROR_INVALID_SECURITY_DESCR);
		return FALSE;
	}
	if ( lpsd->Revision != SECURITY_DESCRIPTOR_REVISION )
	{
		SetLastError(ERROR_UNKNOWN_REVISION);
		return FALSE;
	}

	return TRUE;
}

/*****************************************************************************
 *  GetSecurityDescriptorDacl                   [ADVAPI32.@]
 */
/* MAKE_EXPORT GetSecurityDescriptorDacl_new=GetSecurityDescriptorDacl */
BOOL WINAPI GetSecurityDescriptorDacl_new(
        IN PSECURITY_DESCRIPTOR pSecurityDescriptor,
        OUT LPBOOL lpbDaclPresent,
        OUT PACL *pDacl,
        OUT LPBOOL lpbDaclDefaulted)
{
	SECURITY_DESCRIPTOR* lpsd=(SECURITY_DESCRIPTOR*)pSecurityDescriptor;

	TRACE("GetSecurityDescriptorDacl(%p,%p,%p,%p)\n",
		pSecurityDescriptor, lpbDaclPresent, pDacl, lpbDaclDefaulted);

	if (lpsd->Revision != SECURITY_DESCRIPTOR_REVISION)
	{
		SetLastError(ERROR_UNKNOWN_REVISION);
		return FALSE;
	}

	if ( (*lpbDaclPresent = (SE_DACL_PRESENT & lpsd->Control) ? 1 : 0) )
	{
	  if ( SE_SELF_RELATIVE & lpsd->Control)
		*pDacl = (PACL) ((LPBYTE)lpsd + (DWORD)lpsd->Dacl);
	  else
		*pDacl = lpsd->Dacl;

	  *lpbDaclDefaulted = (( SE_DACL_DEFAULTED & lpsd->Control ) ? 1 : 0);
	}

	return TRUE;
}       

/******************************************************************************
 *  SetSecurityDescriptorDacl                   [ADVAPI32.@]
 */
/* MAKE_EXPORT SetSecurityDescriptorDacl_new=SetSecurityDescriptorDacl */
BOOL WINAPI 
SetSecurityDescriptorDacl_new (
        PSECURITY_DESCRIPTOR pSecurityDescriptor,
        BOOL daclpresent,
        PACL dacl,
        BOOL dacldefaulted )
{
	SECURITY_DESCRIPTOR* lpsd=(SECURITY_DESCRIPTOR*)pSecurityDescriptor;

	if (lpsd->Revision!=SECURITY_DESCRIPTOR_REVISION)
	{
		SetLastError(ERROR_UNKNOWN_REVISION);
		return FALSE;
	}
	if (lpsd->Control & SE_SELF_RELATIVE)
	{
		SetLastError(ERROR_INVALID_SECURITY_DESCR);
		return FALSE;
	}

	if (!daclpresent)
	{
		lpsd->Control &= ~SE_DACL_PRESENT;
		return TRUE;
	}

	lpsd->Control |= SE_DACL_PRESENT;
	lpsd->Dacl = dacl;

	if (dacldefaulted)
		lpsd->Control |= SE_DACL_DEFAULTED;
	else
		lpsd->Control &= ~SE_DACL_DEFAULTED;

	return TRUE;
}
/******************************************************************************
 *  GetSecurityDescriptorSacl                   [ADVAPI32.@]
 */
/* MAKE_EXPORT GetSecurityDescriptorSacl_new=GetSecurityDescriptorSacl */
BOOL WINAPI GetSecurityDescriptorSacl_new(
        IN PSECURITY_DESCRIPTOR pSecurityDescriptor,
        OUT LPBOOL lpbSaclPresent,
        OUT PACL *pSacl,
        OUT LPBOOL lpbSaclDefaulted)
{
	SECURITY_DESCRIPTOR* lpsd=(SECURITY_DESCRIPTOR*)pSecurityDescriptor;
	
	TRACE("GetSecurityDescriptorSacl(%p,%p,%p,%p)\n",
		lpsd, lpbSaclPresent, *pSacl, lpbSaclDefaulted);

	if (lpsd->Revision != SECURITY_DESCRIPTOR_REVISION)
	{
		SetLastError(ERROR_UNKNOWN_REVISION);
		return FALSE;
	}

	if ( (*lpbSaclPresent = (SE_SACL_PRESENT & lpsd->Control) ? 1 : 0) )
	{
	  if (SE_SELF_RELATIVE & lpsd->Control)
		*pSacl = (PACL) ((LPBYTE)lpsd + (DWORD)lpsd->Sacl);
	  else
		*pSacl = lpsd->Sacl;

	  *lpbSaclDefaulted = (( SE_SACL_DEFAULTED & lpsd->Control ) ? 1 : 0);
	}

	return TRUE;
}       

/**************************************************************************
 * SetSecurityDescriptorSacl                    [ADVAPI32.@]
 */
/* MAKE_EXPORT SetSecurityDescriptorSacl_new=SetSecurityDescriptorSacl */
BOOL WINAPI SetSecurityDescriptorSacl_new (
        PSECURITY_DESCRIPTOR pSecurityDescriptor,
        BOOL saclpresent,
        PACL sacl,
        BOOL sacldefaulted)
{
	SECURITY_DESCRIPTOR* lpsd=(SECURITY_DESCRIPTOR*)pSecurityDescriptor;

	if (lpsd->Revision!=SECURITY_DESCRIPTOR_REVISION)
	{
		SetLastError(ERROR_UNKNOWN_REVISION);
		return FALSE;
	}
	if (lpsd->Control & SE_SELF_RELATIVE)
	{
		SetLastError(ERROR_INVALID_SECURITY_DESCR);
		return FALSE;
	}
	if (!saclpresent) {
		lpsd->Control &= ~SE_SACL_PRESENT;
		return TRUE;
	}
	lpsd->Control |= SE_SACL_PRESENT;
	lpsd->Sacl = sacl;
	if (sacldefaulted)
		lpsd->Control |= SE_SACL_DEFAULTED;
	else
		lpsd->Control &= ~SE_SACL_DEFAULTED;
	return TRUE;
}
/******************************************************************************
 * MakeSelfRelativeSD [ADVAPI32.@]
 *
 * PARAMS
 *   lpabssecdesc  []
 *   lpselfsecdesc []
 *   lpbuflen      []
 */
/* MAKE_EXPORT MakeSelfRelativeSD_new=MakeSelfRelativeSD */
BOOL WINAPI
MakeSelfRelativeSD_new(
        IN PSECURITY_DESCRIPTOR pAbsoluteSecurityDescriptor,
        IN PSECURITY_DESCRIPTOR pSelfRelativeSecurityDescriptor,
        IN OUT LPDWORD lpdwBufferLength)
{
    DWORD offsetRel;
    ULONG length;
    SECURITY_DESCRIPTOR* pAbs = (SECURITY_DESCRIPTOR*)pAbsoluteSecurityDescriptor;
    SECURITY_DESCRIPTOR_RELATIVE* pRel = (SECURITY_DESCRIPTOR_RELATIVE*)pSelfRelativeSecurityDescriptor;

    TRACE(" %p %p %p(%d)\n", pAbs, pRel, lpdwBufferLength,
        lpdwBufferLength ? *lpdwBufferLength: -1);

    if (!lpdwBufferLength || !pAbs || !pRel)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    length = GetSecurityDescriptorLength_new(pAbs);
    if (*lpdwBufferLength < length)
    {
        *lpdwBufferLength = length;
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        return FALSE;
    }

    if (pAbs->Control & SE_SELF_RELATIVE)
    {
        memcpy(pRel, pAbs, length);
        return TRUE;
    }

    pRel->Revision = pAbs->Revision;
    pRel->Sbz1 = pAbs->Sbz1;
    pRel->Control = pAbs->Control | SE_SELF_RELATIVE;

    offsetRel = sizeof(SECURITY_DESCRIPTOR_RELATIVE);
    if (pAbs->Owner)
    {
        pRel->Owner = offsetRel;
        length = GetLengthSid_new(pAbs->Owner);
        memcpy((LPBYTE)pRel + offsetRel, pAbs->Owner, length);
        offsetRel += length;
    }
    else
    {
        pRel->Owner = 0;
    }

    if (pAbs->Group)
    {
        pRel->Group = offsetRel;
        length = GetLengthSid_new(pAbs->Group);
        memcpy((LPBYTE)pRel + offsetRel, pAbs->Group, length);
        offsetRel += length;
    }
    else
    {
        pRel->Group = 0;
    }

    if (pAbs->Sacl)
    {
        pRel->Sacl = offsetRel;
        length = pAbs->Sacl->AclSize;
        memcpy((LPBYTE)pRel + offsetRel, pAbs->Sacl, length);
        offsetRel += length;
    }
    else
    {
        pRel->Sacl = 0;
    }

    if (pAbs->Dacl)
    {
        pRel->Dacl = offsetRel;
        length = pAbs->Dacl->AclSize;
        memcpy((LPBYTE)pRel + offsetRel, pAbs->Dacl, length);
    }
    else
    {
        pRel->Dacl = 0;
    }

    return TRUE;
}

/******************************************************************************
 * GetSecurityDescriptorControl                 [ADVAPI32.@]
 */
/* MAKE_EXPORT GetSecurityDescriptorControl_new=GetSecurityDescriptorControl */
BOOL WINAPI GetSecurityDescriptorControl_new ( PSECURITY_DESCRIPTOR  pSecurityDescriptor,
                 PSECURITY_DESCRIPTOR_CONTROL pControl, LPDWORD lpdwRevision)
{
    SECURITY_DESCRIPTOR *lpsd = (SECURITY_DESCRIPTOR*) pSecurityDescriptor;

    TRACE("GetSecurityDescriptorControl(%p,%p,%p)\n",pSecurityDescriptor,pControl,lpdwRevision);

    *lpdwRevision = lpsd->Revision;

    if (*lpdwRevision != SECURITY_DESCRIPTOR_REVISION)
    {
        SetLastError(ERROR_UNKNOWN_REVISION);
        return FALSE;
    }

    *pControl = lpsd->Control;

    return TRUE;
}               

/*      ##############################
        ######  ACL FUNCTIONS   ######
        ##############################
*/

/*************************************************************************
 * InitializeAcl [ADVAPI32.@]
 */
/* MAKE_EXPORT InitializeAcl_new=InitializeAcl */
BOOL WINAPI InitializeAcl_new(PACL acl, DWORD size, DWORD rev)
{
	TRACE("InitializeAcl(%p 0x%08x 0x%08x)\n", acl, size, rev);

	if (rev!=ACL_REVISION)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}
	if (size<sizeof(ACL))
	{
		SetLastError(ERROR_INSUFFICIENT_BUFFER);
		return FALSE;
	}
	if (size>0xFFFF)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	memset(acl,'\0',sizeof(ACL));
	acl->AclRevision        = (UCHAR)rev;
	acl->AclSize            = (USHORT)size;
	acl->AceCount           = 0;
	return TRUE;
}

/*      ##############################
        ######  MISC FUNCTIONS  ######
        ##############################
*/

LONG WINAPI
FillSecurityDescriptor(
	IN SECURITY_INFORMATION RequestedInformation,
	OUT PSECURITY_DESCRIPTOR pSecurityDescriptor,
	IN ULONG nLength,
	OUT PULONG lpnLengthNeeded
)
{
    DWORD               nNeeded;
    LPBYTE      pBuffer;
    DWORD               iLocNow;
    SECURITY_DESCRIPTOR *pSDRelative;

    nNeeded = sizeof(SECURITY_DESCRIPTOR);
    if (RequestedInformation & OWNER_SECURITY_INFORMATION)
        nNeeded += sizeof(sidWorld);
    if (RequestedInformation & GROUP_SECURITY_INFORMATION)
        nNeeded += sizeof(sidWorld);
    if (RequestedInformation & DACL_SECURITY_INFORMATION)
        nNeeded += WINE_SIZE_OF_WORLD_ACCESS_ACL;
    if (RequestedInformation & SACL_SECURITY_INFORMATION)
        nNeeded += WINE_SIZE_OF_WORLD_ACCESS_ACL;

    *lpnLengthNeeded = nNeeded;

    if (nNeeded > nLength)
        return ERROR_INSUFFICIENT_BUFFER;

    if (!InitializeSecurityDescriptor_new((SECURITY_DESCRIPTOR*) pSecurityDescriptor, 
			SECURITY_DESCRIPTOR_REVISION))
        return ERROR_INVALID_SECURITY_DESCR;

    pSDRelative = (PISECURITY_DESCRIPTOR) pSecurityDescriptor;
    pSDRelative->Control |= SE_SELF_RELATIVE;
    pBuffer = (LPBYTE) pSDRelative;
    iLocNow = sizeof(SECURITY_DESCRIPTOR);

    if (RequestedInformation & OWNER_SECURITY_INFORMATION)
    {
        memcpy(pBuffer + iLocNow, &sidWorld, sizeof(sidWorld));
        pSDRelative->Owner = (PACL) iLocNow;
        iLocNow += sizeof(sidWorld);
    }
    if (RequestedInformation & GROUP_SECURITY_INFORMATION)
    {
        memcpy(pBuffer + iLocNow, &sidWorld, sizeof(sidWorld));
        pSDRelative->Group = (PACL) iLocNow;
        iLocNow += sizeof(sidWorld);
    }
    if (RequestedInformation & DACL_SECURITY_INFORMATION)
    {
        GetWorldAccessACL((PACL) (pBuffer + iLocNow));
        pSDRelative->Dacl = (PACL) iLocNow;
        iLocNow += WINE_SIZE_OF_WORLD_ACCESS_ACL;
    }
    if (RequestedInformation & SACL_SECURITY_INFORMATION)
    {
        GetWorldAccessACL((PACL) (pBuffer + iLocNow));
        pSDRelative->Sacl = (PACL) iLocNow;
        /* iLocNow += WINE_SIZE_OF_WORLD_ACCESS_ACL; */
    }
    return ERROR_SUCCESS;
}

/******************************************************************************
 * LookupPrivilegeValueW                        [ADVAPI32.@] !!20040505
 * Retrieves LUID used on a system to represent the privilege name.
 *
 * NOTES
 *   lpLuid should be PLUID
 *
 * PARAMS
 *   lpSystemName [I] Address of string specifying the system
 *   lpName       [I] Address of string specifying the privilege
 *   lpLuid       [I] Address of locally unique identifier
 *
 * RETURNS STD
 */
/* MAKE_EXPORT LookupPrivilegeValueW_new=LookupPrivilegeValueW */
BOOL WINAPI
LookupPrivilegeValueW_new( LPCWSTR lpSystemName, LPCWSTR lpName, PLUID lpLuid )
{
    FIXMEW("LookupPrivilegeValueW(%s,%s,%p): stub\n", lpSystemName, 
        lpName, lpLuid);
    return TRUE;
}

/******************************************************************************
 * LookupPrivilegeValueA                        [ADVAPI32.@]
 */
/* MAKE_EXPORT LookupPrivilegeValueA_new=LookupPrivilegeValueA */
BOOL WINAPI
LookupPrivilegeValueA_new( LPCSTR lpSystemName, LPCSTR lpName, PLUID lpLuid )
{
    FIXME("LookupPrivilegeValueA(%s,%s,%p): stub\n", lpSystemName, 
        lpName, lpLuid);
    return TRUE;
}

/******************************************************************************
 * GetFileSecurityW [ADVAPI32.@]
 *
 * Obtains Specified information about the security of a file or directory
 * The information obtained is constrained by the callers access rights and
 * privileges
 *
 * PARAMS
 *   lpFileName           []
 *   RequestedInformation []
 *   pSecurityDescriptor  []
 *   nLength              []
 *   lpnLengthNeeded      []
 */
/* MAKE_EXPORT GetFileSecurityW_new=GetFileSecurityW */
BOOL WINAPI
GetFileSecurityW_new( LPCWSTR lpFileName, 
                    SECURITY_INFORMATION RequestedInformation,
                    SECURITY_DESCRIPTOR* pSecurityDescriptor,
                    DWORD nLength, LPDWORD lpnLengthNeeded )
{
    LONG res;

    FIXMEW("GetFileSecurityW(%s) : returns fake SECURITY_DESCRIPTOR\n", lpFileName);

	res = FillSecurityDescriptor(RequestedInformation, pSecurityDescriptor, nLength, lpnLengthNeeded);

	if (res == ERROR_SUCCESS)
		return TRUE;
	if (res == ERROR_INSUFFICIENT_BUFFER)
		return TRUE;
	return FALSE;
}

/******************************************************************************
 * GetFileSecurityA [ADVAPI32.@]
 *
 * Obtains Specified information about the security of a file or directory
 * The information obtained is constrained by the callers access rights and
 * privileges
 */
/* MAKE_EXPORT GetFileSecurityA_new=GetFileSecurityA */
BOOL WINAPI
GetFileSecurityA_new( LPCSTR lpFileName, 
                    SECURITY_INFORMATION RequestedInformation,
                    SECURITY_DESCRIPTOR* pSecurityDescriptor,
                    DWORD nLength, LPDWORD lpnLengthNeeded )
{
    BOOL r;
    LPWSTR name = NULL;

    /*if( lpFileName )
    {
        len = MultiByteToWideChar( CP_ACP, 0, lpFileName, -1, NULL, 0 );
        name = HeapAlloc( GetProcessHeap(), 0, len*sizeof(WCHAR) );
        MultiByteToWideChar( CP_ACP, 0, lpFileName, -1, name, len );
    }*/

    r = GetFileSecurityW_new( name, RequestedInformation, pSecurityDescriptor,
                          nLength, lpnLengthNeeded );
    /*HeapFree( GetProcessHeap(), 0, name );*/

    return r;
}

/******************************************************************************
 * LookupAccountSidA [ADVAPI32.@]
 */
/* MAKE_EXPORT LookupAccountSidA_new=LookupAccountSidA */
BOOL WINAPI
LookupAccountSidA_new(
        IN LPCSTR system,
        IN PSID sid,
        OUT LPSTR account,
        IN OUT LPDWORD accountSize,
        OUT LPSTR domain,
        IN OUT LPDWORD domainSize,
        OUT PSID_NAME_USE name_use )
{
        static const char ac[] = "Administrator";
        static const char dm[] = "DOMAIN";
        FIXME("LookupAccountSidA(%s,sid=%p,%p,%p(%lu),%p,%7p(%lu),%p): semi-stub\n",
              system,sid,
              account,accountSize,accountSize?*accountSize:0,
              domain,domainSize,domainSize?*domainSize:0,
              name_use);

        if (accountSize) *accountSize = strlen(ac)+1;
        if (account && (*accountSize > strlen(ac)))
          strcpy(account, ac);

        if (domainSize) *domainSize = strlen(dm)+1;
        if (domain && (*domainSize > strlen(dm)))
          strcpy(domain,dm);

        if (name_use) *name_use = SidTypeUser;
        return TRUE;
}

/******************************************************************************
 * LookupAccountSidW [ADVAPI32.@]
 *
 * PARAMS
 *   system      []
 *   sid         []
 *   account     []
 *   accountSize []
 *   domain      []
 *   domainSize  []
 *   name_use    []
 */
/* MAKE_EXPORT LookupAccountSidW_new=LookupAccountSidW */
BOOL WINAPI
LookupAccountSidW_new(
        IN LPCWSTR system,
        IN PSID sid,
        OUT LPWSTR account,
        IN OUT LPDWORD accountSize,
        OUT LPWSTR domain,
        IN OUT LPDWORD domainSize,
        OUT PSID_NAME_USE name_use )
{
    static const WCHAR ac[] = {'A','d','m','i','n','i','s','t','r','a','t','o','r',0};
    static const WCHAR dm[] = {'D','O','M','A','I','N',0};
        FIXMEW("LookupAccountSidW(%s,sid=%p,%p,%p(%lu),%p,%p(%lu),%p): semi-stub\n",
              system,sid,
              account,accountSize,accountSize?*accountSize:0,
              domain,domainSize,domainSize?*domainSize:0,
              name_use);

        if (accountSize) *accountSize = lstrlenW(ac)+1;
        if (account && (*accountSize > (DWORD)lstrlenW(ac)))
            lstrcpyW(account, ac);

        if (domainSize) *domainSize = lstrlenW(dm)+1;
        if (domain && (*domainSize > (DWORD)lstrlenW(dm)))
            lstrcpyW(domain,dm);

        if (name_use) *name_use = SidTypeUser;
        return TRUE;
}

/******************************************************************************
 * SetFileSecurityA [ADVAPI32.@]
 * Sets the security of a file or directory
 */
/* MAKE_EXPORT SetFileSecurityA_new=SetFileSecurityA */
BOOL WINAPI
SetFileSecurityA_new( LPCSTR lpFileName,
					SECURITY_INFORMATION RequestedInformation,
					PSECURITY_DESCRIPTOR pSecurityDescriptor)
{
  FIXME("SetFileSecurityA(%s) : stub\n", lpFileName);
  return TRUE;
}

/******************************************************************************
 * SetFileSecurityW [ADVAPI32.@]
 * Sets the security of a file or directory
 *
 * PARAMS
 *   lpFileName           []
 *   RequestedInformation []
 *   pSecurityDescriptor  []
 */
/* MAKE_EXPORT SetFileSecurityW_new=SetFileSecurityW */
BOOL WINAPI
SetFileSecurityW_new( LPCWSTR lpFileName, 
                    SECURITY_INFORMATION RequestedInformation,
                    PSECURITY_DESCRIPTOR pSecurityDescriptor )
{
  FIXMEW("SetFileSecurityW(%s) : stub\n", lpFileName); 
  return TRUE;
}

/******************************************************************************
 * GetNamedSecurityInfoW [ADVAPI32.@]
 */
/* MAKE_EXPORT GetNamedSecurityInfoW_new=GetNamedSecurityInfoW */
DWORD WINAPI GetNamedSecurityInfoW_new( LPWSTR name, SE_OBJECT_TYPE type,
    SECURITY_INFORMATION info, PSID* owner, PSID* group, PACL* dacl,
    PACL* sacl, PSECURITY_DESCRIPTOR* descriptor )
{
    DWORD needed, offset;
    SECURITY_DESCRIPTOR_RELATIVE *relative = NULL;
    BYTE *buffer;

    TRACE( "%s %d %d %p %p %p %p %p\n", name, type, info, owner,
           group, dacl, sacl, descriptor );

    /* A NULL descriptor is allowed if any one of the other pointers is not NULL */
    if (!name || !(owner||group||dacl||sacl||descriptor) ) return ERROR_INVALID_PARAMETER;

    /* If no descriptor, we have to check that there's a pointer for the requested information */
    if( !descriptor && (
        ((info & OWNER_SECURITY_INFORMATION) && !owner)
    ||  ((info & GROUP_SECURITY_INFORMATION) && !group)
    ||  ((info & DACL_SECURITY_INFORMATION)  && !dacl)
    ||  ((info & SACL_SECURITY_INFORMATION)  && !sacl)  ))
        return ERROR_INVALID_PARAMETER;

    needed = !descriptor ? 0 : sizeof(SECURITY_DESCRIPTOR_RELATIVE);
    if (info & OWNER_SECURITY_INFORMATION)
        needed += sizeof(sidWorld);
    if (info & GROUP_SECURITY_INFORMATION)
        needed += sizeof(sidWorld);
    if (info & DACL_SECURITY_INFORMATION)
        needed += WINE_SIZE_OF_WORLD_ACCESS_ACL;
    if (info & SACL_SECURITY_INFORMATION)
        needed += WINE_SIZE_OF_WORLD_ACCESS_ACL;

    if(descriptor)
    {
        /* must be freed by caller */
        *descriptor = HeapAlloc( GetProcessHeap(), 0, needed );
        if (!*descriptor) return ERROR_NOT_ENOUGH_MEMORY;

        if (!InitializeSecurityDescriptor_new( (SECURITY_DESCRIPTOR*) *descriptor, SECURITY_DESCRIPTOR_REVISION ))
        {
            HeapFree( GetProcessHeap(), 0, *descriptor );
            return ERROR_INVALID_SECURITY_DESCR;
        }

        relative = (SECURITY_DESCRIPTOR_RELATIVE*) *descriptor;
        relative->Control |= SE_SELF_RELATIVE;

        buffer = (BYTE *)relative;
        offset = sizeof(SECURITY_DESCRIPTOR_RELATIVE);
    }
    else
    {
        buffer = (BYTE*) HeapAlloc( GetProcessHeap(), 0, needed );
        if (!buffer) return ERROR_NOT_ENOUGH_MEMORY;
        offset = 0;
    }

    if (info & OWNER_SECURITY_INFORMATION)
    {
        memcpy( buffer + offset, &sidWorld, sizeof(sidWorld) );
        if(relative)
            relative->Owner = offset;
        if (owner)
            *owner = buffer + offset;
        offset += sizeof(sidWorld);
    }
    if (info & GROUP_SECURITY_INFORMATION)
    {
        memcpy( buffer + offset, &sidWorld, sizeof(sidWorld) );
        if(relative)
            relative->Group = offset;
        if (group)
            *group = buffer + offset;
        offset += sizeof(sidWorld);
    }
    if (info & DACL_SECURITY_INFORMATION)
    {
        GetWorldAccessACL( (PACL)(buffer + offset) );
        if(relative)
        {
            relative->Control |= SE_DACL_PRESENT;
            relative->Dacl = offset;
        }
        if (dacl)
            *dacl = (PACL)(buffer + offset);
        offset += WINE_SIZE_OF_WORLD_ACCESS_ACL;
    }
    if (info & SACL_SECURITY_INFORMATION)
    {
        GetWorldAccessACL( (PACL)(buffer + offset) );
        if(relative)
        {
            relative->Control |= SE_SACL_PRESENT;
            relative->Sacl = offset;
        }
        if (sacl)
            *sacl = (PACL)(buffer + offset);
    }

    return ERROR_SUCCESS;
}

/******************************************************************************
 * GetNamedSecurityInfoA [ADVAPI32.@]
 */
/* MAKE_EXPORT GetNamedSecurityInfoA_new=GetNamedSecurityInfoA */
DWORD WINAPI GetNamedSecurityInfoA_new(LPSTR pObjectName,
        SE_OBJECT_TYPE ObjectType, SECURITY_INFORMATION SecurityInfo,
        PSID* ppsidOwner, PSID* ppsidGroup, PACL* ppDacl, PACL* ppSacl,
        PSECURITY_DESCRIPTOR* ppSecurityDescriptor)
{
    DWORD len;
    LPWSTR wstr = NULL;
    DWORD r;

    TRACE("%s %d %d %p %p %p %p %p\n", pObjectName, ObjectType, SecurityInfo,
        ppsidOwner, ppsidGroup, ppDacl, ppSacl, ppSecurityDescriptor);

    if( pObjectName )
    {
        len = MultiByteToWideChar( CP_ACP, 0, pObjectName, -1, NULL, 0 );
        wstr = (LPWSTR) HeapAlloc( GetProcessHeap(), 0, len*sizeof(WCHAR));
        MultiByteToWideChar( CP_ACP, 0, pObjectName, -1, wstr, len );
    }

    r = GetNamedSecurityInfoW_new( wstr, ObjectType, SecurityInfo, ppsidOwner,
                           ppsidGroup, ppDacl, ppSacl, ppSecurityDescriptor );

    HeapFree( GetProcessHeap(), 0, wstr );

    return r;
}

/* MAKE_EXPORT RegGetKeySecurity_new=RegGetKeySecurity */
LONG WINAPI RegGetKeySecurity_new(
	IN     HKEY hKey,
	IN     SECURITY_INFORMATION SecurityInformation,
	OUT    PSECURITY_DESCRIPTOR pSecurityDescriptor,
	IN OUT LPDWORD lpcbSecurityDescriptor
)
{
	TRACE("(%x,%ld,%p,%ld)\n",hKey,SecurityInformation,pSecurityDescriptor,
			pSecurityDescriptor?*pSecurityDescriptor:0);

    if (!lpcbSecurityDescriptor)
        return ERROR_INVALID_PARAMETER;

	return FillSecurityDescriptor(SecurityInformation, pSecurityDescriptor,
			*lpcbSecurityDescriptor, lpcbSecurityDescriptor);
}

#if 0 /* LSA disabled */
/******************************************************************************
 * LsaOpenPolicy [ADVAPI32.@]
 *
 * PARAMS
 *   x1 []
 *   x2 []
 *   x3 []
 *   x4 []
 */
NTSTATUS WINAPI
LsaOpenPolicy(
        IN PLSA_UNICODE_STRING SystemName,
        IN PLSA_OBJECT_ATTRIBUTES ObjectAttributes,
        IN ACCESS_MASK DesiredAccess,
        IN OUT PLSA_HANDLE PolicyHandle)
{
        FIXME("(%s,%p,0x%08lx,%p):stub\n",
              SystemName?debugstr_w(SystemName->Buffer):"null",
              ObjectAttributes, DesiredAccess, PolicyHandle);
        dumpLsaAttributes(ObjectAttributes);
        if(PolicyHandle) *PolicyHandle = (LSA_HANDLE)0xcafe;
        return TRUE;
}

/******************************************************************************
 * LsaQueryInformationPolicy [ADVAPI32.@]
 */
NTSTATUS WINAPI
LsaQueryInformationPolicy( 
        IN LSA_HANDLE PolicyHandle,
        IN POLICY_INFORMATION_CLASS InformationClass,
        OUT PVOID *Buffer)
{
        FIXME("(%p,0x%08x,%p):stub\n",
              PolicyHandle, InformationClass, Buffer);

        if(!Buffer) return FALSE;
        switch (InformationClass)
        {
          case PolicyAuditEventsInformation: /* 2 */
            {
              PPOLICY_AUDIT_EVENTS_INFO p = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(POLICY_AUDIT_EVENTS_INFO));
              p->AuditingMode = FALSE; /* no auditing */
              *Buffer = p;
            }
            break;
          case PolicyPrimaryDomainInformation: /* 3 */
          case PolicyAccountDomainInformation: /* 5 */
            {
              struct di
              { POLICY_PRIMARY_DOMAIN_INFO ppdi;
                SID sid;
              };
              SID_IDENTIFIER_AUTHORITY localSidAuthority = {SECURITY_NT_AUTHORITY};
                
              struct di * xdi = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(xdi));
              RtlInitUnicodeString(&(xdi->ppdi.Name), HEAP_strdupAtoW(GetProcessHeap(),0,"DOMAIN"));
              xdi->ppdi.Sid = &(xdi->sid);
              xdi->sid.Revision = SID_REVISION;
              xdi->sid.SubAuthorityCount = 1;
              xdi->sid.IdentifierAuthority = localSidAuthority;
              xdi->sid.SubAuthority[0] = SECURITY_LOCAL_SYSTEM_RID;
              *Buffer = xdi;
            }
            break;
          case  PolicyAuditLogInformation:      
          case  PolicyPdAccountInformation:     
          case  PolicyLsaServerRoleInformation:
          case  PolicyReplicaSourceInformation:
          case  PolicyDefaultQuotaInformation:
          case  PolicyModificationInformation:
          case  PolicyAuditFullSetInformation:
          case  PolicyAuditFullQueryInformation:
          case  PolicyDnsDomainInformation:
            {
              FIXME("category not implemented\n");
              return FALSE;
            }
        }
        return TRUE; 
}                         

/******************************************************************************
 * LsaLookupSids [ADVAPI32.@]
 */
NTSTATUS WINAPI
LsaLookupSids(
        IN LSA_HANDLE PolicyHandle,
        IN ULONG Count,
        IN PSID *Sids,
        OUT PLSA_REFERENCED_DOMAIN_LIST *ReferencedDomains,
        OUT PLSA_TRANSLATED_NAME *Names )
{
        FIXME("%p %lu %p %p %p\n",
          PolicyHandle, Count, Sids, ReferencedDomains, Names);
        return FALSE;
}

/******************************************************************************
 * LsaFreeMemory [ADVAPI32.@]
 */
NTSTATUS WINAPI
LsaFreeMemory(IN PVOID Buffer)
{
        TRACE("(%p)\n",Buffer);
        return HeapFree(GetProcessHeap(), 0, Buffer);
}
/******************************************************************************
 * LsaClose [ADVAPI32.@]
 */
NTSTATUS WINAPI
LsaClose(IN LSA_HANDLE ObjectHandle)
{
        FIXME("(%p):stub\n",ObjectHandle);
        return 0xc0000000;
}
#endif

/******************************************************************************
 * RevertToSelf [ADVAPI32.@]
 *
 * PARAMS
 *   void []
 */
/* MAKE_EXPORT RevertToSelf_new=RevertToSelf */
BOOL WINAPI
RevertToSelf_new( void )
{
	FIXME_OUT("RevertToSelf(), stub\n");
	return TRUE;
}

/******************************************************************************
 * ImpersonateSelf [ADVAPI32.@]
 */
/* MAKE_EXPORT ImpersonateSelf_new=ImpersonateSelf */
BOOL WINAPI
ImpersonateSelf_new(SECURITY_IMPERSONATION_LEVEL ImpersonationLevel)
{
	FIXME("ImpersonateSelf(%08x), stub\n", ImpersonationLevel);
	return TRUE;
}

/******************************************************************************
 * AccessCheck [ADVAPI32.@]
 *
 * FIXME check cast LPBOOL to PBOOLEAN
 */
/* MAKE_EXPORT AccessCheck_new=AccessCheck */
BOOL WINAPI
AccessCheck_new(
        PSECURITY_DESCRIPTOR SecurityDescriptor,
        HANDLE ClientToken,
        DWORD DesiredAccess,
        PGENERIC_MAPPING GenericMapping,
        PPRIVILEGE_SET PrivilegeSet,
        LPDWORD PrivilegeSetLength,
        LPDWORD GrantedAccess,
        LPBOOL AccessStatus)
{
	FIXME("AccessCheck(%p, %04x, %08lx, %p, %p, %p, %p, %p), stub\n",
		SecurityDescriptor, ClientToken, DesiredAccess, GenericMapping, 
		PrivilegeSet, PrivilegeSetLength, GrantedAccess, AccessStatus);
	*AccessStatus = TRUE;
	return TRUE;
}

/*************************************************************************
 * SetKernelObjectSecurity [ADVAPI32.@]
 */
/* MAKE_EXPORT SetKernelObjectSecurity_new=SetKernelObjectSecurity */
BOOL WINAPI
SetKernelObjectSecurity_new (
        IN HANDLE Handle,
        IN SECURITY_INFORMATION SecurityInformation,
        IN PSECURITY_DESCRIPTOR SecurityDescriptor )
{
	FIXME("SetKernelObjectSecurity(0x%08x 0x%08lx %p): stub\n", Handle, SecurityInformation, SecurityDescriptor);
	return TRUE;
}

/******************************************************************************
 * PrivilegeCheck [ADVAPI32.@]
 */
/* MAKE_EXPORT PrivilegeCheck_new=PrivilegeCheck */
BOOL WINAPI PrivilegeCheck_new( HANDLE ClientToken, PPRIVILEGE_SET RequiredPrivileges, LPBOOL pfResult)
{
        FIXME("PrivilegeCheck(%p %p %p): stub\n", ClientToken, RequiredPrivileges, pfResult);
        if (pfResult)
                *pfResult=TRUE;
        return TRUE;
}

/******************************************************************************
 *  AddAccessAllowedAce [ADVAPI32.@]
 */
/* MAKE_EXPORT AddAccessAllowedAce_new=AddAccessAllowedAce */
BOOL WINAPI AddAccessAllowedAce_new(
        IN OUT PACL pAcl,
        IN DWORD dwAceRevision,
        IN DWORD AccessMask,
        IN PSID pSid)
{
	FIXME("AddAccessAllowedAce(%p,0x%08lx,0x%08lx,%p),stub!\n",
		pAcl, dwAceRevision, AccessMask, pSid);
	return TRUE;
}

/******************************************************************************
 * GetAce [ADVAPI32.@]
 */
/* MAKE_EXPORT GetAce_new=GetAce */
BOOL WINAPI GetAce_new(PACL pAcl,DWORD dwAceIndex,LPVOID *pAce )
{
	PACE_HEADER ace;

	TRACE("(%p,%d,%p)\n",pAcl,dwAceIndex,pAce);

	if (!pAcl || !pAce || dwAceIndex >= pAcl->AceCount)
	{		
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	ace = (PACE_HEADER)(pAcl + 1);
	for (;dwAceIndex;dwAceIndex--)
		ace = (PACE_HEADER)(((BYTE*)ace)+ace->AceSize);

	*pAce = ace;

	return TRUE;
}

/******************************************************************************
 * DeleteAce [ADVAPI32.@]
 */
/* MAKE_EXPORT DeleteAce_new=DeleteAce */
BOOL WINAPI DeleteAce_new(PACL pAcl, DWORD dwAceIndex)
{
	PACE_HEADER pAce;

	if (!GetAce_new(pAcl,dwAceIndex,(LPVOID*)&pAce)) return FALSE;

	PACE_HEADER pcAce;
	DWORD len = 0;

	/* skip over the ACE we are deleting */
	pcAce = (PACE_HEADER)(((BYTE*)pAce)+pAce->AceSize);
	dwAceIndex++;

	/* calculate the length of the rest */
	for (; dwAceIndex < pAcl->AceCount; dwAceIndex++)
	{
		len += pcAce->AceSize;
		pcAce = (PACE_HEADER)(((BYTE*)pcAce) + pcAce->AceSize);
	}

	/* slide them all backwards */
	memmove(pAce, ((BYTE*)pAce)+pAce->AceSize, len);
	pAcl->AceCount--;

	TRACE("pAcl=%p dwAceIndex=%d status=0x%08x\n", pAcl, dwAceIndex, status);

	return TRUE;
}



/*************************************************************************
 * CreateRestrictedToken [ADVAPI32.@]
 */
/* MAKE_EXPORT CreateRestrictedToken_new=CreateRestrictedToken */
BOOL WINAPI CreateRestrictedToken_new(
    HANDLE baseToken,
    DWORD flags,
    DWORD nDisableSids,
    PSID_AND_ATTRIBUTES disableSids,
    DWORD nDeletePrivs,
    PLUID_AND_ATTRIBUTES deletePrivs,
    DWORD nRestrictSids,
    PSID_AND_ATTRIBUTES restrictSids,
    PHANDLE newToken)
{
	//bullshit
	if (newToken) *newToken = baseToken;	
    return ( newToken ? TRUE : FALSE );
}

/******************************************************************************
 * CreateWellKnownSid [ADVAPI32.@]
 */
/* MAKE_EXPORT CreateWellKnownSid_new=CreateWellKnownSid */
BOOL WINAPI CreateWellKnownSid_new( DWORD WellKnownSidType, PSID DomainSid, PSID pSid, DWORD* cbSid)
{
	FIXME("CreateWellKnownSid(%ld,%p,%p,%p,%p),mostly stub!\n",WellKnownSidType,DomainSid,pSid,cbSid);
	//BUGBUG what to do with cbSid? out sid size there?
    SID_IDENTIFIER_AUTHORITY localSidAuthority = {SECURITY_NT_AUTHORITY};
	return InitializeSid_new(pSid, &localSidAuthority, 1);
}

/* MAKE_EXPORT LogonUserA_new=LogonUserA */
BOOL WINAPI LogonUserA_new(LPSTR lpszUsername,
	LPSTR lpszDomain,
	LPSTR lpszPassword,
	DWORD dwLogonType,
	DWORD dwLogonProvider,
	PHANDLE phToken)
{
	CHAR lpszCurrentUser[50];
	BOOL fMatch = FALSE;
	ULONG nLength = 50;
	DWORD result = 0;

	/* dwLogonProvider and dwLogonType must be supported */
	if(dwLogonProvider == LOGON32_PROVIDER_WINNT50 || dwLogonType == LOGON32_LOGON_NEW_CREDENTIALS
		|| dwLogonType == LOGON32_LOGON_NETWORK_CLEARTEXT)
	{
		TRACE("%s %s %s %d %d %p not supported\n", lpszUsername, lpszDomain, dwLogonType,
						dwLogonProvider, phToken);
		SetLastError(ERROR_NOT_SUPPORTED);
		return FALSE;
	}

	if(dwLogonProvider > LOGON32_PROVIDER_WINNT50 || dwLogonType > LOGON32_LOGON_UNLOCK
		|| dwLogonType < LOGON32_LOGON_INTERACTIVE)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	/* if the domain is not null, it must be a "." in Win9x (compatibility reason) */
	if(!IsBadStringPtr(lpszDomain, -1) && strcmpi(lpszDomain, "."))
	{
		TRACE("%s %s %s %d %d %p not supported\n", lpszUsername, lpszDomain, dwLogonType,
						dwLogonProvider, phToken);
		SetLastError(ERROR_NOT_SUPPORTED);
		return FALSE;
	}

	if(IsBadWritePtr(phToken, sizeof(DWORD)))
	{
		TRACE_OUT("phToken is NULL !\n");
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	/* Get the current user */
	result = GetUserName(lpszCurrentUser, &nLength);

	if(!result)
	{
		TRACE("retrieving current user failed with error %d\n", GetLastError());
		SetLastError(result);
		return FALSE;
	}

	/* lpszUsername must be current user in Windows 98 */
	if(strcmpi(lpszCurrentUser, lpszUsername))
	{
		TRACE("lpszCurrentUser %s does not match lpszUsername %s\n", lpszCurrentUser, lpszUsername);
		SetLastError(ERROR_CANNOT_OPEN_PROFILE);
		return FALSE;
	}

	/* Now check if the password match*/
	result = WNetVerifyPassword(lpszPassword, &fMatch);

	if(result != NO_ERROR)
	{
		TRACE("WNetVerifyPassword failed with error %d\n", GetLastError());
		return FALSE;
	}

	if(fMatch == FALSE)
	{
		TRACE_OUT("lpszPassword does not match !\n");
		return FALSE;
	}

	*(int*)phToken = 0xCAFE;

	return TRUE;
}