#include <stdio.h>
#include <string.h>

static char strerror_not_implemented[64];

char *strerror(int errnum)
{
	strerror_r(errnum, strerror_not_implemented, 64);
	
	return strerror_not_implemented;
}

int strerror_r(int errnum, char *strerrbuf, size_t buflen)
{
	snprintf(strerrbuf, buflen,
		"strerror(%d)", errnum);
	
	return 0;
}
