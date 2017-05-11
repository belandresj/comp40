/*
 *      filename: a2plain.c
 *      by Jun Wang, Yucheng He, 10/09/2015
 *      hw3 of comp40
 *
 *      This is the source file for a2methods.h which contains all the 
 *      functions for operations of UArray2
 */


#include <stdlib.h>
#include <a2plain.h>
#include <assert.h>
#include "uarray2.h"

/* 
 * define a private version of each function in A2Methods_T that we implement
 */

/* 
 * private abbreviation
 */
typedef A2Methods_UArray2 A2;

/* The constructor function for uarray2*/
static A2 new(int width, int height, int size)
{
	return UArray2_new(width, height, size);
}

/* The constructor function for uarray2*/
static A2 new_with_blocksize(int width, int height, int size,
					    int blocksize)
{
	(void)blocksize;
	return UArray2_new(width, height, size);
}

/* The functions for freeing memory of array2 */
static void a2free(A2 *array2p)
{
        UArray2_free((UArray2_T *) array2p);
}

/* The width, height, and size of the array2 */
static int width(A2 array2)
{
        return UArray2_width(array2);
}

static int height(A2 array2)
{
        return UArray2_height(array2);
}

static int size(A2 array2)
{
        return UArray2_size(array2);
}

/* The data are not stored in block, thus return 1 */
static int blocksize(A2 array2)
{
        (void) array2;
        return 1;
}

/* 
 * The at function
 * Return the pointer of the element at the position (i, j)
 */
static A2Methods_Object *at(A2 array2, int i, int j)
{
        int width = UArray2_width(array2);
        int height = UArray2_height(array2);
        assert(i >= 0 || i < width);
        assert(j >= 0 || j < height);
        return UArray2_at(array2, i, j);
}

/* The mapping functions */
typedef void applyfun(int i, int j, UArray2_T array2, void *elem, void *cl);

static void map_row_major(A2 array2, A2Methods_applyfun apply, void *cl)
{
        UArray2_map_row_major(array2, (applyfun *) apply, cl);
}

static void map_col_major(A2 array2, A2Methods_applyfun apply, void *cl)
{
        UArray2_map_col_major(array2, (applyfun *) apply, cl);
}

/* The small mapping functions */
struct small_closure {
        A2Methods_smallapplyfun *apply;
        void *cl;
};

static void apply_small(int i, int j, UArray2_T array2, void *elem, void *vcl)
{
        struct small_closure *cl = vcl;
        (void) i;
        (void) j;
        (void) array2;
        cl->apply(elem, cl->cl);
}

static void small_map_row_major(A2 a2, A2Methods_smallapplyfun apply,
                                void *cl)
{
        struct small_closure mycl = {apply, cl};
        UArray2_map_row_major(a2, apply_small, &mycl);
}

static void small_map_col_major(A2 a2, A2Methods_smallapplyfun apply,
                                void *cl)
{
        struct small_closure mycl = {apply, cl};
        UArray2_map_col_major(a2, apply_small, &mycl);
}

static struct A2Methods_T uarray2_methods_plain_struct = {
        new,
        new_with_blocksize,
        a2free,
        width,
        height,
        size,
        blocksize,
        at,
        map_row_major,
        map_col_major,
        NULL,
        map_row_major,
        small_map_row_major,
        small_map_col_major,
        NULL,
        small_map_row_major,
};

// finally the payoff: here is the exported pointer to the struct

A2Methods_T uarray2_methods_plain = &uarray2_methods_plain_struct;
