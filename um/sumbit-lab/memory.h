/*
 *	filename: memory.h
 *      by Yucheng He and Griffin Richards
 *	11/15/2015
 *	Inculde the interface for struct Mem
 */

#ifndef MEMORY_H_
#define MEMORY_H_

#include <seq.h>
#include <stdint.h>
#include <stdio.h>
#include <stack.h>
#include <segment.h>
#include <assert.h>


typedef struct Mem {
	Seq_T segs;
	Stack_T unmapped;
	uint32_t prog_counter;
} *Mem;


/* initialize the new mem and return it */
Mem new_mem(Segment_T program);

/* free the mem */
void free_mem(Mem* myMem);

/* 
 * create a segment with the size of nums, and return the index of the segs of 
 * the mapped segment. There are two conditons, if the unmapped stack is not 
 * empty, we reuse the segment index stored in unmapped stack and pop that out.
 * otherwise we just create a new segments and addhi into segs, the return index
 * is the length of segs.
 */
uint32_t map_segment(Mem myMem, uint32_t nums);

/* unmap the segment (push the index into unmapped stack)*/
void unmap_segment(Mem myMem, uint32_t index);

/* load the val at myMem[rb][rc] */
uint32_t load_segment(Mem myMem, uint32_t rb, uint32_t rc);

/* store the val into myMem[rb][rc] */
void store_segment(Mem myMem, uint32_t val, uint32_t rb, uint32_t rc);

#endif