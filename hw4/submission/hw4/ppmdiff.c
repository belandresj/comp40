#include <stdio.h>
#include <stdlib.h>
#include "pnm.h"
#include "assert.h"
#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"
#include "pnm.h"
#include "math.h"

struct mycl {
	float* sum;
	Pnm_ppm myPPM;
	unsigned denominator1;
};
void compute(int i, int j, A2Methods_UArray2 array2, void *elem, void *cl);
int main(int argc, char *argv[])
{    
        FILE *fp1 = NULL;
	FILE *fp2 = NULL;

        if (argc > 3) {
                fprintf(stderr, "Too much input.\n");
                exit(EXIT_FAILURE);
        }

        if(argv[1][0] == '-') {
		fp1 = stdin;
		fp2 = fopen(argv[2], "r");
	} else if (argv[2][0] == '-'){
		fp2 = stdin;
		fp1 = fopen(argv[1], "r");
	} else {
		fp1 = fopen(argv[1], "r");
		fp2 = fopen(argv[2], "r");
	}
	
	A2Methods_T methods = uarray2_methods_plain; 
	assert(methods); 
	A2Methods_mapfun *map = methods->map_default; 
	assert(map);
        Pnm_ppm myPPM1 = Pnm_ppmread(fp1, methods);
 	Pnm_ppm myPPM2 = Pnm_ppmread(fp2, methods);
        assert(myPPM1 &&  myPPM2);
        unsigned w1 = myPPM1->width;
	unsigned w2 = myPPM2->width;
	unsigned h1 = myPPM1->height;
        unsigned h2 = myPPM2->height;

        unsigned lesserWidth = 0;
        unsigned lesserHeight = 0;

        if(w1 > w2){
		lesserWidth = w2;
	}
	else{
		lesserWidth = w1;
	}
	if(h1 > h2){
		lesserHeight = h2;
	}
	else{
		lesserHeight = h1;
	}

	if ((int)(w2 - w1) < -1 || (int)(w2 - w1) > 1) {
		fprintf(stderr, "Image size too different");
		printf("1.0\n");
	}
	if ((int)(h2 - h1) < -1 || (int)(h2 - h1) > 1) {
		fprintf(stderr, "Image size too different");
		printf("1.0\n");
	}
	
	struct mycl *newCl = malloc(sizeof(struct mycl));
	*newCl->sum = 0;
	newCl->myPPM = myPPM2;
	newCl->denominator1 = myPPM1->denominator;
	myPPM1->width = lesserWidth;
	myPPM1->height = lesserHeight;
	map(myPPM1->pixels, compute, newCl);
	
	float sum = *(newCl->sum);
	double E = sqrt((double)(sum / (3 * lesserWidth * lesserHeight)));

	printf("%.3f\n", (float) E);

		        
	
        fclose(fp1);
	fclose(fp2);
        return 0;
}

void compute(int i, int j, A2Methods_UArray2 array2, void *elem, void *cl)
{
	(void) array2;
	struct mycl* myCl = (struct mycl*) cl;
	float* sum = myCl->sum;
	Pnm_ppm newPPM = myCl-> myPPM;
        struct Pnm_rgb* rgb2 =(newPPM)->methods->at((newPPM)->pixels, i, j); 
	struct Pnm_rgb* rgb1 = (struct Pnm_rgb*) elem;
	unsigned denominator1 = myCl->denominator1;
	unsigned denominator2 = myCl->myPPM->denominator;
	float red1 = ((float)(rgb1)->red) / denominator1;
	float green1 = ((float)(rgb1)->green) / denominator1;
	float blue1 = ((float)(rgb1)->blue) / denominator1;
	float red2 = ((float)(rgb2)->red) / denominator2;
	float green2 = ((float)(rgb2)->green) / denominator2;
	float blue2 = ((float)(rgb2)->blue) / denominator2;
	*sum = *sum +(red1 - red2) * (red1 - red2) +(green1 - green2) * (green1 -green2) +(blue1 - blue2) * (blue1 - blue2);

}
