#include <stdio.h>
#include <stdlib.h>
#include "memory.h"

#define blockSize 5000
#define malloc( x ) my_malloc( x, __FILE__, __LINE__ )
#define free( x ) my_free( x, __FILE__, __LINE__ )

static char heap[blockSize];

void* my_malloc(unsigned int size, char* file, int line)
{
	static struct memEntry* root = 0;
	static int initialized = 0; 
	struct memEntry* ptr;
	struct memEntry* succ;
	void* result = NULL;
	
	if(size == 0)
	{
		printf("nothing to allocate\n");
		return 0;
	} 

	if(initialized == 0)/*root is not initialized*/
	{
		root = (struct memEntry*) heap;
		root->prev = 0;
		root->next = 0;
		root->prev = root->next;
		root->size = blockSize - sizeof(struct memEntry);
		root->free = 1;
		initialized = 1;	
	}

	ptr = root;
	while(ptr != 0)
	{
		if(ptr->size < size)/*check if block fits data*/
		{
			ptr = ptr->next; 
		}
		else if(ptr->free == -1)/*block is being used*/
		{
			ptr = ptr->next;
		}
		else if(ptr->size < (size + sizeof(struct memEntry))) 
		{
			ptr->free = -1;
			result = (char*) ptr + sizeof(struct memEntry);
			return result;
		}else
		{
			succ = (struct memEntry*) ((char*)ptr + sizeof(struct memEntry) + size);
			succ->prev = ptr;
			succ->next = ptr->next;
			if(ptr->next != 0)
			{
				ptr->next->prev = succ; 
			}
			ptr->next = succ;
			succ->size = ptr->size + sizeof(struct memEntry)-size;
			succ->free = 1;
			ptr->size = size;
			ptr->free = -1;
			result = (char*)ptr + sizeof(struct memEntry);
			return result;
		}
	}/*end of while*/ 
	printf("cannot allocate because there is no memory left\n");
	return 0;
}/*end of function*/

void my_free(void* ptr, char* file, int line)
{
	int loc  = (int)((char*)ptr - (char*)heap);
	if(loc < 0 || loc > blockSize){
		printf("Trying to free a pointer that was never allocated\n");
		return;
	}
	
	struct memEntry *curr, *previous, *nxt;

	curr = (struct memEntry*)((char*)ptr - sizeof(struct memEntry));

	/*printf("curr->free is %d\n", curr->free);*/
	
	if(curr->free != -1 && curr->free != 1)
	{
		printf("Trying to free a pointer to dynamic memory that was not returned from my_malloc\n");
		return;
	}

	if(curr->free == 1){
		printf("Trying to free a pointer that has already been freed\n");
		return;
	}

	previous = curr->prev;

	if(previous == NULL || previous->free == -1)
	{
		curr->free = 1;
		previous = curr;
	} else {
		previous->size += (sizeof(struct memEntry) + curr->size);
		previous->next = curr->next;
	 	if(curr->next != 0)
		{
			curr->next->prev = previous;
		}
	}

	nxt = curr->next;
	if(nxt != 0 && nxt->free == 1)
	{
		previous->size += (sizeof(struct memEntry) + nxt->size);
		previous->next = nxt->next;
		if (nxt->next != NULL)
		{
			nxt->next->prev = previous;
		}
	}
	printf("successfully freed\n");
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

