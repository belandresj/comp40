/*
 *      filename: bit2.c
 *      by Jun Wang, Yucheng He, 09/21/2015
 *      hw2 in comp40
 *
 *      file summary
 */

#include <stdio.h>
#include "assert.h"
#include "bit2.h"
#include "mem.h"

#define T Bit2_T

/*
 *      This struct is used to store the width and the height of the 2-d 
 *      bit array
 *      width: The number of elements in one row, at least 0
 *      height: The number of elements in one column, at least 0
 *      bit: the 1-d bit array
 */
struct T {
        int width;
        int height;
        Bit_T bit;
};

/* 
 *      Creat a new 2-d bit array.
 */
T Bit2_new(int width, int height) 
{
/*
 *      length is the length of the bit array
 */
        int length = width * height;
        assert((width > 0 && height > 0) || (width == 0 && height == 0));
        T bit2; 
        NEW(bit2);
        bit2->bit = Bit_new(length);
        bit2->width = width;
        bit2->height = height;
        return bit2;
}

/* 
 *      Free the memory allocated to the 2-d bit array.
 */
void Bit2_free(Bit2_T *bit2P)
{
        assert(bit2P != NULL && *bit2P != NULL);
        Bit_free(&((*bit2P)->bit));
        FREE(*bit2P);
}

/* 
 *      Get the width of the 2-d bit array.
 */
int Bit2_width(Bit2_T bit2)
{
        assert(bit2 != NULL);
        return bit2->width;
}

/* 
 *      Get the height of the 2-d bit array.
 */
int Bit2_height(Bit2_T bit2)
{
        assert(bit2 != NULL);
        return bit2->height;
}

/* 
 *      Store the value of n at the entry(i,j) in the 2-d bit array.
 */
int Bit2_put(Bit2_T bit2, int i, int j, int n)
{
        assert(bit2 != NULL);
        assert((i >= 0 && i < bit2->width) && (j >= 0 && j< bit2->height));
        assert(n == 1 || n == 0);
        int index = i + j * bit2->width;
        int prev = Bit_put(bit2->bit, index, n);
        return prev;
}

/*      
 *      Retrieve the value at the entry(i,j) in the 2-d bit array.
 */
int Bit2_get(Bit2_T bit2, int i, int j)
{
        assert(bit2 != NULL);
        assert((i >= 0 && i < bit2->width) && (j >= 0 && j< bit2->height));
        int index =  i + j * bit2->width;
        int val = Bit_get(bit2->bit, index);
        return val;
}

/* 
 *      Call the apply function for all entries in the 2-d bit array 
 *      according to row major.
 */
void Bit2_map_row_major(Bit2_T bit2, void apply(int i, int j, Bit2_T bit2,
                                                 int n, void *cl), void *cl)
{
        assert(bit2 != NULL);
        int i;
        int j;
        for (j = 0; j < bit2->height; j++) {
                for (i = 0; i < bit2->width; i++)
                        apply(i, j, bit2, Bit2_get(bit2, i, j), cl);
        }
                    
}

/* 
 *      Call the apply function for all entries in the 2-d bit array 
 *      according to column major.
 */
void Bit2_map_col_major(Bit2_T bit2, void apply(int i, int j, Bit2_T bit2, 
                                                 int n, void *cl), void *cl)
{
        assert(bit2 != NULL);
        int i;
        int j;
        for (i = 0; i < bit2->width; i++) {
                for (j = 0; j < bit2->height; j++)
                        apply(i, j, bit2, Bit2_get(bit2, i, j), cl);
        }
}

