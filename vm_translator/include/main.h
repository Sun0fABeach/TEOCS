#ifndef _MAIN_H_
#define _MAIN_H_

/* Prints a custom message plus errno message regarding fname. First argument
   must be a format string containing exactly one '%s' reference. Call this
   function only when errno is not 0. */
void filehandleMsg(const char *format, const char *fname);

#endif
