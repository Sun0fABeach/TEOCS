#ifndef _STRLIB_H_
#define _STRLIB_H_

#include <stdbool.h>

/*
   Returns true, when s ends with pattern, false otherwise.
   Will return true if pattern is an empty string.
*/
bool endsWith(const char *s, const char *ending);

/*
   Evaluates to true when s starts with pattern, false otherwise.
   Evaluates to true if pattern is an empty string.
   CAUTION: Will evaluate argument s twice!
*/
#define startsWith(s, pattern) (strstr((s), (pattern)) == (s))

/*
   Returns a pointer to the first non-whitespace character in s,
   starting from the left.
*/
char *trimLeft(char *s);

/*
   Searches for the first non-whitespace character, going right to left
   from the end of the string. Once found, a terminating null character
   will be placed after it. Retruns s.
*/
char *trimRight(char *s);

/*
   Trims whitespace from s using functions trimLeft and trimRight.
   Evaluates to the return value of trimLeft.
*/
#define trim(s) trimLeft(trimRight(s))

#endif
