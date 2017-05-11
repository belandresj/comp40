/*
 *      filename: ppmtrans_helper.h
 *      by Jun Wang, Yucheng He, 10/09/2015
 *      hw3 of comp40
 *
 *      This is the header file for ppmtrans_helper.c file which declare a 
 *      struct named a2fun_closure as the closure pointer for map function and
 *      the rotation functions at different degrees.
 */

#ifndef ____ppmtrans_helper__
#define ____ppmtrans_helper__

#include <stdio.h>
#include "a2methods.h"
#include "pnm.h"

/*
 * This struct is the closure pointer for all the map function.
 *
 * methods is used to store the methods for map function (plain or block).
 *
 * newArray2 is the array2 used to store the pixels of the image after 
 * transformation.
 */
struct a2fun_closure {
        A2Methods_T methods;
        A2Methods_UArray2 newArray2;
};


void rotate90(int i, int j, A2Methods_UArray2 array2, void *elem, void *cl);
void rotate180(int i, int j, A2Methods_UArray2 array2, void *elem, void *cl);
void rotate270(int i, int j, A2Methods_UArray2 array2, void *elem, void *cl);
void rotate0(int i, int j, A2Methods_UArray2 array2, void *elem, void *cl);

#endif /* defined(____ppmtrans_helper__) */
