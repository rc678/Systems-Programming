#include <stdio.h>
#include <stdlib.h>
#include "memory.h"

#define blockSize 5000
#define malloc( x ) my_malloc( x, __FILE__, __LINE__ )
#define free( x ) my_free( x, __FILE__, __LINE__ )

static char heap[blockSize]; /*block used to manage dynamic memory*/

/*Allocates space for new data in our dynamic memory block if there is space available*/
void* my_malloc(unsigned int size, char* file, int line)
{
	static struct memEntry* root = 0; /*pointer to first memEntry in heap*/
	static int initialized = 0; /*becomes 1 for remainder of process after first successful malloc*/
	struct memEntry* ptr; /*pointer to iterate through memEntries*/
	struct memEntry* succ; /*pointer to next memEntry*/
	void* result = NULL; /*return pointer*/
	
	/*Cannot allocate data with no size*/
	if(size == 0)
	{
		printf("Error in file %s on line %d: nothing to allocate\n", file, line);
		return 0;
	} 

	/*Data of a negative size is an error*/
	if(size < 0)
	{
		printf("Error in file %s on line %d: not a valid memory size\n", file, line);
		return 0;
	}

	/*Initialze first memEntry if heap is currently empty*/
	if(initialized == 0)/*root is not initialized*/
	{
		root = (struct memEntry*) heap;
		root->prev = 0;
		root->next = 0;
		root->prev = root->next;
		root->size = blockSize - sizeof(struct memEntry); /*set size of first memEntry to size of rest of heap*/
		root->free = 1;
		initialized = 1;	
	}

	ptr = root;

	/*iterate through memEntries in heap*/
	while(ptr != 0)
	{
		int loc  = (int)(((char*)ptr + sizeof(struct memEntry) + size) - (char*)heap); /*used to check if we have reached the end of the heap*/
		if(ptr->size < size) /*if this chunk of memory is too small, jump to next memEntry*/
		{
			/*If we have also reached the end of the heap, exit the loop*/
			if (loc > blockSize){
				break;
			}
			ptr = ptr->next; 
		}
		else if(ptr->free == -1)/*if the block is in use, jump to next memEntry*/
		{
			ptr = ptr->next;
		}
		else if(ptr->size < (size + sizeof(struct memEntry))) /*If this chunk of memory is freed and large enough for data but not large enough for a new memEntry, allocate data here*/ 
		{
			ptr->free = -1;
			result = (char*) ptr + sizeof(struct memEntry);
			return result;
		}
		else /*Otherwise make a new memEntry and add it after the current one in the linked list of memEntries. Then allocate the data here*/
		{
			succ = (struct memEntry*) ((char*)ptr + sizeof(struct memEntry) + size);
			succ->prev = ptr;
			succ->next = ptr->next;
			if(ptr->next != 0)
			{
				ptr->next->prev = succ; /*put succ between current memEntry and next memEntry*/
			}
			ptr->next = succ;
			succ->size = ptr->size + sizeof(struct memEntry)-size; /*adjust size appropriately for future allocation*/
			succ->free = 1;
			ptr->size = size; /*Set new size of memEntry to new allocation size*/
			ptr->free = -1;
			result = (char*)ptr + sizeof(struct memEntry);
			return result;
		}
	}/*end of while*/

	/*If we broke the loop before successfully allocating, there must be saturation*/
	printf("Error in file %s on line %d: Dynamic memory is saturated - there are no available chunks of memory big enough to allocate this data\n", file, line); 
	return 0;
}/*end of function*/

