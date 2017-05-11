/*
 *	filename: test.c
 *      by Yucheng He and Griffin Richards
 *	11/15/2015
 *	Inculde all the unit tests for memory segements
 */


#include <memory.h>
/* 
 * we create a new mem and check every variables in the mem is 
 * with proper value.
 */
void test_new_mem()
{
	printf("Testing new_mem.\n");
	Segment_T program = seg_new();
	Mem myMem = new_mem(program);
	assert(Seq_length(myMem->segs) == 1);
	assert(Stack_empty(myMem->unmapped) == 1);  
	assert(myMem->prog_counter == 0);
	seg_free(program);
	free_mem(&myMem);
	return;
}

/* 
 * we map a segment with specific length, and then check the length
 * of the segment mapped is that length or not.
 */
void test_map_segment()
{
	printf("Testing map_segment.\n");
	Segment_T seg = seg_new();
	Mem myMem = new_mem(seg);
	int i = Seq_length((Seq_T)Seq_get(myMem->segs, map_segment(myMem, 32)));
	assert(i == 32);
	seg_free(seg);
	free_mem(&myMem);
	return;
}

/* we unmap a segment and check the unmapped is empty or not */
void test_unmap_segment()
{
	printf("Testing unmap_segment.\n");
	Segment_T seg = seg_new();
	Mem myMem = new_mem(seg);
	assert(Stack_empty(myMem->unmapped) == 1);
	unmap_segment(myMem, 0);
	assert(Stack_empty(myMem->unmapped) == 0);
	seg_free(seg);
	free_mem(&myMem);
	return;
}

/* 
 * we test the load and store together, we first store a val into
 * the specific location and load that val back, check two vals are
 * are the same or not.
 */
void test_loadAndStore_segment()
{
	printf("Testing load_segment and store_segment.\n");
	Segment_T seg = seg_new();
	Mem myMem = new_mem(seg);
	uint32_t test = 111;
	store_segment(myMem, 0, 0, test);
	uint32_t result = load_segment(myMem, 0, 0);
	assert(test == result);
	seg_free(seg);
	free_mem(&myMem);
	return;
}

/* call each test in main */
int main()
{
	printf("Start Test.\n");
	test_new_mem();
	test_map_segment();
	test_unmap_segment();
	test_loadAndStore_segment();
	printf("All tests passed.\n");
	return 0;
}
