/*
 *      filename: ppmtrans_helper.c
 *      by Jun Wang, Yucheng He, 10/09/2015
 *      hw3 of comp40
 *
 *      This is the source file which contains all the rotation function which
 *      will be used for ppmtrans program.
 */

#include <stdlib.h>
#include "ppmtrans_helper.h"
#include "pnm.h"

/* Rotate the image by 90 degree */
void rotate90(int i, int j, A2Methods_UArray2 array2, void *elem, void *cl)
{
	/* Define a struct for storing the closure pointer */
        struct a2fun_closure *f = cl;
        int height = f->methods->height(array2);
	/* x and y are the indices for the elements in the new array2 */
        int x = height - j -1;
        int y = i;
        Pnm_rgb temp = f->methods->at(f->newArray2, x, y);
        *temp = *(Pnm_rgb) elem;
}

/* Rotate the image by 180 degree */
void rotate180(int i, int j, A2Methods_UArray2 array2, void *elem, void *cl)
{
	/* Define a struct for storing the closure pointer */
        struct a2fun_closure *f = cl;
        int width = f->methods->width(array2);
        int height = f->methods->height(array2);
	/* x and y are the indices for the elements in the new array2 */
        int x = width - i - 1;
        int y = height - j - 1;
        Pnm_rgb temp = f->methods->at(f->newArray2, x, y);
        *temp = *(Pnm_rgb) elem;
}

/* Rotate the image by 270 degree */
void rotate270(int i, int j, A2Methods_UArray2 array2, void *elem, void *cl)
{
	/* Define a struct for storing the closure pointer */
        struct a2fun_closure *f = cl;
        int width = f->methods->width(array2);
	/* x and y are the indices for the elements in the new array2 */
        int x = j;
        int y = width - 1 - i;
        Pnm_rgb temp = f->methods->at(f->newArray2, x, y);
        *temp = *(Pnm_rgb) elem;
}

/* Rotate the image by 0 degree */
void rotate0(int i, int j, A2Methods_UArray2 array2, void *elem, void *cl)
{
	/* Define a struct for storing the closure pointer */
        struct a2fun_closure *f = cl;
        (void) array2;
        Pnm_rgb temp = f->methods->at(f->newArray2, i, j);
        *temp = *(Pnm_rgb) elem;
}
