/*
 *  KernelEx
 *  Copyright (C) 2008-2009, Xeno86
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

#ifndef __SHAREDMEM_H
#define __SHAREDMEM_H

#ifdef __cplusplus
extern "C" {
#endif

int  create_shared_heap();
void destroy_shared_heap();

void* malloc(size_t size);
void* calloc(size_t count, size_t size);
void  free(void* ptr);
void* realloc(void* ptr, size_t size);
void* recalloc(void* ptr, size_t size);

#ifdef __cplusplus
}
#endif

#endif
