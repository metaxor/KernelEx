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

#include <windows.h>

/* special structure used internally to save some space */
typedef struct 
{
	DWORD dwMajorVersion;
	DWORD dwMinorVersion;
	DWORD dwBuildNumber;
	DWORD dwPlatformId;
	CHAR*  lpszCSDVersion;
	WORD   wServicePackMajor;
	WORD   wServicePackMinor;
	WORD   wSuiteMask;
	BYTE  wProductType;
	BYTE  wReserved;
} OSVERSIONINFOEXA_PRIV;

typedef enum
{
	WIN95,   /* Windows 95 */
	WIN98,   /* Windows 98 */
	WINME,   /* Windows Me */
	NT40,    /* Windows NT 4.0 */
	NT2K,    /* Windows 2000 */
	WINXP,   /* Windows XP */
	WIN2K3,  /* Windows 2003 */
	VISTA,   /* Windows Vista */
	WIN2K8,  /* Windows 2008 */
	NB_WINDOWS_VERSIONS
} WINDOWS_VERSION;

static const OSVERSIONINFOEXA_PRIV VersionData[NB_WINDOWS_VERSIONS] =
{
	/* WIN95 */
	{
		4, 0, 0x40003B6, VER_PLATFORM_WIN32_WINDOWS,
		"",
		0, 0, 0, 0, 0
	},
	/* WIN98 (second edition) */
	{
		4, 10, 0x40A08AE, VER_PLATFORM_WIN32_WINDOWS,
		" A ",
		0, 0, 0, 0, 0
	},
	/* WINME */
	{
		4, 90, 0x45A0BB8, VER_PLATFORM_WIN32_WINDOWS,
		" ",
		0, 0, 0, 0, 0
	},
	/* NT40 */
	{
		4, 0, 0x565, VER_PLATFORM_WIN32_NT,
		"Service Pack 6a",
		6, 0, 0, VER_NT_WORKSTATION, 0
	},
	/* NT2K */
	{
		5, 0, 0x893, VER_PLATFORM_WIN32_NT,
		"Service Pack 4",
		4, 0, 0, VER_NT_WORKSTATION, 30
	},
	/* WINXP */
	{
		5, 1, 0xA28, VER_PLATFORM_WIN32_NT,
		"Service Pack 2",
		2, 0, VER_SUITE_SINGLEUSERTS, VER_NT_WORKSTATION, 30
	},
	/* WIN2K3 */
	{
		5, 2, 0xECE, VER_PLATFORM_WIN32_NT,
		"Service Pack 1",
		1, 0, VER_SUITE_SINGLEUSERTS, VER_NT_SERVER, 0
	},
	/* WINVISTA */
	{
		6, 0, 0x1770, VER_PLATFORM_WIN32_NT,
		" ",
		0, 0, VER_SUITE_SINGLEUSERTS, VER_NT_WORKSTATION, 0
	},
	/* WIN2K8 */
	{
		6, 0, 0x1771, VER_PLATFORM_WIN32_NT,
		"Service Pack 1",
		0, 0, VER_SUITE_SINGLEUSERTS, VER_NT_SERVER, 0
	}
};

