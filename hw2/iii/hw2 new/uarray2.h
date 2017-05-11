/*
 *  filename: uarray2.h
 *  by Jun Wang, Yucheng He, 09/22/2015
 *  hw2-design, comp40
 *
 *  This file is the interface of the uarray2 2D UArray
 *  
 */

# include <stdio.h>
# include <stdlib.h>


#ifndef UARRAY2_INCLUDED
#define UARRAY2_INCLUDED
#define T UArray2_T
typedef struct T* T;

/* Create the new struct T, include all the member variable in T */
extern T UArray2_new(int width, int height, int size);


/* Free the memory for UArray2 */
extern void UArray2_free(T *uarry2P);

/* Get the width of UArray2 */
extern int UArray2_width(T uarray2);

/* Get the height of UArray2 */
extern int UArray2_height(T uarray2);


/* Get the size of the elements */
extern int UArray2_size(T uarray2);

/* Get the pointer to the element store in [i, j] */
extern void *UArray2_at(T uarray2, int i, int j);

/* 
 *      Call the function apply at the order of column increase first, then
 *      row. 
 */
extern void UArray2_map_row_major(T uarray2, 
                           void apply(int i, 
                                      int j, 
                                      T uarray2, 
                                      void *val, 
                                      void *cl), 
                           void *cl);
/* 
 *      Call the function apply at the order of row increase first, then
 *      column
 */
extern void UArray2_map_col_major(T uarray2, 
                           void apply(int i, 
                                      int j, 
                                      T uarray2, 
                                      void *val, 
                                      void *cl), 
                           void *cl);
/*
 *      Call the function apply by blocks
 */
extern void UArray2_map_block_major(T uarray2,
                                    void apply(int i,
                                               int j,
                                               T uarray2,
                                               void *val,
                                               void *cl),
                                    void *cl);
/*
 *      Call the function apply to visit one block
 */
extern void visit_one_block(T uarray2,
                            int i,
                            int j,
                            void apply(int i,
                                       int j,
                                       T urray2,
                                       void* val,
                                       void* cl),
                            void* cl);





#undef T
#endif
