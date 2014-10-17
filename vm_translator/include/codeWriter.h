#ifndef _CODEWRITER_H_
#define _CODEWRITER_H_

#include <stdbool.h>
#include "defs.h"

#define lowerSP() asmPuts("@SP\nM=M-1\nA=M")
#define raiseSP() asmPuts("@SP\nM=M+1")
#define setSP(value) asmPuts("@SP\nA=M\nM="value)

#define asmPuts(command) fputs(command"\n", asm_file)
#define asmPrintf(format, arg) fprintf(asm_file, format"\n", arg)
#define asmPrintf2(format, arg1, arg2) fprintf(asm_file, format"\n", arg1, arg2)


/* Sets .asm file to write output to + the asm filename. */
void initCodeWriter(FILE *asm_f, char *asm_fname);

/* Sets a new filename for asm variables to use. */
bool setFilename(const char *vm_fname);

/* Writes bootstrap code for the asm program. */
bool writeInit();

/* Writes asm instructions effecting an arithmetic command. Returns true if
   write operation was successful, false otherwise. */
bool writeArithmetic(const char *a_command);

/* Writes instructions effecting a push or pop command. Returns true if
   write operation was successful, false otherwise. */
bool writePushPop(const command_type p_command, const char *segment,
				  const char *index);

/* Writes an asm label. */
bool writeLabel(const char *label);

/* Writes unconditional goto command. */
bool writeGoto(const char *label);

/* Writes conditional goto command: jump if stack top != 0. */
bool writeIfGoto(const char *label);

/* Writes function call code. */
bool writeCall(const char *name, char *nargs);

/* Writes function entry code. */
bool writeFunction(const char *name, char *nlocals);

/* Writes function return code. */
bool writeReturn();

#endif
