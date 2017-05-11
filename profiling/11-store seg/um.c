/*
 *	filename: um.c
 *      by Yucheng He and Griffin Richards
 *	11/23/2015
 *	The main function for um. use load and execute to
 *      run the um code.
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "load.h"
#include "execute.h"

int main(int argc, char* argv[])
{
	/* the argc should be 2 */
	assert(argc == 2);
	FILE* input = fopen(argv[1], "r");
	assert(input != NULL);
	/* load the file into Segment_T program */
	Segment_T program = load(input, argv[1]);
	fclose(input);
	/* execute the program */
	execute(program);
	exit(0);
}

