#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include "codeWriter.h"
#include "main.h"
#include "defs.h"


static void writePush(const char *segment, const char *index);
static void writePop(const char *segment, const char *index);
static void dereferencePush(const char *variable, const char *index);
static void dereferencePop(const char *variable, const char *index);
static void writeComp(const char *cmp_tag, unsigned short *cmp_count);
static void writeBinOp(char operator);
static void writeUnaryOp(char operator);
static bool errnoCheck();
static void cleanup();


static char vm_filename[FILENAME_MAX];
static char asm_filename[FILENAME_MAX];
static FILE *asm_file;
static unsigned short eq_count, gt_count, lt_count;
static int resume_index;
static char *current_func;


/* Sets .asm file, the asm filename and resets internal asm label counters. */
void initCodeWriter(FILE *asm_f, char *asm_fname)
{
	asm_file = asm_f;
	strcpy(asm_filename, asm_fname);
	eq_count = gt_count = lt_count = 0;
	if(current_func) free(current_func);
	current_func = strdup("default_name");

	atexit(cleanup);
}


/* Sets a new filename for custom asm variables to use. */
bool setFilename(const char *vm_fname)
{
	char *separator = strrchr(vm_fname, '/');
	if(separator)	//take filename from longer path
		vm_fname = separator + 1;
	strcpy(vm_filename, vm_fname);
	*strrchr(vm_filename, '.') = '\0';

	asmPrintf("//Entering file %s...", vm_fname);
	return errnoCheck();
}


/* Writes bootstrap code for the asm program. */
bool writeInit()
{
	asmPuts("@256");
	asmPuts("D=A");
	asmPuts("@SP");
	asmPuts("M=D");

	return writeCall("Sys.init", "0");
}



/*+++++++++++++++++++++++++++++++
  +++++		ARITHMETICS		+++++
  +++++++++++++++++++++++++++++++*/

/* Writes asm instructions effecting an arithmetic command. Returns true if
   write operation was successful, false otherwise. */
bool writeArithmetic(const char *a_command)
{
	if(strlen(a_command) == 2) {
		switch(*a_command) {
			case 'e':  	//eq
				writeComp("EQ", &eq_count);
				break;
			case 'g':	//gt
				writeComp("GT", &gt_count);
				break;
			case 'l':	//lt
				writeComp("LT", &lt_count);
				break;
			default:	//or
				writeBinOp('|');
		}
	} else {
		switch(a_command[1]) {
			case 'd':	//add
				writeBinOp('+');
				break;
			case 'u':	//sub
				writeBinOp('-');
				break;
			case 'e':	//neg
				writeUnaryOp('-');
				break;
			case 'n':	//and
				writeBinOp('&');
				break;
			default:	//not
				writeUnaryOp('!');
		}
	}
	return errnoCheck();
}


/* Responsible for writing assembly equals/greater-than/less-than tests. */
static void writeComp(const char *cmp_tag, unsigned short *cmp_count)
{
	lowerSP();
	asmPuts("D=M");
	lowerSP();
	asmPuts("D=M-D");
	asmPrintf2("@%s%u", cmp_tag, *cmp_count);	//condition true?
	asmPrintf("D;J%s", cmp_tag);				//jmp to true-condition branch
	setSP("0");		//write false
	asmPrintf2("@END%s%u", cmp_tag, *cmp_count);
	asmPuts("0;JMP");
	asmPrintf2("(%s%u)", cmp_tag, *cmp_count);	//true-condition branch
	setSP("-1");	//write true
	asmPrintf2("(END%s%u)", cmp_tag, (*cmp_count)++);
	raiseSP();
}


/* Responsible for writing assembly 2-argument operations (+,-,&,|) */
static void writeBinOp(char operator)
{
	lowerSP();
	asmPuts("D=M");
	lowerSP();
	asmPrintf("M=M%cD", operator);
	raiseSP();
}


/* Responsible for writing assembly 1-argument operations (-,!) */
static void writeUnaryOp(char operator)
{
	lowerSP();
	asmPrintf("M=%cM", operator);
	raiseSP();
}



/*+++++++++++++++++++++++++++++++
  +++++		 PUSH/POP		+++++
  +++++++++++++++++++++++++++++++*/

