#ifndef _DEFS_H_
#define _DEFS_H_

#define MAX_ERRLENGTH 100
#define WORD_WIDTH 16
#define COMP_LEN 3
#define COMP_BIT_LEN 7
#define HASHSIZE 101

//ALU command bit translations
#define LOAD_0		"0101010"
#define LOAD_1		"0111111"
#define LOAD_D		"0001100"
#define LOAD_A		"0110000"
#define LOAD_M		"1110000"
#define	MINUS_1		"0111010"
#define MINUS_D		"0001111"
#define MINUS_A		"0110011"
#define MINUS_M		"1110011"
#define	NOT_D		"0001101"
#define NOT_A		"0110001"
#define NOT_M		"1110001"
#define D_PLUS_1	"0011111"
#define A_PLUS_1	"0110111"
#define M_PLUS_1	"1110111"
#define D_MINUS_1	"0001110"
#define A_MINUS_1	"0110010"
#define M_MINUS_1	"1110010"
#define D_PLUS_A	"0000010"
#define D_PLUS_M	"1000010"
#define D_MINUS_A	"0010011"
#define A_MINUS_D	"0000111"
#define D_MINUS_M	"1010011"
#define M_MINUS_D	"1000111"
#define D_AND_A		"0000000"
#define D_AND_M		"1000000"
#define D_OR_A		"0010101"
#define D_OR_M		"1010101"

//command types
typedef enum {	A_COMMAND,
				C_COMMAND,
				L_COMMAND	} c_type;

//bit fields of codeword
typedef enum {	MODE_BIT, DUMMY_1, DUMMY_2,
				A_BIT, C1, C2, C3, C4, C5, C6,
				WRITE_TO_A, WRITE_TO_D, WRITE_TO_M,
				JUMP_L, JUMP_E, JUMP_G,				} bit_field;

#endif
