/*
 *      filename: simlines.
 *      by Jun Wang, Yucheng He, 09/18/2015
 *      hw1, comp40
 *
 *      This file is the source file for the program named simlines. It can
 *      read several text file and find all the similar lines in each file.
 *      After reading the file, it can also print location information of
 *      all the similar lines in files with a specific format. All the other
 *      functions are implenmented in interface.c file.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "readaline.h"
#include "atom.h"
#include "seq.h"
#include "table.h"
#include "interface.h"

/*
 *      The main function is used to read the text files one by one and 
 *      read all the lines into the atom which is key in the table.
 *      The stuct storing the location information will be stored in the
 *      sequence which is the corresponding value in the table.
 */
int main(int argc, char* argv[]) 
{       
        /*Exit the program if there is no input text file*/
        if (argc == 1) {
                return 0;
        }
        
        /*Declare an object of table to store the atoms and the sequence*/
        Table_T mytable = Table_new(0, NULL, NULL);
        
        /*Define a for loop to read the text file one by one.*/
        for (int i = 1; i < argc; i++) {
                FILE *fp = fopen(argv[i], "r");
                        /* If the file name is not right, exit failure */
                        if (fp == NULL) {
                        Table_map(mytable, vfree, NULL);
                        Table_free(&mytable);
                        exit(EXIT_FAILURE);
                        }
                readFile(fp, argv[i], mytable);
                fclose(fp);
        }
        
        /* 
         * Print the similar lines if the length of the sequence stored in
         * the table is larger than 1.
         */
        /* cl is the value of isFirst passed by &cl*/
        int cl = 1;
        Table_map(mytable, printGroupMatch, &cl);

        /*
         * Free the memory allocated to the struct and the sequence by
         * traversing the whole table
         */
        Table_map(mytable, vfree, NULL);

        /*Free the memory allocated by the table*/
        Table_free(&mytable);
        return 0;
}

