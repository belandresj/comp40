/*
 *      filename: inteface.c
 *      by Jun Wang, Yucheng He, 09/18/2015
 *      hw1, comp40
 *
 *      This is the interface of the program simline including all the 
 *      functions the program needs.
 */




#include <stdio.h>
#include <stdlib.h>
#include "readaline.h"
#include "atom.h"
#include "seq.h"
#include "table.h"
#include "interface.h"

/*
 *      This struct is used to store the location information including 
 *      filename and the specific line number for the corresponding 
 *      lines in the files.     
 */
struct Location {
        char *filename;
        int lineNum;
};

/*
 *      convertString is used to read a string storing all the characters 
 *      of a single line in a text file and edit this tring. Through 
 *      this function, there will be only one space between the neighboring
 *      words in the string and get rid of the unecessary characters like
 *      comma.
 *      The inputString is the string readed by readaline function and it 
 *      containes unecessary characters. The size is the length of the 
 *      inputString. The resultString is the pointer pointing to the pointer
 *      of the string after converting by this function.
 */
size_t convertString(char *inputString, size_t size, char **resultString) {
/*
 *      len is the size for allocating the memory of the output string.
 *
 *      n is the size of the output string after filtering the unecessary
 *      characters.
 *      
 *      ptr is the pointer pointing to the output string.
 */
        size_t len = 80;
        size_t n = 0;
        char *ptr = (char *) malloc(len);

        /*i is the index for pointing the specific position in original string*/
        size_t i=0;
        
        /*Check whether the original string has only a newline character.*/
        if (inputString[0] == '\n') {
                ptr[0] = '\0';
                *resultString = ptr;
                return 1;
        }
        
/*      
 *      Check whether there are unnecessary characters at beginning part of 
 *      the original string
 */
        while (isCharacter(inputString[i]) == 0) {
                i++;
                if (i == size - 1) {
                        ptr[0] = '\0';
                        *resultString = ptr;
                        return 1;
                }
        }
/* 
 *      The for loop will read the input string one by one and remove the 
 *      unecessary characters.
 */
        for (; i < size; i++) {
/*
 *      Double the memory space for the output string if the original space 
 *      is not enough.
 */
                if (n == len) {
                        len = 2 * len;
                        ptr =  (char *) realloc(ptr, len);
                }
/*
 *      Assign the character from original string into output string
 *      if the character is useful. 
 */
                if (isCharacter(inputString[i]) == 1) {
                        ptr[n] = inputString[i];
                        if (inputString[i+1] == '\n') {
                                n++;
                                ptr[n] = '\0';
                                *resultString = ptr;
                                return n + 1;
                        }
                        n++;
                        continue;
                }
/*
 *      When the replace the multiple unnecessary character with
 *      one space 
 */
                do {
                        if (isCharacter(inputString[i + 1]) == 1) {
                                ptr[n] = ' ';
                                break;
                        } else {
                                i++;
                        }
                } while (i < size - 1); 
                /* Put '\0' into the last position of output string*/
                if (i == size - 1) {
                        ptr[n] = '\0';
                        *resultString = ptr;
                        return n + 1;
                }
                n++;
        }
/*
 *      Increase the memory space for the output string if the space is 
 *      not enough
 */
        *resultString = ptr;
        return n + 1;
}

/*
 *      readFile function is used to read all the lines from the text file
 *      into several strings which are stored in the Atom. Then, this atom
 *      will be checked in the table. If the table has no corresponding 
 *      atom, this new atom will be stored as a key in that table and the 
 *      pointer pointing to the corresponding location information in the 
 *      sequence will be stored as the value in the table. If the table has
 *      the corresponding atom in the table, the corresponding sequence will
 *      be updated by adding the new struct with the location information. 
 *
 */
