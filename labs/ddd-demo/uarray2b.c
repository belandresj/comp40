#include "uarray2b.h"
#include "uarray2.h"
#include <stdlib.h>
#include <stdio.h>
#include <uarray.h>
#include <math.h>
#include <assert.h>

#define T UArray2b_T
struct UArray2b_T {
        int height, width, blocksize;
        struct UArray2_T *blocks;
        /* 2D array of blocks where block is represented as UArray_T
         * Element (i, j) is located in blocks[blocki, blockj] at offset n
         * where
         *    blocki = i / blocksize
         *    blockj = j / blocksize
         *    n      = blocksize * (i % blocksize) + (j % blocksize)
         */
};

/* create using a 2d array containing a T array of UArray2_T */
T UArray2b_new(int width, int height, int size, int blocksize)
{
        T blockArray          = malloc(sizeof(*blockArray));
        blockArray->height    = height;
        blockArray->width     = width;
        blockArray->blocksize = blocksize;

        int bwidth = width / blocksize;
        if (width % blocksize != 0)
                bwidth++;
        int bheight = height / blocksize;
        if (height % blocksize != 0)
                bheight++;

        blockArray->blocks = UArray2_new(bwidth, bheight, sizeof(UArray_T));

        for (int i = 0; i < bwidth; i++) {
                for (int j = 0; j < bheight; j++) {
                        UArray_T *pblock = UArray2_at(blockArray->blocks, i, j);
                        *pblock = UArray_new(blocksize * blocksize, size);
                }
        }
        return blockArray;
}

void UArray2b_free(T *array2b)
{
        assert((array2b != NULL) && (*array2b != NULL));
        T a2b = *array2b;
        int bwidth = UArray2_width(a2b->blocks);
        int bheight = UArray2_height(a2b->blocks);
        for (int i = 0; i < bwidth; i++) {
                for (int j = 0; j < bheight; j++) {
                        UArray_free(UArray2_at(a2b->blocks, i, j));
                }
        }
        UArray2_free(&(a2b->blocks));
        free(*array2b);
        *array2b = NULL;
}

T UArray2b_new_64K_block(int width, int height, int size)
{
        //create blocks of up to 64KB
        int blocksize = sqrt(65536 / size);
        return UArray2b_new(width, height, size, blocksize);
}

int UArray2b_height(T array2b)
{
        assert(array2b != NULL);
        return array2b->height;
}

int UArray2b_width(T array2b)
{
        assert(array2b != NULL);
        return array2b->width;
}

int UArray2b_size(T array2b)
{
        assert(array2b != NULL);
        return UArray2_size(array2b->blocks);
}

int UArray2b_blocksize(T array2b)
{
        assert(array2b != NULL);
        return array2b->blocksize;
}

void *UArray2b_at(T array2b, int i, int j)
{
        assert(array2b != NULL);
        int blocksize = array2b->blocksize;
        UArray_T *temp =
            UArray2_at(array2b->blocks, i / blocksize, j / blocksize);
        UArray_T block = *temp;
        return UArray_at(block, blocksize * (i % blocksize) + (j % blocksize));
}

void UArray2b_map(T array2b, void apply(), void *cl)
{
        assert(array2b != NULL);
        int blocksize = array2b->blocksize;
        int max = blocksize * blocksize;
        int bheight = UArray2_height(array2b->blocks);
        int bwidth = UArray2_width(array2b->blocks);
        for (int blockj = 0; blockj < bheight; blockj++) {
                for (int blocki = 0; blocki < bwidth; blocki++) {
                        UArray_T *temp =
                            UArray2_at(array2b->blocks, blocki, blockj);
                        for (int n = 0; n < max; n++) {
                                /* n == blocksize * (i % blocksize) 
                                        + (j % blocksize)
                                   n / blocksize == i % blocksize
                                   n % blocksize == j % blocksize
                                 */
                                int i = blocki * blocksize + n % blocksize;
                                int j = blockj * blocksize + n / blocksize;

                                if (i < array2b->width && j < array2b->height)
                                        apply(i, j, array2b,
                                              UArray_at(*temp, n), cl);
                        }
                }
        }
}
