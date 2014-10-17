// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Fill.asm

// Runs an infinite loop that listens to the keyboard input. 
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel. When no key is pressed, the
// program clears the screen, i.e. writes "white" in every pixel.

// Put your code here.
(WHITEIDLE)
	@KBD
	D=M
	@BLACKSTART
	D;JNE
	@WHITEIDLE
	0;JMP
(BLACKIDLE)
	@KBD
	D=M
	@WHITESTART
	D;JEQ
	@BLACKIDLE
	0;JMP

(WHITESTART)
	@SCREEN
	D=A
	@p
	M=D
(WHITELOOP)
	@p
	A=M
	M=0
	@p
	M=M+1
	D=M
	@24576
	D=D-A
	@WHITEIDLE
	D;JEQ
	@KBD
	D=M
	@BLACKSTART
	D;JNE
	@WHITELOOP
	0;JMP

(BLACKSTART)
	@SCREEN
	D=A
	@p
	M=D
(BLACKLOOP)
	@p
	A=M
	M=-1
	@p
	M=M+1
	D=M
	@24576
	D=D-A
	@BLACKIDLE
	D;JEQ
	@KBD
	D=M
	@WHITESTART
	D;JNE
	@BLACKLOOP
	0;JMP
