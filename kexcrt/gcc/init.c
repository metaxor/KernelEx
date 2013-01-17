#include <stdlib.h>
#include "init.h"

#define MAX_ATEXIT        32

typedef void (__cdecl *_PVFV)(void);

static _PVFV _atexitlist[MAX_ATEXIT];
static int _atexitlist_cnt;

extern _PVFV __CTOR_LIST__[];
extern _PVFV __DTOR_LIST__[];

extern void _pei386_runtime_relocator (void);

int atexit(_PVFV func)
{
    if (_atexitlist_cnt < MAX_ATEXIT)
    {
        _atexitlist[_atexitlist_cnt++] = func; 
        return 0;
    }

    return -1;
}

void
__do_global_dtors (void)
{
  static _PVFV *p = __DTOR_LIST__ + 1;

  /*
   * Call each destructor in the destructor list until a null pointer
   * is encountered.
   */
  while (*p)
    {
      (*(p)) ();
      p++;
    }
}

void
__do_global_ctors (void)
{
  unsigned long nptrs = (unsigned long) __CTOR_LIST__[0];
  unsigned i;

  /*
   * If the first entry in the constructor list is -1 then the list
   * is terminated with a null entry. Otherwise the first entry was
   * the number of pointers in the list.
   */
  if (nptrs == -1)
    {
      for (nptrs = 0; __CTOR_LIST__[nptrs + 1] != 0; nptrs++)
	;
    }

  /* 
   * Go through the list backwards calling constructors.
   */
  for (i = nptrs; i >= 1; i--)
    {
      __CTOR_LIST__[i] ();
    }

  /*
   * Register the destructors for processing on exit.
   */
  atexit (__do_global_dtors);
}

static int initialized = 0;

void __init(void)
{
  _pei386_runtime_relocator();
  
  if (!initialized)
    {
      initialized = 1;
      __do_global_ctors ();
    }
}

static int exited = 0;

void __exit(void)
{
	if (exited)
		return;
	exited = 1;
	
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
