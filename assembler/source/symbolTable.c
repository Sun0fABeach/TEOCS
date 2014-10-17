#include <stdlib.h>
#include <string.h>
#include "defs.h"
#include "symbolTable.h"


typedef struct entry {
	struct entry *next;
	char *variable;
	int address;
} Entry;

static Entry *table[HASHSIZE];


static unsigned int hash(char *variable);
static Entry *getEntry(char *variable);


/* returns address associated with the variable name, or -1 if the name has
   not been entered into the table yet */
int getAddress(char *variable)
{
	for(Entry *ep = table[hash(variable)]; ep; ep = ep->next)
		if(!strcmp(variable, ep->variable))
			return ep->address;
	return -1;
}


/* adds a new variable/address entry pair to the symbol table. returns the
   address on success, or -1 if any allocation error occured inside the 
   function */
int setAddress(char *variable, int address)
{
	Entry *ep;
	unsigned int hashval;

	if(!(ep = getEntry(variable))) {		//not found
		ep = malloc(sizeof(Entry));
		if(!ep || !(ep->variable = strdup(variable)))	//alloc error
			return -1;
		hashval = hash(variable);
		ep->next = table[hashval];
		table[hashval] = ep;
	}
	return ep->address = address;
}


/* deallocates all entries of the table */
void resetTable()
{
	Entry *ep, *next;
	for(int i = 0; i < HASHSIZE; i++) {
		for(ep = table[i]; ep; ep = next) {
			next = ep->next;
			free(ep->variable);
			free(ep);
		}
		table[i] = NULL;
	}
}


/* returns a hash value 0 <= x < HASHSIZE, whose computation is based on the
   variable name */
static unsigned int hash(char *variable)
{
	unsigned int hashval;
	for(hashval = 0; *variable; variable++)
		hashval = *variable + 31 * hashval;
	return hashval % HASHSIZE;
}


/* returns entry that holds the variable name, or NULL if the name has
   not been entered into the table yet */
static Entry *getEntry(char *variable)
{
	for(Entry *ep = table[hash(variable)]; ep; ep = ep->next)
		if(!strcmp(variable, ep->variable))
			return ep;
	return NULL;
}
