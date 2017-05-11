/*
 *      filename: conversions-6.c
 *      by Will Dolan, Yucheng He, 10/21/2015
 *      hw4 of comp40
 *
 *      This file contains the implementation for conversions, which performs
 *      all of the necessary type or value conversions for the 40image program.
 *      Both the compression conversions and decompression conversions are
 *      implemented here, as they utilize the same data structures and methods.
 *      We follow the trajectory outlined in the spec, so for compression,
 *      the order of methods goes:
 *      input RGB -> RGBs as floats -> Component Video Style -> 32 bit Words
 *      The decompression order of conversions is the above, in reverse.
 *      It is worth adding that we consolidated the Rgb Float and CVCS steps
 *      in decompression.
 *      THIS IS THE SIX BIT VERSION.
 */

#include "conversions.h"
#include <arith40.h>
#include <bitpack.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <a2methods.h>
#include <a2plain.h>
#include <a2blocked.h>

/*-----------------------------------FUNCTIONS------------------------------*/

/*-------------Compression Functions------------------*/
int signed_float_to_6bit(float cos_y);
unsigned int float_to_6bit(float cos_y);

void RGB_to_RGBfloat(int i, int j, A2Methods_UArray2 array2, 
                void* elem, void* cl);
void RGBfloat_to_CVCS(int i, int j, A2Methods_UArray2 array2, 
                void* elem, void* cl);
void make_word(int i, int j, A2Methods_UArray2 array2, void* elem, void* cl);

/*-------------Decompression Functions----------------*/
float six_bit_to_float_s(int index);
float six_bit_to_float_u(unsigned index);
void extract_word(int i,int j, A2Methods_UArray2 array2, void* elem, void* cl);
void CVCS_to_RGB(int i, int j, A2Methods_UArray2 array2, void* elem, void* cl);


/*-----------------------------------STRUCTS------------------------------*/

struct CVCS {
        float Y;
        float pb;
        float pr;
} *CVCS;

struct rgb {
        float r;
        float g;
        float b;
} *rgb;

struct a2fun_closure {
        A2Methods_T methods;
        A2Methods_UArray2 clUArray2;
        unsigned denominator;
} *a2fun_closure;

/*----------------------COMPRESSION IMPLEMENTATIONS-------------------------*/

/* This file calls all of the necessary methods needed to convert an RGB struct
   like the ones in Pnm_ppm arrays into an array of 64 bit words */

void compress_convert(Pnm_ppm image, A2Methods_UArray2 words)
{
        struct a2fun_closure *RGBcl = malloc(sizeof(struct a2fun_closure));
        struct a2fun_closure *CVCScl = malloc(sizeof(struct a2fun_closure));
        assert(RGBcl != NULL && CVCScl != NULL);
        
        A2Methods_T methods = uarray2_methods_plain;
        int width = image->width;
        int height = image->height;

        /* trim the image if either dimension is odd */
        if (width % 2 == 1) {
                width -= 1;
        }
        if (height % 2 == 1) {
                height -= 1;
        }

        /* convert the RGB to RGB float type by using a closure containing 
           the RGB struct*/
        A2Methods_UArray2 RGBfloats = methods->new(width, height, 
                                                   sizeof(struct rgb));
        RGBcl->clUArray2 = image->pixels;
        RGBcl->methods = methods;
        RGBcl->denominator = image->denominator;
        
        methods->map_default(RGBfloats, RGB_to_RGBfloat, RGBcl);

        /* convert the RGB float to CVCS type */
        CVCScl->clUArray2 = methods->new(width, height, sizeof(struct CVCS));
        CVCScl->methods = methods;
        CVCScl->denominator = image->denominator;
        methods->map_default(RGBfloats, RGBfloat_to_CVCS, CVCScl);

        /* pack each 2-by-2 block into a 32-bit word */
        methods->map_default(words, make_word, CVCScl);

        methods->free(&(RGBfloats));
        free(RGBcl);
        methods->free(&(CVCScl->clUArray2));
        free(CVCScl);
}

void RGB_to_RGBfloat(int i, int j, A2Methods_UArray2 array2, 
                void* elem, void* cl)
{
        struct a2fun_closure *RGBcl = (struct a2fun_closure *) cl;
        A2Methods_T methods = RGBcl->methods;
        A2Methods_T pixels = RGBcl->clUArray2;
        unsigned denominator = RGBcl->denominator;
        
        struct Pnm_rgb *unconverted_RGB = 
                                (struct Pnm_rgb *)methods->at(pixels, i, j);
        (void)array2;

        struct rgb *converted_RGB = (struct rgb *)elem;

        converted_RGB->r = ((float)(unconverted_RGB->red)) / denominator;
        converted_RGB->g = ((float)(unconverted_RGB->green)) / denominator;
        converted_RGB->b = ((float)(unconverted_RGB->blue)) / denominator;
}

