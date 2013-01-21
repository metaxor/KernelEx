/*
 * Locale support
 *
 * Copyright 1995 Martin von Loewis
 * Copyright 1998 David Lee Lambert
 * Copyright 2000 Julio César Gázquez
 * Copyright 2002 Alexandre Julliard for CodeWeavers
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

#include "common.h"
#include "locale_unicode.h"


/******************************************************************************
 *           CompareStringW    (KERNEL32.@)
 *
 * See CompareStringA.
 */
/* MAKE_EXPORT CompareStringW_new=CompareStringW */
INT WINAPI CompareStringW_new(LCID lcid, DWORD style,
                              LPCWSTR str1, INT len1, LPCWSTR str2, INT len2)
{
	INT ret;

	if (!str1 || !str2)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return 0;
	}

	if( style & ~(NORM_IGNORECASE|NORM_IGNORENONSPACE|NORM_IGNORESYMBOLS|
		SORT_STRINGSORT|NORM_IGNOREKANATYPE|NORM_IGNOREWIDTH|LOCALE_USE_CP_ACP|0x10000000) )
	{
		SetLastError(ERROR_INVALID_FLAGS);
		return 0;
	}

	/* this style is related to diacritics in Arabic, Japanese, and Hebrew */
	if (style & 0x10000000)
		WARN_OUT("Ignoring unknown style 0x10000000\n");

	if (len1 < 0) len1 = strlenW(str1);
	if (len2 < 0) len2 = strlenW(str2);

	ret = wine_compare_string(style, str1, len1, str2, len2);

	if (ret) /* need to translate result */
		return (ret < 0) ? CSTR_LESS_THAN : CSTR_GREATER_THAN;
	return CSTR_EQUAL;
}

/******************************************************************************
 *           GetStringTypeW    (KERNEL32.@)
 *
 * See GetStringTypeA.
 */
/* MAKE_EXPORT GetStringTypeW_new=GetStringTypeW */
BOOL WINAPI GetStringTypeW_new( DWORD type, LPCWSTR src, INT count, LPWORD chartype )
{
	if (count == -1) count = strlenW(src) + 1;
	switch(type)
	{
	case CT_CTYPE1:
		while (count--) *chartype++ = get_char_typeW( *src++ ) & 0xfff;
		break;
	case CT_CTYPE2:
		while (count--) *chartype++ = get_char_typeW( *src++ ) >> 12;
		break;
	case CT_CTYPE3:
	{
#if 0
		WARN("CT_CTYPE3: semi-stub.\n");
		while (count--)
		{
			int c = *src;
			WORD type1, type3 = 0; /* C3_NOTAPPLICABLE */

			type1 = get_char_typeW( *src++ ) & 0xfff;
			/* try to construct type3 from type1 */
			if(type1 & C1_SPACE) type3 |= C3_SYMBOL;
			if(type1 & C1_ALPHA) type3 |= C3_ALPHA;
			if ((c>=0x30A0)&&(c<=0x30FF)) type3 |= C3_KATAKANA;
			if ((c>=0x3040)&&(c<=0x309F)) type3 |= C3_HIRAGANA;
			if ((c>=0x4E00)&&(c<=0x9FAF)) type3 |= C3_IDEOGRAPH;
			if ((c>=0x0600)&&(c<=0x06FF)) type3 |= C3_KASHIDA;
			if ((c>=0x3000)&&(c<=0x303F)) type3 |= C3_SYMBOL;

			if ((c>=0xFF00)&&(c<=0xFF60)) type3 |= C3_FULLWIDTH;
			if ((c>=0xFF00)&&(c<=0xFF20)) type3 |= C3_SYMBOL;
			if ((c>=0xFF3B)&&(c<=0xFF40)) type3 |= C3_SYMBOL;
			if ((c>=0xFF5B)&&(c<=0xFF60)) type3 |= C3_SYMBOL;
			if ((c>=0xFF21)&&(c<=0xFF3A)) type3 |= C3_ALPHA;
			if ((c>=0xFF41)&&(c<=0xFF5A)) type3 |= C3_ALPHA;
			if ((c>=0xFFE0)&&(c<=0xFFE6)) type3 |= C3_FULLWIDTH;
			if ((c>=0xFFE0)&&(c<=0xFFE6)) type3 |= C3_SYMBOL;

			if ((c>=0xFF61)&&(c<=0xFFDC)) type3 |= C3_HALFWIDTH;
			if ((c>=0xFF61)&&(c<=0xFF64)) type3 |= C3_SYMBOL;
			if ((c>=0xFF65)&&(c<=0xFF9F)) type3 |= C3_KATAKANA;
			if ((c>=0xFF65)&&(c<=0xFF9F)) type3 |= C3_ALPHA;
			if ((c>=0xFFE8)&&(c<=0xFFEE)) type3 |= C3_HALFWIDTH;
			if ((c>=0xFFE8)&&(c<=0xFFEE)) type3 |= C3_SYMBOL;
			*chartype++ = type3;
		}
		break;
#else
		//use implementation from SHLWAPI - ordinal 35
		HMODULE hShlwapi = GetModuleHandle("SHLWAPI.DLL");
		BOOL (WINAPI *GetStringType3ExW)(LPCWSTR, INT, LPWORD);
		if (!hShlwapi)
			hShlwapi = LoadLibrary("SHLWAPI.DLL");
		if (!hShlwapi)
		{
			SetLastError( ERROR_INVALID_PARAMETER );
			return FALSE;
		}
		GetStringType3ExW = (BOOL (WINAPI*)(LPCWSTR, INT, LPWORD)) 
				kexGetProcAddress(hShlwapi, (LPSTR) 35);
		return GetStringType3ExW(src, count, chartype);
#endif
	}
	default:
		SetLastError( ERROR_INVALID_PARAMETER );
		return FALSE;
	}
	return TRUE;
}


