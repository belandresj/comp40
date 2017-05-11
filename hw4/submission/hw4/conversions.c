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
float fivebit_to_float(int index);
float ninebit_to_float(unsigned index);
int float_to_5bit(float cos_y);
unsigned int float_to_9bit(float cos_y);

void RGB_to_RGBfloat(int i, int j, A2Methods_UArray2 array2, 
                void* elem, void* cl);
void RGBfloat_to_CVCS(int i, int j, A2Methods_UArray2 array2, 
                void* elem, void* cl);
void make_word(int i, int j, A2Methods_UArray2 array2, void* elem, void* cl);

void extract_word(int i, int j, A2Methods_UArray2 array2, void* elem, void* cl);
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
        A2Methods_UArray2 myUArray2;
        unsigned denominator;
} *a2fun_closure;

/*----------------------COMPRESSION IMPLEMENTATIONS---------------------------*/


void compress_convert(Pnm_ppm image, A2Methods_UArray2 words)
{
        struct a2fun_closure *RGBcl = malloc(sizeof(struct a2fun_closure));
        struct a2fun_closure *CVCScl = malloc(sizeof(struct a2fun_closure));
        A2Methods_T methods = uarray2_methods_plain;
        int width = image->width;
        int height = image->height;

        if (width % 2 == 1) {
                width -= 1;
        }
        if (height % 2 == 1) {
                height -= 1;
        }

        /* convert the RGB to RGB float type */
        A2Methods_UArray2 RGBfloats = methods->new(width, height, sizeof(struct rgb));
        RGBcl->myUArray2 = image->pixels;
        RGBcl->methods = methods;
        RGBcl->denominator = image->denominator;
        methods->map_default(RGBfloats, RGB_to_RGBfloat, RGBcl);

        /* convert the RGB float to CVCS type */
        CVCScl->myUArray2 = methods->new(width, height, sizeof(struct CVCS));
        CVCScl->methods = methods;
        CVCScl->denominator = image->denominator;
        methods->map_default(RGBfloats, RGBfloat_to_CVCS, CVCScl);

        /* pack each 2-by-2 block into a 32-bit word */
        methods->map_default(words, make_word, CVCScl);
        //methods->map_default(words, print_words, NULL);


        methods->free(&(RGBfloats));
        free(RGBcl);
        methods->free(&(CVCScl->myUArray2));
        free(CVCScl);


}

void RGB_to_RGBfloat(int i, int j, A2Methods_UArray2 array2, 
                void* elem, void* cl)
{
        struct a2fun_closure *RGBcl = (struct a2fun_closure *) cl;
        A2Methods_T methods = RGBcl->methods;
        A2Methods_T pixels = RGBcl->myUArray2;
        unsigned denominator = RGBcl->denominator;
        struct Pnm_rgb *unconverted_RGB = (struct Pnm_rgb *)methods->at(pixels, i, j);
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
                at(CVCScl->myUArray2, i, j);

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

        A2Methods_UArray2 CVCSarray = CVCScl->myUArray2;
        A2Methods_T methods = CVCScl->methods;
        (void)array2;

        /* here we double the idices for row and column, because one 2x2 block
           gets compressed into one pixel. */
        int i1 = 2 * i;
        int i2 = 2 * i + 1;
        int j1 = 2 * j;
        int j2 = 2 * j + 1; 
        struct CVCS *cvcs1 = (struct CVCS *)methods->at(CVCSarray, i1, j1);
        struct CVCS *cvcs2 = (struct CVCS *)methods->at(CVCSarray, i2, j1);
        struct CVCS *cvcs3 = (struct CVCS *)methods->at(CVCSarray, i1, j2);
        struct CVCS *cvcs4 = (struct CVCS *)methods->at(CVCSarray, i2, j2);

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

        float a = (Y4 + Y3 + Y2 + Y1) / 4.0;
        float b = (Y4 + Y3 - Y2 - Y1) / 4.0;
        float c = (Y4 - Y3 + Y2 - Y1) / 4.0;
        float d = (Y4 - Y3 - Y2 + Y1) / 4.0;
        float Pb = (pb1 + pb2 + pb3 + pb4) / 4.0;
        float Pr = (pr1 + pr2 + pr3 + pr4) / 4.0;

        tempWord = Bitpack_newu(tempWord, 9, 23, float_to_9bit(a));
        tempWord = Bitpack_news(tempWord, 5, 18, float_to_5bit(b));
        tempWord = Bitpack_news(tempWord, 5, 13, float_to_5bit(c));
        tempWord = Bitpack_news(tempWord, 5, 8, float_to_5bit(d));
        tempWord = Bitpack_newu(tempWord, 4, 4, Arith40_index_of_chroma(Pb));
        tempWord = Bitpack_newu(tempWord, 4, 0, Arith40_index_of_chroma(Pr));

        *(uint64_t *) elem = tempWord;    

}

