/*
 *  KernelEx
 *  Copyright (C) 2008-2011, Xeno86
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
#include "debug.h"
#include "resolver.h"
#include "apiconf.h"
#include "apiconfmgr.h"
#include "internals.h"
#include "../vxd/interface.h"
#include "thunks.h"
#include "SettingsDB.h"
#include "ModInit.h"
#include "apihook.h"

using namespace std;

char system_path[MAX_PATH];
int system_path_len;

static PLONG jtab;
LONG old_jtab[JTAB_SIZE];
static HKEY known_dlls_key;

FLoadTreeNotify_t FLoadTreeNotify;


/** Get API configuration for selected module.
 * @param module Target module.
 * @param cp Value receives configuration information for extended API.
 * @return True to use extended API, false use standard API.
 */
static bool get_config(MODREF* moduleMR, config_params& cp)
{
	IMTE** pmteModTable = *ppmteModTable;
	PDB98* ppdbCur = *pppdbCur;
	volatile MODREF_KEX module(moduleMR);
	DBGASSERT(ppdbCur->pExeMODREF != NULL);
	MODREF_KEX process(ppdbCur->pExeMODREF);

	//shared modules should use standard api
	if (IS_SHARED(pmteModTable[module.mr.mteIndex]->baseAddress))
		return false;

	//if settings are already known, exit immediatelly
	if (module.as.flags & LDR_VALID_FLAG)
		goto __end;

	//we need process settings to know if process.LDR_OVERRIDE_PROC_MOD is set
	if (!(process.as.flags & LDR_VALID_FLAG))
	{
		//try to take settings from database...
		pmteModTable = *ppmteModTable;
		process.as = SettingsDB::instance
			.get_appsetting(pmteModTable[process.mr.mteIndex]->pszFileName);

		//...if settings are not there, take them from parent process...
		if (!(process.as.flags & LDR_VALID_FLAG))
		{
			PDB98* ppdbParent = ppdbCur->ParentPDB;

			//...IF there is parent process and...
			if (ppdbParent && !(ppdbParent->Flags & (fTerminated | fTerminating | 
					  fNearlyTerminating | fDosProcess | fWin16Process)))
			{
				MODREF_KEX parent(ppdbParent->pExeMODREF);

				//...unless parent disallows us to inherit them
				if ((parent.as.flags & LDR_VALID_FLAG) && !(parent.as.flags & LDR_NO_INHERIT))
				{
					process.as = parent.as;
#ifdef _ENABLE_APIHOOK
					//don't inherit hook flag
					process.as.flags &= ~LDR_HOOK_APIS;	
#endif
				}
			}
		}
	}

	//at this point we know whether override is enabled or not
	if ((process.as.flags & LDR_VALID_FLAG))
	{
		//if it is then take process settings
		if (process.as.flags & LDR_OVERRIDE_PROC_MOD)
		{
			module.as = process.as;
			goto __end;
		}
	}
	//if process still doesn't have settings, set some reasonable defaults
	else
	{
		if (apiconfmgr.are_extensions_disabled())
		{
			process.as.flags = LDR_VALID_FLAG | LDR_KEX_DISABLE;
		}
		else
		{
			process.as.conf = apiconfmgr.get_default_configuration();
			process.as.flags = LDR_VALID_FLAG;
		}
	}

	//if module == process then we've already got everything we need
	if (&module.as == &process.as)
		goto __end;

	//lookup module settings in database
	pmteModTable = *ppmteModTable;
	module.as = SettingsDB::instance
		.get_appsetting(pmteModTable[module.mr.mteIndex]->pszFileName);

	if (module.as.flags & LDR_VALID_FLAG)
	{
#ifdef _ENABLE_APIHOOK
		//copy hook flag from process to module
		if (process.as.flags & LDR_HOOK_APIS)
			module.as.flags |= LDR_HOOK_APIS;
#endif
		goto __end;
	}

	//if module has no settings, take them from process
	module.as = process.as;

__end:
	DBGASSERT(module.as.flags & LDR_VALID_FLAG);
	if (module.as.flags & LDR_KEX_DISABLE)
		return false;

	DBGASSERT(module.as.conf != NULL);
	cp.apiconf = module.as.conf;
#ifdef _ENABLE_APIHOOK
	cp.hook_apis = (module.as.flags & LDR_HOOK_APIS) != 0;
#endif
	return true;
}

