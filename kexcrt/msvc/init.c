#include <stdlib.h>
#include "init.h"

#define MAX_ATEXIT        32

typedef void (__cdecl *_PVFV)(void);

static _PVFV _atexitlist[MAX_ATEXIT];
static int _atexitlist_cnt;

// C initializers
#pragma data_seg(".CRT$XIA")
_PVFV __xi_a[] = { NULL };
#pragma data_seg(".CRT$XIZ")
_PVFV __xi_z[] = { NULL };

// C++ initializers
#pragma data_seg(".CRT$XCA")
_PVFV __xc_a[] = { NULL };
#pragma data_seg(".CRT$XCZ")
_PVFV __xc_z[] = { NULL };

// C pre-terminators
#pragma data_seg(".CRT$XPA")
_PVFV __xp_a[] = { NULL };
#pragma data_seg(".CRT$XPZ")
_PVFV __xp_z[] = { NULL };

// C terminators
#pragma data_seg(".CRT$XTA")
_PVFV __xt_a[] = { NULL };
#pragma data_seg(".CRT$XTZ")
_PVFV __xt_z[] = { NULL };

#pragma data_seg()  /* reset */

#pragma comment(linker, "/merge:.CRT=.data")

static void _initterm(_PVFV * pfbegin, _PVFV * pfend)
{
    while (pfbegin < pfend)
    {
        if (*pfbegin != NULL)
            (**pfbegin)();
        ++pfbegin;
    }
}

void __init(void)
{
	// do initializations
    _initterm( __xi_a, __xi_z );
    // do C++ initializations
    _initterm( __xc_a, __xc_z );
}

int atexit(_PVFV func)
{
    if (_atexitlist_cnt < MAX_ATEXIT)
    {
        _atexitlist[_atexitlist_cnt++] = func; 
        return 0;
    }

    return -1;
}

static int exit_done;

void __exit(void)
{
	if (exit_done)
		return;
	
	exit_done = 1;
	
	// do pre-terminators
    _initterm(__xp_a, __xp_z);
    // do terminators
    _initterm(__xt_a, __xt_z);

    if (_atexitlist_cnt)
    {
		_PVFV* p;
		for (p = _atexitlist + _atexitlist_cnt - 1 ; p >= _atexitlist ; p--)
		{
			if (*p != NULL)
				(**p)();
			p--;
		}
    }
}
