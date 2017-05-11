/*
 *	filename: um.c
 *      by Yucheng He and Griffin Richards
 *	12/4/2015
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <sys/stat.h>

typedef struct Segment_T {
  uint32_t * instrs;
  uint32_t length;
  int size;
}*Segment_T;

/* create a new segment with the size of nums */
static inline Segment_T Seg_new(int nums)
{
	assert(nums > 0);
	Segment_T newSeg = malloc(sizeof(*newSeg));
	newSeg->instrs = malloc(sizeof(uint32_t) * nums);
	newSeg->length = 0;
	newSeg->size = nums;
	return newSeg;
}

/* free the seg */
static inline void Seg_free(Segment_T seg)
{
	assert(seg != NULL);
	free(seg->instrs);
	free(seg);
	return;
}

/* store value in the seg at index i */
static inline void Seg_put(Segment_T seg, uint32_t i, uint32_t val)
{
	/* rely on the assert in Seq_put for check the bound */
	assert(seg != NULL);
	assert(i < seg->length);
	(seg->instrs)[i] = val;
	return;
}

/* get the value at the seg at index i */
static inline uint32_t Seg_get(Segment_T seg, uint32_t i)
{
	assert(seg != NULL);
	assert(i < seg->length);
	return (seg->instrs)[i];
}

/* add a uint32_t into a seg */
static inline void addInstr(Segment_T seg, uint32_t instr)
{
	assert(seg != NULL);
	int length = seg->length;
	int size = seg->size;
	if (size == length) {
		uint32_t* newInstrs = 
			realloc(seg->instrs, sizeof(uint32_t) * size * 2);
		seg->size = size * 2;
		free(seg->instrs);
		seg->instrs = newInstrs;
	}
	
	(seg->instrs)[length] = instr;
	seg->length = length + 1;
	return;
}

static inline uint32_t getInstr(FILE* input, int a)
{
	uint32_t word = 0;
	a = a << 24;
	int b = getc(input) << 16;
	int c = getc(input) << 8;
	int d = getc(input);

	return (uint32_t) word + a + b + c + d;
}

static inline void checkInput(int size)
{
	/* check the size of file should be the multiple of 4 and not 0 */
	if (size % 4 != 0 || size == 0) {
		fprintf(stderr, "Improper file content\n");
		exit(EXIT_FAILURE);
	} 
	return;
}

static inline int getSize(char* filename)
{
 	struct stat fileInfo;
	stat(filename, &fileInfo);
	int size = fileInfo.st_size;
	return size;
}

static inline Segment_T load(FILE* input, char* filename)
{	
	int size = getSize(filename);
	checkInput(size);
	Segment_T program = Seg_new(size / 4);
	int a;
	/* pack four bytes into a word instruction wiht getInstr */
	while ((a = getc(input)) != EOF) {
		addInstr(program, getInstr(input, a));
	} 
	
	return program;
}

typedef struct Stack_T {
	uint32_t* stack;
	int stackLen;
	int stackSize;
} *Stack_T;

typedef struct Mem {
	Segment_T* segs;
	Stack_T unmapped;
	int prog_counter;
	int segLen;
	int segSize;
} *Mem;

static inline Stack_T Stack_new()
{
	Stack_T newStack = malloc(sizeof(*newStack));

	newStack->stack = malloc(sizeof(uint32_t) * 100);
	newStack->stackSize = 100;
	newStack->stackLen = 0;
	return newStack;
}

static inline void Stack_free(Stack_T myStack)
{
	assert(myStack != NULL && myStack->stack != NULL);
	free(myStack->stack);
	free(myStack);
	return;
}

static inline void Stack_push(Stack_T myStack, uint32_t index)
{
	assert(myStack != NULL && myStack->stack != NULL);
	int len = myStack->stackLen;
	int size = myStack->stackSize;
	if (len == size) {
		uint32_t* newStack = malloc(sizeof(uint32_t) * size * 2);
		for (int i = 0; i < len; i++) {
			newStack[i] = (myStack->stack)[i];
		}
		free(myStack->stack);
		myStack->stack = newStack;
		myStack->stackSize = size * 2;
	}
	(myStack->stack)[len] = index;
	myStack->stackLen = len + 1;
	return;

}