int float_to_5bit(float cos_y)
{
        if (cos_y < -0.3) {
                return -15;
        }
        if (cos_y > 0.3) {
                return 15;
        }
        return (int)(cos_y / 0.02);
}

unsigned int float_to_9bit(float cos_y)
{
        return (unsigned) (cos_y * 511);

}

/*------------------------DECOMPRESSION FUNCTIONS-------------------------*/
void decompress_convert(A2Methods_UArray2 words, Pnm_ppm image)
{
        struct a2fun_closure *CVCScl = malloc(sizeof(struct a2fun_closure));
        A2Methods_T methods = uarray2_methods_plain;
        int width = image->width;
        int height = image->height;

        /* unpack each 32-bit word into 4 CVCS blocks */
        CVCScl->myUArray2 = methods->new(width, height, sizeof(struct CVCS));
        CVCScl->methods = methods;
        CVCScl->denominator = image->denominator;
        methods->map_default(words, extract_word, CVCScl);

        /* convert the CVCS structs to RGB structs */
        methods->map_default(image->pixels, CVCS_to_RGB, CVCScl);

        methods->free(&(CVCScl->myUArray2));
        free(CVCScl);   
}

void extract_word(int i, int j, A2Methods_UArray2 array2, void* elem, void* cl)
{
        struct a2fun_closure *CVCScl = (struct a2fun_closure *) cl;
        uint64_t tempWord = *(uint64_t *) elem;
        A2Methods_UArray2 CVCSarray = CVCScl->myUArray2;
        A2Methods_T methods = CVCScl->methods;
        (void)array2;

        /* here we double the idices for row and column, because one 2x2 block
           gets compressed into one pixel. */
        int i1 = 2 * i;
        int i2 = 2 * i + 1;
        int j1 = 2 * j;
        int j2 = 2 * j + 1; 
        struct CVCS *cvcs1 = (struct CVCS *)methods->at(CVCSarray, i1, j1);
        struct CVCS *cvcs2 = (struct CVCS *)methods->at(CVCSarray, i2, j1);
        struct CVCS *cvcs3 = (struct CVCS *)methods->at(CVCSarray, i1, j2);
        struct CVCS *cvcs4 = (struct CVCS *)methods->at(CVCSarray, i2, j2);

        float a = ninebit_to_float( Bitpack_getu(tempWord, 9, 23));
        float b = fivebit_to_float( Bitpack_gets(tempWord, 5, 18));
        float c = fivebit_to_float( Bitpack_gets(tempWord, 5, 13));
        float d = fivebit_to_float( Bitpack_gets(tempWord, 5, 8));
        float Pb = Arith40_chroma_of_index( Bitpack_getu(tempWord, 4, 4));
        float Pr = Arith40_chroma_of_index( Bitpack_getu(tempWord, 4, 0));

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
        struct CVCS* YBR = (struct CVCS *)CVCScl->methods->at(CVCScl->myUArray2, i, j);
        struct Pnm_rgb* RGB = (struct Pnm_rgb*) elem;
        float Y = YBR->Y;
        float pb = YBR->pb;
        float pr = YBR->pr;
        int denominator = (int)CVCScl->denominator;

        (void) array2;
        
        int red = (Y + 1.402 * pr) * denominator;
        int green = (Y - 0.344136 * pb - 0.714136 * pr) * denominator;
        int blue = (Y + 1.772 * pb) * denominator;
        
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

float fivebit_to_float(int index)
{
        assert(index < 16 && index > -16);
        return index * 0.02;
}

float ninebit_to_float(unsigned index)
{
        return ((float) index / 511);
}
