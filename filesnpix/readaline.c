/*
 *      filename: readaline.c
 *      by Jun Wang, Yucheng He, 09/18/2015
 *      hw1, comp40
 *
 *      This function is used to read a line from the input file.
 *      
 */


#include <stdio.h>
#include <stdlib.h>
#include "readaline.h"

/*
 *      This function is used to read a line from the input file which
 *      is pointed by the file pointer *inputfd. The pointer datap is 
 *      used to pointing the pointer pointing to the string read from
 *      the input file by the function getc.
 */

size_t readaline(FILE *inputfd, char **datap) {
/*      
 *      n is the size of the string after reading from a lin in the file.
 *
 *      len is the size of the string for allocating memory. 
 *
 *      ptr is the pointer pointing to the string for the line in the 
 *      file.
 */
        size_t n = -1;
        size_t len = 5;
        char *ptr = (char *) malloc(len);
        /*Read the string from the text file until the newline symbol*/
        do {
                n++;
                /*  Double the memory space if the space for the 
                 *  string is not enough.
                 */
                if (n == len) {
                        len = 2 * len;
                        ptr = (char *)realloc(ptr, len);
                }
                /*Read the character by getc function one by one*/
                ptr[n] = getc(inputfd);
                /* Terminate the program if read a EOF and free memory
                 * and set the datap pointer to NULL. Then return 0 as 
                 * the length of the string
                 */
                if (ptr[n] == EOF) {
                        free(ptr);
                        *datap = NULL;
                        return 0;
                }
        } while (ptr[n] != '\n'); 

        /*Set the pointer datap to the pointer of the string*/
        *datap = ptr;
        /*return the length of the string including the newline character*/
        return n + 1;
}