/** Finds overridden module index for target module.
 * @param target Module from which functions are imported.
 * @return Index or 0xffff if module doesn't have entry in API configuration tables.
 */
static WORD resolve_mod_index(IMTE_KEX* target)
{
	const char* file_name;

	/* Skip non-system modules */
	if (target->cbFileName - target->cbModName - 1 != system_path_len
			|| strncmp(system_path, target->pszFileName, system_path_len))
		return target->mod_index = 0xffff;

	file_name = target->pszModName;

	for (int i = 0 ; i < overridden_module_count ; i++)
		if (strcmp(file_name, overridden_module_names[i]) == 0)
			return target->mod_index = i + 1;

	/* No override API list for target module available. */
	return target->mod_index = 0xffff;
}

/** Resolves address where target function is in non-shared api library.
 *  Checks if api library is loaded for process, if it isn't it is loaded.
 *  Code executes either under process CS (dynamic resolve) or system CS
 *  (static resolve).
 * @param addr Encoded api library ID + offset in this api library
 * @param caller Module that requests api from api library.
 * @param is_static True if implicit (static), false if explicit (GetProcAddress) resolve
 * @return Valid address to function for calling process.
 */
static PROC resolve_nonshared_addr(DWORD addr, MODREF* caller, BOOL is_static)
{
	MODREF* mr;
	WORD idx;
	DWORD img_base;
	int api_lib_num;
	char dllpath[MAX_PATH];
	ApiLibrary* apilib;
	IMTE** pmteModTable = *ppmteModTable;
	ModuleInitializer* mi = ModuleInitializer::get_instance(true);

	api_lib_num = (addr >> 24) - 0xc0;
	DBGASSERT(api_lib_num > 0); //ensure apilib ID isn't STD's
	apilib = apilibmgr.get_apilib_by_index(api_lib_num);
	DBGASSERT(apilib != NULL);
	DBGASSERT(!apilib->is_shared());
	idx = 0xff00 + api_lib_num;

	//first check if api library has already been loaded
	img_base = mi->get_handle_for_index(idx);
	if (img_base != 0)
		return (PROC)(img_base + (addr & 0x00ffffff));

	//if not - load it

	DBGPRINTF(("Loading non-shared apilib: %s req. by: %s [PID=%08x]\n", 
			apilib->apilib_name, pmteModTable[caller->mteIndex]->pszModName,
			GetCurrentProcessId()));

	apilib->get_dll_path(dllpath);

	_EnterSysLevel(krnl32lock);
	mr = MRLoadTree(dllpath);
	if (!mr)
	{
		FreeLibRemove();
		_LeaveSysLevel(krnl32lock);
		return 0;
	}
	mi->add_module(mr);
	_LeaveSysLevel(krnl32lock);

	if (!is_static)
	{
		DBGPRINTF(("Explicit load: initializing tree %s [PID=%08x]\n", 
				apilib->apilib_name, GetCurrentProcessId()));

		if (FLoadTreeNotify(mr, FALSE))
		{
			FreeLibTree(mr);
			return 0;
		}
	}

	pmteModTable = *ppmteModTable;
	IMTE_KEX* imte = (IMTE_KEX*) pmteModTable[mr->mteIndex];

	img_base = imte->pNTHdr->OptionalHeader.ImageBase;

	if (img_base == (DWORD) apilib->mod_handle)
		DBGASSERT(imte->mod_index == apilib->index + 0xff00);

	imte->mod_index = idx;

	return (PROC)(img_base + (addr & 0x00ffffff));
}

/** Performs resolver actions on new process attach.
 * @return TRUE on success, FALSE otherwise.
 */
BOOL resolver_process_attach()
{
	//reference all shared api libraries
	ApiLibrary* lib;
	int i = 1;
	while ((lib = apilibmgr.get_apilib_by_index(i++)) != NULL)
	{ 
		if (lib->is_shared())
		{
			char dllpath[MAX_PATH];
			lib->get_dll_path(dllpath);
			if (!LoadLibrary(dllpath))
				return FALSE;
		}
	}

	return TRUE;
}