static BOOL original_GetVersionEx(void* buf, BOOL unicode)
{
	BOOL ret;
	OSVERSIONINFOA* ver = (OSVERSIONINFOA*) buf;
	DWORD structsize = ver->dwOSVersionInfoSize;
	
	if (!unicode)
	{
		if (structsize != sizeof(OSVERSIONINFOA) && structsize != sizeof(OSVERSIONINFOEXA))
		{
			SetLastError(ERROR_INSUFFICIENT_BUFFER);
			return FALSE;
		}
	}
	else
	{
		if (structsize != sizeof(OSVERSIONINFOW) && structsize != sizeof(OSVERSIONINFOEXW))
		{
			SetLastError(ERROR_INSUFFICIENT_BUFFER);
			return FALSE;
		}
	}
	
	ver->dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);
	ret = GetVersionExA(ver);
	if (ret)
	{
		char csdVer[128];
		OSVERSIONINFOEXA* verexA = (OSVERSIONINFOEXA*) ver;
		OSVERSIONINFOEXW* verexW = (OSVERSIONINFOEXW*) ver;
		
		switch (structsize)
		{
		case sizeof(OSVERSIONINFOEXA):
			verexA->wServicePackMajor = 0;
			verexA->wServicePackMinor = 0;
			verexA->wSuiteMask = 0;
			verexA->wProductType = 0;
			verexA->wReserved = 0;
		case sizeof(OSVERSIONINFOA):
			break;
		case sizeof(OSVERSIONINFOEXW):
			verexW->wServicePackMajor = 0;
			verexW->wServicePackMinor = 0;
			verexW->wSuiteMask = 0;
			verexW->wProductType = 0;
			verexW->wReserved = 0;
		case sizeof(OSVERSIONINFOW):
			strcpy(csdVer, ver->szCSDVersion);
			MultiByteToWideChar(CP_ACP, 0, csdVer, -1, verexW->szCSDVersion, 
					sizeof(csdVer));
			break;
		}
	}
	ver->dwOSVersionInfoSize = structsize;
	return ret;
}

static DWORD common_GetVersion(WINDOWS_VERSION version)
{
	const OSVERSIONINFOEXA_PRIV* osv = &VersionData[version];
	DWORD result;

	result = MAKELONG(MAKEWORD(osv->dwMajorVersion, osv->dwMinorVersion), 
			(osv->dwPlatformId ^ 2) << 14);
	if (osv->dwPlatformId == VER_PLATFORM_WIN32_NT)
		result |= LOWORD(osv->dwBuildNumber) << 16;
	return result;
}

static BOOL common_GetVersionEx(WINDOWS_VERSION version, void* buf, BOOL unicode)
{
	const OSVERSIONINFOEXA_PRIV* osv = &VersionData[version];

	if (!unicode) 
	{
		OSVERSIONINFOEXA* osa = (OSVERSIONINFOEXA*) buf;
		if (osa->dwOSVersionInfoSize != sizeof(OSVERSIONINFOA) 
				&& osa->dwOSVersionInfoSize != sizeof(OSVERSIONINFOEXA))
		{
			SetLastError(ERROR_INSUFFICIENT_BUFFER);
			return FALSE;
		}
		osa->dwMajorVersion = osv->dwMajorVersion;
		osa->dwMinorVersion = osv->dwMinorVersion;
		osa->dwBuildNumber = osv->dwBuildNumber;
		osa->dwPlatformId = osv->dwPlatformId;
		strcpy(osa->szCSDVersion, osv->lpszCSDVersion);
		if (osa->dwOSVersionInfoSize == sizeof(OSVERSIONINFOEXA))
		{
			osa->wServicePackMajor = osv->wServicePackMajor;
			osa->wServicePackMinor = osv->wServicePackMinor;
			osa->wSuiteMask = osv->wSuiteMask;
			osa->wProductType = osv->wProductType;
			osa->wReserved = osv->wReserved;
		}
	}
	else //if (unicode)
	{
		OSVERSIONINFOEXW* osw = (OSVERSIONINFOEXW*) buf;
		if (osw->dwOSVersionInfoSize != sizeof(OSVERSIONINFOW)
				&& osw->dwOSVersionInfoSize != sizeof(OSVERSIONINFOEXW))
		{
			SetLastError(ERROR_INSUFFICIENT_BUFFER);
			return FALSE;
		}
		osw->dwMajorVersion = osv->dwMajorVersion;
		osw->dwMinorVersion = osv->dwMinorVersion;
		osw->dwBuildNumber = osv->dwBuildNumber;
		osw->dwPlatformId = osv->dwPlatformId;
		MultiByteToWideChar(CP_ACP, 0, osv->lpszCSDVersion, -1, 
				osw->szCSDVersion, sizeof(osw->szCSDVersion) / sizeof(WCHAR));
		if (osw->dwOSVersionInfoSize == sizeof(OSVERSIONINFOEXW))
		{
			osw->wServicePackMajor = osv->wServicePackMajor;
			osw->wServicePackMinor = osv->wServicePackMinor;
			osw->wSuiteMask = osv->wSuiteMask;
			osw->wProductType = osv->wProductType;
			osw->wReserved = osv->wReserved;
		}
	}
	return TRUE;
}

