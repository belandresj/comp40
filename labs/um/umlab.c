/* This is the code from the lab handout */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "assert.h"
#include <fmt.h>
#include "seq.h"
#include <bitpack.h>



typedef enum Um_opcode {
  CMOV = 0, SLOAD, SSTORE, ADD, MUL, DIV,
  NAND, HALT, ACTIVATE, INACTIVATE, OUT, IN, LOADP, LV
} Um_opcode;

typedef uint32_t Um_instruction;

Um_instruction three_register(Um_opcode op, int ra, int rb, int rc)
{
	Um_instruction word = 0;
	word = Bitpack_newu(word, 4, 28, op);
	word = Bitpack_newu(word, 3, 6, ra);
	word = Bitpack_newu(word, 3, 3, rb);
	word = Bitpack_newu(word, 3, 0, rc);
	return word;	
}

Um_instruction loadval(unsigned ra, unsigned val)
{
	Um_instruction word = 0;
	word = Bitpack_newu(word, 4, 28, 13);
	word = Bitpack_newu(word, 3, 25, ra);
	word = Bitpack_newu(word, 25, 0, val);
	return word;
}

static inline Um_instruction cmove(unsigned ra, unsigned rb, unsigned rc)
{
	return three_register(CMOV, ra, rb, rc);
}

static inline Um_instruction sload(unsigned ra, unsigned rb, unsigned rc)
{
	return three_register(SLOAD, ra, rb, rc);
}

static inline Um_instruction sstore(unsigned ra, unsigned rb, unsigned rc)
{
	return three_register(SSTORE, ra, rb, rc);
}

static inline Um_instruction add(unsigned ra, unsigned rb, unsigned rc)
{
	return three_register(ADD, ra, rb, rc);
}

static inline Um_instruction mul(unsigned ra, unsigned rb, unsigned rc)
{
	return three_register(MUL, ra, rb, rc);
}

static inline Um_instruction division(unsigned ra, unsigned rb, unsigned rc)
{
	return three_register(DIV, ra, rb, rc);
}

static inline Um_instruction nand(unsigned ra, unsigned rb, unsigned rc)
{
	return three_register(NAND, ra, rb, rc);
}

static inline Um_instruction halt(void)
{
	return three_register(HALT, 0, 0, 0);
}

static inline Um_instruction activate(unsigned ra, unsigned rb, unsigned rc)
{
	return three_register(ACTIVATE, ra, rb, rc);
}

static inline Um_instruction inactivate(unsigned ra, unsigned rb, unsigned rc)
{
	return three_register(INACTIVATE, ra, rb, rc);
}

static inline Um_instruction output(unsigned rc)
{
	return three_register(OUT, 0, 0, rc);
}

static inline Um_instruction input(unsigned rc)
{
	return three_register(IN, 0, 0, rc);
}

static inline Um_instruction loadprogram(unsigned rb, unsigned rc)
{
	return three_register(LOADP, 0, rb, rc);
}


enum regs { r0 = 0, r1, r2, r3, r4, r5, r6, r7 };

extern void Um_write_sequence(FILE *output, Seq_T stream)
{
	for(int i = 0; i < Seq_length(stream); i++) {
		Um_instruction word = (uintptr_t) Seq_get(stream, i);
		for (int j = 24; j >= 0; j -= 8) {
			fputc(Bitpack_getu(word, 8, j), output);
		}
	}
}



static inline void emit(Seq_T stream, Um_instruction inst)
{
	assert(sizeof(inst) <= sizeof(uintptr_t));
	Seq_addhi(stream, (void *)(uintptr_t) inst);
}

static void emit_out_string(Seq_T stream, const char *s, int aux_reg)
{
	int i = 0;
	while (s[i] != '\0') {
		emit(stream, loadval(aux_reg, s[i]));
		emit(stream, output(aux_reg));
		i++;
	}
}

static inline Um_instruction get_inst(Seq_T stream, int i)
{
	assert(sizeof(Um_instruction) <= sizeof(uintptr_t));
	return (Um_instruction) (uintptr_t) (Seq_get(stream, i));
}

static inline void put_inst(Seq_T stream, int i, Um_instruction inst)
{
	assert(sizeof(inst) <= sizeof(uintptr_t));
	Seq_put(stream, i, (void *)(uintptr_t) inst);
}

static void add_label(Seq_T stream, int location_to_patch, int label_value)
{
	Um_instruction inst = get_inst(stream, location_to_patch);
	unsigned k = Bitpack_getu(inst, 25, 0);
	inst = Bitpack_newu(inst, 25, 0, label_value + k);
	put_inst(stream, location_to_patch, inst);
}

void emit_halt_test(Seq_T stream)
{
	emit(stream, halt());
	emit(stream, loadval(r1, 'B'));
	emit(stream, output(r1));
	emit(stream, loadval(r1, 'a'));
	emit(stream, output(r1));
	emit(stream, loadval(r1, 'd'));
	emit(stream, output(r1));
	emit(stream, loadval(r1, '!'));
	emit(stream, output(r1));
	emit(stream, loadval(r1, '\n'));
	emit(stream, output(r1));
}


void emit_goto_test(Seq_T stream)
{
	int patch_L = Seq_length(stream);
	emit(stream, loadval(r7, 0));	     /* will be patched to 'r7 := L' */
	emit(stream, loadprogram(r0, r7));   /* should goto label L          */
	emit_out_string(stream, "GOTO failed.\n", r1);
	emit(stream, halt());
	/* define 'L' to be here */
	add_label(stream, patch_L, Seq_length(stream));	
	emit_out_string(stream, "GOTO passed.\n", r1);
	emit(stream, halt());
}


void emit_loadval_test(Seq_T stream)
{
	emit(stream, loadval(r1, 'T'));
	emit(stream, output(r1));
	emit(stream, loadval(r1, '\n'));
	emit(stream, output(r1));
	emit(stream, halt());
}

void emit_input_test(Seq_T stream)
{
	emit(stream, input(r1));
	emit(stream, output(r1));
	emit(stream, loadval(r1, '\n'));
	emit(stream, output(r1));
	emit(stream, halt());
}
