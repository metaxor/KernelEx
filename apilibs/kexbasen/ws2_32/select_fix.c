/*
 *  KernelEx
 *  Copyright (C) 2011, Xeno86
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

/* fixes 100% cpu usage when timeout < 1ms */

/* MAKE_EXPORT select_new=select */
int FAR PASCAL select_new(
	int nfds,	
	fd_set FAR * readfds,	
	fd_set FAR * writefds,	
	fd_set FAR * exceptfds,	
	const struct timeval FAR * timeout	
)
{
	struct timeval tv;
	if (timeout && timeout->tv_sec == 0 && timeout->tv_usec > 0 && timeout->tv_usec < 1000)
	{
		tv.tv_sec = 0;
		tv.tv_usec = 1000;
		timeout = &tv;
	}
	return select(nfds, readfds, writefds, exceptfds, timeout);
}