/** Original ExportFromOrdinal function. @see ExportFromOrdinal */
static PROC WINAPI OriExportFromOrdinal(IMAGE_NT_HEADERS* PEh, WORD ordinal)
{
	DWORD img_base;
	IMAGE_EXPORT_DIRECTORY* Exports;
	DWORD export_table_size;
	DWORD export_no;
	DWORD export_cnt;
	DWORD* function_table;
	DWORD addr;

	export_table_size = PEh->OptionalHeader
			.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;

	if (!export_table_size)
		return NULL;
	
	img_base = PEh->OptionalHeader.ImageBase;
	Exports = (IMAGE_EXPORT_DIRECTORY *)(img_base + PEh->OptionalHeader
			.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);

	export_no = ordinal - Exports->Base;
	export_cnt = Exports->NumberOfFunctions;

	if (!export_cnt || export_no > export_cnt)
		return NULL;

	function_table = (DWORD*)(Exports->AddressOfFunctions + img_base);
	addr = function_table[export_no];

	if (!addr)
		return NULL;

	addr += img_base;

	//handle export forwarding case
	if (addr >= (DWORD)Exports && addr < (DWORD)Exports + export_table_size)
	{
		char module_name[MAX_PATH];
		char* p;
		char c;
		HMODULE hmod;

		p = module_name;
		do
		{
			c = *(char*) addr;
			addr++;
			if (c == '.') break;
			*p = c;
			p++;
		} 
		while (c);
		*p = '\0';

		hmod = GetModuleHandle(module_name); //should be IGetModuleHandle
		if (!hmod) 
			return NULL;

		return GetProcAddress(hmod, (char*) addr); //should be IGetProcAddress
	}

	return (PROC) addr;
}

/** Original ExportFromName function. @see ExportFromName */
static PROC WINAPI OriExportFromName(IMAGE_NT_HEADERS* PEh, WORD hint, LPCSTR name)
{
	DWORD img_base;
	IMAGE_EXPORT_DIRECTORY* Exports;
	DWORD* names;
	WORD* ordinals;
	int lo;
	int hi;
	int curr;
	int res;

	if (!PEh->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size)
		return NULL;

	img_base = PEh->OptionalHeader.ImageBase;
	Exports = (IMAGE_EXPORT_DIRECTORY *)(img_base + PEh->OptionalHeader
			.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);

	hi = Exports->NumberOfNames;

	if (!hi) 
		return NULL;

	ordinals = (WORD*)(Exports->AddressOfNameOrdinals + img_base);
	names = (DWORD*)(img_base + Exports->AddressOfNames);
	
	if (hint < hi)
	{
		res = strcmp(name, (char*)(names[hint] + img_base));
		if (!res)
		{
			return OriExportFromOrdinal(PEh, (WORD)(Exports->Base + ordinals[hint]));
		}
		else if (res > 0) 
		{
			lo = hint + 1;
			hi--;
		}
		else
		{
			lo = 0;
			hi = hint - 1;
		}
		curr = (hi + lo) >> 1;
	}
	else 
	{
		lo = 0;
		hi--;
		curr = hi >> 1;
	}	

	while (lo <= hi)
	{
		res = strcmp(name, (char*)(names[curr] + img_base));
		if (!res)
		{
			return OriExportFromOrdinal(PEh, (WORD)(Exports->Base + ordinals[curr]));
		}
		else if (res > 0)
		{
			lo = curr + 1;
		}
		else
		{
			hi = curr - 1;
		}
		curr = (hi + lo) >> 1;
	}
	return 0;
}

/** Encode function pointer + API library information into single dword.
 * @see decode_address
 * @param addr Absolute address.
 * @param apilib Pointer to API library.
 * @return Encoded pointer.
 */
DWORD encode_address(DWORD addr, const ApiLibrary* apilib)
{
	//note: rules have to be the same as in decode_address
	int index = apilib->index;

	//ensure that we haven't run out of indexes
	DBGASSERT(index + 0xc0 < 0xff);

	//STD apilib
	if (index == 0)
	{
		//normal address (shared or nonshared library) 
		//or ordinal number for export forwarding
		if (addr < 0xc0000000 || addr >= 0xffff0000)
			return addr;
		
		//extremely rare scenario: driver hijacked apis so the address is now
		//above 0xc0000000 and we use these for encoded apilib addresses
		//so we have to create a stub in shared arena with proper address
		return (DWORD) new redir_stub(addr, false);
	}

	//non-shared apilib
	if (!apilib->is_shared())
	{
		//max non-shared apilib size 16MB
		DBGASSERT(addr - (DWORD) apilib->mod_handle < 0x01000000);

		return ((0xc0 + index) << 24) + addr - (DWORD) apilib->mod_handle;
	}
	//shared apilib
	return addr;
}

