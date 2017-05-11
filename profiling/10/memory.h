/*
 *	filename: memory.h
 *      by Yucheng He and Griffin Richards
 *	11/23/2015
 *	Inculde the interface for struct Mem
 */

#ifndef MEMORY_H_
#define MEMORY_H_

#include <seq.h>
#include <stdint.h>
#include <stdio.h>
#include <segment.h>
#include <assert.h>

typedef struct Stack_T {
	uint32_t* stack;
	int stackLen;
	int stackSize;
} *Stack_T;

Stack_T Stack_new();

void Stack_free(Stack_T myStack);


void Stack_push(Stack_T myStack, uint32_t index);


uint32_t Stack_pop(Stack_T myStack);

/* the struct for Mem */
typedef struct Mem {
	Segment_T* segs;
	Stack_T unmapped;
	int prog_counter;
	int segLen;
	int segSize;
} *Mem;





/* initialize the new mem and return it */
Mem new_mem(Segment_T program);

/* free the mem */
void free_mem(Mem* myMem);

/* map a new segment with the length of nums */
uint32_t map_segment(Mem myMem, uint32_t nums);

/* unmap the segment (push the index into unmapped stack)*/
void unmap_segment(Mem myMem, uint32_t index);

/* load the val at myMem[rb][rc] */
uint32_t load_segment(Mem myMem, uint32_t rb, uint32_t rc);

/* store the val into myMem[rb][rc] */
void store_segment(Mem myMem, uint32_t val, uint32_t rb, uint32_t rc);

#endif