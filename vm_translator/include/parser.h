#ifndef _PARSER_H_
#define _PARSER_H_

#include <stdbool.h>
#include <stdio.h>
#include "defs.h"


extern char *base_command;	//the first token of the current vm line


/* Sets input file for the parser to work with. */
void initParser(FILE *vm_f);

/* Tries to advance the parser to the next line and returns the result as a
   parser state. */
parser_state advanceParser();

/* Returns the command type of the current vm command */
command_type commandType();

/* Returns the next argument of the current vm command. */
char *nextArgument();

#endif
