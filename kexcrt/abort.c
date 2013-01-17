#include <windows.h>

void __cdecl abort(void)
{
	RaiseException(0xdeafcafe, EXCEPTION_NONCONTINUABLE, 0, NULL);
}
