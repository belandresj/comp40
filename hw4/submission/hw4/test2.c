#include <bitpack.h>
#include <stdio.h>
 
int main(int argc, char* argv[])
{
    (void) argc;
    (void) argv;
    uint64_t a = 255;
    uint64_t b = 128;
    int64_t c = -128;
    if (Bitpack_fitsu(b, 7) == false) {
        printf("b not fit in 7 bit\n");
    }
    if (Bitpack_fitss(c, 8) == true) {
        printf("c fit in 8 bit\n");
    }
 
    uint64_t d = Bitpack_getu(a, 2, 1);
    printf("%lu\n", d);
 
    int64_t e = Bitpack_gets(a, 3, 1);
    printf("%d\n",(int) e);
 
    uint64_t f = Bitpack_getu(Bitpack_newu(a, 4, 2, 11), 4, 2);
    uint64_t f1 = Bitpack_newu(a, 4, 2, 11);
    printf("%lu\n", f);
    printf("f1 %lu\n", f1);
 
    int64_t g = Bitpack_gets(Bitpack_news(a, 6, 1, -11), 6, 1);
    printf("%d\n",(int) g);
    return 0;
     
}