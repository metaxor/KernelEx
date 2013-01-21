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
#include "apilib.h"
#include <algorithm>
#include "resolver.h"
#include "internals.h"
#include "debug.h"
#include "is_sorted.hpp"
#include "pemanip.h"

using namespace std;

#define ALLOC_CAPACITY 10

const char** overridden_module_names;
int          overridden_module_count;

ApiLibraryManager apilibmgr;

void ApiLibrary::get_dll_path(char* out)
{
	strcpy(out, kernelex_dir);
	strcat(out, apilib_name);
	strcat(out, ".DLL");
}

ApiLibraryManager::ApiLibraryManager()
{
	apilib_ptrs = NULL;
	apilib_cnt = 0;
	overridden_module_names = NULL;
	overridden_module_count = 0;

	initialized = initialize();
}

ApiLibraryManager::~ApiLibraryManager()
{
}

bool ApiLibraryManager::initialize()
{
	ApiLibrary* std_apilib = NULL;

	apilib_cnt = 0;
	apilib_ptrs = (ApiLibrary**) malloc(ALLOC_CAPACITY * sizeof(ApiLibrary*));

	if (!apilib_ptrs)
		goto __error;

	std_apilib = (ApiLibrary*) malloc(sizeof(ApiLibrary) + strlen("STD"));
		
	if (!std_apilib)
		goto __error;

	strcpy(std_apilib->apilib_name, "STD");
	std_apilib->mod_handle = NULL;
	std_apilib->api_tables = NULL;
	std_apilib->index = 0;

	apilib_ptrs[apilib_cnt++] = std_apilib;

	return true;

__error:
	if (std_apilib)
		free(std_apilib->api_tables);
	free(std_apilib);
	free(apilib_ptrs);
	return false;
}

bool ApiLibraryManager::load_apilib(const char* apilib_name)
{
	IMTE** pmteModTable;
	MODREF* mr;

	if (!initialized)
	{
		DBGPRINTF(("Failed to initialize api library manager\n"));
		return false;
	}

	if (apilib_cnt >= 0xff)
	{
		DBGPRINTF(("Too many api libraries loaded\n"));
		return false;
	}

	//STD is a special api library name
	if (!strcmp(apilib_name, "STD"))
		return true;
	
	//check if library wasn't loaded in this instance
	for (int i = 0 ; i < apilib_cnt ; i++)
		if (!strcmp(apilib_ptrs[i]->apilib_name, apilib_name))
			return true;

	ApiLibrary* apilib = NULL;

	DBGPRINTF(("Loading api library: %s... \n", apilib_name));

	char dllpath[MAX_PATH];
	int size = sizeof(ApiLibrary) + strlen(apilib_name);

	apilib = (ApiLibrary*) malloc(size);

	if (!apilib)
		goto __error;

	strcpy(apilib->apilib_name, apilib_name);

	apilib->get_dll_path(dllpath);
	apilib->mod_handle = LoadLibrary(dllpath);

	if (!apilib->mod_handle)
	{
		DBGPRINTF(("Failed to load api library\n"));
		goto __error;
	}

	fgat_t get_api_table;
	get_api_table = (fgat_t) GetProcAddress(
			apilib->mod_handle, "get_api_table");

	if (!get_api_table)
	{
		DBGPRINTF(("Failed to get api library entry point\n"));
		goto __error;
	}

	const apilib_api_table* file_api_tables;
	file_api_tables = get_api_table();

	if (!file_api_tables)
	{
		DBGPRINTF(("Failed to get api tables\n"));
		goto __error;
	}

	apilib->api_tables = make_shared_api_tables(file_api_tables);

	if (!apilib->api_tables)
	{
		DBGPRINTF(("Failed to create shared api tables\n"));
		goto __error;
	}

	DBGPRINTF(("loaded @ 0x%08x... \n", (DWORD) apilib->mod_handle));

	//allocate space for new ApiLibraries
	if (apilib_cnt % ALLOC_CAPACITY == 0)
	{
		void* new_block = realloc(apilib_ptrs,
				(apilib_cnt + ALLOC_CAPACITY) * sizeof(ApiLibrary*));

		if (!new_block)
			goto __error;

		apilib_ptrs = (ApiLibrary**) new_block;
	}

//	DBGPRINTF(("Listing modules overridden by api library:\n"));
	for (apilib_api_table* p = apilib->api_tables ; p->target_library ; p++)
	{
//		DBGPRINTF(("  * %s\n", p->target_library));
		if (!add_overridden_module(p->target_library))
		{
			DBGPRINTF(("Failed to add overridden module %s. Skipping\n", p->target_library));
		}
	}

	//set index value which is used by encode_address()
	apilib->index = apilib_cnt;

	//set mod_index for newly loaded api libraries
	mr = MRFromHLib(apilib->mod_handle);
	DBGASSERT(mr);
	pmteModTable = *ppmteModTable;
	((IMTE_KEX*) pmteModTable[mr->mteIndex])->mod_index = 0xff00 + apilib->index;

	//add to table of new ApiLibraries
	apilib_ptrs[apilib_cnt++] = apilib;

	DBGPRINTF(("ok\n"));
	return true;

__error:
	if (apilib)
	{
		if (apilib->mod_handle)
			FreeLibrary(apilib->mod_handle);
		if (apilib->api_tables)
			free(apilib->api_tables);
		free(apilib);
	}
	return false;
}

