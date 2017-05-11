#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <compress40.h>
#include <a2methods.h>
#include <a2plain.h>
#include <a2blocked.h>
#include <pnm.h>
#include "conversions.h"
#include <bitpack.h>

void print(int i, int j, A2Methods_UArray2 array2, void* elem, void* cl);

void compress40(FILE* input)
{
    A2Methods_T methods = uarray2_methods_plain; 
    assert(methods);
 
    /* default to best map */
    A2Methods_mapfun *map = methods->map_default; 
    assert(map);
 
    Pnm_ppm image;
    /* check the format of ppm file is right or not */
    TRY
    image = Pnm_ppmread(input, methods);
    EXCEPT(Pnm_Badformat)
        fprintf(stderr, "The input file is in bad format.\n");
        exit(1);
    END_TRY;
    
    A2Methods_UArray2 words = methods->new((image->width)/ 2,
                      (image->height)/ 2, sizeof(uint64_t));
                      
    compress_convert(image, words);
    int width = methods->width(words) * 2;
    int height = methods->height(words) * 2;
    
    printf("COMP40 Compressed Image Format 2\n%u %u",width, height);
    printf("\n");
    map(words, print, NULL);
    
    methods->free(&words);
    Pnm_ppmfree(&image);

}

void print(int i, int j, A2Methods_UArray2 array2, void* elem, void* cl)
{
       uint64_t word = *(uint64_t *) elem;
       (void) array2;
       (void) i;
       (void) j;
       (void) cl;

       char ch1 = Bitpack_getu(word, 8, 24);
       putchar(ch1);
       char ch2 = Bitpack_getu(word, 8, 16);
       putchar(ch2);
       char ch3 = Bitpack_getu(word, 8, 8);
       putchar(ch3);
       char ch4 = Bitpack_getu(word, 8, 0);
       putchar(ch4);
       
}
