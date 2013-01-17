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

#include <windows.h>
#include <algorithm>
#include "apilib.h"
#include "apiconf.h"
#include "resolver.h"
#include "internals.h"
#include "debug.h"

using namespace std;

ApiConfiguration::ApiConfiguration(const char* name)
{
	initialized = prepare(name);
}

ApiConfiguration::ApiConfiguration(const char* new_name, const ApiConfiguration& src)
{
	if (!prepare(new_name))
		goto __error;

	for (int i = 0 ; i < src.api_tables_count ; i++)
	{
		void* mem;
		const ModuleApi& stab = src.api_tables[i];
		ModuleApi& dtab = api_tables[i];

		mem = malloc(stab.named_apis_count * sizeof(NamedApi));
		if (!mem)
			goto __error;

		dtab.named_apis = (NamedApi*) mem;
		dtab.named_apis_count = stab.named_apis_count;
		copy(stab.named_apis, stab.named_apis + stab.named_apis_count, 
				dtab.named_apis);

		mem = malloc(stab.unnamed_apis_count * sizeof(PROC));
		if (!mem)
			goto __error;

		dtab.unnamed_apis = (PDWORD) mem;
		dtab.unnamed_apis_count = stab.unnamed_apis_count;
		copy(stab.unnamed_apis, stab.unnamed_apis + stab.unnamed_apis_count, 
				dtab.unnamed_apis);
	}

	initialized = true;
	return;

__error:
	initialized = false;
}

ApiConfiguration::~ApiConfiguration()
{
	for (int i = 0 ; i < api_tables_count ; i++)
	{
		free(api_tables[i].named_apis);
		free(api_tables[i].unnamed_apis);
	}
	free(conf_name);
	free(api_tables);
}

bool ApiConfiguration::prepare(const char* name)
{
	int size = strlen(name) + 1;
	conf_name = (char*) malloc(size);
	if (!conf_name)
		return false;

	strcpy(conf_name, name);

	api_tables = (ModuleApi*) calloc(overridden_module_count, sizeof(ModuleApi));
	if (!api_tables)
		return false;

	api_tables_count = overridden_module_count;
	return true;
}

bool ApiConfiguration::merge(const ApiLibrary* apilib)
{
	if (!apilib)
		return false;
	
	for (int i = 0 ; i < overridden_module_count ; i++)
	{
		const apilib_api_table* psrc = apilib->api_tables;
		while (psrc->target_library)
		{
			if (!strcmpi(psrc->target_library, overridden_module_names[i]))
			{
				if (i >= api_tables_count)
				{
					void* mem = recalloc(api_tables, overridden_module_count * sizeof(ModuleApi));
					if (!mem)
						return false;
					api_tables = (ModuleApi*) mem;
					api_tables_count = overridden_module_count;
				}
				ModuleApi* pdst = api_tables + i;

				//join ordinal apis

				unsigned short max_ord;
				if (psrc->ordinal_apis_count)
					max_ord = psrc->ordinal_apis[psrc->ordinal_apis_count - 1].ord;
				else
					max_ord = 0;

				if (pdst->unnamed_apis_count < max_ord)
				{
					void* mem = recalloc(pdst->unnamed_apis, max_ord * sizeof(PROC));
					if (!mem)
						return false;
					pdst->unnamed_apis = (PDWORD) mem;
					pdst->unnamed_apis_count = max_ord;
				}

				//join tables
				for (int i = psrc->ordinal_apis_count - 1 ; i >= 0  ; i--)
				{
					const apilib_unnamed_api& pua = psrc->ordinal_apis[i];
					pdst->unnamed_apis[pua.ord - 1] = encode_address(pua.addr, apilib);
				}

				//join named apis

				const apilib_named_api* sfirst = psrc->named_apis;
				const apilib_named_api* slast = sfirst + psrc->named_apis_count;
				NamedApi* dfirst = pdst->named_apis;
				NamedApi* dlast = dfirst + pdst->named_apis_count;

				//check how much space is needed first
				int space = 0;
				while (sfirst != slast && dfirst != dlast)
				{
					int cmp_result = strcmp(dfirst->api_name, sfirst->name);
					if (cmp_result < 0)
					{
						dfirst++;
					}
					else if (cmp_result == 0)
					{
						dfirst++;
						sfirst++;
						while (sfirst != slast && !strcmp(sfirst->name, (sfirst - 1)->name))
							sfirst++;
					}
					else
					{
						space++;
						sfirst++;
						while (sfirst != slast && !strcmp(sfirst->name, (sfirst - 1)->name))
							sfirst++;
					}
				}
		
				//entries after end of dest
				while (sfirst != slast)
				{
					sfirst++;
					space++;
					while (sfirst != slast && !strcmp(sfirst->name, (sfirst - 1)->name))
						sfirst++;
				}

				//allocate new mem
				if (space)
				{
					void* mem = realloc(pdst->named_apis, 
							(pdst->named_apis_count + space) * sizeof(NamedApi));
					if (!mem)
						return false;

					pdst->named_apis = (NamedApi*) mem;
				}

				sfirst = psrc->named_apis;
				dfirst = pdst->named_apis;
				dlast = dfirst + pdst->named_apis_count;
				pdst->named_apis_count += space;

				//join tables
				while (sfirst != slast && dfirst != dlast)
				{
					int cmp_result = strcmp(dfirst->api_name, sfirst->name);
					if (cmp_result < 0)
						dfirst++;
					else if (cmp_result == 0)
					{
						//same apis => update address
						(dfirst++)->address = encode_address((sfirst++)->addr, apilib);
						while (sfirst != slast && !strcmp(sfirst->name, (sfirst - 1)->name))
							sfirst++;
					}
					else //cmp_result > 0
					{
						//insert new api
						copy_backward(dfirst, dlast, dlast + 1);
						dlast++;
						dfirst->api_name = sfirst->name;
						dfirst->address = encode_address(sfirst->addr, apilib);
						dfirst++;
						sfirst++;
						while (sfirst != slast && !strcmp(sfirst->name, (sfirst - 1)->name))
							sfirst++;
					}
				}

				//entries after end of dest
				while (sfirst != slast)
				{
					dfirst->api_name = sfirst->name;
					dfirst->address = encode_address(sfirst->addr, apilib);
					dfirst++;
					sfirst++;
					while (sfirst != slast && !strcmp(sfirst->name, (sfirst - 1)->name))
						sfirst++;
				}

				break;
			}
			psrc++;
		}
	}

	return true;
}