/** Decodes pointers created with encode_address.
 * @see encode_address
 * @param p Encoded pointer.
 * @param target_NThdr Target module NT header.
 * @param caller Calling module ref.
 * @param is_static True if implicit resolve, false if explicit.
 * @return Decoded function pointer.
 */
inline PROC decode_address(DWORD p, IMAGE_NT_HEADERS* target_NThdr, MODREF* caller, BOOL is_static)
{
	//note: rules have to be the same as in encode_address
	//zero address
	if (!p)
		return (PROC) p;
	//export forwarding - ordinal number
	if ((p & 0xffff0000) == 0xffff0000)
		return OriExportFromOrdinal(target_NThdr, LOWORD(p));
	//non-shared system library
	if (!IS_SHARED(p))
		return (PROC)(p + target_NThdr->OptionalHeader.ImageBase);
	//non-shared api library
	if (p >= 0xc0000000)
		return resolve_nonshared_addr(p, caller, is_static);
	//shared system or api library
	return (PROC) p;
}

/** Performs function resolve by ordinal number.
 * @param target Target module, module from which we want function.
 * @param caller Calling module, module which wants function.
 * @param is_static True if static resolve, false if dynamic.
 * @param ordinal Function ordinal number.
 * @return Function pointer.
 */
PROC WINAPI ExportFromOrdinal(IMTE_KEX* target, MODREF* caller, BOOL is_static, WORD ordinal)
{
	PROC ret;
	
	//if caller is unknown - assume it is process's exe
	if (!caller)
		caller = (*pppdbCur)->pExeMODREF;
	
	config_params cp;
	if (get_config(caller, cp))
	{
		WORD mod_index = target->mod_index;

		if (!mod_index)
			mod_index = resolve_mod_index(target);

		DBGASSERT(mod_index);
		mod_index--;

		if (!cp.apiconf->is_table_empty(mod_index))
			ret = decode_address(cp.apiconf->get(mod_index, ordinal), 
					target->pNTHdr, caller, is_static);
		else 
			ret = OriExportFromOrdinal(target->pNTHdr, ordinal);
#ifdef _ENABLE_APIHOOK
		if (cp.hook_apis)
		{
			IMTE* icaller = (*ppmteModTable)[caller->mteIndex];
			if (DWORD(ret) < target->pNTHdr->OptionalHeader.ImageBase 
					+ target->pNTHdr->OptionalHeader.BaseOfData)
			{
				ret = apihook::hook(is_static, icaller->pszFileName,
						target->pszFileName, (LPSTR) ordinal, ret);
			}
		}
#endif
	}
	else 
		ret = OriExportFromOrdinal(target->pNTHdr, ordinal);

	if (!ret && is_static)
	{
		DBGPRINTF(("%s: unresolved export %s:%d\n", 
				((*ppmteModTable)[caller->mteIndex])->pszModName,
				target->pszModName, ordinal));
	}

	return ret;
}

/** Performs function resolve by name.
 * @param target Target module, module from which we want function.
 * @param caller Calling module, module which wants function.
 * @param is_static True if static resolve, false if dynamic.
 * @param hint Hint number, tells resolver where to start search.
 * @param name Function name.
 * @return Function pointer.
 */
