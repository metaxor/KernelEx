/*
 *  KernelEx
 *  Copyright (C) 2010, Xeno86
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

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#define _WSPIAPI_COUNTOF(_Array) (sizeof(_Array) / sizeof(_Array[0]))
#include <ws2tcpip.h>
#include <wspiapi.h>

/* MAKE_EXPORT getaddrinfo_new=getaddrinfo */
int WINAPI getaddrinfo_new(
		const char* hostname,
		const char* servname,
		const struct addrinfo* hints,
		struct addrinfo** res
)
{
	return WspiapiLegacyGetAddrInfo(hostname, servname, hints, res);
}

/* MAKE_EXPORT freeaddrinfo_new=freeaddrinfo */
void WINAPI freeaddrinfo_new(
		struct addrinfo* ai
)
{
	WspiapiLegacyFreeAddrInfo(ai);
}

/* MAKE_EXPORT getnameinfo_new=getnameinfo */
int WINAPI getnameinfo_new(
		const struct sockaddr* sa,
		socklen_t salen,
		char* host,
		size_t hostlen,
		char* serv,
		size_t servlen,
		int flags
)
{
	return WspiapiLegacyGetNameInfo(sa, salen, host, hostlen, serv, servlen, flags);
}
