#ifndef _COMPILATIONENGINE_H_
#define _COMPILATIONENGINE_H_

#include <stdbool.h>
#include <stdio.h>


/* Passes .asm file and filename to the codeWriter module and writes
   bootstrap code for this asm program. */
void initCompilation(FILE *asm_file, char *asm_fname);

/* Compiles a single .vm file. Returns true if compilation was successful,
   false if an error occurred. */
bool compileUnit(FILE *vm_file, char *vm_fname);

#endif
