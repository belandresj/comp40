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

void read_words(int i, int j, A2Methods_UArray2 array2, void* elem, void* cl);
void print2(int i, int j, A2Methods_UArray2 array2, void* elem, void* cl);


void decompress40(FILE* input)
{
        A2Methods_T methods = uarray2_methods_plain; 
        assert(methods);
        /* default to best map */
        A2Methods_mapfun *map = methods->map_default; 
        assert(map);

        unsigned width, height, denominator;
        denominator = 255;
        int read = fscanf(input, "COMP40 Compressed Image Format 2\n%u %u", 
                        &width, &height);
        assert(read == 2);
        int c = getc(input);
        assert(c = '\n');

        A2Methods_UArray2 array2 = methods->new(width, height, 
                        sizeof(struct Pnm_rgb));
        Pnm_ppm image = malloc(sizeof(struct Pnm_ppm));
        image->width = width ;
        image->height = height;
        image->denominator = denominator;
        image->pixels = array2;
        image->methods = methods;

        A2Methods_UArray2 words = methods->new(width/ 2, height/ 2, 
                        sizeof(uint64_t));

        methods->map_default(words, read_words, input);

        decompress_convert(words, image);

        Pnm_ppmwrite(stdout, image);
        //methods->map_default(image->pixels, print2, NULL);

        methods->free(&words);
        Pnm_ppmfree(&image);
}

void print2(int i, int j, A2Methods_UArray2 array2, void* elem, void* cl)
{
        (void)cl;
        (void)array2;
        struct Pnm_rgb *rgb = (struct Pnm_rgb *)elem;
        printf("row: %d, col %d, red: %u, green: %u, blue%u\n", j, i, rgb->red, rgb->green, rgb->blue);
}

void read_words(int i, int j, A2Methods_UArray2 array2, void* elem, void* cl)
{
        uint64_t word = 0;
        (void) array2;
        (void) i;
        (void) j;
        FILE *fp = (FILE *)cl;

        uint64_t byte = getc(fp);
        word = Bitpack_newu(word, 8, 24, byte);

        byte = getc(fp);
        word = Bitpack_newu(word, 8, 16, byte);
        byte = getc(fp);
        word = Bitpack_newu(word, 8, 8, byte);
        byte = getc(fp);
        word = Bitpack_newu(word, 8, 0, byte);

        *(uint64_t *) elem = word;
}
