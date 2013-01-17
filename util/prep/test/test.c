#include <stdio.h>

// MAKE_EXPORT fun=funny
void fun()
{
	int foobar;
	foobar++;
}

typedef int sad;

/* MAKE_EXPORT barbar=jarjar
 */
			//difficutly gets higher
/* _cdecl barbar(bool b)
{
}
*/
#ifdef WINDOWS
__declspec(dllexport)
//total fuckup
#else
	    __cdecl
#endif



long long barbar(int a, //test
	bool b, /* dfas */ char /* sad */ c,
	//dumb stuff
	
	float f,
					 double d)
					 {
	} int main() {
	fun();
	return (int) barbar(0, true, 'a', 2.3f, 0.34);
}