static DWORD version_compare_values(ULONG left, ULONG right, UCHAR condition)
{
	switch (condition)
	{
		case VER_EQUAL:
			if (left != right) return ERROR_OLD_WIN_VERSION;
			break;
		case VER_GREATER:
			if (left <= right) return ERROR_OLD_WIN_VERSION;
			break;
		case VER_GREATER_EQUAL:
			if (left < right) return ERROR_OLD_WIN_VERSION;
			break;
		case VER_LESS:
			if (left >= right) return ERROR_OLD_WIN_VERSION;
			break;
		case VER_LESS_EQUAL:
			if (left > right) return ERROR_OLD_WIN_VERSION;
			break;
		default:
			return ERROR_OLD_WIN_VERSION;
	}
	return ERROR_SUCCESS;
}

static DWORD common_VerifyVersionInfo(WINDOWS_VERSION version, 
		const OSVERSIONINFOEXA* info, DWORD dwTypeMask, DWORDLONG dwlConditionMask)
{
	const OSVERSIONINFOEXA_PRIV* osv = &VersionData[version];
	DWORD status;

	if (!(dwTypeMask && dwlConditionMask)) return ERROR_BAD_ARGUMENTS;

	if (dwTypeMask & VER_PRODUCT_TYPE)
	{
		status = version_compare_values(osv->wProductType, info->wProductType, (UCHAR)(dwlConditionMask >> 7*3 & 0x07));
		if (status != ERROR_SUCCESS)
			return status;
	}
	if (dwTypeMask & VER_SUITENAME)
		switch(dwlConditionMask >> 6*3 & 0x07)
		{
		case VER_AND:
			if ((info->wSuiteMask & osv->wSuiteMask) != info->wSuiteMask)
				return ERROR_OLD_WIN_VERSION;
			break;
		case VER_OR:
			if (!(info->wSuiteMask & osv->wSuiteMask) && info->wSuiteMask)
				return ERROR_OLD_WIN_VERSION;
			break;
		default:
			return ERROR_BAD_ARGUMENTS;
		}
	if (dwTypeMask & VER_PLATFORMID)
	{
		status = version_compare_values(osv->dwPlatformId, info->dwPlatformId, (UCHAR)(dwlConditionMask >> 3*3 & 0x07));
		if (status != ERROR_SUCCESS)
			return status;
	}
	if (dwTypeMask & VER_BUILDNUMBER)
	{
		status = version_compare_values(osv->dwBuildNumber, info->dwBuildNumber, (UCHAR)(dwlConditionMask >> 2*3 & 0x07));
		if (status != ERROR_SUCCESS)
			return status;
	}

	if (dwTypeMask & (VER_MAJORVERSION|VER_MINORVERSION|VER_SERVICEPACKMAJOR|VER_SERVICEPACKMINOR))
	{
		UCHAR condition = 0;
		BOOL do_next_check = TRUE;

		if (dwTypeMask & VER_MAJORVERSION)
			condition = (UCHAR)(dwlConditionMask >> 1*3 & 0x07);
		else if (dwTypeMask & VER_MINORVERSION)
			condition = (UCHAR)(dwlConditionMask >> 0*3 & 0x07);
		else if (dwTypeMask & VER_SERVICEPACKMAJOR)
			condition = (UCHAR)(dwlConditionMask >> 5*3 & 0x07);
		else if (dwTypeMask & VER_SERVICEPACKMINOR)
			condition = (UCHAR)(dwlConditionMask >> 4*3 & 0x07);

		if (dwTypeMask & VER_MAJORVERSION)
		{
			status = version_compare_values(osv->dwMajorVersion, info->dwMajorVersion, condition);
			do_next_check = (osv->dwMajorVersion == info->dwMajorVersion) &&
				((condition != VER_EQUAL) || (status == ERROR_SUCCESS));
		}
		if ((dwTypeMask & VER_MINORVERSION) && do_next_check)
		{
			status = version_compare_values(osv->dwMinorVersion, info->dwMinorVersion, condition);
			do_next_check = (osv->dwMinorVersion == info->dwMinorVersion) &&
				((condition != VER_EQUAL) || (status == ERROR_SUCCESS));
		}
		if ((dwTypeMask & VER_SERVICEPACKMAJOR) && do_next_check)
		{
			status = version_compare_values(osv->wServicePackMajor, info->wServicePackMajor, condition);
			do_next_check = (osv->wServicePackMajor == info->wServicePackMajor) &&
				((condition != VER_EQUAL) || (status == ERROR_SUCCESS));
		}
		if ((dwTypeMask & VER_SERVICEPACKMINOR) && do_next_check)
		{
			status = version_compare_values(osv->wServicePackMinor, info->wServicePackMinor, condition);
		}

		if (status != ERROR_SUCCESS)
			return status;
	}

	return ERROR_SUCCESS;
}

