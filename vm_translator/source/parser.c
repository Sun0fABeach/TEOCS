#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "parser.h"
#include "dyn_input.h"
#include "defs.h"

static bool validLine();

static char *line;
static FILE *vm_file;
char *base_command;


/* Sets input variable, which is private to this file. */
void initParser(FILE *vm_f)
{
	vm_file = vm_f;
}


/* Puts next usable line from vm_file, if there is one, into variable line. */
/* Returns the outcome of this operation as a parser state. */
parser_state advanceParser() {
	char *comment_probe;

	while(!feof(vm_file)) {
		if(!(line = fBufRead(vm_file))) {
			fprintf(stderr, "A memory error occured while reading a line!\n");
			return PARSER_ERROR;
		}
		if(ferror(vm_file)) {
			fprintf(stderr, "A reading error occured!\n");
			return PARSER_ERROR;
		}
		if(validLine()) {
			if((comment_probe = strchr(line, '/')))		//strip possible comment
				*comment_probe = '\0';
			return LINE_FOUND;
		}
	}
	return EOF_REACHED;
}


/* Returns the command type of the current vm command. */
command_type commandType()
{
	size_t command_len;
	base_command = strtok(line, " \t\r\n");

	if((command_len = strlen(base_command)) == 2)
		return C_ARITHMETIC;
	if(command_len == 3) {
		if(*base_command == 'p') return C_POP;
		return C_ARITHMETIC;
	}
	if(command_len == 4) {
		if(*base_command == 'p') return C_PUSH;
		if(*base_command == 'g') return C_GOTO;
		return C_CALL;
	}
	if(command_len == 5) return C_LABEL;
	if(command_len == 6) return C_RETURN;
	if(command_len == 7) return C_IF;
	return C_FUNCTION;
}


/* Returns the next argument of the current vm command. */
char *nextArgument()
{
	return strtok(NULL, " \t\r\n");
}


/* Checks whether current line has more than just whitespace or a comment. */
static bool validLine()
{
	char c1 = '\0', c2 = '\0';
   	sscanf(line, " %c%c", &c1, &c2);
	return (!c1 || (c1 == '/' && c2 == '/')) ? false : true;
}
