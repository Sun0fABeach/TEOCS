#include <stdio.h>
#include <string.h>
#include "code.h"
#include "defs.h"

static void enterDest(char *codeword, char adm);
static char *prepareWord(char *alu_word, const char *comp);

/* sets destination bits in codeword according to destination command fragment */
void assembleDest(char *codeword, const char *dest)
{
	char c1, c2, c3;
	sscanf(dest, " %c %c %c", &c1, &c2, &c3);
	enterDest(codeword, c1);
	if(c2 == '=') return;
	enterDest(codeword, c2);
	if(c3 == '=') return;
	enterDest(codeword, c3);
}


/* sets destination bits in codeword according to computation command fragment */
void assembleComp(char *codeword, const char *comp)
{
	char alu_word[COMP_LEN];
	prepareWord(alu_word, comp);		//get rid of whitespace

	switch(strlen(alu_word)) {			//differentiate comp part by length
		case 1:
			switch(*alu_word) {
				case '0':
					strncpy(codeword + A_BIT, LOAD_0, COMP_BIT_LEN);
					break;
				case '1':
					strncpy(codeword + A_BIT, LOAD_1, COMP_BIT_LEN);
					break;
				case 'A':
					strncpy(codeword + A_BIT, LOAD_A, COMP_BIT_LEN);
					break;
				case 'D':
					strncpy(codeword + A_BIT, LOAD_D, COMP_BIT_LEN);
					break;
				default:
					strncpy(codeword + A_BIT, LOAD_M, COMP_BIT_LEN);
			}
			break;

		case 2:
			if(*alu_word == '!') {
				switch(*(alu_word + 1)) {
					case 'A':
						strncpy(codeword + A_BIT, NOT_A, COMP_BIT_LEN);
						break;
					case 'D':
						strncpy(codeword + A_BIT, NOT_D, COMP_BIT_LEN);
						break;
					default:
						strncpy(codeword + A_BIT, NOT_M, COMP_BIT_LEN);
				}
			} else {	//*alu_word == '-'
				switch(*(alu_word + 1)) {
					case '1':
						strncpy(codeword + A_BIT, MINUS_1, COMP_BIT_LEN);
						break;
					case 'A':
						strncpy(codeword + A_BIT, MINUS_A, COMP_BIT_LEN);
						break;
					case 'D':
						strncpy(codeword + A_BIT, MINUS_D, COMP_BIT_LEN);
						break;
					default:
						strncpy(codeword + A_BIT, MINUS_M, COMP_BIT_LEN);
				}
			}
			break;

		default:	//length == 3
			switch(*(alu_word + 1)) {
				case '&':
					if(!(strcmp(alu_word, "A&D")) || !(strcmp(alu_word, "D&A")))
						strncpy(codeword + A_BIT, D_AND_A, COMP_BIT_LEN);
					else
						strncpy(codeword + A_BIT, D_AND_M, COMP_BIT_LEN);
					break;
				case '|':
					if(!(strcmp(alu_word, "A|D")) || !(strcmp(alu_word, "D|A")))
						strncpy(codeword + A_BIT, D_OR_A, COMP_BIT_LEN);
					else
						strncpy(codeword + A_BIT, D_OR_M, COMP_BIT_LEN);
					break;
				case '+':
					if(!strcmp(alu_word, "D+1"))
						strncpy(codeword + A_BIT, D_PLUS_1, COMP_BIT_LEN);
					else if(!strcmp(alu_word, "A+1"))
						strncpy(codeword + A_BIT, A_PLUS_1, COMP_BIT_LEN);
					else if(!strcmp(alu_word, "M+1"))
						strncpy(codeword + A_BIT, M_PLUS_1, COMP_BIT_LEN);
					else if(!(strcmp(alu_word, "A+D")) ||
							!(strcmp(alu_word, "D+A")))
						strncpy(codeword + A_BIT, D_PLUS_A, COMP_BIT_LEN);
					else
						strncpy(codeword + A_BIT, D_PLUS_M, COMP_BIT_LEN);
					break;
				default:
					if(!strcmp(alu_word, "D-1"))
						strncpy(codeword + A_BIT, D_MINUS_1, COMP_BIT_LEN);
					else if(!strcmp(alu_word, "A-1"))
						strncpy(codeword + A_BIT, A_MINUS_1, COMP_BIT_LEN);
					else if(!strcmp(alu_word, "M-1"))
						strncpy(codeword + A_BIT, M_MINUS_1, COMP_BIT_LEN);
					else if(!(strcmp(alu_word, "A-D")))
						strncpy(codeword + A_BIT, A_MINUS_D, COMP_BIT_LEN);
					else if(!(strcmp(alu_word, "D-A")))
						strncpy(codeword + A_BIT, D_MINUS_A, COMP_BIT_LEN);
					else if(!(strcmp(alu_word, "D-M")))
						strncpy(codeword + A_BIT, D_MINUS_M, COMP_BIT_LEN);
					else
						strncpy(codeword + A_BIT, M_MINUS_D, COMP_BIT_LEN);
			}
	}
}


/* sets destination bits in codeword according to jump command fragment */
void assembleJump(char *codeword, const char *jump)
{
	jump = strchr(jump, 'J');
	if(!(*++jump == 'G' || *jump == 'E'))	//first jmp bit
		codeword[JUMP_L] = '1';
	if(!(*jump == 'L' || *jump == 'E'))		//third jmp bit
		codeword[JUMP_G] = '1';
	if(!(*jump++ == 'N' || *jump == 'T'))	//second jmp bit
		codeword[JUMP_E] = '1';
}


/* sets the destination bit of codeword according to given character */
static void enterDest(char *codeword, char adm)
{
	switch(adm) {
		case 'A':
			codeword[WRITE_TO_A] = '1';
			break;
		case 'D':
			codeword[WRITE_TO_D] = '1';
			break;
		default:
			codeword[WRITE_TO_M] = '1';
	}
}


/* translates decimal into binary string and enters result in codeword */
void enterDecimal(char *codeword, int decimal)
{
	for(int mask = 16384; mask >= 1; mask /= 2)
		*++codeword = decimal & mask ? '1' : '0';
}


/* saves the comp part of the current instruction to alu_word,
   stripped of all whitespace. returns the whitespace-free comp part*/
static char *prepareWord(char *alu_word, const char *comp)
{
	int len = sscanf(comp, " %1[-01ADM!] %1[-1ADM+&|] %1[1ADM]",
					 alu_word, alu_word + 1, alu_word + 2);
	alu_word[len] = '\0';
	return alu_word;
}
