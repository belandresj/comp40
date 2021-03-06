/*
 *      filename: interface.h
 *      by Jun Wang, Yucheng He, 09/18/2015
 *      hw1, comp40
 *      
 *      This is the header file for the interface of the program simlines.
 */
#ifndef INTERFACE_INCLUDED
#define INTERFACE_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include "readaline.h"
#include "atom.h"
#include "seq.h"
#include "table.h"
#include "interface.h"



size_t convertString(char *inputString, size_t size, char **resultString);
void readFile(FILE *fstream, char *filename, Table_T tableData);
void printGroupMatch(const void* key, void **value, void* cl);
void vfree(const void* key, void **value, void* cl);
int isCharacter(char c);







#endif
