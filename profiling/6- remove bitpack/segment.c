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
	assert(nums > 0);
	Segment_T newSeg = malloc(sizeof(*newSeg));
	assert(newSeg != NULL);
	newSeg->instrs = malloc(sizeof(uint32_t) * nums);
	assert(newSeg->instrs != NULL);
	newSeg->length = 0;
	newSeg->size = nums;
	return newSeg;
}

/* free the seg */
void Seg_free(Segment_T seg)
{
	assert(seg != NULL);
	free(seg->instrs);
	free(seg);
	return;
}

/* store value in the seg at index i */
void Seg_put(Segment_T seg, uint32_t i, uint32_t val)
{
	/* rely on the assert in Seq_put for check the bound */
	assert(seg != NULL);
	assert(i < seg->length);
	(seg->instrs)[i] = val;
	return;
}

/* get the value at the seg at index i */
uint32_t Seg_get(Segment_T seg, uint32_t i)
{
	assert(seg != NULL);
	assert(i < seg->length);
	return (seg->instrs)[i];
}

/* add a uint32_t into a seg */
void addInstr(Segment_T seg, uint32_t instr)
{
	assert(seg != NULL);
	int length = seg->length;
	int size = seg->size;
	if (size == length) {
		uint32_t* newInstrs = malloc(sizeof(uint32_t) * size * 2);
		for (int i = 0; i < length; i++) {
		  newInstrs[i] = (seg->instrs)[i];
		}
		seg->size = size * 2;
		free(seg->instrs);
		seg->instrs = newInstrs;
	}
	
	(seg->instrs)[length] = instr;
	seg->length = length + 1;
	return;
}
