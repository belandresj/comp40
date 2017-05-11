/*
 *     
 *	filename: sudoku.c
 *	by Yucheng He, Jun Wang, 09/28/2015 
 *	HW2 1 comp40
 *
 *	The sudoku.c contains all the functions needed for sudoku
 *	to check the sudoku is solved or not.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "pnmrdr.h"
#include "uarray2.h"
#include "uarray.h"

void checkSolved(UArray2_T myUArray2, int denominator);
void checkRow(int i, int j, UArray2_T myUArray2, void* val, void* cl);
void checkCol(int i, int j, UArray2_T myUArray2, void* val, void* cl);
void checkBlock(int i, int j, UArray2_T myUArray2, void* val, void* cl);
void checkZero(int i, int j, UArray2_T myUArray2, void* val, void* cl);
void resetUArray(UArray_T uarray);


int main(int argc, char* argv[])
{
	FILE* fp = NULL;
/*      
 *      check the input file number 
 */
	if (argc > 2) {
		exit(EXIT_FAILURE);
	} else if(argc == 1) {
		fp = stdin;
	} else {
		fp = fopen(argv[1], "rb");
	}
/*      
 *      read the image file using Pnmrdr 
 */
	Pnmrdr_T myPnmrdr = Pnmrdr_new(fp);
	Pnmrdr_mapdata myData = Pnmrdr_data(myPnmrdr);
	UArray2_T myUArray2 = UArray2_new(9, 9, sizeof(int));
	
/* 
 *      read the first 81 int into myUArray2 
 */
	for (int j = 0; j < 9; j++) {
		for (int i = 0; i < 9; i++) {
			int sudokuData = Pnmrdr_get(myPnmrdr);
			*(int*) UArray2_at(myUArray2, i, j) = sudokuData;
		}
	}

	int denominator = myData.denominator;
/*      
 *      free Pnmrdr and close the file 
 */
	Pnmrdr_free(&myPnmrdr);
	fclose(fp);	
/*      
 *      check all the rules for sudoku 
 */
	checkSolved(myUArray2, denominator);
	return 0;
}

/*
 *      Check sudoku solved or not, solved return 0, not solved return 1
 */
void checkSolved(UArray2_T myUArray2, int denominator)
{
/*      
 *      check the denominator is nine or not 
 */
	if ( denominator != 9) {
		UArray2_free(&myUArray2);
		exit(1);
	}
 
/*      
 *      check contain zero or not, zeroNum will be the number of
 *      zero in UArray2 
 */
	int zeroNum = 0;
	UArray2_map_row_major(myUArray2, checkZero, &zeroNum);
	if (zeroNum > 0) {
		UArray2_free(&myUArray2);
		exit(1);
	}

/*      
 *      create a tempUArray to store the times of each number appears
 *      in every 9 elements 
 */
	UArray_T tempUArray = UArray_new(9, sizeof(int));

/*      
 *      reset the tempUArray, each element is zero 
 */
	resetUArray(tempUArray);
/* 
 *      check each column's pixels have same intensity or not 
 */
	UArray2_map_row_major(myUArray2, checkRow, &tempUArray);

/*      
 *      reset the tempUArray, each element is zero
 */
	resetUArray(tempUArray);
/*      
 *      check each column's pixels have same intensity or not 
 */
	UArray2_map_col_major(myUArray2, checkCol, &tempUArray);

/* 
 *      reset the tempUArray, each element is zero 
 */
	resetUArray(tempUArray);
/*      
 *      check each block's pixels have same intensity or not 
 */
	UArray2_map_block_major(myUArray2, checkBlock, &tempUArray);
	
/*      
 *      free tempUArray 
 */
	UArray_free(&tempUArray);
	UArray2_free(&myUArray2);
/* 
 *      if sudoku solved exit 0 
 */
	exit(0);	
}

/* 
 *      check the each row contains only one for each 1-9 number
 *      if not, free the tempUArray. val is the value of myUArray2[i][j]
 *      cl is the pointer pointing to tempUArray 
 */
void checkRow(int i, int j, UArray2_T myUArray2, void* val, void* cl)
{
/* 
 *      not use i 
 */
	(void) i;
/*      
 *      cast cl and val and assign to tempUArray and index 
 */
	UArray_T* tempUArray = (UArray_T*) cl;
	int index = (*(int*) val) - 1;
/* 
 *      use the (value of myUArray2) - 1 as the index of tempUArray
 *      check tempUArray[index] == j (jth row) or not.
 *      if equal, tempUArray[index]++; if not free memory and exit(1) 
 */
	if (*(int*) UArray_at(*tempUArray, index) == j) {
		(*(int*) UArray_at(*tempUArray, index))++;
	} else {
		UArray_free(tempUArray);
		UArray2_free(&myUArray2);
		exit(1);
	}
}

/*      
 *      check the each column contains only one for each 1-9 number
 *      if not, free the tempUArray. val is the value of myUArray2[i][j]
 *      cl is the pointer pointing to tempUArray 
 */
void checkCol(int i, int j, UArray2_T myUArray2, void* val, void* cl)
{
/* 
 *      not use j 
 */
	(void) j;
/* 
 *      cast cl and val and assign to tempUArray and index 
 */
	UArray_T* tempUArray = (UArray_T*) cl;
	int index = (*(int*) val) - 1;
/* 
 *      use the (value of myUArray2) - 1 as the index of tempUArray
 *      check tempUArray[index] == i (ith column) or not.
 *      if equal, tempUArray[index]++; if not free memory and exit(1) 
 */
	if (*(int*) UArray_at(*tempUArray, index) == i) {
		(*(int*) UArray_at(*tempUArray, index))++;
	} else {
		UArray_free(tempUArray);
		UArray2_free(&myUArray2);
		exit(1);
	}

}

/* 
 *      check the each block contains only one for each 1-9 number
 *      if not, free the tempUArray. val is the value of myUArray2[i][j]
 *      cl is the pointer pointing to tempUArray 
 */
 void checkBlock(int i, int j, UArray2_T myUArray2, void* val, void* cl)
{
/* 
 *      cast cl and val and assign to tempUArray and index 
 */
	UArray_T* tempUArray = (UArray_T*) cl;
	int index = (*(int*) val) - 1;
/*      
 *      the num repreasts the numth block, we read the block as
 *      English reading order. 
 */
	int num = (i / 3) + (j / 3) * 3;
/* 
 *      use the (value of myUArray2) - 1 as the index of tempUArray
 *      check tempUArray[index] == num (ith column) or not.
 *      if equal, tempUArray[index]++; if not free memory and exit(1) 
 */
	if (*(int*) UArray_at(*tempUArray, index) == num) {
		(*(int*) UArray_at(*tempUArray, index))++;
	} else {
		UArray_free(tempUArray);
		UArray2_free(&myUArray2);
		exit(1);
	}
}	

/* 
 *      check myUArray2 contains zero or not, and count the number of zeroes.
 *      cl is the pointer pionting to the count of zeroes 
 */
void checkZero(int i, int j, UArray2_T myUArray2, void* val, void* cl)
{
/*      
 *      not use i, j and myUArray2 
 */
	(void) i;
	(void) j;
	(void) myUArray2;
/*      
 *      check each element is equal to zero or not. if is count++ 
 */
	if (*(int*) val == 0) {
		(*(int*) cl) ++;
	}
}

/*      
 *      reset the uarray, each element is equal to zero
 */
void resetUArray(UArray_T uarray)
{
	for (int i = 0; i < UArray_length(uarray); i++) {
		*(int*)UArray_at(uarray, i) = 0;
	}
}


