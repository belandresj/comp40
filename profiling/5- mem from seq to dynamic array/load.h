/*
 *	filename: load.h
 *      by Yucheng He and Griffin Richards
 *	11/23/2015
 *	Include the interface for load function.
 *      Source code is in load.c
 */

#ifndef LOAD_H_
#define LOAD_H_

#include "segment.h"
#include <stdio.h>

/* given a file pointer and filename, return a segment */
Segment_T load(FILE* input, char* filename);

#endif