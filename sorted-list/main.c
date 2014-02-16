/*
 * sorted-list.c
 */

#include	<string.h>
#include	"sorted-list.h"
#include <stdio.h>

int compareInts(void *p1, void *p2)
{
	int i1 = *(int*)p1;
	int i2 = *(int*)p2;

	return i1 - i2;
}

int compareDoubles(void *p1, void *p2)
{
	double d1 = *(double*)p1;
	double d2 = *(double*)p2;

	return (d1 < d2) ? -1 : ((d1 > d2) ? 1 : 0);
}

int compareStrings(void *p1, void *p2)
{
	char *s1 = p1;
	char *s2 = p2;

	return strcmp(s1, s2);
}

int main()
{
	printf("in main\n");
        SortedListPtr list = SLCreate(compareInts);    
        printf("after list\n");
        int z;  

        int* a = (int*) malloc(sizeof(double));
        int* b = (int*) malloc(sizeof(double));
        int* c = (int*) malloc(sizeof(double));
        int* d = (int*) malloc(sizeof(double));
        int* e = (int*) malloc(sizeof(double));
        int* f = (int*) malloc(sizeof(double));
        int* g = (int*) malloc(sizeof(double));

        *a = 11; 
        z = SLInsert(list, (void*)a);
        printf("%d\n", z); 
        *b = 20;
        z = SLInsert(list, (void*)b);
        printf("%d\n", z); 
        *c = 5;
        z = SLInsert(list, (void*)c);
        printf("%d\n", z); 
        *d = -8;
        z = SLInsert(list, (void*)d);
        printf("%d\n", z);
        *e = 9;
        z = SLInsert(list, (void*)e);
        printf("%d\n", z);
        *f = 11;
        z = SLInsert(list, (void*)f);
        printf("%d\n", z);
        *g = 52;
        z = SLInsert(list, (void*)g);
        printf("%d\n", z);


        printf("after insert\n");

        nodePtr ptr = list->head;

        while (ptr != NULL){
                printf("%d\n", *(int*) ptr->data);
                ptr = ptr->next;
        }

	SLDestroy(list);
        free(a);
        free(b);
        free(c);
        free(d);
        free(e);
        free(f);
        free(g);
        
        return 0;
}