void RGBfloat_to_CVCS(int i, int j, A2Methods_UArray2 array2, 
                void* elem, void* cl)
{
        struct a2fun_closure *CVCScl = (struct a2fun_closure *) cl;
        struct rgb *floatRGB = (struct rgb *)elem;
        (void)array2;

        struct CVCS *YBR = (struct CVCS *)CVCScl->methods->
                at(CVCScl->clUArray2, i, j);

        float r = floatRGB->r;
        float g = floatRGB->g;
        float b = floatRGB->b;

        YBR->Y = (0.299 * r) + (0.587 * g) + (0.114 * b);
        YBR->pb = (-0.168736 * r) - (0.331264 * g) + (0.5 * b);
        YBR->pr = (0.5 * r) - (0.418488 * g) - (0.081312 * b);
}

void make_word(int i, int j, A2Methods_UArray2 array2, void* elem, void* cl)
{
        struct a2fun_closure *CVCScl = (struct a2fun_closure *) cl;
        uint64_t tempWord = 0;

        A2Methods_UArray2 CVCSarray = CVCScl->clUArray2;
        A2Methods_T methods = CVCScl->methods;
        (void)array2;

        /* here we double the idices for row and column, because one 2x2 block
           gets compressed into one pixel and we are mapping into the
           compressed uarray2. */
        int i1 = 2 * i;
        int i2 = 2 * i + 1;
        int j1 = 2 * j;
        int j2 = 2 * j + 1; 
        
        /*store the contents of the CVCS array into four structs */
        struct CVCS *cvcs1 = (struct CVCS *)methods->at(CVCSarray, i1, j1);
        struct CVCS *cvcs2 = (struct CVCS *)methods->at(CVCSarray, i2, j1);
        struct CVCS *cvcs3 = (struct CVCS *)methods->at(CVCSarray, i1, j2);
        struct CVCS *cvcs4 = (struct CVCS *)methods->at(CVCSarray, i2, j2);

        /*extract the values contained in the cvcs structures */
        float Y1 = cvcs1->Y;
        float Y2 = cvcs2->Y;
        float Y3 = cvcs3->Y;
        float Y4 = cvcs4->Y;

        float pb1 = cvcs1->pb;
        float pb2 = cvcs2->pb;
        float pb3 = cvcs3->pb;
        float pb4 = cvcs4->pb;

        float pr1 = cvcs1->pr;
        float pr2 = cvcs2->pr;
        float pr3 = cvcs3->pr;
        float pr4 = cvcs4->pr;

        /* calculate a, b c and d with the cvcs structure values */
        float a = (Y4 + Y3 + Y2 + Y1) / 4.0;
        float b = (Y4 + Y3 - Y2 - Y1) / 4.0;
        float c = (Y4 - Y3 + Y2 - Y1) / 4.0;
        float d = (Y4 - Y3 - Y2 + Y1) / 4.0;
        float Pb = (pb1 + pb2 + pb3 + pb4) / 4.0;
        float Pr = (pr1 + pr2 + pr3 + pr4) / 4.0;

        /* store the a,b,c,d values into a 32 bit word, then set the element
           we are mapping to to this word.*/
        tempWord = Bitpack_newu(tempWord, 6, 26, float_to_6bit(a));
        tempWord = Bitpack_news(tempWord, 6, 20, signed_float_to_6bit(b));
        tempWord = Bitpack_news(tempWord, 6, 14, signed_float_to_6bit(c));
        tempWord = Bitpack_news(tempWord, 6, 8, signed_float_to_6bit(d));
        tempWord = Bitpack_newu(tempWord, 4, 4, Arith40_index_of_chroma(Pb));
        tempWord = Bitpack_newu(tempWord, 4, 0, Arith40_index_of_chroma(Pr));

        *(uint64_t *) elem = tempWord;    

}

/* convert float values to 5 bit representations. If the values are over -.3,.3
   then set them to the minimum or maximum possible values (+/- 15) */
int signed_float_to_6bit(float cos_y)
{
        if (cos_y < -0.3) {
                return -30;
        }
        if (cos_y > 0.3) {
                return 30;
        }
        return (int)(cos_y / 0.01);
}

/* returns the integer representation of a float between 0 and 1 */
unsigned int float_to_6bit(float cos_y)
{
        return (unsigned) (cos_y * 63);

}

/*------------------------DECOMPRESSION FUNCTIONS-------------------------*/

/* This function calls all of the necessary methods needed to convert 
   an array of 64 bit words into an RGB struct like the ones in Pnm_ppm 
   arrays*/

