#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "assert.h"
#include "pnm.h"
#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"

typedef A2Methods_UArray2 A2;

static A2Methods_T methods;

void rotate90(int i, int j, A2 a, void *elem, void *newI)
{
        Pnm_rgb src = elem;
        A2 temp = newI;
        Pnm_rgb dest = methods->at(temp, (methods->height(a) - j - 1), i);
        *dest = *src;
}

void rotate180(int i, int j, A2 a, void *elem, void *newI)
{
        Pnm_rgb src = elem;
        A2 temp = newI;
        Pnm_rgb dest =
            methods->at(temp,
                        (methods->width(a) - i - 1),
                        (methods->height(a) - j - 1));
        *dest = *src;
}

void rotate270(int i, int j, A2 a, void *elem, void *newI)
{
        Pnm_rgb src = elem;
        A2 temp = newI;
        Pnm_rgb dest = methods->at(temp, j, (methods->width(a) - i - 1));
        *dest = *src;
}

int main(int argc, char *argv[])
{
        int rotation = 0;
        FILE *readfile = NULL;
        methods = uarray2_methods_plain;
        void (*map) (A2 pixmap, void apply(int, int, A2, void *, void *),
                     void *cl)
            = methods->map_default;

#define SET_METHODS(METHODS, MAP, WHAT) do {                           \
      methods = (METHODS);                                             \
      assert(methods);                                                 \
      map = methods->MAP;                                              \
      if (map == NULL) {                                               \
              fprintf(stderr, "%s does not support " WHAT "mapping\n", \
                      argv[0]);                                        \
        exit(1);                                                       \
      }                                                                \
    } while(0)

        int i;
        for (i = 1; i < argc; i++) {
                if (!strcmp(argv[i], "-row-major")) {
                        SET_METHODS(uarray2_methods_plain,
                                    map_row_major, "row-major");
                } else if (!strcmp(argv[i], "-col-major")) {
                        SET_METHODS(uarray2_methods_plain,
                                    map_col_major, "column-major");
                } else if (!strcmp(argv[i], "-block-major")) {
                        SET_METHODS(uarray2_methods_blocked,
                                    map_block_major, "block-major");
                } else if (!strcmp(argv[i], "-rotate")) {
                        assert(i + 1 < argc);
                        char *endptr;
                        rotation = strtol(argv[++i], &endptr, 10);
                        assert(*endptr == '\0');    /* parsed all correctly */
                        assert(rotation == 0 || rotation == 90
                               || rotation == 180 || rotation == 270);
                } else if (*argv[i] == '-') {
                        fprintf(stderr, "%s: unknown option '%s'\n", argv[0],
                                argv[i]);
                        exit(1);
                } else if (argc - i > 2) {
                        fprintf(stderr, "Usage: %s [-rotate <angle>] "
                                "[-{row,col,block}-major] [filename]\n",
                                argv[0]);
                        exit(1);
                } else {
                        break;
                }
        }

        Pnm_ppm image;          /* creates and stores original image */
        if (readfile != NULL)
                image = Pnm_ppmread(readfile, methods);
        else
                image = Pnm_ppmread(stdin, methods);

        Pnm_ppm newImage = malloc(sizeof(*newImage));

        assert(newImage);
        if (rotation == 180 || rotation == 0) {
                newImage->width  = image->width;   /* switches parameters */
                newImage->height = image->height;  /* if necessary        */
        } else if (rotation == 90 || rotation == 270) {
                newImage->width  = image->height;
                newImage->height = image->width;
        }
        Pnm_rgb temp;
        newImage->methods     = methods;
        newImage->denominator = image->denominator;
        newImage->pixels      = methods->new(newImage->width,
                                             newImage->height,
                                             sizeof(*temp));

        if (rotation == 0)
                Pnm_ppmwrite(stdout, image);
        else if (rotation == 90) {
                A2 temp = image->pixels;
                map(temp, rotate90, newImage->pixels);
                Pnm_ppmwrite(stdout, newImage);
        } else if (rotation == 180) {
                A2 temp = image->pixels;
                map(temp, rotate180, newImage->pixels);
                Pnm_ppmwrite(stdout, newImage);
        } else if (rotation == 270) {
                A2 temp = image->pixels;
                map(temp, rotate270, newImage->pixels);
                Pnm_ppmwrite(stdout, newImage);
        }

        Pnm_ppmwrite(stdout, newImage);

        Pnm_ppmfree(&image);
        Pnm_ppmfree(&newImage);
}