PROC WINAPI ExportFromName(IMTE_KEX* target, MODREF* caller, BOOL is_static, WORD hint, LPCSTR name)
{
	PROC ret;

	//if caller is unknown - assume it is process's exe
	if (!caller)
		caller = (*pppdbCur)->pExeMODREF;

	config_params cp;
	if (get_config(caller, cp))
	{
		WORD mod_index = target->mod_index;

		if (!mod_index)
			mod_index = resolve_mod_index(target);

		DBGASSERT(mod_index);
		mod_index--;

		if (!cp.apiconf->is_table_empty(mod_index))
			ret = decode_address(cp.apiconf->get(mod_index, hint, name), 
					target->pNTHdr, caller, is_static);
		else 
			ret = OriExportFromName(target->pNTHdr, hint, name);
#ifdef _ENABLE_APIHOOK
		if (cp.hook_apis)
		{
			IMTE* icaller = (*ppmteModTable)[caller->mteIndex];
			if (DWORD(ret) < target->pNTHdr->OptionalHeader.ImageBase 
					+ target->pNTHdr->OptionalHeader.BaseOfData)
			{
				ret = apihook::hook(is_static, icaller->pszFileName,
						target->pszFileName, name, ret);
			}
		}
#endif
	}
	else 
		ret = OriExportFromName(target->pNTHdr, hint, name);

	if (!ret && is_static)
	{
		DBGPRINTF(("%s: unresolved export %s:%s\n", 
				((*ppmteModTable)[caller->mteIndex])->pszModName,
				target->pszModName, name));
	}

	return ret;
}

/** Determines whether process has API extensions enabled. */
bool are_extensions_enabled()
{
	config_params cp;
	MODREF* exe = (*pppdbCur)->pExeMODREF;
	DBGASSERT(exe != NULL);
	return get_config(exe, cp);
}

/** Determines whether module has API extensions enabled.
 *  Use this function when there is no access to MODREF.
 * @param path Full path of the module (uppercase).
 */
bool are_extensions_enabled_module(const char* path)
{
	//find entry for given module...
	appsetting as = SettingsDB::instance.get_appsetting(path);

	if (!(as.flags & LDR_VALID_FLAG))
	{
		//...no entry? try process exe settings...
		PDB98* ppdbCur = *pppdbCur;
		MODREF* exe = ppdbCur->pExeMODREF;

		if (exe != NULL)
		{
			config_params cp;
	
			as.flags = LDR_VALID_FLAG;
			if (!get_config(exe, cp))
				as.flags |= LDR_KEX_DISABLE;
		}
		else
		{
			//...so there is NO PROCESS exe yet? try with parent process exe...
			PDB98* ppdbParent = ppdbCur->ParentPDB;

			if (ppdbParent && !(ppdbParent->Flags & (fTerminated | fTerminating | 
					fNearlyTerminating | fDosProcess | fWin16Process)))
			{
				MODREF_KEX parent(ppdbParent->pExeMODREF);

				//...unless parent disallows us to inherit
				if ((parent.as.flags & LDR_VALID_FLAG) && !(parent.as.flags & LDR_NO_INHERIT))
					as = parent.as;
			}
		}
	}

	//...so everything else failed eh? - take defaults then...
	if (!(as.flags & LDR_VALID_FLAG))
	{
		as.flags = LDR_VALID_FLAG;
		if (apiconfmgr.are_extensions_disabled())
			as.flags |= LDR_KEX_DISABLE;
	}

	if (as.flags & LDR_KEX_DISABLE)
		return false;
	return true;
}

typedef BOOL (__stdcall *IsKnownDLL_t)(char*, const char*);

static BOOL WINAPI IsKnownKexDLL(char* name, const char* ext)
{
	LONG res;
	DWORD type;
	char new_path[MAX_PATH];
	DWORD size = sizeof(new_path);

	if (ext && strcmp(ext, "DLL") != 0)
		return FALSE;
	
	if ((*pppdbCur)->pExeMODREF == NULL)
		return FALSE;
	
	if (are_extensions_enabled())
	{
		int len = strlen(name);

		//workaround windows bug
		int pos = len - 4;
		if (pos > 0 && name[pos] == '.')
		{
			name[pos] = '\0';
			len = pos;
		}

		char* file = name;

		//find where directory part ends
		while (len > 0)
		{
			len--;
			if (name[len] == '\\')
			{
				file = name + len + 1;
				break;
			}
		}

		if (!len || (len == system_path_len && !strncmp(name, system_path, len)))
			res = RegQueryValueEx(known_dlls_key, file, NULL, &type, (BYTE*) new_path, &size);
		else
			res = ERROR_INVALID_FUNCTION;
	}
	else
		res = ERROR_INVALID_FUNCTION;
	
	if (res == ERROR_SUCCESS && type == REG_SZ)
	{
		memcpy(name, (const char*) kernelex_dir, kernelex_dir.length());
		memcpy(name + kernelex_dir.length(), new_path, size);
		return TRUE;
	}
	else
	{
		IsKnownDLL_t IsKnownDLL = (IsKnownDLL_t) old_jtab[JTAB_KNO_DLL];
		return IsKnownDLL(name, NULL);
	}
}

