#include <stdio.h>
#include <stdbool.h>
#include "compilationEngine.h"
#include "parser.h"
#include "codeWriter.h"
#include "defs.h"


/* Passes .asm file and filename to the codeWriter module and writes
   bootstrap code for this asm program. */
void initCompilation(FILE *asm_file, char *asm_fname)
{
	initCodeWriter(asm_file, asm_fname);
	writeInit();
}


/* Compiles a single .vm file. Returns true if compilation was successful,
   false if an error occurred. */
bool compileUnit(FILE *vm_file, char *vm_fname)
{
	parser_state pstate;
	command_type ctype;
	char *arg1;

	initParser(vm_file);
	if(!setFilename(vm_fname))
		return false;

	while((pstate = advanceParser()) == LINE_FOUND) {
		switch((ctype = commandType())) {

			case C_PUSH: case C_POP:
				arg1 = nextArgument();
				if(!writePushPop(ctype, arg1, nextArgument()))
					return false;
				break;

			case C_ARITHMETIC:
				if(!writeArithmetic(base_command))
					return false;
				break;

			case C_LABEL:
				if(!writeLabel(nextArgument()))
					return false;
				break;

			case C_GOTO:
				if(!writeGoto(nextArgument()))
					return false;
				break;

			case C_IF:
				if(!writeIfGoto(nextArgument()))
					return false;
				break;

			case C_CALL:
				arg1 = nextArgument();
				if(!writeCall(arg1, nextArgument()))
					return false;
				break;

			case C_FUNCTION:
				arg1 = nextArgument();
				if(!writeFunction(arg1, nextArgument()))
					return false;
				break;

			default:	//C_RETURN
				if(!writeReturn())
					return false;
		}
	}

	if(pstate == PARSER_ERROR) return false;
	return true;
}
