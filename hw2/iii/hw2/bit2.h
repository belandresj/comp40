/*
 *	filename: bit2.h
 *	by Jun Wang, Yucheng He, 09/22/2015
 *	hw2 of comp40
 *
 *	This is the header file of the 2-d unboxed bit array (bit2)
 */

#ifndef BIT2_INCLUDED
#define BIT2_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include "bit.h"

#define T Bit2_T
typedef struct T *T;

/* Creat a new 2-d bit array.*/
extern T Bit2_new (int width, int height);

/* Free the memory allocated to the 2-d bit array.*/
extern void Bit2_free (Bit2_T *bit2P);

/* Get the width of the 2-d bit array.*/
extern int Bit2_width (Bit2_T bit2);

/* Get the height of the 2-d bit array.*/
extern int Bit2_height (Bit2_T bit2);

/* Store the value of n at the entry(i,j) in the 2-d bit array.*/
extern int Bit2_put (Bit2_T bit2, int i, int j, int n);

/* Retrieve the value at the entry(i,j) in the 2-d bit array.*/
extern int Bit2_get (Bit2_T bit2, int i, int j);

/* 
 * Call the apply function for all entries in the 2-d bit array according to
 * row major.
 */
extern void Bit2_map_row_major (Bit2_T bit2, 
				void apply(int i, int j, Bit2_T bit2,
                                           int n, void *cl), 
				void *cl);

/* 
 * Call the apply function for all entries in the 2-d bit array according to
 * column major.
 */
extern void Bit2_map_col_major (Bit2_T bit2, 
				void apply(int i, int j, Bit2_T bit2, 
                                           int n, void *cl), 
				void *cl);


#undef T
#endif