/*
 * strupr.c
 */

#include <string.h>
#include <ctype.h>

char* strupr(char *s)
{
	unsigned char *c = s;

	while (*c) {
		*c = toupper(*c);
		c++;
	}

	return s;
}
