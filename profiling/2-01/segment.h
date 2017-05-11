/*
 *	filename: segement.h
 *      by Yucheng He and Griffin Richards
 *	11/23/2015
 *	Inculde the interface for Segment_T
 */

#ifndef SEGMENT_H_
#define SEGMENT_H_

#include <stdlib.h>
#include <stdint.h>
#include <seq.h>

/* These functions can handle the cast of void pointer to 
 * uint32_t and the client will not know the structure we used 
 * keep it secret.
 */

typedef struct Seq_T *Segment_T;

/* create a new segment */
Segment_T Seg_new(int nums);

/* free the seg */
void Seg_free(Segment_T seg);

/* store value in the seg at index i */
void Seg_put(Segment_T seg, uint32_t i, uint32_t val);

/* get the value at the seg at index i */
uint32_t Seg_get(Segment_T seg, uint32_t i);

void addInstr(Segment_T seg, uint32_t instr);

#endif