static BOOL WINAPI common_VerifyVersionInfoA(WINDOWS_VERSION version,
		LPOSVERSIONINFOEXA lpVersionInfo, DWORD dwTypeMask, 
		DWORDLONG dwlConditionMask)
{
	DWORD ret = common_VerifyVersionInfo(version, lpVersionInfo, dwTypeMask, dwlConditionMask);
	if (ret)
	{
		SetLastError(ret);
		return FALSE;
	}
	return TRUE;
}

static BOOL WINAPI common_VerifyVersionInfoW(WINDOWS_VERSION version,
		LPOSVERSIONINFOEXW lpVersionInfo, DWORD dwTypeMask, 
		DWORDLONG dwlConditionMask)
{
	DWORD ret;
	OSVERSIONINFOEXA VersionInfoA;
	VersionInfoA.dwMajorVersion = lpVersionInfo->dwMajorVersion;
	VersionInfoA.dwMinorVersion = lpVersionInfo->dwMinorVersion;
	VersionInfoA.dwBuildNumber = lpVersionInfo->dwBuildNumber;
	VersionInfoA.dwPlatformId = lpVersionInfo->dwPlatformId;
	VersionInfoA.wServicePackMajor = lpVersionInfo->wServicePackMajor;
	VersionInfoA.wServicePackMinor = lpVersionInfo->wServicePackMinor;
	VersionInfoA.wSuiteMask = lpVersionInfo->wSuiteMask;
	VersionInfoA.wProductType = lpVersionInfo->wProductType;

	ret = common_VerifyVersionInfo(version, &VersionInfoA, dwTypeMask, dwlConditionMask);
	if (ret)
	{
		SetLastError(ret);
		return FALSE;
	}
	return TRUE;
}

/* MAKE_EXPORT VerSetConditionMask_new=VerSetConditionMask */
ULONGLONG WINAPI VerSetConditionMask_new(ULONGLONG dwlConditionMask, 
		DWORD dwTypeBitMask, BYTE dwConditionMask)
{
	if (dwTypeBitMask == 0)
		return dwlConditionMask;
	dwConditionMask &= 0x07;
	if (dwConditionMask == 0)
		return dwlConditionMask;

	if (dwTypeBitMask & VER_PRODUCT_TYPE)
		dwlConditionMask |= dwConditionMask << 7*3;
	else if (dwTypeBitMask & VER_SUITENAME)
		dwlConditionMask |= dwConditionMask << 6*3;
	else if (dwTypeBitMask & VER_SERVICEPACKMAJOR)
		dwlConditionMask |= dwConditionMask << 5*3;
	else if (dwTypeBitMask & VER_SERVICEPACKMINOR)
		dwlConditionMask |= dwConditionMask << 4*3;
	else if (dwTypeBitMask & VER_PLATFORMID)
		dwlConditionMask |= dwConditionMask << 3*3;
	else if (dwTypeBitMask & VER_BUILDNUMBER)
		dwlConditionMask |= dwConditionMask << 2*3;
	else if (dwTypeBitMask & VER_MAJORVERSION)
		dwlConditionMask |= dwConditionMask << 1*3;
	else if (dwTypeBitMask & VER_MINORVERSION)
		dwlConditionMask |= dwConditionMask << 0*3;
	return dwlConditionMask;
}