void decompress_convert(A2Methods_UArray2 words, Pnm_ppm image)
{
        struct a2fun_closure *CVCScl = malloc(sizeof(struct a2fun_closure));
        A2Methods_T methods = uarray2_methods_plain;
        int width = image->width;
        int height = image->height;

        /* unpack each 32-bit word into 4 CVCS blocks */
        CVCScl->clUArray2 = methods->new(width, height, sizeof(struct CVCS));
        CVCScl->methods = methods;
        CVCScl->denominator = image->denominator;
        methods->map_default(words, extract_word, CVCScl);

        /* convert the CVCS structs to RGB structs */
        methods->map_default(image->pixels, CVCS_to_RGB, CVCScl);

        methods->free(&(CVCScl->clUArray2));
        free(CVCScl);   
}

void extract_word(int i, int j, A2Methods_UArray2 array2, void* elem, void* cl)
{
        struct a2fun_closure *CVCScl = (struct a2fun_closure *) cl;
        uint64_t tempWord = *(uint64_t *) elem;
        A2Methods_UArray2 CVCSarray = CVCScl->clUArray2;
        A2Methods_T methods = CVCScl->methods;
        (void)array2;

        /* here we double the idices for row and column, because one 2x2 block
           gets compressed into one pixel and we are mapping from the
           compressed uarray2. */
        
        int i1 = 2 * i;
        int i2 = 2 * i + 1;
        int j1 = 2 * j;
        int j2 = 2 * j + 1; 
        
        /*store the contents of the CVCS array into four structs */
        struct CVCS *cvcs1 = (struct CVCS *)methods->at(CVCSarray, i1, j1);
        struct CVCS *cvcs2 = (struct CVCS *)methods->at(CVCSarray, i2, j1);
        struct CVCS *cvcs3 = (struct CVCS *)methods->at(CVCSarray, i1, j2);
        struct CVCS *cvcs4 = (struct CVCS *)methods->at(CVCSarray, i2, j2);

        /* convert from the bitpack representation back into floating point */
        float a = six_bit_to_float_u( Bitpack_getu(tempWord, 6, 26));
        float b = six_bit_to_float_s( Bitpack_gets(tempWord, 6, 20));
        float c = six_bit_to_float_s( Bitpack_gets(tempWord, 6, 14));
        float d = six_bit_to_float_s( Bitpack_gets(tempWord, 6, 8));
        float Pb = Arith40_chroma_of_index( Bitpack_getu(tempWord, 4, 4));
        float Pr = Arith40_chroma_of_index( Bitpack_getu(tempWord, 4, 0));

        /*store the values contained in the bitpacks into a cvcs structure */
        cvcs1->Y = a - b - c + d;
        cvcs2->Y = a - b + c - d;
        cvcs3->Y = a + b - c - d;
        cvcs4->Y = a + b + c + d;

        cvcs1->pb = Pb;
        cvcs1->pr = Pr;
        cvcs2->pb = Pb;
        cvcs2->pr = Pr;
        cvcs3->pb = Pb;
        cvcs3->pr = Pr;
        cvcs4->pb = Pb;
        cvcs4->pr = Pr;

}

void CVCS_to_RGB(int i, int j, A2Methods_UArray2 array2, void* elem, void* cl)
{
        struct a2fun_closure *CVCScl = (struct a2fun_closure *) cl;
        struct CVCS* YBR = (struct CVCS *)CVCScl->methods->
                                                  at(CVCScl->clUArray2, i, j);
        struct Pnm_rgb* RGB = (struct Pnm_rgb*) elem;
        float Y = YBR->Y;
        float pb = YBR->pb;
        float pr = YBR->pr;
        int denominator = (int)CVCScl->denominator;

        (void) array2;
        
        int red = (Y + 1.402 * pr) * denominator;
        int green = (Y - 0.344136 * pb - 0.714136 * pr) * denominator;
        int blue = (Y + 1.772 * pb) * denominator;
        
        /* if the values given by the decompression arithmetic are 
        less than zero or greater than the denominator, then they are set to
        equal zero or the denominator */
        
        if(red > denominator) {
                red = denominator;
        }
        if(red < 0){
               red = 0;
        }
        RGB->red = red;

        if(green > denominator) {
                green = denominator;
        }
        if(green < 0){
               green = 0;
        }
        RGB->green = green;
        
        if(blue > denominator) {
                blue = denominator;
        }
        if(blue < 0){
               blue = 0;
        }
        RGB->blue = blue;
}

float six_bit_to_float_s(int index)
{
        assert(index < 31 && index > -31);
        return index * 0.01;
}

float six_bit_to_float_u(unsigned index)
{
        return ((float) index / 63);
}
