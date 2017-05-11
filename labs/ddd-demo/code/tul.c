#include <stdio.h>
#include <stdlib.h>

#include "tul.h"



void read_three_unsigned_longs(const char *inputline, 
			       struct three_unsigned_longs *p)
{
	int i = 0;
	i = sscanf(inputline, "%lu %lu %lu", &(p->a), &(p->b), &(p->c));	
	printf("i: %d\n", i);

}