/* MAKE_EXPORT GetVersion_WIN95=GetVersion */
DWORD WINAPI GetVersion_WIN95(void)
{
	return common_GetVersion(WIN95);
}

/* MAKE_EXPORT GetVersion_WIN98=GetVersion */
DWORD WINAPI GetVersion_WIN98(void)
{
	return common_GetVersion(WIN98);
}

/* MAKE_EXPORT GetVersion_WINME=GetVersion */
DWORD WINAPI GetVersion_WINME(void)
{
	return common_GetVersion(WINME);
}

/* MAKE_EXPORT GetVersion_NT40=GetVersion */
DWORD WINAPI GetVersion_NT40(void)
{
	return common_GetVersion(NT40);
}

/* MAKE_EXPORT GetVersion_NT2K=GetVersion */
DWORD WINAPI GetVersion_NT2K(void)
{
	return common_GetVersion(NT2K);
}

/* MAKE_EXPORT GetVersion_WINXP=GetVersion */
DWORD WINAPI GetVersion_WINXP(void)
{
	return common_GetVersion(WINXP);
}

/* MAKE_EXPORT GetVersion_WIN2K3=GetVersion */
DWORD WINAPI GetVersion_WIN2K3(void)
{
	return common_GetVersion(WIN2K3);
}

/* MAKE_EXPORT GetVersion_VISTA=GetVersion */
DWORD WINAPI GetVersion_VISTA(void)
{
	return common_GetVersion(VISTA);
}

/* MAKE_EXPORT GetVersion_WIN2K8=GetVersion */
DWORD WINAPI GetVersion_WIN2K8(void)
{
	return common_GetVersion(WIN2K8);
}

/* MAKE_EXPORT GetVersionExA_ORIG=GetVersionExA */
BOOL WINAPI GetVersionExA_ORIG(LPOSVERSIONINFOA lpVersionInfo)
{
	return original_GetVersionEx(lpVersionInfo, FALSE);
}

/* MAKE_EXPORT GetVersionExW_ORIG=GetVersionExW */
BOOL WINAPI GetVersionExW_ORIG(LPOSVERSIONINFOW lpVersionInfo)
{
	return original_GetVersionEx(lpVersionInfo, TRUE);
}

/* MAKE_EXPORT GetVersionExA_WIN95=GetVersionExA */
BOOL WINAPI GetVersionExA_WIN95(LPOSVERSIONINFOA lpVersionInfo)
{
	return common_GetVersionEx(WIN95, lpVersionInfo, FALSE);
}

/* MAKE_EXPORT GetVersionExW_WIN95=GetVersionExW */
BOOL WINAPI GetVersionExW_WIN95(LPOSVERSIONINFOW lpVersionInfo)
{
	return common_GetVersionEx(WIN95, lpVersionInfo, TRUE);
}

/* MAKE_EXPORT GetVersionExA_WIN98=GetVersionExA */
BOOL WINAPI GetVersionExA_WIN98(LPOSVERSIONINFOA lpVersionInfo)
{
	return common_GetVersionEx(WIN98, lpVersionInfo, FALSE);
}

/* MAKE_EXPORT GetVersionExW_WIN98=GetVersionExW */
BOOL WINAPI GetVersionExW_WIN98(LPOSVERSIONINFOW lpVersionInfo)
{
	return common_GetVersionEx(WIN98, lpVersionInfo, TRUE);
}

/* MAKE_EXPORT GetVersionExA_WINME=GetVersionExA */
BOOL WINAPI GetVersionExA_WINME(LPOSVERSIONINFOA lpVersionInfo)
{
	return common_GetVersionEx(WINME, lpVersionInfo, FALSE);
}

/* MAKE_EXPORT GetVersionExW_WINME=GetVersionExW */
BOOL WINAPI GetVersionExW_WINME(LPOSVERSIONINFOW lpVersionInfo)
{
	return common_GetVersionEx(WINME, lpVersionInfo, TRUE);
}

