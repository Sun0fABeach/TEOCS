#include <string.h>
#include <ctype.h>
#include "strlib.h"

bool endsWith(const char *s, const char *ending)
{
	size_t s_len = strlen(s);
	size_t ending_len = strlen(ending);

	if(ending_len > s_len) return false;
	if(strcmp(s + s_len - ending_len, ending)) return false;
	return true;
}

char *trimLeft(char *s)
{
	while(isspace(*s)) s++;
	return s;
}

char *trimRight(char *s)
{
	char *p = s + strlen(s);
	while(isspace(*--p) && p >= s)
		;
	*(p + 1) = '\0';
	return s;
}
