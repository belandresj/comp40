#include <bitpack.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "assert.h"
#include "math.h"
#include "stdint.h"
#include <except.h>
 
Except_T Bitpack_Overflow = { "Overflow packing bits" };
 
bool Bitpack_fitsu(uint64_t n, unsigned width)
{
       assert(width <= 64 );  
    uint64_t upperbound = pow(2, width) - 1;
    if (n > upperbound) {
        return false;
    } else {
        return true;
    }
 
}
 
bool Bitpack_fitss(int64_t n, unsigned width)
{
    assert(width <= 64 );
    int64_t upperbound = pow(2, width) - 1;
    int64_t lowerbound = - pow(2, width);
    if (n >= lowerbound && n <= upperbound) {
        return true;
    } else {
        return false;
    }
 
}
uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb)
{
       assert(width + lsb <= 64 );
       uint64_t mask = (uint64_t)(pow(2, width) - 1) << lsb;
    uint64_t val = (word & mask) >> lsb;
    return val;
}
 
int64_t Bitpack_gets(uint64_t word, unsigned width, unsigned lsb)
{
       assert(width + lsb <= 64 );
    uint64_t mask = (uint64_t)(pow(2, width) - 1) << lsb;
    int64_t val = (word & mask) >> lsb;
     
    if (val >= (int64_t)(pow(2, width - 1))) {
        return val - (int64_t)(pow(2, width));
    } else {
        return val;
    }
}
 
uint64_t Bitpack_newu(uint64_t word, unsigned width, unsigned lsb, uint64_t value)
{
        assert(width + lsb <= 64 );
        bool valid_fit = Bitpack_fitsu(value, width);
        
        if(!valid_fit){
               RAISE(Bitpack_Overflow);
               assert(0);
        }

    uint64_t mask1 = (uint64_t)(pow(2, 32) - 1);
    uint64_t mask2 = (uint64_t)(pow(2, width) - 1) << lsb;
    uint64_t mask = mask1 - mask2;
    uint64_t newVal = value << lsb;
    uint64_t newWord = word & mask;
     
    return newWord + newVal;
   
}
 
uint64_t Bitpack_news(uint64_t word, unsigned width, unsigned lsb,  int64_t value)
{
        assert(width + lsb <= 64 );
        bool valid_fit = Bitpack_fitss(value, width);
        
        if(!valid_fit){
               RAISE(Bitpack_Overflow);
               assert(0);
        }
 
    uint64_t mask1 = (uint64_t)(pow(2, 32) - 1);
    uint64_t mask2 = (uint64_t)(pow(2, width) - 1) << lsb;
    uint64_t maskWord = mask1 - mask2;
    uint64_t maskVal = (uint64_t) (pow(2, width) -1);
    uint64_t newVal = (value & maskVal) << lsb;
    uint64_t newWord = word & maskWord;
     
    return newWord + newVal;
}