bool ApiConfiguration::merge(const char* module, unsigned short ordinal, 
							 const ApiLibrary* apilib, int id)
{
	if (!apilib)
		return false;

	const apilib_api_table* psrc = apilib->api_tables;

	while (psrc->target_library)
	{
		if (!strcmpi(module, psrc->target_library))
			break;
		psrc++;
	}

	if (!psrc->target_library)
		return false;

	apilib_unnamed_api to_find;
	to_find.ord = ordinal;

	const apilib_unnamed_api* first = psrc->ordinal_apis;
	const apilib_unnamed_api* last = first + psrc->ordinal_apis_count;

	const apilib_unnamed_api* low = lower_bound(first, last, to_find);

	if (low == last || to_find < *low)
		return false;

	low += id;

	if (low >= last || to_find < *low)
		return false;

	int i;
	for (i = 0 ; i < overridden_module_count ; i++)
		if (!strcmpi(module, overridden_module_names[i]))
			break;

	if (i == overridden_module_count)
		return false;

	if (api_tables_count <= i)
	{
		void* mem = recalloc(api_tables, overridden_module_count * sizeof(ModuleApi));
		if (!mem)
			return false;
		api_tables = (ModuleApi*) mem;
		api_tables_count = overridden_module_count;
	}

	ModuleApi* pdst = api_tables + i;
	if (pdst->unnamed_apis_count < ordinal)
	{
		void* mem = recalloc(pdst->unnamed_apis, ordinal * sizeof(PROC));
		if (!mem)
			return false;
		pdst->unnamed_apis = (PDWORD) mem;
		pdst->unnamed_apis_count = ordinal;
	}

	pdst->unnamed_apis[ordinal - 1] = encode_address(low->addr, apilib);
	
	return true;
}

bool ApiConfiguration::merge(const char* module, const char* api_name, 
							 const ApiLibrary* apilib, int id)
{
	if (!apilib)
		return false;

	const apilib_api_table* psrc = apilib->api_tables;

	while (psrc->target_library)
	{
		if (!strcmpi(module, psrc->target_library))
			break;
		psrc++;
	}

	if (!psrc->target_library)
		return false;

	apilib_named_api to_find;
	to_find.name = api_name;

	const apilib_named_api* first = psrc->named_apis;
	const apilib_named_api* last = first + psrc->named_apis_count;

	const apilib_named_api* low = lower_bound(first, last, to_find);

	if (low == last || to_find < *low)
		return false;

	low += id;

	if (low >= last || to_find < *low)
		return false;

	int i;
	for (i = 0 ; i < overridden_module_count ; i++)
		if (!strcmpi(module, overridden_module_names[i]))
			break;

	if (i == overridden_module_count)
		return false;

	if (api_tables_count <= i)
	{
		void* mem = recalloc(api_tables, overridden_module_count * sizeof(ModuleApi));
		if (!mem)
			return false;
		api_tables = (ModuleApi*) mem;
		api_tables_count = overridden_module_count;
	}

	ModuleApi* pdst = api_tables + i;
	NamedApi* dfirst = pdst->named_apis;
	NamedApi* dlast = dfirst + pdst->named_apis_count;

	NamedApi dto_find;
	dto_find.api_name = api_name;

	NamedApi* dlow = lower_bound(dfirst, dlast, dto_find);

	//scenario 1: existing entry
	if (dlow < dlast && !(dto_find < *dlow))
	{
		dlow->address = encode_address(low->addr, apilib);
	}
	//scenario 2: no matching entry
	else
	{
		void* mem = realloc(pdst->named_apis, 
				(pdst->named_apis_count + 1) * sizeof(NamedApi));
		if (!mem)
			return false;

		pdst->named_apis = (NamedApi*) mem;
		pdst->named_apis_count++;

		dlow = (NamedApi*)((DWORD) dlow - (DWORD) dfirst + (DWORD) pdst->named_apis);
		dfirst = pdst->named_apis;
		dlast = dfirst + pdst->named_apis_count;
		copy_backward(dlow, dlast - 1, dlast);
		dlow->api_name = low->name;
		dlow->address = encode_address(low->addr, apilib);
	}

	return true;
}