/* MAKE_EXPORT GetVersionExA_NT40=GetVersionExA */
BOOL WINAPI GetVersionExA_NT40(LPOSVERSIONINFOA lpVersionInfo)
{
	return common_GetVersionEx(NT40, lpVersionInfo, FALSE);
}

/* MAKE_EXPORT GetVersionExW_NT40=GetVersionExW */
BOOL WINAPI GetVersionExW_NT40(LPOSVERSIONINFOW lpVersionInfo)
{
	return common_GetVersionEx(NT40, lpVersionInfo, TRUE);
}

/* MAKE_EXPORT GetVersionExA_NT2K=GetVersionExA */
BOOL WINAPI GetVersionExA_NT2K(LPOSVERSIONINFOA lpVersionInfo)
{
	return common_GetVersionEx(NT2K, lpVersionInfo, FALSE);
}

/* MAKE_EXPORT GetVersionExW_NT2K=GetVersionExW */
BOOL WINAPI GetVersionExW_NT2K(LPOSVERSIONINFOW lpVersionInfo)
{
	return common_GetVersionEx(NT2K, lpVersionInfo, TRUE);
}

/* MAKE_EXPORT GetVersionExA_WINXP=GetVersionExA */
BOOL WINAPI GetVersionExA_WINXP(LPOSVERSIONINFOA lpVersionInfo)
{
	return common_GetVersionEx(WINXP, lpVersionInfo, FALSE);
}

/* MAKE_EXPORT GetVersionExW_WINXP=GetVersionExW */
BOOL WINAPI GetVersionExW_WINXP(LPOSVERSIONINFOW lpVersionInfo)
{
	return common_GetVersionEx(WINXP, lpVersionInfo, TRUE);
}

/* MAKE_EXPORT GetVersionExA_WIN2K3=GetVersionExA */
BOOL WINAPI GetVersionExA_WIN2K3(LPOSVERSIONINFOA lpVersionInfo)
{
	return common_GetVersionEx(WIN2K3, lpVersionInfo, FALSE);
}

/* MAKE_EXPORT GetVersionExW_WIN2K3=GetVersionExW */
BOOL WINAPI GetVersionExW_WIN2K3(LPOSVERSIONINFOW lpVersionInfo)
{
	return common_GetVersionEx(WIN2K3, lpVersionInfo, TRUE);
}

/* MAKE_EXPORT GetVersionExA_VISTA=GetVersionExA */
BOOL WINAPI GetVersionExA_VISTA(LPOSVERSIONINFOA lpVersionInfo)
{
	return common_GetVersionEx(VISTA, lpVersionInfo, FALSE);
}

/* MAKE_EXPORT GetVersionExW_VISTA=GetVersionExW */
BOOL WINAPI GetVersionExW_VISTA(LPOSVERSIONINFOW lpVersionInfo)
{
	return common_GetVersionEx(VISTA, lpVersionInfo, TRUE);
}

/* MAKE_EXPORT GetVersionExA_WIN2K8=GetVersionExA */
BOOL WINAPI GetVersionExA_WIN2K8(LPOSVERSIONINFOA lpVersionInfo)
{
	return common_GetVersionEx(WIN2K8, lpVersionInfo, FALSE);
}

/* MAKE_EXPORT GetVersionExW_WIN2K8=GetVersionExW */
BOOL WINAPI GetVersionExW_WIN2K8(LPOSVERSIONINFOW lpVersionInfo)
{
	return common_GetVersionEx(WIN2K8, lpVersionInfo, TRUE);
}

/* MAKE_EXPORT VerifyVersionInfoA_NT2K=VerifyVersionInfoA */
BOOL WINAPI VerifyVersionInfoA_NT2K(LPOSVERSIONINFOEXA lpVersionInfo, DWORD dwTypeMask, 
		DWORDLONG dwlConditionMask)
{
	return common_VerifyVersionInfoA(NT2K, lpVersionInfo, dwTypeMask, dwlConditionMask);
}

