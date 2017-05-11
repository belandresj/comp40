/*
 *	filename: uarray2.c
 *	by: Yucheng He, Jun Wang 	
 *	Hw3 
 *	the souce file for uarray2
 */

# include "uarray.h"
# include "uarray2.h"
# include <stdio.h>
# include <stdlib.h>
# include "assert.h"
# include "mem.h"

#define T UArray2_T

/*
 *      The struct for the 2-d uarray
 *      width: number of elements in one row, at least 0
 *      height: number of elements in one column, at least 0
 *      uarray: the 1D UArray to represant the 2D UArray2
 */
struct T {
        int width;
        int height;
        UArray_T uarray;
};

/* 
 *      Allocate the memory for UArray2 (width * height * size)
 *      Set the value to the struct member variable              
 */
T UArray2_new(int width, int height, int size)
{
        int length = width * height;
        assert((width > 0 && height > 0) 
               || (width == 0 && height == 0)); 
        T uarray2;
        NEW(uarray2);
        uarray2->uarray = UArray_new(length, size);
        uarray2->width = width;
        uarray2->height = height;
        return uarray2;
}

/*      
 *      Free the memory for UArray2 
 */
void UArray2_free(T *uarray2P)
{
        assert(uarray2P != NULL && *uarray2P != NULL);
        UArray_free(&((*uarray2P)->uarray));
        FREE(*uarray2P);
}

/*      
 *      Get the width of UArray2 
 */
int UArray2_width(T uarray2)
{
        assert(uarray2 != NULL);
        return uarray2->width;
}

/*      
 *      Get the height of UArray2 
 */
int UArray2_height(T uarray2)
{
        assert(uarray2 != NULL);
        return uarray2->height;
}

/*      
 *      Get the size of the elements 
 */
int UArray2_size(T uarray2)
{       
        assert(uarray2 != NULL);
        return UArray_size(uarray2->uarray);
}

/*      
 *      Get the pointer to the element store in [i, j]
 */
void *UArray2_at(T uarray2, int i, int j)
{
        assert(uarray2 != NULL);
        assert(i >= 0 && i < uarray2->width);
        assert(j >= 0 && j < uarray2->height);
        int index = (j * uarray2->width) + i;
        return UArray_at(uarray2->uarray, index);
}

/* 
 *      Call the function apply at the order of column increase first, then
 *      row. 
 */
void UArray2_map_row_major(T uarray2, 
                           void apply(int i, 
                                      int j, 
                                      T uarray2, 
                                      void *p1, 
                                      void *cl), 
                           void *cl)
{
        assert(uarray2 != NULL);
        assert(uarray2->uarray != NULL);
        assert(apply != NULL);
        for (int j = 0; j < uarray2->height; j++) {
                for (int i = 0; i < uarray2->width; i++) {
                        apply(i, j, uarray2, UArray2_at(uarray2, i, j), cl);
                }
        }
}

/*      
 *      Call the function apply at the order of row increase first, then
 *      column 
 */
void UArray2_map_col_major(T uarray2, 
                           void apply(int i, 
                                      int j, 
                                      T uarray2, 
                                      void *p1, 
                                      void *cl), 
                           void *cl)
{
        assert(uarray2 != NULL);
        assert(uarray2->uarray != NULL);
        assert(apply != NULL);
        for (int i = 0; i < uarray2->width; i++) {
                for (int j = 0; j < uarray2->height; j++) {
                        apply(i, j, uarray2, UArray2_at(uarray2, i, j), cl);
                }
        }
}



