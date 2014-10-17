#ifndef _SYMTABLE_H_
#define _SYMTABLE_H_

/* returns address associated with the variable name, or -1 if the name has
   not been entered into the table yet */
int getAddress(char *variable);

/* adds a new variable/address entry pair to the symbol table. returns the
   address on success, or -1 if any allocation error occured inside the 
   function */
int setAddress(char *variable, int address);

/* deallocates all entries of the table */
void resetTable();

#endif