/******************************************************************************
 *           GetStringTypeExW    (KERNEL32.@)
 *
 * See GetStringTypeExA.
 */
/* MAKE_EXPORT GetStringTypeExW_new=GetStringTypeExW */
BOOL WINAPI GetStringTypeExW_new( LCID locale, DWORD type, LPCWSTR src, INT count, LPWORD chartype )
{
	/* locale is ignored for Unicode */
	return GetStringTypeW_new( type, src, count, chartype );
}

/*************************************************************************
 *           LCMapStringW    (KERNEL32.@)
 *
 * See LCMapStringA.
 */
/* MAKE_EXPORT LCMapStringW_new=LCMapStringW */
INT WINAPI LCMapStringW_new(LCID lcid, DWORD flags, LPCWSTR src, INT srclen,
                            LPWSTR dst, INT dstlen)
{
	LPWSTR dst_ptr;

	if (!src || !srclen || dstlen < 0)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return 0;
	}

	/* mutually exclusive flags */
	if ((flags & (LCMAP_LOWERCASE | LCMAP_UPPERCASE)) == (LCMAP_LOWERCASE | LCMAP_UPPERCASE) ||
		(flags & (LCMAP_HIRAGANA | LCMAP_KATAKANA)) == (LCMAP_HIRAGANA | LCMAP_KATAKANA) ||
		(flags & (LCMAP_HALFWIDTH | LCMAP_FULLWIDTH)) == (LCMAP_HALFWIDTH | LCMAP_FULLWIDTH) ||
		(flags & (LCMAP_TRADITIONAL_CHINESE | LCMAP_SIMPLIFIED_CHINESE)) == (LCMAP_TRADITIONAL_CHINESE | LCMAP_SIMPLIFIED_CHINESE))
	{
		SetLastError(ERROR_INVALID_FLAGS);
		return 0;
	}

	if (!dstlen) dst = NULL;

	lcid = ConvertDefaultLocale(lcid);

	if (flags & LCMAP_SORTKEY)
	{
		INT ret;
		if (src == dst)
		{
			SetLastError(ERROR_INVALID_FLAGS);
			return 0;
		}

		if (srclen < 0) srclen = strlenW(src);

		TRACE("(0x%04x,0x%08x,%s,%d,%p,%d)\n",
			  lcid, flags, debugstr_wn(src, srclen), srclen, dst, dstlen);

		ret = wine_get_sortkey(flags, src, srclen, (BYTE *)dst, dstlen);
		if (ret == 0)
			SetLastError(ERROR_INSUFFICIENT_BUFFER);
		else
			ret++;
		return ret;
	}

	/* SORT_STRINGSORT must be used exclusively with LCMAP_SORTKEY */
	if (flags & SORT_STRINGSORT)
	{
		SetLastError(ERROR_INVALID_FLAGS);
		return 0;
	}

	if (srclen < 0) srclen = strlenW(src) + 1;

	TRACE("(0x%04x,0x%08x,%s,%d,%p,%d)\n",
		  lcid, flags, debugstr_wn(src, srclen), srclen, dst, dstlen);

	if (!dst) /* return required string length */
	{
		INT len;

		for (len = 0; srclen; src++, srclen--)
		{
			WCHAR wch = *src;
			/* tests show that win2k just ignores NORM_IGNORENONSPACE,
			 * and skips white space and punctuation characters for
			 * NORM_IGNORESYMBOLS.
			 */
			if ((flags & NORM_IGNORESYMBOLS) && (get_char_typeW(wch) & (C1_PUNCT | C1_SPACE)))
				continue;
			len++;
		}
		return len;
	}

	if (flags & LCMAP_UPPERCASE)
	{
		for (dst_ptr = dst; srclen && dstlen; src++, srclen--)
		{
			WCHAR wch = *src;
			if ((flags & NORM_IGNORESYMBOLS) && (get_char_typeW(wch) & (C1_PUNCT | C1_SPACE)))
				continue;
			*dst_ptr++ = toupperW(wch);
			dstlen--;
		}
	}
	else if (flags & LCMAP_LOWERCASE)
	{
		for (dst_ptr = dst; srclen && dstlen; src++, srclen--)
		{
			WCHAR wch = *src;
			if ((flags & NORM_IGNORESYMBOLS) && (get_char_typeW(wch) & (C1_PUNCT | C1_SPACE)))
				continue;
			*dst_ptr++ = tolowerW(wch);
			dstlen--;
		}
	}
	else
	{
		if (src == dst)
		{
			SetLastError(ERROR_INVALID_FLAGS);
			return 0;
		}
		for (dst_ptr = dst; srclen && dstlen; src++, srclen--)
		{
			WCHAR wch = *src;
			if ((flags & NORM_IGNORESYMBOLS) && (get_char_typeW(wch) & (C1_PUNCT | C1_SPACE)))
				continue;
			*dst_ptr++ = wch;
			dstlen--;
		}
	}

	if (srclen)
	{
		SetLastError(ERROR_INSUFFICIENT_BUFFER);
		return 0;
	}

	return dst_ptr - dst;
}

