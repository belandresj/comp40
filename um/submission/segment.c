/*
 *	filename: segement.c
 *      by Yucheng He and Griffin Richards
 *	11/23/2015
 *	Inculde the interface for Segment_T
 */

#include "segment.h"
#include "assert.h"

/* create a new segment with the size of nums */
Segment_T Seg_new(int nums)
{
	return Seq_new(nums);
}

/* free the seg */
void Seg_free(Segment_T seg)
{
	assert(seg != NULL);
	Seq_free(&seg);
	return;
}

/* store value in the seg at index i */
void Seg_put(Segment_T seg, uint32_t i, uint32_t val)
{
	/* rely on the assert in Seq_put for check the bound */
	Seq_put(seg, i, (void*) (uintptr_t) val);
	return;
}

/* get the value at the seg at index i */
uint32_t Seg_get(Segment_T seg, uint32_t i)
{
	return (uint32_t) (uintptr_t) Seq_get(seg, i);
}

/* add a uint32_t into a seg */
void addInstr(Segment_T seg, uint32_t instr)
{
	Seq_addhi(seg, (void*)(uintptr_t) instr);
	return;
}
