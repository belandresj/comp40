/*
 *	filename: memory.c
 *      by Yucheng He and Griffin Richards
 *	11/23/2015
 *	Inculde the source codes for all the functions in the interface memory.h.
 */

#include "memory.h"

/* initialize the new mem and return it */
Mem new_mem(Segment_T program)
{
	Mem myMem = malloc(sizeof(*myMem));

	assert(myMem != NULL);
	myMem->segs = Seq_new(2);
	
	assert(myMem->segs != NULL);
	Seq_addlo(myMem->segs, (void*) program);
	
	myMem->unmapped = Stack_new();
	myMem->prog_counter = 0;
	return myMem;
}

/* free the mem */
void free_mem(Mem* myMem)
{
	assert(myMem != NULL);
	for (int i = 0; i < Seq_length((*myMem)->segs); i++) {
		Seg_free(Seq_get((*myMem)->segs, i));
	}
	
	Seq_free(&((*myMem)->segs));
	Stack_free(&((*myMem)->unmapped));
	free(*myMem);
}

/* 
 * create a segment with the size of nums, and return the index of the segs of 
 * the mapped segment. There are two conditons, if the unmapped stack is not 
 * empty, we reuse the segment index stored in unmapped stack and pop that out.
 * otherwise we just create a new segments and addhi into segs, the return index
 * is the length of segs.
 */
uint32_t map_segment(Mem myMem, uint32_t nums)
{
	assert(myMem != NULL);
	Segment_T newSeg = Seg_new(nums);
	for (uint32_t i = 0; i < nums; i++) {
	      Seq_addhi((Seq_T)newSeg, (void*) (uintptr_t) 0);
	}
	uint32_t index;
	if (Stack_empty(myMem->unmapped) == 1) {
		Seq_addhi(myMem->segs, (void*) newSeg);
		index = Seq_length(myMem->segs) - 1;
		
	} else {
		index = (int) (uintptr_t) Stack_pop(myMem->unmapped);
		Seg_free(Seq_put(myMem->segs, index, (void*) newSeg));
	};
	return index;
}

/* unmap the segment (push the index into unmapped stack)*/
void unmap_segment(Mem myMem, uint32_t index)
{
	assert(myMem != NULL);
	Stack_push(myMem->unmapped, (void*) (uintptr_t) index);
}

/* load the val at myMem[rb][rc] */
uint32_t load_segment(Mem myMem, uint32_t rb, uint32_t rc)
{
	assert(myMem != NULL);
	Segment_T seg = (Segment_T) Seq_get(myMem->segs, rb);
	return Seg_get(seg, rc);
  
}

/* store the val into myMem[rb][rc] */
void store_segment(Mem myMem, uint32_t ra, uint32_t rb, uint32_t val)
{

	assert(myMem != NULL);
	Segment_T seg = (Segment_T) Seq_get(myMem->segs, ra);
	Seg_put(seg, rb, val);
	return;
}