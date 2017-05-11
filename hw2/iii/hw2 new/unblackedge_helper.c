/*
 *      filename: unblackedge_helper.c
 *      by Jun Wang, Yucheng He, 09/28/2015
 *      hw2, comp40
 *
 *      This is the source file for the implementation of all the helper 
 *      function of the program named unblackedges
 */


#include <stdio.h>
#include "assert.h"
#include "unblackedge_helper.h"
#include "pnmrdr.h"
#include "seq.h"
#include "bit2.h"

/*
 *      This struct is used to store the position of the pixels of the pbm 
 *      image which is stored in the 2-d bit matrix. 
 *      x is the index of the column.
 *      y is the index of the row.
 */
struct Index {
	int x;
	int y;
};

/*
 *      The pbmread function is used to read the pbm image and store the 
 *      value of the pixels into the 2-d bit array and return this 2-d bit
 *      array.
 *      filename is the name of the pbm image file which is used to be read.
 */
Bit2_T pbmread(FILE *filename)
{
/*      
 *      myPnmrdr is used to declare an object for reading the image
 */
        Pnmrdr_T myPnmrdr = Pnmrdr_new(filename);

/*      
 *      myPnmData is used to store the properties of the image
 */
        Pnmrdr_mapdata myPnmData = Pnmrdr_data(myPnmrdr);

/*      
 *      read the width and height of the image
 */
	int width = myPnmData.width;
	int height = myPnmData.height;
	
	
/*      
 *      Construct an object of the 2-d bit array
 */
	Bit2_T data;
	data = Bit2_new(width, height);
	
/*      
 *      Read the pixels into the 2-d bit array
 */
	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			int pix = Pnmrdr_get(myPnmrdr);
			Bit2_put(data, i, j, pix);
		}
	}
	Pnmrdr_free(&myPnmrdr);
	return data;
}

/*
 *      The pbmFilter function is used to find all the black edges pixels and 
 *      remove these pixels from the 2-d bit array.
 *      bitmap is the 2-d bit array storing the value of all the pixels in
 *      the pbm image.
 */
void pbmFilter(Bit2_T bitmap)
{
/*
 *      width is the width of the image, the number of the columns
 *      height is the height of the image, the number of the rows
 *
 *      i and j is the index which is used to determine the position of the
 *      pixel. i represents which column of the pixel is in the 2-d bit array
 *      and j represents which row of the pixel is in the 2-d bit array.
 */
	int width = Bit2_width(bitmap);
	int height = Bit2_height(bitmap);
        int i = 0;
        int j = 0;
/*
 *      Set the value of the pixel at the four corners of the image to 0 in the
 *      2-d bit array.
 */
	Bit2_put(bitmap, 0, 0, 0);
	Bit2_put(bitmap, width - 1, 0, 0);
	Bit2_put(bitmap, 0, height - 1, 0);
	Bit2_put(bitmap, width - 1, height - 1, 0);
/*      
 *      Remove the black edge pixel from the top edge (without corner).
 */
	for (i = 1; i < width - 1; i++) {
		if (Bit2_get(bitmap, i, j) == 1) {
			removeBlackEdge(i, j, bitmap);
		}
	}
/*
 *      Remove the black edge pixel from the right edge (without conrer).
 */
	for (j = 1; j < height - 1; j++) {
		if (Bit2_get(bitmap, i, j) == 1) {
			removeBlackEdge(i, j, bitmap);
		}
	}
/*
 *      Remove the black edge pixel from the bottom edge (without conrer).
 */
	for (i = width - 2; i > 0; i--) {
		if (Bit2_get(bitmap, i, j) == 1) {
			removeBlackEdge(i, j, bitmap);
		}
	}
/*
 *      Remove the black edge pixel from the left edge (without conrer).
 */
	for (j = height - 2; j > 0; j--) {
		if (Bit2_get(bitmap, i, j) == 1) {
			removeBlackEdge(i, j, bitmap);
		}
	}
}
	
/*      
 *      removeBlackEdge is used to remove all the black edge pixels which are
 *      connected to the black pixel at (i, j) which is at the edge of the
 *      image. However, this function will not remove other black pixels at
 *      the edge.
 *	i and j is the index for the black pixel at the edge of the image.
 *      bitmap is the 2-d bit array.
 */