static inline uint32_t Stack_pop(Stack_T myStack)
{
	assert(myStack != NULL && myStack->stack != NULL);
	assert(myStack->stackLen != 0);
	int len = myStack->stackLen;
	uint32_t i = (myStack->stack)[len- 1];
	myStack->stackLen = len - 1;
	return i;
}

/* initialize the new mem and return it */
static inline Mem new_mem(Segment_T program)
{
	Mem myMem = malloc(sizeof(*myMem));
	assert(myMem != NULL);
	myMem->segs = malloc(sizeof(Segment_T) * 100);
	assert(myMem->segs != NULL);
	(myMem->segs)[0] = program;
	myMem->segLen = 1;
	myMem->segSize = 100;
	myMem->unmapped = Stack_new();
	myMem->prog_counter = 0;
	return myMem;
}

/* free the mem */
static inline void free_mem(Mem* myMem)
{
	assert(myMem != NULL);
	for (int i = 0; i < (*myMem)->segLen; i++) {
		Seg_free(((*myMem)->segs)[i]);
	}
	free(((*myMem)->segs));
	Stack_free((*myMem)->unmapped);
	free(*myMem);
}

static inline void addSeg(Mem myMem, Segment_T seg)
{
	assert(myMem != NULL);
	int len = myMem->segLen;
	int size = myMem->segSize;
	if (len == size) {
		Segment_T* newSegs = malloc(sizeof(Segment_T) * size * 2);
		for (int i = 0; i < len; i++) {
			newSegs[i] = (myMem->segs)[i];
		}
		free(myMem->segs);
		myMem->segs = newSegs;
		myMem->segSize = size * 2;
	}
	(myMem->segs)[len] = seg;
	myMem->segLen = len + 1;
	return;
}

/* 
 * create a segment with the size of nums, and return the index of the segs of 
 * the mapped segment. There are two conditons, if the unmapped stack is not 
 * empty, we reuse the segment index stored in unmapped stack and pop that out.
 * otherwise we just create a new segments and addhi into segs, the return index
 * is the length of segs.
 */
static inline uint32_t map_segment(Mem myMem, uint32_t nums)
{
	assert(myMem != NULL);
	
	Segment_T newSeg = Seg_new(nums);
	for (uint32_t i = 0; i < nums; i++) {
	      (newSeg->instrs)[i] = 0;
	}
	newSeg->length = nums;
	uint32_t index;
	if (myMem->unmapped->stackLen == 0) {
		addSeg(myMem, newSeg);
		index = myMem->segLen - 1;
	} else {
		index = Stack_pop(myMem->unmapped);
		Seg_free((myMem->segs)[index]);
		(myMem->segs)[index] = newSeg;
	};
	return index;
}

/* unmap the segment (push the index into unmapped stack)*/
static inline void unmap_segment(Mem myMem, uint32_t index)
{
	assert(myMem != NULL);
	Stack_push(myMem->unmapped, index);
}

/* load the val at myMem[rb][rc] */
static inline uint32_t load_segment(Mem myMem, uint32_t rb, uint32_t rc)
{
	assert(myMem != NULL);
	return ((myMem->segs)[rb]->instrs)[rc];
  
}

/* store the val into myMem[rb][rc] */
static inline void store_segment(Mem myMem, uint32_t ra, uint32_t rb,
                                                                   uint32_t val)
{

	assert(myMem != NULL);
	((myMem->segs)[ra]->instrs)[rb] = val;
	return;
}

uint32_t R[8] = {0};

/* contional move instruction */
static inline void cMove(uint32_t a, uint32_t b, uint32_t c)
{
	if(R[c] != 0) {
		R[a] = R[b];
	}
	return;
}

