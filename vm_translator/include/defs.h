#ifndef _DEFS_H_
#define _DEFS_H_

#define ERRLENGTH_MAX 300
#define VMFILES_MAX 100
typedef enum {LINE_FOUND, EOF_REACHED, PARSER_ERROR} parser_state;
typedef enum {C_ARITHMETIC, C_PUSH, C_POP, C_LABEL, C_GOTO,
			  C_IF, C_FUNCTION, C_RETURN, C_CALL} command_type;

#endif
