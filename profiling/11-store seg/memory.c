/*
 *	filename: memory.c
 *      by Yucheng He and Griffin Richards
 *	11/23/2015
 *	Inculde the source codes for all the functions in the interface 
 *      memory.h.
 */

#include "memory.h"

Stack_T Stack_new()
{
	Stack_T newStack = malloc(sizeof(*newStack));

	newStack->stack = malloc(sizeof(uint32_t) * 100);
	newStack->stackSize = 100;
	newStack->stackLen = 0;
	return newStack;
}

void Stack_free(Stack_T myStack)
{
	assert(myStack != NULL && myStack->stack != NULL);
	free(myStack->stack);
	free(myStack);
	return;
}

void Stack_push(Stack_T myStack, uint32_t index)
{
	assert(myStack != NULL && myStack->stack != NULL);
	int len = myStack->stackLen;
	int size = myStack->stackSize;
	if (len == size) {
		uint32_t* newStack = malloc(sizeof(uint32_t) * size * 2);
		for (int i = 0; i < len; i++) {
			newStack[i] = (myStack->stack)[i];
		}
		free(myStack->stack);
		myStack->stack = newStack;
		myStack->stackSize = size * 2;
	}
	(myStack->stack)[len] = index;
	myStack->stackLen = len + 1;
	return;

}

uint32_t Stack_pop(Stack_T myStack)
{
	assert(myStack != NULL && myStack->stack != NULL);
	assert(myStack->stackLen != 0);
	int len = myStack->stackLen;
	uint32_t i = (myStack->stack)[len- 1];
	myStack->stackLen = len - 1;
	return i;
}

/* initialize the new mem and return it */
Mem new_mem(Segment_T program)
{
	Mem myMem = malloc(sizeof(*myMem));
	assert(myMem != NULL);
	myMem->segs = malloc(sizeof(Segment_T) * 100);
	assert(myMem->segs != NULL);
	(myMem->segs)[0] = program;
	myMem->segLen = 1;
	myMem->segSize = 100;
	myMem->unmapped = Stack_new();
	myMem->prog_counter = 0;
	return myMem;
}

/* free the mem */
void free_mem(Mem* myMem)
{
	assert(myMem != NULL);
	for (int i = 0; i < (*myMem)->segLen; i++) {
		Seg_free(((*myMem)->segs)[i]);
	}
	free(((*myMem)->segs));
	Stack_free((*myMem)->unmapped);
	free(*myMem);
}

void addSeg(Mem myMem, Segment_T seg)
{
	assert(myMem != NULL);
	int len = myMem->segLen;
	int size = myMem->segSize;
	if (len == size) {
		Segment_T* newSegs = malloc(sizeof(Segment_T) * size * 2);
		for (int i = 0; i < len; i++) {
			newSegs[i] = (myMem->segs)[i];
		}
		free(myMem->segs);
		myMem->segs = newSegs;
		myMem->segSize = size * 2;
	}
	(myMem->segs)[len] = seg;
	myMem->segLen = len + 1;
	return;
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
	      (newSeg->instrs)[i] = 0;
	}
	newSeg->length = nums;
	uint32_t index;
	if (myMem->unmapped->stackLen == 0) {
		addSeg(myMem, newSeg);
		index = myMem->segLen - 1;
	} else {
		index = Stack_pop(myMem->unmapped);
		Seg_free((myMem->segs)[index]);
		(myMem->segs)[index] = newSeg;
	};
	return index;
}

/* unmap the segment (push the index into unmapped stack)*/
void unmap_segment(Mem myMem, uint32_t index)
{
	assert(myMem != NULL);
	Stack_push(myMem->unmapped, index);
}

/* load the val at myMem[rb][rc] */
uint32_t load_segment(Mem myMem, uint32_t rb, uint32_t rc)
{
	assert(myMem != NULL);
	return ((myMem->segs)[rb]->instrs)[rc];
  
}

/* store the val into myMem[rb][rc] */
void store_segment(Mem myMem, uint32_t ra, uint32_t rb, uint32_t val)
{

	assert(myMem != NULL);
	((myMem->segs)[ra]->instrs)[rb] = val;
	return;
}