ApiLibrary* ApiLibraryManager::get_apilib(const char* apilib_name)
{
	for (int i = 0 ; i < apilib_cnt ; i++)
		if (!strcmp(apilib_ptrs[i]->apilib_name, apilib_name))
			return apilib_ptrs[i];
	DBGPRINTF(("Api library %s not found\n", apilib_name));
	return NULL;
}

ApiLibrary* ApiLibraryManager::get_apilib_by_index(int index)
{
	if (index < 0 || index >= apilib_cnt)
		return NULL;

	DBGASSERT(apilib_ptrs[index]);
	DBGASSERT(apilib_ptrs[index]->index == index);
	return apilib_ptrs[index];
}

bool ApiLibraryManager::are_api_tables_sorted(const apilib_api_table* tab)
{
	for (const apilib_api_table* p = tab ; p->target_library ; p++)
	{
		if (!::is_sorted(p->named_apis, p->named_apis + p->named_apis_count)) 
			return false;

		if (!::is_sorted(p->ordinal_apis, p->ordinal_apis + p->ordinal_apis_count)) 
			return false;
	}
	return true;
}

apilib_api_table* ApiLibraryManager::make_shared_api_tables(const apilib_api_table* in)
{
	bool copy_strings = IS_SHARED(in) ? false : true;
	bool sorted = are_api_tables_sorted(in);
	
	if (!sorted)
	{
		DBGPRINTF(("Error: api tables are not sorted\n"));
		return NULL;
	}
	
	if (!copy_strings && sorted)
	{
		//nothing bad will happen if I call HeapFree 
		//on non-HeapAlloc'ed mem later right?
		return const_cast<apilib_api_table*>(in);
	}

	int size = required_api_table_space(in, copy_strings);
	apilib_api_table* out = (apilib_api_table*) malloc(size);

	if (!out) 
		return NULL;

	int i;
	char* pstr = (char*) out + size;

	//copy apilib_api_tables
	const apilib_api_table* pin;
	apilib_api_table* pout;

	for (pin = in, pout = out ; pin->target_library ; pin++, pout++)
	{
		if (copy_strings)
		{
			pstr -= strlen(pin->target_library) + 1;
			strcpy(pstr, pin->target_library);
			pout->target_library = pstr;
		}
		else
		{
			pout->target_library = pin->target_library;
		}
	}
	*pout++ = *pin++; //terminating NULL entry

	//copy apilib_named_apis
	const apilib_named_api* pina;
	apilib_named_api* pona;
	pona = (apilib_named_api*) pout;

	for (pin = in, pout = out ; pin->target_library ; pin++, pout++)
	{
		pout->named_apis = pona;
		pout->named_apis_count = pin->named_apis_count;

		for (i = 0, pina = pin->named_apis ; i < pin->named_apis_count ; i++, pina++, pona++)
		{
			if (copy_strings)
			{
				pstr -= strlen(pina->name) + 1;
				strcpy(pstr, pina->name);
				pona->name = pstr;
			}
			else
			{
				pona->name = pina->name;
			}
			pona->addr = pina->addr;
		}
	}

	//copy apilib_unnamed_apis
	const apilib_unnamed_api* piua;
	apilib_unnamed_api* poua;
	poua = (apilib_unnamed_api*) pona;

	for (pin = in, pout = out ; pin->target_library ; pin++, pout++)
	{
		pout->ordinal_apis = poua;
		pout->ordinal_apis_count = pin->ordinal_apis_count;

		for (i = 0, piua = pin->ordinal_apis ; i < pin->ordinal_apis_count ; i++, piua++, poua++)
		{
			*poua = *piua;
		}
	}

	//finish line
	return out;
}

int ApiLibraryManager::required_api_table_space(const apilib_api_table* tab, bool copy_strings)
{
	int space = 0;

	for (const apilib_api_table* p = tab ; p->target_library ; p++)
	{
		space += sizeof(apilib_api_table);
		space += p->named_apis_count * sizeof(apilib_named_api);
		space += p->ordinal_apis_count * sizeof(apilib_unnamed_api);

		if (copy_strings)
		{
			space += strlen(p->target_library) + 1;
			for (int i = 0 ; i < p->named_apis_count ; i++)
				space += strlen(p->named_apis[i].name) + 1;
		}
	}
	space += sizeof(apilib_api_table);
	
	return space;
}