static BOOL WINAPI KexLoadTreeNotify(MODREF* mr, BOOL is_static)
{
	ModuleInitializer* mi = ModuleInitializer::get_instance(false);
	if (mi && mi->has_initialize() && !mi->initialize_modules())
		return TRUE;

	return FLoadTreeNotify(mr, is_static);
}

typedef BOOL (WINAPI * GetOrdinal_t)(DWORD, DWORD, DWORD, DWORD*, WORD*, DWORD);

static BOOL WINAPI KexResourceCheck(DWORD un0, DWORD un1, DWORD un2, DWORD* pNameOrId, WORD* pResult, DWORD un3)
{
	DWORD NameOrId = *pNameOrId; //parameter from IMAGE_RESOURCE_DIRECTORY_ENTRY

	//not a named resource and index > 32767 allowed by 9x ?
	if (!(NameOrId & 0x80000000) && NameOrId >= 0x8000 && NameOrId < 0x10000)
	{
		//we need to check if module has extensions enabled
		if (mod_ext_ena)
		{
			NameOrId |= 0x8000; //??
			*pResult = NameOrId;
			return TRUE;
		}
	}

	//fall back
	GetOrdinal_t GetOrdinal = (GetOrdinal_t) old_jtab[JTAB_RES_CHK];
	return GetOrdinal(un0, un1, un2, pNameOrId, pResult, un3);
}

/** Retrieves address of kernel32 function exported by ordinal.
 * @param [in] ord function ordinal number
 * @return function address or NULL if not found
 */
PROC WINAPI GetK32OrdinalAddress(WORD wOrd)
{
	IMAGE_DOS_HEADER* dosh = (IMAGE_DOS_HEADER*) h_kernel32;
	IMAGE_NT_HEADERS* nth = (IMAGE_NT_HEADERS*)((LONG)dosh + dosh->e_lfanew);
	return OriExportFromOrdinal(nth, wOrd);
}

/** GetProcAddress variant used to bypass CORE's resolver hook 
 *  (get real procedure address not overridden one).
 * @param [in] hModule module handle
 * @param [in] lpProcName procedure name or ordinal number (high word zeroed)
 * @return function address or NULL if not found
 */
PROC WINAPI iGetProcAddress(HMODULE hModule, LPCSTR lpProcName)
{
	DBGASSERT(MRFromHLib != NULL);
	DBGASSERT(ppmteModTable != NULL);

	MODREF* mr = MRFromHLib(hModule);
	if (mr == NULL)
		return NULL;
	IMTE* imte = (*ppmteModTable)[mr->mteIndex];
	IMAGE_NT_HEADERS* nt_hdr = imte->pNTHdr;

	if ((DWORD)lpProcName < 0x10000) 
		return OriExportFromOrdinal(nt_hdr, LOWORD(lpProcName));
	return OriExportFromName(nt_hdr, 0, lpProcName);
}

static void reset_imtes()
{
	DBGPRINTF(("Reseting IMTEs\n"));
	_EnterSysLevel(krnl32lock);

	WORD imteMax = *pimteMax;
	IMTE** pmteModTable = *ppmteModTable;
	for (WORD i = 0 ; i < imteMax ; i++)
	{
		IMTE_KEX* imte = (IMTE_KEX*) pmteModTable[i];
		if (imte)
		{
			imte->mod_index = 0;
			for (MODREF* mr = imte->pMR ; mr != NULL ; mr = mr->pNextMteMR)
			{
				MODREF_KEX kmr(mr);
				kmr.as.conf = NULL;
				kmr.as.flags = 0;
			}
		}
	}

	_LeaveSysLevel(krnl32lock);
}

#ifdef _DEBUG

