/*
 *      filename: unblackedges.c
 *      by Jun Wang, Yucheng He, 09/23/2015
 *      hw2 in comp40
 *
 *      This source file contains the main function for reading and exporting
 *      the image file.
 */

#include <stdio.h>
#include <stdlib.h>
#include "pnmrdr.h"
#include "bit2.h"
#include "unblackedge_helper.h"

int main(int argc, char *argv[])
{
        FILE *fpRead = NULL;
/*      
 *      Exit the program if there are more than 1 image to be read
 */
        
        if (argc > 2) {
                fprintf(stderr, "Too much input.\n");
                exit(EXIT_FAILURE);
        }
/*
 *      Read the input image from the standard input if no image is
 *      waiting to be read.
 */
        if(argc == 1) {
                fpRead = stdin;
        }
/*
 *      Read the input image if there is 1 images need to be read
 */
        else {
                fpRead = fopen(argv[1],"rb");
/*
 *      If the file cannot be read, exit with error information
 */
                if (fpRead == NULL) {
                        fprintf(stderr, "Wrong file name.\n");
                        exit(EXIT_FAILURE);
                }
        }
        
        
/*
 *      Read the data from the pbm into the 2-d bit array
 */
        Bit2_T bitmap;
        bitmap = pbmread(fpRead);
        fclose(fpRead);
        pbmFilter(bitmap);

/*
 *      Writing the images into files
 */
        FILE *fpWrite = NULL;
        fpWrite = stdout;
        pbmwrite(fpWrite, bitmap);
        if (fpWrite != NULL)
                fclose(fpWrite);
        Bit2_free(&bitmap);
        return 0;
        
        
}