int removeBlackEdge(int i, int j, Bit2_T bitmap)
{
/*
 *      myIndex is the pointer pointing to the struct storing the location (i,j)
 *      information of the black pixel at the edge.
 *
 *      mySeq is the sequence storing the indes of all the black edge pixels 
 *      which is connected to this black pixels at the edge (i,j). It will
 *      be initialized by adding the struct of the black pixels at the 
 *      edge (i,j) as the first element.
 */
 	struct Index *myIndex = newIndex(i, j);
	Seq_T mySeq = Seq_seq(myIndex, NULL);
	Bit2_put(bitmap, i, j, 0);
/*
 *      curIndex is the pointing which used to pointing the last element
 *      in the sequence in the while loop.
 *      
 *      m and n is used to store the value of the index in the struct
 *      pointed by the curIndex.
 *
 *      x and y is used to store the index of the neighboring pixels of the
 *      black edge pixels.
 */
	struct Index *curIndex = NULL;
        int m;
        int n;
        int x;
        int y;
/*
 *      This while loop will be used to find all the black pixels which is 
 *      connected to the black edge pixel (i, j) and set the value of the pixel
 *      to 0.
 *      This loop will terminate when the lenght of the sequence is 0, which 
 *      means that all the black pixels which are connected to the black edge
 *      pixel (i, j) has been found in this loop.
 */
	while (Seq_length(mySeq) > 0) {
/*
 *      Set the pointer curIndex pointing to the last element of the sequence.
 */
                curIndex = Seq_remhi(mySeq);
/*
 *      m and n is the index stored in the struct pointed by this pointer.
 */
		m = curIndex->x;
		n = curIndex->y;
/*
 *      Free the memory of the struct pointed by the pointer curIndex.
 */
		free(curIndex);
/*
 *      Check all the neighboring pixels of pixel (m, n)
 */
		x = m - 1;
		y = n;
		checkPix (x, y, mySeq, bitmap);
		
		x = m;
		y = n - 1;
		checkPix (x, y, mySeq, bitmap);
		x = m + 1;
		y = n;
		checkPix (x, y, mySeq, bitmap);
		x = m;
		y = n + 1;
		checkPix (x, y, mySeq, bitmap);
	}
	Seq_free(&mySeq);
	return 0;
}

/*
 *      newIndex function is used to put a pair of index of the pixel into
 *      the struct and return the pointer of this struct.
 *      i and j is the index of the pixel in the 2-d bit array.
 */
void* newIndex(int i, int j)
{
	struct Index *myIndex = malloc(sizeof(struct Index));
	assert(myIndex != NULL);
	myIndex->x = i;
	myIndex->y = j;
	return myIndex;
}


/*
 *      isEdge function is used to determine whether the pixel (i, j) is at 
 *      the edge of the pbm image.
 *      x and y is the index of the pixel.
 *      bitmap is the 2-d bit array.
 *	return 1: it is the edge
 *	return 0: it is not the edge
 */
int isEdge(int x, int y, Bit2_T bitmap)
{
	int width = Bit2_width(bitmap);
	int height = Bit2_height(bitmap);
	assert(x >= 0 && x < width);
	assert(y >= 0 && y < height);
	if (x == 0 || x == width - 1)
		return 1;
	if (y == 0 || y == height - 1)
		return 1;
	return 0;
}

/*
 *      checkPix function is used to check the specific pixel in the 2-d bit
 *      array (bitmap). If this pixel is black, since it is connected to the
 *      black edge pixel, it measn this pixel is also a black edge pixel.
 *      The struct storing the index of this pixel will be pushed into the
 *      sequence and the corresponding value in the 2-d bit array will be
 *      set 0.
 *      x and y is the index of the pixel to be checked. 
 *      mySeq is the sequence used to store the struct of the index.
 *      bitmap is the 2-d bit array for storing the value of the pixels.
 *
 *	return 1: Add the pixel into the mySeq
 *	return 0: Don't add pixel into mySeq
 */
int checkPix (int x, int y, Seq_T mySeq, Bit2_T bitmap) 
{
/*
 *      Get the width and the height of the bitmap.
 *      tempPix is the value of the pixel (x, y) in the 2-d bit array (bitmap)
 */
        int width = Bit2_width(bitmap);
        int height = Bit2_height(bitmap);
        if (x < 0 || x >= width || y < 0 || y >= height)
                return 0;
        int tempPix = Bit2_get(bitmap, x, y);
/*
 *      If the pixel is not at the edge and the value of the pixel is 1. Push
 *      the corresponding struct into the sequence (mySeq) and set the value
 *      of the pixel to 0 in the 2-d bit array (bitmap).
 */
	if (isEdge(x, y, bitmap) == 0 && tempPix == 1) {
		struct Index *tempIndex = newIndex(x, y);
		Seq_addhi(mySeq, tempIndex);
		Bit2_put(bitmap, x, y, 0);
		return 1;
	}
	else
		return 0;
}

/*
 *      The pbmwrite function is used to export the pbm image file.
 */
void pbmwrite(FILE *fp, Bit2_T bitmap)
{
	int width = Bit2_width(bitmap);
	int height = Bit2_height(bitmap);
	fprintf(fp, "P1\n%d %d\n", width, height);
/*
 *      Print the pixel into the file one by one.
 */
	int tempPix;
	char pix;
	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			tempPix = Bit2_get(bitmap, i, j);
			if (tempPix == 1) 
				pix = '1';
			else 
				pix = '0';
			fwrite(&pix, 1, sizeof(pix), fp);
		}
		fprintf(fp, "\n");
	}
}
