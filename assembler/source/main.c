#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "strlib.h"
#include "parser.h"
#include "code.h"
#include "symbolTable.h"
#include "defs.h"

static char *createHackFname();
static void printFopenError(const char *fname);
static void initTable();
static void loadLabels();
static void setZeroes(char *codeword);
static bool assemble();
static int assembleACommand(char *codeword, int *next_ram_addr);
static void assembleCCommand(char *codeword);

static FILE *asm_file,
	 		*hack_file;
static const char *asm_fname;
static char hack_fname[FILENAME_MAX],
			err_msg[MAX_ERRLENGTH];


int main(int argc, char *argv[])
{
	if(argc < 2) {
		fprintf(stderr, "Usage: assembler <asm files>\n");
		exit(EXIT_FAILURE);
	}

	while(--argc) {
		asm_fname = *++argv;

		if(!endsWith(asm_fname, ".asm")) {			//correct input filename?
			fprintf(stderr, "%s is not an asm file!\n", asm_fname);
			continue;
		}

		if(strlen(asm_fname) > FILENAME_MAX) {		//input filename too long?
			fprintf(stderr, "File %s exceeds maximum filename length! "
					"(%d chars)\n", asm_fname, FILENAME_MAX);
			continue;
		}

		if(!(asm_file = fopen(asm_fname, "r"))) {	//input file can't be opened?
			printFopenError(asm_fname);
			continue;
		}

		createHackFname();

		if(!(hack_file = fopen(hack_fname, "w"))) {	   //output file can't be
			printFopenError(hack_fname);			   //opened?
			fclose(asm_file);
			continue;
		}

		printf("Assembling file %s ...\n", asm_fname); //all good: assemble!
		initTable();				//load symbol table with default variables
		loadLabels();										//first pass
		rewind(asm_file);
		puts(assemble() ? "Done." : "Aborted assembly.");	//second pass

		fclose(asm_file);
		fclose(hack_file);

		resetTable();
	}

	exit(EXIT_SUCCESS);
}


/* prints error, in case a file could not be opened */
static void printFopenError(const char *fname)
{
	snprintf(err_msg, MAX_ERRLENGTH,
			"Error while opening file %s", fname);
	perror(err_msg);
}


/* replaces file suffix of asm_fname with a new '.hack' suffix and places
   the result into hack_fname. returns hack_fname */
static char *createHackFname()
{
	strcpy(hack_fname, asm_fname);
	*strrchr(hack_fname, '.') = '\0';
	strcat(hack_fname, ".hack");
	return hack_fname;
}


/* fills symbol table with predefined variables */
static void initTable()
{
	setAddress("SP", 0);
	setAddress("LCL", 1);
	setAddress("ARG", 2);
	setAddress("THIS", 3);
	setAddress("THAT", 4);
	setAddress("R0", 0); setAddress("R1", 1); setAddress("R2", 2);
	setAddress("R3", 3); setAddress("R4", 4); setAddress("R5", 5);
	setAddress("R6", 6); setAddress("R7", 7); setAddress("R8", 8);
	setAddress("R9", 9); setAddress("R10", 10); setAddress("R11", 11);
	setAddress("R12", 12); setAddress("R13", 13); setAddress("R14", 14);
	setAddress("R15", 15);
	setAddress("SCREEN", 16384);
	setAddress("KBD", 24576);
}


/* first pass to add labels to symbol table */
static void loadLabels()
{
	int lnum = 0;
	initParser(asm_file);

	while(hasNextCommand()) {
		if(commandType() == L_COMMAND)
			setAddress(getSymbol(), lnum);
		else
			lnum++;
	}
}


/* assembles given asm_file, writes translation to hack_file.
   returns true if assembly was successful, false otherwise	*/
static bool assemble()
{
	char codeword[WORD_WIDTH + 1];
	int next_ram_addr = 16;			//starting address for RAM variables
	c_type cmd_type;

	initParser(asm_file);

	while(hasNextCommand()) {
		setZeroes(codeword);

		if((cmd_type = commandType()) == A_COMMAND) {
			if(assembleACommand(codeword, &next_ram_addr) == -1) {
				fprintf(stderr, "Memory error!\n");
				return false;
			}

		} else if(cmd_type == C_COMMAND) {
			assembleCCommand(codeword);

		} else continue;				//ignore L_COMMAND at second pass

		if(fprintf(hack_file, "%s\n", codeword) < 0) {
			snprintf(err_msg, MAX_ERRLENGTH,
					"Error while writing to file %s", hack_fname);
			perror(err_msg);
			return false;
		}
	}
	return true;
}


/* fills binary word string with zeroes */
static void setZeroes(char *codeword)
{
	int i;
	for(i = 0; i < WORD_WIDTH; i++)
		codeword[i] = '0';
	codeword[i] = '\0';
}


/* translates A command and enters the result in codeword. returns the
   decimal/address value given with the command, or -1 if a memory error
   occurred while entering a new variable-address pair in the symbol table */
static int assembleACommand(char *codeword, int *next_ram_addr)
{
	int address;
	char *token = getSymbol();

	if(isdigit(*token)) {							//decimal addressing
		address = atoi(token);
		enterDecimal(codeword, address);
	} else {										//addressing via variable
		if((address = getAddress(token)) >= 0) {		//variable is in table
			enterDecimal(codeword, address);
		} else {										//variable is new
			enterDecimal(codeword, *next_ram_addr);
			return setAddress(token, (*next_ram_addr)++);
		}
	}
	return address;
}


/* enters C command in codeword */
static void assembleCCommand(char *codeword)
{
	char *part;
	codeword[0] = codeword[1] = codeword[2] = '1';

	if((part = getDest())) assembleDest(codeword, part);
						   assembleComp(codeword, getComp());
	if((part = getJump())) assembleJump(codeword, part);
}