/*************************************************************************
 *           FoldStringW    (KERNEL32.@)
 *
 * See FoldStringA.
 */
/* MAKE_EXPORT FoldStringW_new=FoldStringW */
INT WINAPI FoldStringW_new(DWORD dwFlags, LPCWSTR src, INT srclen,
                       LPWSTR dst, INT dstlen)
{
	int ret;

	switch (dwFlags & (MAP_COMPOSITE|MAP_PRECOMPOSED|MAP_EXPAND_LIGATURES))
	{
	case 0:
		if (dwFlags)
		  break;
		/* Fall through for dwFlags == 0 */
	case MAP_PRECOMPOSED|MAP_COMPOSITE:
	case MAP_PRECOMPOSED|MAP_EXPAND_LIGATURES:
	case MAP_COMPOSITE|MAP_EXPAND_LIGATURES:
		SetLastError(ERROR_INVALID_FLAGS);
		return 0;
	}

	if (!src || !srclen || dstlen < 0 || (dstlen && !dst) || src == dst)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return 0;
	}

	ret = wine_fold_string(dwFlags, src, srclen, dst, dstlen);
	if (!ret)
		SetLastError(ERROR_INSUFFICIENT_BUFFER);
	return ret;
}

/*************************************************************************
 *           FoldStringA    (KERNEL32.@)
 *
 * Map characters in a string.
 *
 * PARAMS
 *  dwFlags [I] Flags controlling chars to map (MAP_ constants from "winnls.h")
 *  src     [I] String to map
 *  srclen  [I] Length of src, or -1 if src is NUL terminated
 *  dst     [O] Destination for mapped string
 *  dstlen  [I] Length of dst, or 0 to find the required length for the mapped string
 *
 * RETURNS
 *  Success: The length of the string written to dst, including the terminating NUL. If
 *           dstlen is 0, the value returned is the same, but nothing is written to dst,
 *           and dst may be NULL.
 *  Failure: 0. Use GetLastError() to determine the cause.
 */