bool ApiLibraryManager::add_overridden_module(const char* mod)
{
	apilib_api_table*& std_api_table =  apilib_ptrs[0]->api_tables;

	//ensure that module isn't already on list
	for (int i = 0 ; i < overridden_module_count ; i++)
	{
		if (!strcmpi(overridden_module_names[i], mod))
			return true;
	}

	apilib_api_table table;
	if (!parse_system_dll(mod, &table))
	{
		DBGPRINTF(("Failed to parse system DLL: %s\n", mod));
		return false;
	}

	//allocate space for new overridden modules
	if (overridden_module_count % ALLOC_CAPACITY == 0)
	{
		void* new_block = realloc(overridden_module_names,
				(overridden_module_count + ALLOC_CAPACITY) * sizeof(char*));
		
		if (!new_block)
			return false;

		overridden_module_names = (const char**) new_block;

		new_block = realloc(std_api_table,
				(overridden_module_count + 1 + ALLOC_CAPACITY) * sizeof(apilib_api_table));
			//+ 1 because api_tables are NULL terminated

		if (!new_block)
			return false;

		std_api_table = (apilib_api_table*) new_block;
	}

	std_api_table[overridden_module_count] = table;

	//add to table of overridden modules
	overridden_module_names[overridden_module_count] 
			= std_api_table[overridden_module_count].target_library;
	overridden_module_count++;

	memset(&std_api_table[overridden_module_count], 0, sizeof(apilib_api_table));

	return true;
}

bool ApiLibraryManager::parse_system_dll(const char* dll_name, apilib_api_table* api_tab)
{
	PEmanip pemod;
	HMODULE mem_dll = GetModuleHandle(dll_name);
	
	if (mem_dll)
	{
		if (!pemod.OpenMemory(mem_dll))
			return false;
	}
	else
	{
		char path[MAX_PATH];
		GetSystemDirectory(path, sizeof(path));
		strcat(path, "\\");
		strcat(path, dll_name);

		if (!pemod.OpenFile(path))
			return false;
	}

	IMAGE_NT_HEADERS* PEh = pemod.GetPEHeader();
	DWORD ExportsRva = PEh->OptionalHeader
			.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress; 

	IMAGE_EXPORT_DIRECTORY* Exports = (IMAGE_EXPORT_DIRECTORY*) 
			pemod.RvaToPointer(ExportsRva);

	DWORD ExportsSize = PEh->OptionalHeader
			.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;

	if (!Exports || !ExportsSize)
		return false;

	DWORD* Names = (DWORD*) pemod.RvaToPointer(Exports->AddressOfNames);
	WORD* OrdinalTable = (WORD*) pemod.RvaToPointer(Exports->AddressOfNameOrdinals);
	DWORD* FunctionTable = (DWORD*) pemod.RvaToPointer(Exports->AddressOfFunctions);
	
	//offset for shared libraries
	unsigned long offset = 0;
	
	if (!mem_dll)
	{
		//check if we deal with shared library but it is not loaded yet
		if (IS_SHARED(PEh->OptionalHeader.ImageBase))
			offset = (unsigned long) LoadLibrary(dll_name);
	}
	else
	{
		if (IS_SHARED(mem_dll))
			offset = (unsigned long) mem_dll;
	}

	//calculate required space
	int space = 0;

	space += sizeof(apilib_unnamed_api) * (Exports->NumberOfFunctions + 1);
	space += sizeof(apilib_named_api) * (Exports->NumberOfNames + 1);

	DWORD* pname = Names;
	for (DWORD i = 0 ; i < Exports->NumberOfNames ; i++)
		space += strlen((char*) pemod.RvaToPointer(*pname++)) + 1;

	void* mem = malloc(space);
	
	if (!mem)
		return false;

	char* pstr = (char*) mem + space;

	// read exports by name
	apilib_named_api* pna = (apilib_named_api*) mem;
	api_tab->named_apis = pna;
	api_tab->named_apis_count = Exports->NumberOfNames;
	pname = Names;
	for (DWORD i = 0 ; i < Exports->NumberOfNames ; i++, pna++)
	{
		WORD ord = OrdinalTable[i];
		DWORD addr = FunctionTable[ord];
		char* api_name = (char*) pemod.RvaToPointer(*pname++);

		pstr -= strlen(api_name) + 1;
		strcpy(pstr, api_name);
		pna->name = pstr;

		//export forwarding case
		if (addr >= ExportsRva && addr < ExportsRva + ExportsSize)
			addr = 0xffff0000 | ord + Exports->Base;
		else
			addr += offset;

		pna->addr = addr;
	}
	
	// read exports by ordinals
	apilib_unnamed_api* pua = (apilib_unnamed_api*) pna;
	api_tab->ordinal_apis = pua;
	api_tab->ordinal_apis_count = Exports->NumberOfFunctions;
	DBGASSERT(Exports->NumberOfFunctions < 0x10000);

	for (DWORD i = 0 ; i < Exports->NumberOfFunctions ; i++, pua++)
	{
		DWORD addr = FunctionTable[i];

		pua->ord = (WORD)(Exports->Base + i);

		//export forwarding case
		if (addr >= ExportsRva && addr < ExportsRva + ExportsSize)
			addr = 0xffff0000 | i + Exports->Base;
		else
			addr += offset;

		pua->addr = addr;
	}

	char* new_mod = (char*) malloc(strlen(dll_name) + 1);
	if (!new_mod)
	{
		free(mem);
		return false;
	}

	strcpy(new_mod, dll_name);
	strupr(new_mod);
	api_tab->target_library = new_mod;

	return true;
}

