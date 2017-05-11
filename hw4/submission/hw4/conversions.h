/*
 * conversions.h
 *      by Yucheng He and Will Dolan, 10/24/15
 *      HW4
 *
 * 
 */

#ifndef CONVERSIONSH_INCLUDED
#define CONVERSIONSH_INCLUDED

#include <pnm.h>

void compress_convert  (Pnm_ppm image, A2Methods_UArray2 words);
void decompress_convert(A2Methods_UArray2 words, Pnm_ppm image);
/*float fivebit_to_float(int index)
float ninebit_to_float(unsigned index)
int float_to_5bit(float cos_y)
unsigned int float_to_9bit(float cos_y)
void RGB_to_RGBfloat(int i, int j, A2Methods_UArray2 array2, void* elem, void* cl)
void RGBfloat_to_CVCS(int i, int j, A2Methods_UArray2 array2, void* elem, void* cl)
void CVCS_to_RGB(int i, int j, A2Methods_UArray2 array2, void* elem, void cl)
void make_word(int i, int j, A2Methods_UArray2 array2, void* elem, void* cl)
*/

#endif