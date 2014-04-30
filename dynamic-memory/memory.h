struct memEntry{
	struct memEntry* prev;/*pointer to previous block*/
	struct memEntry* next;/*pointer to next block*/
	int free;/*is block free or not*/
	int size;/*size of the memory block*/
};

void* my_malloc(unsigned int size, char* file, int line);
void my_free(void* ptr, char* file, int line);
