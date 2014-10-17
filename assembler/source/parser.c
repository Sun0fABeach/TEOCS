#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "strlib.h"
#include "parser.h"
#include "dyn_input.h"
#include "defs.h"

static bool validLine();

static FILE *asm_file;
static char *line;


/* sets input variable, which is private to this file */
void initParser(FILE *asm_f)
{
	asm_file = asm_f;
}


/* puts next usable line from asm_file into variable line, if there is one. */
/* returns true, if another usable line could be found, false otherwise */
bool hasNextCommand() {
	char *comment_probe;

	while(!feof(asm_file)) {
		if(!(line = fBufRead(asm_file))) {
			fprintf(stderr, "A memory error occured while reading a line!\n");
			return false;
		}
		if(ferror(asm_file)) {
			fprintf(stderr, "A reading error occured!\n");
			return false;
		}
		if(validLine(line)) {
			if((comment_probe = strchr(line, '/')))		//strip possible comment
				*comment_probe = '\0';
			return true;
		}
	}
	return false;
}


/* returns: A_COMMAND, if command is of type @ref
   			C_COMMAND, if command is of type dest=comp;jmp
			L_COMMAND, if command is of type (label) 	*/
c_type commandType()
{
	char *probe;
	if(!(probe = strpbrk(line, "@(")))
		return C_COMMAND;
	return *probe == '@' ? A_COMMAND : L_COMMAND;
}


/* returns pointer to the beginning of the destination part of the command,
   or NULL if there is none */
char *getDest()
{
	char *eq_sign, *start = line;
	if(!(eq_sign = strchr(line, '=')))
		return NULL;
	line = eq_sign + 1;
	return start;
}


/* returns pointer to the beginning of the computation part of the command.
   a call of getDest() regarding the current line MUST have happened before
   calling this function */
char *getComp()
{
	return line;
}


/* returns pointer to the beginning of the jump part of the command, or NULL if
   there is none. a call of getDest() regarding the current line MUST have
   happened before calling this function */
char *getJump()
{
	char *semicolon;
	if((semicolon = strchr(line, ';')))
		return semicolon + 1;
	return NULL;
}


/* returns decimal string or variable/label name of an A_COMMAND or L_COMMAND */
char *getSymbol()
{
	char *probe;
	if(!(probe = strchr(line, '@'))) {
		probe = strchr(line, '(');
		*strchr(line, ')') = '\0';
	}
	return trim(probe + 1);
}


/* checks whether line has more than just whitespace or a comment */
static bool validLine(char *line)
{
	char c1 = '\0', c2 = '\0';
   	sscanf(line, " %c%c", &c1, &c2);
	return (!c1 || c1 == '/' && c2 == '/') ? false : true;
}
