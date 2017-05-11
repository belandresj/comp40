/*
 *      filename: brightness.c
 *      by Jun Wang, Yucheng He, 09/18/2015
 *      hw1 in comp40
 *
 *
 *      The brightness.c file is a source file for the program named 
 *      brightness. This program can be used to read the pixel of the
 *      grayscale image and calculate the average brightness of the
 *      image.
 *      The total value of the brightness of each pixel in the image 
 *      will be read by the Pnmrdr_get and another function called 
 *      calBrightness will be used to calculate the average brightness
 *      of the whole image. 
 *
 *
 */



#include <stdio.h>
#include <stdlib.h>
#include "pnmrdr.h"

/*      
 *      Thie function is used to calculate the average brighgtness of
 *      the whole image by the sum of all pixels' brightness calculated
 *      in the main function. 
 */
double calBrightness(double sumPix, int denominatorPix, int numPix);
        
/*
 *      The main function is used to read the image file and call the
 *      functions in Pnmrdr and the calBrightness. The input variable
 *      argc is the number of the input file names and the *argv[] stores
 *      string with different input file names.
 */

int main(int argc, char *argv[])
{    
        FILE *fp = NULL;
        /*Exit the program if there are more than 1 image to be read*/
        if (argc > 2) {
                fprintf(stderr, "Too much input.\n");
                exit(EXIT_FAILURE);
        }
        /*Read the input image from the standard input if no image is
         *waiting to be read.
         */
        if(argc == 1) {
                fp = stdin;
        }
        /*Read the input image if there is 1 images need to be read*/
        else {
                fp = fopen(argv[1],"rb");
        }
        
        /*myPnmrdr is used to declare an object for reading the image*/
        Pnmrdr_T myPnmrdr = Pnmrdr_new(fp);

        /*myData is used to store the properties of the image*/
        Pnmrdr_mapdata myData = Pnmrdr_data(myPnmrdr);

        /*read the data of each pixels */
        int pixelNum = myData.width * myData.height;
        double sum = 0;
        for (int i = 1; i <= pixelNum; i++) {
                sum = sum + Pnmrdr_get(myPnmrdr);
        }
        
        /*Calculate the average brightness by calBrightness*/
        double brightness = calBrightness(sum, myData.denominator, pixelNum);
        printf("%.3f \n", brightness);
        Pnmrdr_free(&myPnmrdr);
        fclose(fp);
        return 0;
}


/*
 *      The calBrightness is used to calculate the average brightness 
 *      of the image.
 *      sumPix is the sum of the brightness in all pixels calculated in
 *      the main function. The denominatorPix is the denominator value 
 *      of the image and the numPix is the total number of pixels in the
 *      image.
 *      The return value is the average brightness which should be between 
 *      0 and 1.
 */
double calBrightness(double sumPix, int denominatorPix, int numPix) 
{
        double aveBrightness;
        aveBrightness = sumPix / ( denominatorPix * numPix );
        return aveBrightness;
}
