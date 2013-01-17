/*
 * USER string functions
 *
 * Copyright 1993 Yngvi Sigurjonsson (yngvi@hafro.is)
 * Copyright 1996 Alexandre Julliard
 * Copyright 1996 Marcus Meissner
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

#include <ctype.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <windows.h>
#include "../kernel32/locale_unicode.h"


/***********************************************************************
 *           CharNextExW   (USER32.@)
 */
/* MAKE_EXPORT CharNextExW_new=CharNextExW */
LPWSTR WINAPI CharNextExW_new( WORD codepage, LPCWSTR ptr, DWORD flags )
{
	/* doesn't make sense, there are no codepages for Unicode */
	return NULL;
}


/***********************************************************************
 *           CharNextW   (USER32.@)
 */
/* MAKE_EXPORT CharNextW_new=CharNextW */
LPWSTR WINAPI CharNextW_new(LPCWSTR x)
{
	if (*x) x++;

	return (LPWSTR)x;
}


/***********************************************************************
 *           CharPrevExW   (USER32.@)
 */
/* MAKE_EXPORT CharPrevExW_new=CharPrevExW */
LPSTR WINAPI CharPrevExW_new( WORD codepage, LPCWSTR start, LPCWSTR ptr, DWORD flags )
{
	/* doesn't make sense, there are no codepages for Unicode */
	return NULL;
}


/***********************************************************************
 *           CharPrevW   (USER32.@)
 */
/* MAKE_EXPORT CharPrevW_new=CharPrevW */
LPWSTR WINAPI CharPrevW_new(LPCWSTR start,LPCWSTR x)
{
	if (x>start) return (LPWSTR)(x-1);
	else return (LPWSTR)x;
}


/***********************************************************************
 *           CharToOemBuffW   (USER32.@)
 */
/* MAKE_EXPORT CharToOemBuffW_new=CharToOemBuffW */
BOOL WINAPI CharToOemBuffW_new( LPCWSTR s, LPSTR d, DWORD len )
{
	if ( !s || !d ) return TRUE;
	WideCharToMultiByte( CP_OEMCP, 0, s, len, d, len, NULL, NULL );
	return TRUE;
}


/***********************************************************************
 *           CharToOemW   (USER32.@)
 */
/* MAKE_EXPORT CharToOemW_new=CharToOemW */
BOOL WINAPI CharToOemW_new( LPCWSTR s, LPSTR d )
{
	return CharToOemBuffW_new( s, d, strlenW( s ) + 1 );
}


/***********************************************************************
 *           OemToCharBuffW   (USER32.@)
 */
/* MAKE_EXPORT OemToCharBuffW_new=OemToCharBuffW */
BOOL WINAPI OemToCharBuffW_new( LPCSTR s, LPWSTR d, DWORD len )
{
	MultiByteToWideChar( CP_OEMCP, 0, s, len, d, len );
	return TRUE;
}


/***********************************************************************
 *           OemToCharW   (USER32.@)
 */
/* MAKE_EXPORT OemToCharW_new=OemToCharW */
BOOL WINAPI OemToCharW_new( LPCSTR s, LPWSTR d )
{
	return OemToCharBuffW_new( s, d, strlen( s ) + 1 );
}


/***********************************************************************
 *           CharLowerW   (USER32.@)
 */
/* MAKE_EXPORT CharLowerW_new=CharLowerW */
LPWSTR WINAPI CharLowerW_new(LPWSTR x)
{
	if (HIWORD(x)) return strlwrW(x);
	else return (LPWSTR)((UINT_PTR)tolowerW(LOWORD(x)));
}


/***********************************************************************
 *           CharUpperW   (USER32.@)
 */
/* MAKE_EXPORT CharUpperW_new=CharUpperW */
LPWSTR WINAPI CharUpperW_new(LPWSTR x)
{
	if (HIWORD(x)) return struprW(x);
	else return (LPWSTR)((UINT_PTR)toupperW(LOWORD(x)));
}


/***********************************************************************
 *           CharLowerBuffW   (USER32.@)
 */
/* MAKE_EXPORT CharLowerBuffW_new=CharLowerBuffW */
DWORD WINAPI CharLowerBuffW_new( LPWSTR str, DWORD len )
{
	DWORD ret = len;
	if (!str) return 0; /* YES */
	for (; len; len--, str++) *str = tolowerW(*str);
	return ret;
}


/***********************************************************************
 *           CharUpperBuffW   (USER32.@)
 */
/* MAKE_EXPORT CharUpperBuffW_new=CharUpperBuffW */
DWORD WINAPI CharUpperBuffW_new( LPWSTR str, DWORD len )
{
	DWORD ret = len;
	if (!str) return 0; /* YES */
	for (; len; len--, str++) *str = toupperW(*str);
	return ret;
}


/***********************************************************************
 *           IsCharLowerW   (USER32.@)
 */
/* MAKE_EXPORT IsCharLowerW_new=IsCharLowerW */
BOOL WINAPI IsCharLowerW_new(WCHAR x)
{
	return (get_char_typeW(x) & C1_LOWER) != 0;
}


/***********************************************************************
 *           IsCharUpperW   (USER32.@)
 */
/* MAKE_EXPORT IsCharUpperW_new=IsCharUpperW */
BOOL WINAPI IsCharUpperW_new(WCHAR x)
{
	return (get_char_typeW(x) & C1_UPPER) != 0;
}


/***********************************************************************
 *           IsCharAlphaNumericW   (USER32.@)
 */
/* MAKE_EXPORT IsCharAlphaNumericW_new=IsCharAlphaNumericW */
BOOL WINAPI IsCharAlphaNumericW_new(WCHAR x)
{
	return (get_char_typeW(x) & (C1_ALPHA|C1_DIGIT)) != 0;
}


/***********************************************************************
 *           IsCharAlphaW   (USER32.@)
 */
/* MAKE_EXPORT IsCharAlphaW_new=IsCharAlphaW */
BOOL WINAPI IsCharAlphaW_new(WCHAR x)
{
	return (get_char_typeW(x) & C1_ALPHA) != 0;
}
