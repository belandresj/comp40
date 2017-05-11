/*
 *      filename: unblackedge_helper.h
 *      by Jun Wang, Yucheng He, 09/28/2015
 *      hw2, comp40
 *
 *      This is the header file for all the helper function in the program
 *      named unblackedges
 */

#ifndef UNBLACKEDGE_HELPER_INCLUDED
#define UNBLACKEDGE_HELPER_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include "seq.h"
#include "bit2.h"
#include "pnmrdr.h"


Bit2_T pbmread(FILE *filename);
void pbmFilter(Bit2_T bitmap);
int removeBlackEdge(int x, int y, Bit2_T bitmap);
void* newIndex(int i, int j);
int isEdge(int x, int y, Bit2_T bitmap);
int checkPix (int x, int y, Seq_T mySeq, Bit2_T bitmap) ;
void pbmwrite(FILE *fp, Bit2_T bitmap);





#endif