/*Frees chunks of space in our dynamic memory block if input is valid*/
void my_free(void* ptr, char* file, int line)
{
	int loc  = (int)((char*)ptr - (char*)heap); /*relative location of the pointer within the dynamic memory block*/

	/*If the data is not within the boundaries of our dynamic memory block wihtin memory, it was never allocated*/
	if(loc < 0 || loc > blockSize){
		printf("Error in file %s on line %d: Trying to free a pointer that was never allocated\n", file, line);
		return;
	}
	
	struct memEntry *curr, *previous, *nxt;

	curr = (struct memEntry*)((char*)ptr - sizeof(struct memEntry)); /*curr now points to the memEntry corresponding to input data*/

	/*If the offset of the current memEntry where free would be located does not hold valid data, then this must be an invalid address to free within the memory block*/
	if(curr->free != -1 && curr->free != 1)
	{
		printf("Error in file %s on line %d: Trying to free a pointer to dynamic memory that was not returned from my_malloc\n", file, line);
		return;
	}

	/*If the pointer is already free, then it cannot be freed again*/
	if(curr->free == 1){
		printf("Error in file %s on line %d: Trying to free a pointer that has already been freed\n", file, line);
		return;
	}

	previous = curr->prev;

	if(previous == NULL || previous->free == -1) /*if the previous memEntry is in use or null, rest the free boolean*/
	{
		curr->free = 1;
		previous = curr;
	} else { /*otherwise, adjust next and prev pointers to ignore current memEntry and include size of current memEntry in size of previous memEntry*/
		previous->size += (sizeof(struct memEntry) + curr->size);
		previous->next = curr->next;
	 	if(curr->next != 0)
		{
			curr->next->prev = previous;
		}
	}

	nxt = curr->next;
	/*if next memEntry exists and is free, addjust size of previous memEntry to include that one too and adjust pointers around to ignore it*/
	if(nxt != 0 && nxt->free == 1)
	{
		previous->size += (sizeof(struct memEntry) + nxt->size);
		previous->next = nxt->next;
		if (nxt->next != NULL)
		{
			nxt->next->prev = previous;
		}
	}
}