void readFile(FILE* fstream, char* filename, Table_T myTable) 
{
/*
 *      readString is the output of the readaline function. It stores the 
 *      string without processing.
 *      
 *      outputString is the output of the convertString function. It has
 *      removed all the unecessary characters in the string.
 *      
 *      lenLine is the size of the readString.
 *
 *      newLen is the size of the outputString.
 *
 *      linNum is the line number of the corresponding string. It will be 
 *      stored into the struct as the location information.
 */
        char *readString = NULL;
        size_t lenLine = 0;
        size_t newLen = 0;
        char *outputString = NULL;
        int lineNum = 1;
/*      
 *      This loop is used to read all lines from the text file. It is used 
 *      to:
 *      count the line number of the string in the text file,
 *      convert the original string and store it into atom,
 *      store the atom and sequence with its location information into table
 *      
 *      This loop will exit when the readString is NULL, which means that 
 *      there are no more lines in the text file to be read.
 */
        while (1) {
                lenLine = readaline(fstream, &readString);
                /*No more lines, exit the loop*/
                if (readString==NULL) {
                        free(readString);
                        break;
                }
                /*Convert the string*/
                newLen = convertString(readString, lenLine, &outputString);
/*      
 *      Declare an object of atom and store the converted strings and
 *      the length of the string into atom.
 */
                const char *myAtom = NULL;
                myAtom = Atom_new(outputString, (int) newLen);
                /*Create a Location pointer*/
                struct Location *locP = malloc(sizeof(struct Location));
                locP->filename = filename;
                locP->lineNum = lineNum;
/*
 *      Check whether the atom is in the table. If so, the corresponding
 *      struct with location information will be added into the sequence.
 *      If not, create a new sequence to store the struct pointer and add
 *      the atom-sequence pair into the table.
 */
                if (Table_get(myTable, myAtom) == NULL) {
                        Seq_T mySeq = Seq_seq(locP, NULL);
                        Table_put(myTable, myAtom, mySeq);
                } else {
                        Seq_addhi(Table_get(myTable, myAtom), locP);
                }

                lineNum++;
                /*Free memory of strings*/
                free(readString);
                free(outputString);
        }
        
}

/*
 *      printGroupMatch is used to print all the similar lines stored in 
 *      table with all the location information stored in the corresponding
 *      sequence.
 */
void printGroupMatch(const void* key, void** value, void* cl)
{
        /* isFirst is to know whether is the first print Atom*/
        int isFirst = *(int*) cl;
       
        /* Cast input variable: key *value and Seq_length(seq)*/
        char * atom = (char*) key;
        Seq_T seq = (Seq_T) *value;
        int seqLen = Seq_length(seq);
        /* Do not print the atom only contain '\0'*/
        if (*atom == '\0') {
                return;
        }
        /* print the location information if it is simlines*/
        if (seqLen == 1) {
                return;
        }
        /*print '\n', if it is not the first print Atom*/
        if (isFirst != 1) {
                printf("\n");
                }
        printf("%s\n", atom);
        for (int i = 0; i < seqLen; i++) {
                struct Location* myLoc = (struct Location*) Seq_get(seq, i);
                printf("%-20s %7d\n", myLoc->filename, myLoc->lineNum);
        }
        isFirst = 0;
        *(int *)cl = isFirst; 
        return;
}

/*
 *      Free the memory of struct pointer and sequence.
 *
 */
void vfree(const void* key, void ** value, void* cl)
{       
        /* key and cl are not used, so void them*/
        (void) key;
        (void) cl;
        /* Cast input variable*/
        Seq_T seq = (Seq_T) *value;

        int len = Seq_length(seq);
        /* Free the struct pointer*/
        for (int i = 0; i < len; i++){
                free(Seq_get(seq, i));
        }
        /* Free the sequence*/
        Seq_free(&seq);
}

/*
 *       Determine whether the character is useful, if so return 1
 *       if not return 0.
 */
int isCharacter(char c) 
{
        if (c >= 'a' && c <= 'z') {
                return 1;
        }
        else if (c >= 'A' && c <= 'Z') {
                return 1;
        }
        else if (c >= '0' && c <= '9') {
                return 1;
        }
        else if (c == '_') {
                return 1;
        }
        else {
                return 0;
        }
}

