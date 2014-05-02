#include <stdio.h>
#include <stdlib.h>
#include "memory.h"

#define blockSize 5000
#define malloc( x ) my_malloc( x, __FILE__, __LINE__ )
#define calloc( n, x ) my_calloc( n, x, __FILE__, __LINE__ )
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
}/*end of my_malloc*/

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
} /*end of my_free*/

/*Allocates space for array of new data in our dynamic memory block if there is space available. Arguments include number
of elements in array and size of each item in array*/
void* my_calloc(int numMembers, unsigned int size, char* file, int line)
{
	static struct memEntry* root = 0; /*pointer to first memEntry in heap*/
	static int initialized = 0; /*becomes 1 for remainder of process after first successful malloc*/
	struct memEntry* ptr; /*pointer to iterate through memEntries*/
	struct memEntry* succ; /*pointer to next memEntry*/
	void* result = NULL; /*return pointer*/
	
	/*Cannot allocate data with no size*/
	if(size == 0 || numMembers == 0)
	{
		printf("Error in file %s on line %d: nothing to allocate\n", file, line);
		return 0;
	} 

	/*Data of a negative size is an error*/
	if(size < 0 || numMembers < 0)
	{
		printf("Error in file %s on line %d: not a valid memory size\n", file, line);
		return 0;
	}

	if(size * numMembers > blockSize)
	{
		printf ("Error in file %s on line %d: heap is too small to allocate all data\n", file, line);
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
	size = size*numMembers; /*The size of all the data to be allocated contiguously*/

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
			int i;
			for(i = 0; i < ptr->size; i++)
			{
				char *tmp = (char*)ptr + sizeof(struct memEntry) + i;
				tmp = 0;
			}
			ptr->free = -1;
			result = (char*) ptr + sizeof(struct memEntry);
			return result;
		}
		else /*Otherwise make a new memEntry and add it after the current one in the linked list of memEntries. Then allocate the data here*/
		{
			int i;
			for(i = 0; i < ptr->size; i++)
                        {
                                char *tmp = (char*)ptr + sizeof(struct memEntry) + i;
                                tmp = 0;
                        }
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
}/*end of my_calloc*/
