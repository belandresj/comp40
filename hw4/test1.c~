#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <arith40.h>
#include <pnmrdr.h>
 
int main(int argc, char* argv[])
{
    Pnmrdr_T pnm;
    (void) pnm;
    (void) argc;
    (void) argv;
    unsigned y =Arith40_index_of_chroma(0.0);
    printf("%u\n", y);
    float z = Arith40_chroma_of_index(y);
    printf("%f\n", z);
//  uint16_t fiveBit = 0b11111;
//  printf("5bit %d\n", fiveBit);
    float m = 15 * 0.02;
    printf("%f\n", m);
    uint64_t mask =  (uint64_t)(pow(2, 32) - 1);
    uint64_t word = 0x3f4;
    uint64_t val = (word & mask) >> 2;
    printf("mask %lu\n", mask);
    printf("word %lu\n", word);
    printf("get val %lu\n", val);
     
    return 1;
 
}