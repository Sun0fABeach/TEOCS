#ifndef __DYN_INPUT_H_
#define __DYN_INPUT_H_

#include <stdio.h>


/*
  Reads a line from instream and writes it into self-growing buffer.
  Multiple calls overwrite the buffer content written by previous calls.
  Final newline characters are discarded. There is no need to free the buffer
  yourself, since this is handled in the background at program termination.

  RETURNS: pointer to buffer on success, NULL when buffer allocation
  or reallocation error occurred. When no characters could be read,
  (at end-of-file, or empty line) the buffer will contain an empty string.
*/
char *fBufRead(FILE *instream);

#define bufRead() fBufRead(stdin)


/*
  Reads a line from instream and writes it into a dynamically allocated string.
  In each call, new memory is allocated to hold the individual string. There
  is no need to free the memory yourself, since this is handled in the
  background at program termination.

  RETURNS: pointer to string on success, NULL when allocation or reallocation
  error occurred. When no characters could be read, (at end-of-file, or empty
  line) the string will be empty.
*/
char *fReadLine(FILE *instream); //NEEDS IMPROVEMENT (ERROR HANDLING)

#define readLine() fReadLine(stdin)

/*Deallocates all strings allocated via calls to fReadLine()*/
size_t clearStrings();

#endif