/* MAKE_EXPORT VerifyVersionInfoW_NT2K=VerifyVersionInfoW */
BOOL WINAPI VerifyVersionInfoW_NT2K(LPOSVERSIONINFOEXW lpVersionInfo, DWORD dwTypeMask, 
		DWORDLONG dwlConditionMask)
{
	return common_VerifyVersionInfoW(NT2K, lpVersionInfo, dwTypeMask, dwlConditionMask);
}

/* MAKE_EXPORT VerifyVersionInfoA_WINXP=VerifyVersionInfoA */
BOOL WINAPI VerifyVersionInfoA_WINXP(LPOSVERSIONINFOEXA lpVersionInfo, DWORD dwTypeMask, 
		DWORDLONG dwlConditionMask)
{
	return common_VerifyVersionInfoA(WINXP, lpVersionInfo, dwTypeMask, dwlConditionMask);
}

/* MAKE_EXPORT VerifyVersionInfoW_WINXP=VerifyVersionInfoW */
BOOL WINAPI VerifyVersionInfoW_WINXP(LPOSVERSIONINFOEXW lpVersionInfo, DWORD dwTypeMask, 
		DWORDLONG dwlConditionMask)
{
	return common_VerifyVersionInfoW(WINXP, lpVersionInfo, dwTypeMask, dwlConditionMask);
}

/* MAKE_EXPORT VerifyVersionInfoA_WIN2K3=VerifyVersionInfoA */
BOOL WINAPI VerifyVersionInfoA_WIN2K3(LPOSVERSIONINFOEXA lpVersionInfo, DWORD dwTypeMask, 
		DWORDLONG dwlConditionMask)
{
	return common_VerifyVersionInfoA(WIN2K3, lpVersionInfo, dwTypeMask, dwlConditionMask);
}

/* MAKE_EXPORT VerifyVersionInfoW_WIN2K3=VerifyVersionInfoW */
BOOL WINAPI VerifyVersionInfoW_WIN2K3(LPOSVERSIONINFOEXW lpVersionInfo, DWORD dwTypeMask, 
		DWORDLONG dwlConditionMask)
{
	return common_VerifyVersionInfoW(WIN2K3, lpVersionInfo, dwTypeMask, dwlConditionMask);
}

/* MAKE_EXPORT VerifyVersionInfoA_VISTA=VerifyVersionInfoA */
BOOL WINAPI VerifyVersionInfoA_VISTA(LPOSVERSIONINFOEXA lpVersionInfo, DWORD dwTypeMask, 
		DWORDLONG dwlConditionMask)
{
	return common_VerifyVersionInfoA(VISTA, lpVersionInfo, dwTypeMask, dwlConditionMask);
}

/* MAKE_EXPORT VerifyVersionInfoW_VISTA=VerifyVersionInfoW */
BOOL WINAPI VerifyVersionInfoW_VISTA(LPOSVERSIONINFOEXW lpVersionInfo, DWORD dwTypeMask, 
		DWORDLONG dwlConditionMask)
{
	return common_VerifyVersionInfoW(VISTA, lpVersionInfo, dwTypeMask, dwlConditionMask);
}

/* MAKE_EXPORT VerifyVersionInfoA_WIN2K8=VerifyVersionInfoA */
BOOL WINAPI VerifyVersionInfoA_WIN2K8(LPOSVERSIONINFOEXA lpVersionInfo, DWORD dwTypeMask, 
		DWORDLONG dwlConditionMask)
{
	return common_VerifyVersionInfoA(WIN2K8, lpVersionInfo, dwTypeMask, dwlConditionMask);
}

/* MAKE_EXPORT VerifyVersionInfoW_WIN2K8=VerifyVersionInfoW */
BOOL WINAPI VerifyVersionInfoW_WIN2K8(LPOSVERSIONINFOEXW lpVersionInfo, DWORD dwTypeMask, 
		DWORDLONG dwlConditionMask)
{
	return common_VerifyVersionInfoW(WIN2K8, lpVersionInfo, dwTypeMask, dwlConditionMask);
}

