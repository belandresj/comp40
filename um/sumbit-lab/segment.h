/*
 *	filename: segement.h
 *      by Yucheng He and Griffin Richards
 *	11/15/2015
 *	Inculde the interface for Segment_T
 */

#ifndef SEGMENT_H_
#define SEGMENT_H_

#include <stdlib.h>
#include <stdint.h>
#include "seq.h"

/* These functions can handle the cast of void pointer to 
 * uint32_t and the client will not know the structure we used 
 * keep it secret.
 */

typedef struct Seq_T *Segment_T;

/* create a new segment */
Segment_T seg_new();

/* free the seg */
void seg_free(Segment_T seg);

/* store value in the seg at index i */
void seg_put(Segment_T seg, uint32_t i, uint32_t val);

/* get the value at the seg at index i */
uint32_t seg_get(Segment_T seg, uint32_t i);

#endif
