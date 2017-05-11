/*
 *      filename: ppmtrans.c
 *      by Jun Wang, Yucheng He, 10/09/2015
 *      hw3 of comp40
 *
 *      This is the source file including the main function for the ppmtrans
 *      program. This main function will provide the interface for the program
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "assert.h"
#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"
#include "pnm.h"
#include "ppmtrans_helper.h"
#include "cputiming.h"

static void
usage(const char *progname)
{
	fprintf(stderr, "Usage: %s [-rotate <angle>] "
		"[-{row,col,block}-major] [filename]\n",
		progname);
	exit(1);
}

/*
 * The filename for reading should be in the last of the command line
 *
 * main function will provide the interface for user in the ppmtrans program.
 */
int main(int argc, char *argv[]) 
{
	/* 
	 * i is the index for reading the command line
	 * rotation is the degree for rotation
	 */
        int i;
        int rotation = 0;
	
	/* Open a file for recording time cost */
	FILE *fpTime = NULL;
	fpTime = fopen("TimeRecord.txt", "a");
	assert(fpTime != NULL);
        
        /* default to UArray2 methods */
        A2Methods_T methods = uarray2_methods_plain; 
        assert(methods);

        /* default to best map */
        A2Methods_mapfun *map = methods->map_default; 
        assert(map);

#define SET_METHODS(METHODS, MAP, WHAT) do {                            \
                methods = (METHODS);                                    \
                assert(methods != NULL);                                \
                map = methods->MAP;                                     \
                if (map == NULL) {                                      \
                        fprintf(stderr, "%s does not support "          \
                                        WHAT "mapping\n",               \
                                argv[0]);                               \
                        exit(1);                                        \
                }                                                       \
        } while (0)

        /* Extract the option from the command line */
	for (i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "-row-major")) {
			fprintf(fpTime, "Mapping = %s, ", argv[i]);
			SET_METHODS(uarray2_methods_plain, map_row_major,
				    "row-major");
		} else if (!strcmp(argv[i], "-col-major")) {
			fprintf(fpTime, "Mapping = %s, ", argv[i]);
			SET_METHODS(uarray2_methods_plain, map_col_major,
				    "column-major");
		} else if (!strcmp(argv[i], "-block-major")) {
			fprintf(fpTime, "Mapping = %s, ", argv[i]);
			SET_METHODS(uarray2_methods_blocked, map_block_major,
				    "block-major");
		} else if (!strcmp(argv[i], "-rotate")) {
			if (!(i + 1 < argc)) {      /* no rotate value */
				usage(argv[0]);
			}
			char *endptr;
			rotation = strtol(argv[++i], &endptr, 10);
			if (!(rotation == 0 || rotation == 90
			      || rotation == 180 || rotation == 270)) {
				fprintf(stderr, "Rotation must be "
					"0, 90 180 or 270\n");
				usage(argv[0]);
			}
			if (!(*endptr == '\0')) {    /* Not a number */
				usage(argv[0]);
			}
		} else if (*argv[i] == '-') {
			fprintf(stderr, "%s: unknown option '%s'\n", argv[0],
				argv[i]);
		} else if (argc - i > 1) {
			fprintf(stderr, "Too many arguments\n");
			usage(argv[0]);
		} else {
			break;
		}
	}

        /* Define a file pointer to open the image file */
        FILE *fp = NULL;
	/* If the image file is in the standard input */
        if (argc - i == 0) {
                fp = stdin;
        }
	/* If the image file name is at the end of the command line */
        else {
                fp = fopen(argv[argc - 1], "rb");
        }
        assert(fp != NULL);
        
        /* Read the image file into myPPM */
        Pnm_ppm myPPM = Pnm_ppmread(fp, methods);
        fclose(fp);
	
        /* Rotate the image according to the rotation */
        A2Methods_UArray2 myArray2;
	
	/* Start recording the time for the each mapping function */
	CPUTime_T timer = CPUTime_New();
	CPUTime_Start(timer);
        if (rotation == 90) {
                /* Rotate the image by 90 degree */
		myArray2 = methods->new(myPPM->height,
                                        myPPM->width, sizeof(struct Pnm_rgb));
                /* Use the struct to construct the closure pointer */
                struct a2fun_closure *mycl = malloc(sizeof(struct
                                                           a2fun_closure));
                mycl->methods = methods;
                mycl->newArray2 = myArray2;
                map(myPPM->pixels, rotate90, mycl);
		free(mycl);
        } else if (rotation == 180) {
                /* Rotate the image by 180 degree */
		myArray2 = methods->new(myPPM->width,
                                        myPPM->height, sizeof(struct Pnm_rgb));
		/* Use the struct to construct the closure pointer */
                struct a2fun_closure *mycl = malloc(sizeof(struct
                                                           a2fun_closure));
                mycl->methods = methods;
                mycl->newArray2 = myArray2;
                map(myPPM->pixels, rotate180, mycl);
		free(mycl);
        } else if (rotation == 270) {
                /* Rotate the image by 270 degree */
                myArray2 = methods->new(myPPM->height,
                                        myPPM->width, sizeof(struct Pnm_rgb));
                /* Use the struct to construct the closure pointer */
		struct a2fun_closure *mycl = malloc(sizeof(struct
                                                           a2fun_closure));
                mycl->methods = methods;
                mycl->newArray2 = myArray2;
                map(myPPM->pixels, rotate270, mycl);
                free(mycl);
        } else if (rotation == 0) {
                /* Rotate the image by 0 degree */
                myArray2 = methods->new(myPPM->width,
                                        myPPM->height, sizeof(struct Pnm_rgb));
                /* Use the struct to construct the closure pointer */
		struct a2fun_closure *mycl = malloc(sizeof(struct
                                                           a2fun_closure));
                mycl->methods = methods;
                mycl->newArray2 = myArray2;
                map(myPPM->pixels, rotate0, mycl);
                free(mycl);
        } else {
                exit(0);
        }
	/* Finish recording the time */
	double time_used = CPUTime_Stop(timer);
	fprintf(fpTime, "Rotation = %d, Time: %0f \n", rotation, time_used);
	
        /* Free the original pixels stored in myPPM */
	methods->free(&(myPPM->pixels));
        /* Edit myPPM by resetting the pixels pointing to the new array2 */
	myPPM->pixels = myArray2;
        /* Change the width and height stored in myPPM */
	myPPM->width = methods->width(myArray2);
	myPPM->height = methods->height(myArray2);
	
	/* Calculate the total number of per pixels and average time cost */
	int width = myPPM->width;
	int height = myPPM->height;
	int numPix = width * height;
	fprintf(fpTime, "Number of pixels = %d, ", numPix);
	double timePix = time_used / ((double)numPix);
	fprintf(fpTime, "average time per pixel = %0f \n \n", timePix);
	
	/* Close the file for time record and free the memory for timing */
	CPUTime_Free(&timer);
	fclose(fpTime);
	
        /* Export the data by standard output */
	FILE *fpwrite = NULL;
	fpwrite = stdout;
        Pnm_ppmwrite(fpwrite,myPPM);
	if (fpwrite != NULL) {
		fclose(fpwrite);
	}
	Pnm_ppmfree(&myPPM);

	
        return 0;
        
}

