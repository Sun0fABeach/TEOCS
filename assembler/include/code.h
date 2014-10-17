#ifndef _CODE_H_
#define _CODE_H_

/* sets destination bits in codeword according to destination command fragment */
void assembleDest(char *codeword, const char *dest);

/* sets destination bits in codeword according to computation command fragment */
void assembleComp(char *codeword, const char *comp);

/* sets destination bits in codeword according to jump command fragment */
void assembleJump(char *codeword, const char *jump);

/* translates decimal into binary string and enters result in codeword */
void enterDecimal(char *codeword, int decimal);

#endif