/* MAKE_EXPORT FoldStringA_new=FoldStringA */
INT WINAPI FoldStringA_new(DWORD dwFlags, LPCSTR src, INT srclen,
                       LPSTR dst, INT dstlen)
{
	INT ret = 0, srclenW = 0;
	WCHAR *srcW = NULL, *dstW = NULL;

	if (!src || !srclen || dstlen < 0 || (dstlen && !dst) || src == dst)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return 0;
	}

	srclenW = MultiByteToWideChar(CP_ACP, dwFlags & MAP_COMPOSITE ? MB_COMPOSITE : 0,
								  src, srclen, NULL, 0);
	srcW = (WCHAR*) HeapAlloc(GetProcessHeap(), 0, srclenW * sizeof(WCHAR));

	if (!srcW)
	{
		SetLastError(ERROR_NOT_ENOUGH_MEMORY);
		goto FoldStringA_exit;
	}

	MultiByteToWideChar(CP_ACP, dwFlags & MAP_COMPOSITE ? MB_COMPOSITE : 0,
						src, srclen, srcW, srclenW);

	dwFlags = (dwFlags & ~MAP_PRECOMPOSED) | MAP_FOLDCZONE;

	ret = FoldStringW_new(dwFlags, srcW, srclenW, NULL, 0);
	if (ret && dstlen)
	{
		dstW = (WCHAR*) HeapAlloc(GetProcessHeap(), 0, ret * sizeof(WCHAR));

		if (!dstW)
		{
			SetLastError(ERROR_NOT_ENOUGH_MEMORY);
			goto FoldStringA_exit;
		}

		ret = FoldStringW_new(dwFlags, srcW, srclenW, dstW, ret);
		if (!WideCharToMultiByte(CP_ACP, 0, dstW, ret, dst, dstlen, NULL, NULL))
		{
			ret = 0;
			SetLastError(ERROR_INSUFFICIENT_BUFFER);
		}
	}

	HeapFree(GetProcessHeap(), 0, dstW);

FoldStringA_exit:
	HeapFree(GetProcessHeap(), 0, srcW);
	return ret;
}

/*************************************************************************
 *           lstrcmpW    (KERNEL32.@)
 *
 * See lstrcmpA.
 */
/* MAKE_EXPORT lstrcmpW_new=lstrcmpW */
int WINAPI lstrcmpW_new(LPCWSTR str1, LPCWSTR str2)
{
	int ret;

	if ((str1 == NULL) && (str2 == NULL)) return 0;
	if (str1 == NULL) return -1;
	if (str2 == NULL) return 1;

	ret = CompareStringW_new(GetThreadLocale(), 0, str1, -1, str2, -1);
	if (ret) ret -= 2;

	return ret;
}

/*************************************************************************
 *           lstrcmpiW    (KERNEL32.@)
 *
 * See lstrcmpiA.
 */
/* MAKE_EXPORT lstrcmpiW_new=lstrcmpiW */
int WINAPI lstrcmpiW_new(LPCWSTR str1, LPCWSTR str2)
{
	int ret;

	if ((str1 == NULL) && (str2 == NULL)) return 0;
	if (str1 == NULL) return -1;
	if (str2 == NULL) return 1;

	ret = CompareStringW_new(GetThreadLocale(), NORM_IGNORECASE, str1, -1, str2, -1);
	if (ret) ret -= 2;

	return ret;
}