/* add instruction */
static inline void add(uint32_t a, uint32_t b, uint32_t c)
{

	R[a] = (R[b] + R[c]) % 4294967296;
	return;
}

/* multiplication instruction */
static inline void multiply(uint32_t a, uint32_t b, uint32_t c)
{
	R[a] = (R[b] * R[c]) % 4294967296;
	return;
}

/* division instruction */
static inline void divide(uint32_t a, uint32_t b, uint32_t c)
{
	R[a] = R[b] / R[c];
	return;
}

/* Bitwise NAND instruction */
static inline void bitwiseNAND(uint32_t a, uint32_t b, uint32_t c)
{
	R[a] = ~(R[b] & R[c]);
	return;
}

/* halt instruction */
static inline void halt(Mem myMem)
{	
	free_mem(&myMem);
	exit(0);
}

/* ouput instruction */
static inline void output(uint32_t c)
{
	/* output should be in 0 - 255 */
	assert(R[c] < 256);
	/* use putchar to output character */
	putchar(R[c]);
	return;
}

/* ouput instruction */
static inline void input(FILE* instrs, uint32_t c)
{
	uint32_t instr = getc(instrs);
	if (instr == (uint32_t) EOF) {
		instr = - 1;
	}
	R[c] = instr;
}

/* load program instruction */
static inline void loadProgram(Mem myMem, uint32_t b, uint32_t c)
{
	/* if Rb is not 0, copy the M[Rb] into M[0]*/
	if (R[b] != 0) {
		Segment_T temp = (myMem->segs)[R[b]];
		uint32_t length = temp->length;
		Segment_T seg = Seg_new(length);
		for (uint32_t i = 0; i < length; i++) {
			seg->instrs[i] = temp->instrs[i];
		}
		seg->length = length;
		(myMem->segs)[0] = seg;
	}
	/* when Rb is zero, just change the program counter */
	/* program counter is R[c] - 1 (since add 1 in the loop) */
	myMem->prog_counter = R[c] - 1;
	return;
}

static inline void execute(Segment_T program)
{
	/* create the myMem with the program */
	Mem myMem = new_mem(program);
	/*
	 * in while loop read the instruction and execute the instruction, 
	 * until reach halt(), then end the program.
	 */

	while (1) {
		uint32_t word, opcode, a, b, c, val;
		/* load the word with program counter */
		/* remove function call of loadSegment */
		word = ((myMem->segs)[0]->instrs)[myMem->prog_counter];
		uint32_t temp = word;
		/* get the opcode from word, and do corresponding instruction */
		opcode =  temp >> 28;
		if (opcode == 13) {
			/* Get a and val */
			temp = word;
			temp = temp << 4;
			a = temp >> 29;
			temp = word;
			temp = temp << 7;
			val = temp >> 7;
			R[a] = val;
		} else {
			/* Get a, b and c (register index) */
			temp = word;
			temp = temp << 23;
			a = temp >> 29;
			temp = word;
			temp = temp << 26;
			b = temp >> 29;
			temp = word;
			temp = temp << 29;
			c = temp >> 29;
			switch(opcode) {
				case 0 :
					cMove(a, b, c);
					break;
				case 1 :
					R[a] = 
                                        ((myMem->segs)[R[b]]->instrs)[R[c]];
					break;
				case 2 :
					((myMem->segs)[R[a]]->instrs)[R[b]] = 
                                        R[c];
					break;
				case 3 :
					add(a, b, c);
					break;
				case 4 :
					multiply(a, b, c);
					break;
				case 5 :
					divide(a, b, c);
					break;
				case 6 :
					bitwiseNAND(a, b, c);
					break;
				case 7 :
					halt(myMem);
					break;
				case 8 :
					R[b] = map_segment(myMem, R[c]);
					break;
				case 9 :
					unmap_segment(myMem, R[c]);
					break;
				case 10 :
					output(c);
					break;
				case 11 :
					input(stdin, c);
					break;
				case 12 :
					loadProgram(myMem, b, c);
					break;
			}
		}
		(myMem->prog_counter)++;
	}      
}

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

