#ifndef _PARSER_H_
#define _PARSER_H_

#include <stdio.h>
#include <stdbool.h>
#include "defs.h"

/* sets input file for the parser to work with */
void initParser(FILE *asm_f);

/* returns true, if another usable line could be found, false otherwise. */
/* if true is returned, the parser will hold the next valid line */
bool hasNextCommand();

/* returns: A_COMMAND, if current command is of type @ref
   			C_COMMAND, if current command is of type dest=comp;jmp
			L_COMMAND, if curent command is of type (label) */
c_type commandType();

/* returns pointer to the beginning of the computation part of the command.
   a call of getDest() regarding the current line MUST have happened before
   calling this function */
char *getDest();

/* returns pointer to the beginning of the computation part of the command */
char *getComp();

/* returns pointer to the beginning of the jump part of the command, or NULL if
   there is none. a call of getDest() in regarding the current line MUST have
   happened before calling this function */
char *getJump();

/* returns decimal string or variable/label name of a A_COMMAND or L_COMMAND */
char *getSymbol();

#endif
