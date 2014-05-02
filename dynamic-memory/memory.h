struct memEntry{
        struct memEntry* prev;/*pointer to previous memEntry in block*/
        struct memEntry* next;/*pointer to next memEntry in block*/
        int free;/*is block free or not--> 1 if yes and -1 if no*/
        int size;/*size of the memory block*/
};

void* my_malloc(unsigned int size, char* file, int line);
void my_free(void* ptr, char* file, int line);
