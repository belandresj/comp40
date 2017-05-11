/*
 *	filename: execute.c
 *      by Yucheng He and Griffin Richards
 *	11/23/2015
 * 	Include the source code for execute function and all the helper 
 *      functions for execute.
 */

#include "execute.h"
#include "memory.h"
#include "segment.h"

/* global array of register, at first all zeros */
uint32_t R[8] = {0};

/* contional move instruction */
void cMove(uint32_t a, uint32_t b, uint32_t c)
{
	if(R[c] != 0) {
		R[a] = R[b];
	}
	return;
}

/* add instruction */
void add(uint32_t a, uint32_t b, uint32_t c)
{

	R[a] = (R[b] + R[c]) % 4294967296;
	return;
}

/* multiplication instruction */
void multiply(uint32_t a, uint32_t b, uint32_t c)
{
	R[a] = (R[b] * R[c]) % 4294967296;
	return;
}

/* division instruction */
void divide(uint32_t a, uint32_t b, uint32_t c)
{
	R[a] = R[b] / R[c];
	return;
}

/* Bitwise NAND instruction */
void bitwiseNAND(uint32_t a, uint32_t b, uint32_t c)
{
	R[a] = ~(R[b] & R[c]);
	return;
}

/* halt instruction */
void halt(Mem myMem)
{	
	free_mem(&myMem);
	exit(0);
}

/* ouput instruction */
void output(uint32_t c)
{
	/* output should be in 0 - 255 */
	assert(R[c] < 256);
	/* use putchar to output character */
	putchar(R[c]);
	return;
}

/* ouput instruction */
void input(FILE* instrs, uint32_t c)
{
	uint32_t instr = getc(instrs);
	if (instr == (uint32_t) EOF) {
		instr = - 1;
	}
	R[c] = instr;
}

/* load program instruction */
void loadProgram(Mem myMem, uint32_t b, uint32_t c)
{
	/* if Rb is not 0, copy the M[Rb] into M[0]*/
	if (R[b] != 0) {
		Segment_T temp = Seq_get(myMem->segs, R[b]);
		uint32_t length = Seq_length(temp);
		Segment_T seg = Seg_new(length);
		for (uint32_t i = 0; i < length; i++) {
			addInstr(seg, Seg_get(temp, i));
		}
		Seq_put(myMem->segs, 0, seg);
	}
	/* when Rb is zero, just change the program counter */
	/* program counter is R[c] - 1 (since add 1 in the loop) */
	myMem->prog_counter = R[c] - 1;
	return;
}

void execute(Segment_T program)
{
	/* create the myMem with the program */
	Mem myMem = new_mem(program);
	/*
	 * in while loop read the instruction and execute the instruction, 
	 * until reach halt(), then end the program.
	 */

	while (1) {
		uint32_t word, opcode, a, b, c, val;
		/* load the word with program counter */
		word = load_segment(myMem, 0, myMem->prog_counter);
		uint32_t temp = word;
		/* get the opcode from word, and do corresponding instruction */
		opcode =  temp >> 28;
		if (opcode == 13) {
			/* Get a and val */
			temp = word;
			temp = temp << 4;
			a = temp >> 29;
			temp = word;
			temp = temp << 7;
			val = temp >> 7;
			R[a] = val;
		} else {
			/* Get a, b and c (register index) */
			temp = word;
			temp = temp << 23;
			a = temp >> 29;
			temp = word;
			temp = temp << 26;
			b = temp >> 29;
			temp = word;
			temp = temp << 29;
			c = temp >> 29;
			switch(opcode) {
				case 0 :
					cMove(a, b, c);
					break;
				case 1 :
					R[a] = load_segment(myMem, R[b], R[c]);
					break;
				case 2 :
					store_segment(myMem, R[a], R[b], R[c]);
					break;
				case 3 :
					add(a, b, c);
					break;
				case 4 :
					multiply(a, b, c);
					break;
				case 5 :
					divide(a, b, c);
					break;
				case 6 :
					bitwiseNAND(a, b, c);
					break;
				case 7 :
					halt(myMem);
					break;
				case 8 :
					R[b] = map_segment(myMem, R[c]);
					break;
				case 9 :
					unmap_segment(myMem, R[c]);
					break;
				case 10 :
					output(c);
					break;
				case 11 :
					input(stdin, c);
					break;
				case 12 :
					loadProgram(myMem, b, c);
					break;
			}
		}
		(myMem->prog_counter)++;
	}      
}