bool ApiConfiguration::erase(const char* module, unsigned short ordinal)
{
	int i;
	for (i = 0 ; i < overridden_module_count ; i++)
		if (!strcmpi(module, overridden_module_names[i]))
			break;

	if (i == overridden_module_count || i >= api_tables_count)
		return false;

	ModuleApi* pdst = api_tables + i;
	if (pdst->unnamed_apis_count < ordinal)
	{
		return false;
	}

	pdst->unnamed_apis[ordinal - 1] = 0;
	
	return true;
}

bool ApiConfiguration::erase(const char* module, const char* api_name)
{
	int i;
	for (i = 0 ; i < overridden_module_count ; i++)
		if (!strcmpi(module, overridden_module_names[i]))
			break;

	if (i == overridden_module_count || i >= api_tables_count)
		return false;

	ModuleApi* pdst = api_tables + i;
	NamedApi* dfirst = pdst->named_apis;
	NamedApi* dlast = dfirst + pdst->named_apis_count;

	NamedApi dto_find;
	dto_find.api_name = api_name;

	NamedApi* dlow = lower_bound(dfirst, dlast, dto_find);

	if (dlow >= dlast || dto_find < *dlow)
		return false;

	copy(dlow + 1, dlast, dlow);
	pdst->named_apis_count--;
	//should I realloc memory after deleting entry?

	return true;
}

/** Get api address given api table number and api name.
 * @param table_id Valid api table number.
 * @param hint Loader hint.
 * @param api_name Api name.
 * @return Api address or 0 if api address not found.
 */
unsigned long ApiConfiguration::get(unsigned short table_id, 
									unsigned short hint, const char* api_name)
{
	NamedApi* named_apis;
	unsigned short named_apis_count;
	int lo, hi, curr;

	named_apis = api_tables[table_id].named_apis;
	named_apis_count = api_tables[table_id].named_apis_count;

	if (hint < named_apis_count)
	{
		int res = strcmp(api_name, named_apis[hint].api_name);
		if (res > 0) 
		{
			lo = hint + 1;
			hi = named_apis_count - 1;
		}
		else if (res < 0)
		{
			lo = 0;
			hi = hint - 1;
		}
		else
		{
			return named_apis[hint].address;
		}
	}
	else 
	{
		lo = 0;
		hi = named_apis_count - 1;
	}
	curr = (hi + lo) >> 1;

	while (lo <= hi)
	{
		int res = strcmp(api_name, named_apis[curr].api_name);
		if (res > 0)
		{
			lo = curr + 1;
		}
		else if (res < 0)
		{
			hi = curr - 1;
		}
		else
		{
			return named_apis[curr].address;
		}
		curr = (hi + lo) >> 1;
	}
	return 0;
}

/** Get api address given api table number and api ordinal.
 * @param table_id Valid api table number.
 * @param api_ordinal Api ordinal number.
 * @return Api address or 0 if api address not found.
 */
unsigned long ApiConfiguration::get(unsigned short table_id, 
									unsigned short api_ordinal)
{
	unsigned long* unnamed_apis;
	unsigned short unnamed_apis_count;

	unnamed_apis = api_tables[table_id].unnamed_apis;
	unnamed_apis_count = api_tables[table_id].unnamed_apis_count;

	if (api_ordinal > unnamed_apis_count)
		return 0;

	return unnamed_apis[api_ordinal - 1];
}

bool ApiConfiguration::is_initialized()
{
	return initialized;
}

#ifdef _DEBUG

void ApiConfiguration::dump()
{
	printf("ApiConfiguration %s\n", get_name());
	for (int i = 0 ; i < api_tables_count ; i++)
	{
		printf("Covered module #%d: %s\n", i, overridden_module_names[i]);
		printf("Named apis (count = %d)\n", api_tables[i].named_apis_count);
		for (int j = 0 ; j < api_tables[i].named_apis_count ; j++)
			printf("\t%-32s %08x\n", api_tables[i].named_apis[j].api_name, 
					api_tables[i].named_apis[j].address);
		printf("Unnamed apis (count = %d)\n", api_tables[i].unnamed_apis_count);
		for (int j = 0 ; j < api_tables[i].unnamed_apis_count ; j++)
			printf("\t%-5d                            %08x\n", j + 1, api_tables[i].unnamed_apis[j]);
		printf("\n");
	}
	printf("\n");
}

#endif
