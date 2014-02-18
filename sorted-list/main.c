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
        int z, i;
        int* b;

        if (argc == 1){
                return 1;
        }

        int* a = (int*) malloc(sizeof(int));
        *a = atoi(argv[1]);
        z = SLInsert(list, (void*)a);
        printf("%d\n", z);

        for (i = 2; i < argc; i++){
                b = (int*)malloc(sizeof(int));  
                *b = atoi(argv[i]);
                z = SLInsert(list, (void*)b);
                printf("%d\n", z);
        }
        
        printf("after insert\n");

        nodePtr ptr = list->head;

        while (ptr != NULL){
                printf("%d\n", *(int*) ptr->data);
                ptr = ptr->next;
        }

        SLDestroy(list);

        free(a);

        if (argc > 2){
                free(b);
        }

        return 1;
}
