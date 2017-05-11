/*	
 *	uarray2b.c - the interface for UArray2b_T
 *	Author: Yucheng He, Jun Wang
 *	Date: 10/09/2015
 *		
 *	We use UArray2_T to represant the array of block, and 
 *	we use UArray_T to represant the elements in each block.
 *	UArray2b_T represant the image.
 */

#include <stdlib.h>
#include <stdio.h>
#include "uarray2b.h"
#include "uarray.h"
#include "uarray2.h"
#include <math.h>
#include "assert.h"
# include "mem.h"
#define T UArray2b_T
void allocateBlock(int i, int j, UArray2_T uarray2, void *elem, void *cl);
void UArray2b_free(T* array2b);
void applyBlock(UArray2b_T array2b, int block_i, int block_j,
		void apply(int i, int j, T array2b, void* elem, void* cl),
		void* cl);

struct T {
	int width;
	int height;
	int blocksize;
	int size;
	UArray2_T uarray2;

};

/*
 * allocate the memory for each block (uarray) 
 */
void allocateBlock(int i, int j, UArray2_T uarray2, void *elem, void *cl)
{
        (void) i; 
	(void) j;
	(void) uarray2;
	T array2b = *(T*) cl;
	/*
	 * set the size and length of uarray, and allocate the memory
	 */
	 int size = array2b->size;
	 int length = array2b->blocksize * array2b->blocksize;
	 *(UArray_T*) elem = UArray_new(length, size);
}

/* 
 * allcate the memory of uarray2b and create the new uarary2b
 */
T UArray2b_new(int width, int height, int size, int blocksize)
{		
	assert((width > 0 && height > 0 && blocksize > 0) 
               || (width == 0 && height == 0 && blocksize == 0));
        assert(size > 0); 
        /*
	 * set the width, height and size of uarray2(array of block), 
	 */
	int uarray2_width =  ceil((double)width / blocksize);
	int uarray2_height = ceil((double)height / blocksize);
	int uarray2_size = blocksize * blocksize * size;
	/*
	 * create the struct uarray2b and assign the member variable
	 */
	struct T* uarray2b;
	NEW(uarray2b);
	uarray2b->width = width;
	uarray2b->height = height;
	uarray2b->blocksize = blocksize;
	uarray2b->size = size;
	uarray2b->uarray2 = UArray2_new(uarray2_width, 
						 uarray2_height, 
						 uarray2_size);
	/*
	 * call UArray2_map to allocate memory for each block (uarray) 
	 */
	UArray2_map_row_major(uarray2b->uarray2, allocateBlock, &uarray2b);
	return uarray2b;
}

/* 
 * new blocked 2d array: blocksize as large as possible provided
 * block occupies at most 64K (if possible)
 */
T UArray2b_new_64K_block(int width, int height, int size)
{
	/*
	 * set the blocksize and call Uarray2b_new.
	 */
	int blocksize = 0;
	if (size > 65536) {
		blocksize = 1;
	} else {
		blocksize = sqrt(65536 / size);
	}
	return UArray2b_new(width, height, size, blocksize);
		
}

/*
 * free the memory of one block (uarray)
 */
void freeUArray(int i, int j, UArray2_T uarray2, void *elem, void *cl)
{
        (void) i; 
        (void) j;
        (void) cl; 
	(void) uarray2;
	UArray_T* temp =  (UArray_T*) elem;
	UArray_free(temp);

}
/*
 * free the memory for uarray2b
 * free block first, then the array of block (uarray2), then array2b
 */
void UArray2b_free(T* array2b)
{
        assert(array2b != NULL && *array2b != NULL);
        UArray2_map_row_major((*array2b)->uarray2, freeUArray, NULL);
        UArray2_free(&((*array2b)->uarray2));
        FREE(*array2b);
}

int UArray2b_width(T array2b)
{
	return array2b->width;
}

int UArray2b_height(T array2b)
{
	return array2b->height;
}

int UArray2b_size(T array2b)
{
	return array2b->size;
}

int UArray2b_blocksize(T array2b)
{
	return array2b->blocksize;
}

/* 
 * return a pointer to the cell in column i, row j;
 * index out of range is a checked run-time error
 */
void* UArray2b_at(T array2b, int i, int j)
{
	assert(array2b != NULL);
        assert(i >= 0 && i < array2b->width);
        assert(j >= 0 && j < array2b->height);
	/*
	 * get the index of block:(block_i, block_j)
	 * then find the block array using UArray2_at
	 */
	int block_i = i / array2b->blocksize;
	int block_j = j / array2b->blocksize;
	UArray_T* block_array = UArray2_at(array2b->uarray2, block_i, block_j);
	/* 		
	 * cell_i and cell_j is the index of uarray, for finding the 
 	 * cell's value using UArray_at.
 	 */
	int cell_i = i % array2b->blocksize;
	int cell_j = j % array2b->blocksize;
	int cell_index = cell_j * array2b->blocksize + cell_i;
	return UArray_at(*block_array, cell_index);
}

/* 
 * apply function for each block(uarray)
 */
void applyBlock(UArray2b_T array2b, int block_i, int block_j,
		void apply(int i, int j, T array2b, void* elem, void* cl), 
		void* cl)
{
	/*
	 * find the edge index of i and j
	 */
	int end_i = 0;
	int end_j = 0;
	if (block_i == UArray2_width(array2b->uarray2) - 1) {
                end_i = array2b->width - 1;
	} else {
		end_i = (array2b->blocksize * (block_i + 1)) - 1;
	}
	if (block_j == UArray2_height(array2b->uarray2) - 1) {
		end_j = array2b->height - 1;
	} else {
		end_j = (array2b->blocksize * (block_j + 1)) - 1;
	}
		
	/*
	 * map the apply function for each element in block
	 */
	for (int j = array2b->blocksize * block_j; j <= end_j; j++ ) {
		for (int i = array2b->blocksize * block_i; i <= end_i; i++) {
			apply(i, j, array2b, UArray2b_at(array2b, i, j), cl);
		}
	}
}


/* 
 * visits every cell in one block before moving to another block 
 */
void UArray2b_map(T array2b, 
		  void apply(int i, int j, T array2b, 
			     void* elem, void* cl),				              
	          void* cl)
{
	assert(array2b != NULL);
        assert(array2b->uarray2 != NULL);
        assert(apply != NULL);
        /*
	 * call applyBlock one block by one block
	 */

        UArray2_T uarray2 = array2b->uarray2;
        for (int block_j = 0; block_j < UArray2_height(uarray2); block_j++) {
	  	for (int block_i = 0; block_i < UArray2_width(uarray2);
		     block_i++) {
        		     applyBlock(array2b, block_i, block_j, apply, cl);
        	}
        }
} 

