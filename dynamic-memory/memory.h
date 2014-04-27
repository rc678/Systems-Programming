struct memEntry{
	struct memEntry* prev;
	struct memEntry* succ;
	int free;
	int size;
};

void* my_malloc(unsigned int size, char* file, int line);
void my_free(void* ptr, char* file, int line);
