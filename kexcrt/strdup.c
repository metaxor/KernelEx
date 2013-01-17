#include <stdlib.h>
#include <string.h>

char *strdup(const char *str)
{
	char *newstr;
	
	if (!str)
		return NULL;
	
	newstr = (char *) malloc(strlen(str) + 1);
	if (newstr)
		strcpy(newstr, str);

	return newstr;
}
