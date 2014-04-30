#include <stdio.h>
#include <stdlib.h>
#include "memory.h"

#define block 5000
static char heap[block];

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
		break;
	} 

	if(initialized == 0)/*root is not initialized*/
	{
		root = (struct memEntry*) heap;
		root->prev = 0;
		root->next = 0;
		root->prev = root->next;
		root->size = block - sizeof(struct memEntry);
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
		else if(ptr->free == 0)/*block is being used*/
		{
			ptr = ptr->next;
		}
		else if(ptr->size < (size + sizeof(struct memEntry))) 
		{
			ptr->free = 0;
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
			ptr->free = 0;
			result = (char*)ptr + sizeof(struct memEntry);
			return result; 
		}
	}/*end of while*/ 
	return 0;
}/*end of function*/

void my_free(void* ptr, char* file, int line)
{
	struct memEntry* ptr;
	struct memEntry* prev;
	struct memEntry* succ;

	
}

/*include for testing purposes?*/
int main(int argc, char** argv)
{

}