/*include for testing purposes?*/
int main(int argc, char** argv)
{
	/*
	int x;
	free(&x);

	int *y = (int*)malloc(sizeof(int));
	free(y);
	free(y + 1);
	free(y);

	y = (int*)malloc(sizeof(int));
	free(y);

	int *a = (int*)malloc(sizeof(int));
	int *b = (int*)malloc(sizeof(int));
	int *c = (int*)malloc(sizeof(int));
	int *d = (int*)malloc(sizeof(int));
	int *e = (int*)malloc(sizeof(int));
	int *f = (int*)malloc(sizeof(int));
	int *g = (int*)malloc(sizeof(int));
	int *h = (int*)malloc(sizeof(int));

	free(h);

	struct memEntry *temp;

	for(temp = (struct memEntry*)heap; temp != NULL; temp = temp->next){
		if (temp->free == -1){
			printf("size of dynamically allocated space for this memEntry is %d\n", temp->size);
		}
	}
	*/
	
	int casenum;


	if(argc != 2){
		printf("Usage: ./assignment6_ta_tests <int>casenum\n");
	}
	casenum = atoi(argv[1]);


	printf("Case num: %d\n",casenum);

	if(casenum == 1){
		//    1. free pointer never allocated
		int i;
		int *ip;
		printf("\tTA: Mallocing pointer\n");
		ip = malloc(sizeof(int));
		printf("\tTA: Freeing pointer %p\n",&i);
		free(&i);
		printf("Done\n");

	}else if(casenum == 2){
		//    2. free pointer in middle of allocated memory block
		int *ip;
		printf("\tTA: Mallocing pointer\n");
		ip = malloc(10*sizeof(int));
		printf("\tTA: Freeing pointer %p\n",ip+7);
		free(ip+7);
		printf("Done\n");

	}else if(casenum == 3){
		//    3. free pointer between two memory blocks

		char *cp;
		int *ipnext;
		printf("\tTA: Mallocing pointer\n");
		cp = malloc(10*sizeof(char));
		printf("\tTA: Mallocing pointer\n");
		ipnext = malloc(10*sizeof(int));
		printf("\tTA: Freeing pointer %p\n",cp+12);
		free(cp+12);
		printf("Done\n");

	}else if(casenum == 4){
		//    4. freeing pointer a couple bytes before memory block (so into MemEntry struct). Then attempt another malloc.
		char* cp;
		int*ip;
		printf("\tTA: Mallocing pointer\n");
		cp = malloc(15*sizeof(char));
		printf("\tTA: Freeing pointer %p\n",cp-2);
		free(cp-2);
		printf("\tTA: Mallocing pointer\n");
		ip = malloc(16*sizeof(int));
		printf("Done\n");

	}else if(casenum == 5){
		//    5.  redundant freeing of same pointer.
		int *ip1;
		printf("\tTA: Mallocing pointer\n");
		ip1 = malloc(10*sizeof(int));
		printf("\tTA: Freeing pointer %p\n",ip1);
		free(ip1);
		printf("\tTA: Freeing pointer %p\n",ip1);
		free(ip1);
		printf("Done\n");


	}else if(casenum == 6){
		//    6.  saturation with a while loop.  Program should not fail.
		int pcount = 0;
		while(malloc(TA_TEST_BLOCKSIZE*sizeof(int))){
			pcount++;
			printf("Allocated %d pointers of size %d\n",pcount,TA_TEST_BLOCKSIZE);
		}
		printf("Done\n");


	}else if(casenum == 7){
		//    7.  Saturation, then freeing everything, then saturation with same list of malloc's (should yield same # pointers)
		int*ip;
		int pcount = 0;
		int**doubleip;
		int **dip_leftover = 0;
		while(ip = malloc(TA_TEST_BLOCKSIZE*sizeof(int))){
			pcount++;
			if(pcount == 1){
				doubleip = calloc(1,sizeof(int*));
				*doubleip = ip;
			}else{
				if((dip_leftover=realloc(doubleip,pcount*sizeof(int*))) != 0){
					doubleip = dip_leftover;
					doubleip[pcount-1] = ip;
					dip_leftover = 0;
				}else{
					dip_leftover = &ip;
					printf("Allocated 1 extra but couldn't realloc a pointer\n");
					break;
				}

			}
			printf("TA: allocated %d blocks of size %d so far\n", pcount, TA_TEST_BLOCKSIZE);
		}

		//free
		int i;
		int cap = pcount;
		if(dip_leftover != 0){
			cap -= 1;
			free(*dip_leftover);
		}
		for(i = 0; i < cap; i++){
			free(doubleip[i]);
		}

		//malloc again
		int secondpcount = 0;
		while(malloc(TA_TEST_BLOCKSIZE*sizeof(int))){
			secondpcount++;
		}

		if(pcount == secondpcount){
			printf("\tPassed.\n");
		}else{
			printf("\tFailed\n");
		}

		printf("Done\n");

	}else if(casenum == 8){
		//    8.  Regular call to malloc (with some modifications of pointer)

		int *x;
		int *y;
		printf("\tTA: Mallocing pointer\n");
		x = malloc(sizeof(int));
		printf("\tTA: Mallocing pointer\n");
		y = malloc(sizeof(int));

		*x = 5;
		*y = 7;
		printf("\tBefore swap: x=%d,y=%d\n",*x,*y);
		int temp;
		temp = *x;
		*x = *y;
		*y = temp;
		printf("\tAfter swap: x=%d,y=%d\n",*x,*y);
		printf("Done\n");
	}else if(casenum == 9){
		//    9.  Regular call to free (with some modifications of pointer before free)
		int *x;
		int *y;
		printf("\tTA: Mallocing pointer\n");
		x = malloc(sizeof(int));
		printf("\tTA: Mallocing pointer\n");
		y = malloc(sizeof(int));

		*x = 5;
		*y = 7;
		printf("\tBefore swap: x=%d,y=%d\n",*x,*y);
		int temp;
		temp = *x;
		*x = *y;
		*y = temp;
		printf("\tAfter swap: x=%d,y=%d\n",*x,*y);
		printf("\tTA: Freeing pointer %p\n",x);
		free(x);
		printf("\tTA: Freeing pointer %p\n",y);
		free(y);
		printf("Done\n");

	}else if(casenum == 10){
		//   10.  A trickier set of malloc's and free's.
		int **matrix;
		printf("\tAllocating matrix\n");
		matrix = malloc(10*sizeof(int*));
		int i;
		for(i = 0; i < 10; i++){
			matrix[i] = malloc(10*sizeof(int));
		}
		printf("\tFreeing matrix\n");
		for(i = 0; i < 10; i++){
			free(matrix[i]);
		}
		free(matrix);
		printf("Done\n");
	}
}

