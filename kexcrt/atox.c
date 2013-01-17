/*
 * atox.c
 *
 * atoi(), atol(), atoll()
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

TYPE NAME(const char *nptr)
{
	return (TYPE) strntoumax(nptr, (char **)NULL, 10, ~(size_t) 0);
}