/* Writes instructions effecting a push or pop command. Returns true if
   write operation was successful, false otherwise. */
bool writePushPop(const command_type p_command, const char *segment,
				  const char *index)
{
	if(p_command == C_PUSH)
		writePush(segment, index);
	else
		writePop(segment, index);

	return errnoCheck();
}


static void writePush(const char *segment, const char *index)
{
	switch(*segment) {
		case 'c':	//constant
			asmPrintf("@%s", index);
			asmPuts("D=A");
			setSP("D");
			raiseSP();
			break;
		case 'l':	//local
			dereferencePush("LCL", index);
			break;
		case 'a':	//argument
			dereferencePush("ARG", index);
			break;
		case 'p':	//pointer
			asmPrintf("@%s", *index == '0' ? "THIS" : "THAT");
			asmPuts("D=M");
			setSP("D");
			raiseSP();
			break;
		case 's':	//static
			asmPrintf2("@%s.%s", vm_filename, index);
			asmPuts("D=M");
			setSP("D");
			raiseSP();
			break;
		default:
			if(segment[2] == 'i') {			//this
				dereferencePush("THIS", index);
			} else if(segment[2] == 'a'){	//that
				dereferencePush("THAT", index);
			} else {						//temp
				asmPrintf("@R%d", 5 + atoi(index));
				asmPuts("D=M");
				setSP("D");
				raiseSP();
			}
	}
}


/* Pushes value of memory location variable+index on stack. */
static void dereferencePush(const char *variable, const char *index)
{
	asmPrintf("@%s", index);
	asmPuts("D=A");
	asmPrintf("@%s", variable);
	asmPuts("A=M+D");
	asmPuts("D=M");
	setSP("D");
	raiseSP();
}


static void writePop(const char *segment, const char *index)
{
	switch(*segment) {
		case 'l':	//local
			dereferencePop("LCL", index);
			break;
		case 'a':	//argument
			dereferencePop("ARG", index);
			break;
		case 'p':	//pointer
			lowerSP();
			asmPuts("D=M");
			asmPrintf("@%s", *index == '0' ? "THIS" : "THAT");
			asmPuts("M=D");
			break;
		case 's':	//static
			lowerSP();
			asmPuts("D=M");
			asmPrintf2("@%s.%s", vm_filename, index);
			asmPuts("M=D");
			break;
		default:
			if(segment[2] == 'i') {			//this
				dereferencePop("THIS", index);
			} else if(segment[2] == 'a'){	//that
				dereferencePop("THAT", index);
			} else {						//temp
				lowerSP();
				asmPuts("D=M");
				asmPrintf("@R%d", 5 + atoi(index));
				asmPuts("M=D");
			}
	}
}


/* Pops value of stack to memory location variable+index. */
static void dereferencePop(const char *variable, const char *index)
{
	asmPrintf("@%s", index);
	asmPuts("D=A");
	asmPrintf("@%s", variable);
	asmPuts("D=M+D");
	asmPuts("@R13");
	asmPuts("M=D");		//save calculated address
	lowerSP();
	asmPuts("D=M");
	asmPuts("@R13");
	asmPuts("A=M");
	asmPuts("M=D");
}



/*+++++++++++++++++++++++++++++++++++
  +++++		 CONTROL FLOW		+++++
  +++++++++++++++++++++++++++++++++++*/

/* Writes an asm label. */
bool writeLabel(const char *label)
{
	asmPrintf2("(%s$%s)", current_func, label);
	return errnoCheck();
}


/* Writes unconditional goto command. */
bool writeGoto(const char *label)
{
	asmPrintf2("@%s$%s", current_func, label);
	asmPuts("0,JMP");
	return errnoCheck();
}


/* Writes conditional goto command: jump if stack top != 0. */
bool writeIfGoto(const char *label)
{
	lowerSP();
	asmPuts("D=M");
	asmPrintf2("@%s$%s", current_func, label);
	asmPuts("D,JNE");
	return errnoCheck();
}



/*+++++++++++++++++++++++++++++++
  +++++		 FUNCTIONS		+++++
  +++++++++++++++++++++++++++++++*/

