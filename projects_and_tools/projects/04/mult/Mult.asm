// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Mult.asm

// Multiplies R0 and R1 and stores the result in R2.
// (R0, R1, R2 refer to RAM[0], RAM[1], and RAM[3], respectively.)

// Put your code here.
	@R2
	M=0
	@R0
	D=M
	@END
	D;JEQ	//factor = 0?
	@R1
	D=M
	@END
	D;JEQ	//factor = 0?
	@NEGLOOP
	D;JLT	//counter < 0?
(POSLOOP)
	@R0
	D=M
	@R2		//add to sum
	M=D+M
	@R1
	M=M-1	//decr counter
	D=M
	@END
	D;JEQ
	@POSLOOP
	0;JMP
(NEGLOOP)
	@R0
	D=M
	@R2		//add to sum
	M=D+M
	@R1
	M=M+1	//incr counter
	D=M
	@NEGRES
	D;JEQ
	@NEGLOOP
	0;JMP
(NEGRES)
	@R2
	M=-M
(END)
	@END
	0;JMP
