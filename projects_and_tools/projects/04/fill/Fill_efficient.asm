// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Fill.asm

// Runs an infinite loop that listens to the keyboard input. 
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel. When no key is pressed, the
// program clears the screen, i.e. writes "white" in every pixel.

// Put your code here.
(START)
	@SCREEN		//set p to screen base address
	D=A
	@p
	M=D-1
(LOOP)
	@p			//end of screen?:
	D=M	
	@24575
	D=D-A
	@START		//reset pointer
	D;JEQ
	@KBD		//key check
	D=M
	@MAKEBLACK	//blacken
	D;JNE
	@p			//or whiten:
	M=M+1		//incr pointer
	A=M			//dereference
	M=0			//write
	@LOOP
	0;JMP
(MAKEBLACK)
	@p
	M=M+1
	A=M
	M=-1
	@LOOP
	0;JMP