/* Writes function call code. */
bool writeCall(const char *name, char *nargs)
{
	asmPrintf2("//Calling function %s with %s args...", name, nargs);
	asmPrintf2("@%s_Resume%d", current_func, resume_index);//push return address
	asmPuts("D=A");
	setSP("D");
	raiseSP();
	asmPuts("@LCL");	//push meta variables...
	asmPuts("D=M");
	setSP("D");
	raiseSP();
	asmPuts("@ARG");
	asmPuts("D=M");
	setSP("D");
	raiseSP();
	asmPuts("@THIS");
	asmPuts("D=M");
	setSP("D");
	raiseSP();
	asmPuts("@THAT");
	asmPuts("D=M");
	setSP("D");
	raiseSP();
	asmPuts("@SP");				//ARG = SP-nargs-5
	asmPuts("D=M");
	asmPrintf("@%s", nargs);
	asmPuts("D=D-A");
	asmPuts("@5");
	asmPuts("D=D-A");
	asmPuts("@ARG");
	asmPuts("M=D");
	asmPuts("@SP");				//LCL = SP
	asmPuts("D=M");
	asmPuts("@LCL");
	asmPuts("M=D");				//set return location label
	asmPrintf("@%s", name);		//goto function
	asmPuts("0;JMP");
	asmPrintf2("(%s_Resume%d)", current_func, resume_index++);

	return errnoCheck();
}


/* Writes function entry code. */
bool writeFunction(const char *name, char *nlocals)
{
	asmPrintf2("//Entering function %s with %s locals...", name, nlocals);
	resume_index = 0;

	free(current_func);
	if(!(current_func = strdup(name))) {
		fprintf(stderr, "Memory allocation error for function name!\n");
		return false;
	}
	asmPrintf("(%s)", name);

	int npushes = atoi(nlocals);
	for(int i = 0; i < npushes; i++)
		writePush("constant", "0");

	return errnoCheck();
}


/* Writes function return code. */
bool writeReturn()
{
	asmPrintf("//Returning from function %s...", current_func);
	asmPuts("@LCL");	//FRAME=LCL
	asmPuts("D=M");
	asmPuts("@R13");
	asmPuts("M=D");
	lowerSP();			//*ARG=pop() ...placing function result
	asmPuts("D=M");
	asmPuts("@ARG");
	asmPuts("A=M");
	asmPuts("M=D");
	asmPuts("@ARG");	//SP=ARG+1	...setting SP above result
	asmPuts("D=M");
	asmPuts("@SP");
	asmPuts("M=D+1");
	asmPuts("@R13");	//THAT=*(FRAME-1)	...restore variables...
	asmPuts("M=M-1");
	asmPuts("A=M");
	asmPuts("D=M");
	asmPuts("@THAT");
	asmPuts("M=D");
	asmPuts("@R13");	//THIS=*(FRAME-2)
	asmPuts("M=M-1");
	asmPuts("A=M");
	asmPuts("D=M");
	asmPuts("@THIS");
	asmPuts("M=D");
	asmPuts("@R13");	//ARG=*(FRAME-3)
	asmPuts("M=M-1");
	asmPuts("A=M");
	asmPuts("D=M");
	asmPuts("@ARG");
	asmPuts("M=D");
	asmPuts("@R13");	//LCL=*(FRAME-4)
	asmPuts("M=M-1");
	asmPuts("A=M");
	asmPuts("D=M");
	asmPuts("@LCL");
	asmPuts("M=D");
	asmPuts("@R13");	//goto *(FRAME-5)	...return to label after call
	asmPuts("A=M-1");
	asmPuts("A=M");
	asmPuts("0;JMP");

	return errnoCheck();
}


/*+++++++++++++++++++++++++++++++++++
  +++++		 MISCELLANEOUS		+++++
  +++++++++++++++++++++++++++++++++++*/


/* Checks errno flag. Returns false and prints error message if it is set.
   Returns true otherwise. */
static bool errnoCheck()
{
	if(errno) {
		filehandleMsg("Error while writing to output file %s.vm", asm_filename);
		return false;
	}
	return true;
}


/* Free allocs. */
static void cleanup()
{
	free(current_func);
}