void dump_imtes(void)
{
	WORD imteMax = *pimteMax;
	IMTE** pmteModTable = *ppmteModTable;
	int total = 0;
	
	printf("Dumping IMTEs...\n");
	for (WORD i = 0 ; i < imteMax ; i++)
	{
		IMTE_KEX* imte = (IMTE_KEX*) pmteModTable[i];
		if (imte)
		{
			printf("%s\n", imte->pszFileName);

			for (MODREF* mr = imte->pMR ; mr != NULL ; mr = mr->pNextMteMR)
			{
				MODREF_KEX kmr(mr);
				printf("\t%02x %-7s %-12s\n", 
					kmr.as.flags,
					kmr.as.conf ? kmr.as.conf->get_name() : "none",
					pmteModTable[mr->ppdb->pExeMODREF->mteIndex]->pszModName);
			}

			total++;
		}
	}
	printf("\nEnd dump total %d IMTEs\n\n", total);
}

#endif

int resolver_init()
{
	DBGPRINTF(("resolver_init()\n"));

	ioctl_connect_params params;
	if (!VKernelEx_ioctl(IOCTL_CONNECT, &params, sizeof(params)))
	{
		DBGPRINTF(("IOCTL_Connect failed!\n"));
		ShowError(IDS_NODRIVER);
		return 0;
	}

	KernelEx_dataseg* dseg = params.stub_ptr;

	if (!params.status)
	{
		DBGPRINTF(("VKRNLEX failed to initialize!\n"));
		ShowError(IDS_DRIVERINITFAIL);
		return 0;
	}

	if (!dseg)
	{
		DBGPRINTF(("Stub not found\n"));
		ShowError(IDS_DRIVERINITFAIL);
		return 0;
	}
	else
		DBGPRINTF(("Stub found @ 0x%08x\n", dseg));

	if (dseg->version != KEX_STUB_VER)
	{
		DBGPRINTF(("Wrong stub version, expected: %d, got: %d\n", 
				KEX_STUB_VER, dseg->version));
		ShowError(IDS_STUBMISMATCH, KEX_STUB_VER, dseg->version);
		return 0;
	}

	jtab = (PLONG) dseg->jtab;
	FLoadTreeNotify = (FLoadTreeNotify_t) jtab[JTAB_FLD_TRN];

	system_path_len = GetSystemDirectory(system_path, sizeof(system_path));
	RegOpenKey(HKEY_LOCAL_MACHINE, "Software\\KernelEx\\KnownDLLs", &known_dlls_key);

	return 1;
}

void resolver_uninit()
{
	DBGPRINTF(("resolver_uninit()\n"));
	RegCloseKey(known_dlls_key);
	SettingsDB::instance.clear();
	reset_imtes();
}

void resolver_hook()
{
	DBGPRINTF(("resolver_hook()\n"));
	old_jtab[JTAB_EFO_DYN] = InterlockedExchange(jtab + JTAB_EFO_DYN, (LONG) ExportFromOrdinalDynamic_thunk);
	old_jtab[JTAB_EFO_STA] = InterlockedExchange(jtab + JTAB_EFO_STA, (LONG) ExportFromOrdinalStatic_thunk);
	old_jtab[JTAB_EFN_DYN] = InterlockedExchange(jtab + JTAB_EFN_DYN, (LONG) ExportFromNameDynamic_thunk);
	old_jtab[JTAB_EFN_STA] = InterlockedExchange(jtab + JTAB_EFN_STA, (LONG) ExportFromNameStatic_thunk);
	old_jtab[JTAB_KNO_DLL] = InterlockedExchange(jtab + JTAB_KNO_DLL, (LONG) IsKnownKexDLL);
	old_jtab[JTAB_FLD_TRN] = InterlockedExchange(jtab + JTAB_FLD_TRN, (LONG) KexLoadTreeNotify);
	old_jtab[JTAB_SYS_CHK] = InterlockedExchange(jtab + JTAB_SYS_CHK, (LONG) SubSysCheck);
	old_jtab[JTAB_RES_CHK] = InterlockedExchange(jtab + JTAB_RES_CHK, (LONG) KexResourceCheck);
}

void resolver_unhook()
{
	DBGPRINTF(("resolver_unhook()\n"));
	for (int i = 0 ; i < JTAB_SIZE ; i++)
		InterlockedExchange(jtab + i, old_jtab[i]);
}
