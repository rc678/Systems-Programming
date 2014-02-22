/*
 * Sorted-list.c
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "sorted-list.h"

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

/*Destructor functions*/
void destroyBasicTypeAlloc(void *p){
	/*For pointers to basic data types (int*,char*,double*,...)
	 *         Use for allocated memory (malloc,calloc,etc.)*/
	free(p);
}

void destroyBasicTypeNoAlloc(void *p) {
	/*For pointers to basic data types (int*,char*,double*,...)
	 *         Use for memory that has not been allocated (e.g., "int x = 5;SLInsert(mylist,&x);SLRemove(mylist,&x);")*/
	return;
}

int main(int argc, char*argv[])
{
	int choice1 = atoi(argv[1]);
	
	SortedListPtr list;	

	if (choice1 == 1){
		/*Normal SLInsert for the beginning, middle, or end of list. Only test for dynamically allocated data*/
		list = SLCreate(compareInts, destroyBasicTypeAlloc);
		int *a = (int*)malloc(sizeof(int));
		*a = 9;
		if (SLInsert(list, (void *)a) == 0){
			printf("failure\n");
			return 1;
		}
		int *b = (int*)malloc(sizeof(int));
		*b = 12;
		if (SLInsert(list, (void *)b) == 0){ 
			printf("failure\n");
			return 1;
		} 
		int *c = (int*)malloc(sizeof(int));
		*c = -4;
		if (SLInsert(list, (void *)c) == 0){ 
			printf("failure\n");
			return 1;
		} 
		int *d = (int*)malloc(sizeof(int));
		*d = 6;
		if (SLInsert(list, (void *)d) == 0){ 
			printf("failure\n");
			return 1;
		} 
		int *e = (int*)malloc(sizeof(int)); 
		*e = 0;
		if (SLInsert(list, (void *)e) == 0){ 
			printf("failure\n");
			return 1;
		} 

		nodePtr ptr = list->head;

		/*Should print 12 9 6 0 -4*/
		while (ptr != NULL){
			printf("%d ", *(int*) ptr->data);
			ptr = ptr->next;
		}
		printf("\n");

	} else if (choice1 == 2){

		/*same test for SLInsert with doubles. Also testing for duplicates. Test SLDestroy*/
		SortedListPtr l = SLCreate(compareDoubles, destroyBasicTypeNoAlloc);

		double arr[6] = {2.33,-1.11,21.884,5,0, -1.11};
		int i;
		for (i = 0; i < 5; i++){
			if(SLInsert(l, &arr[i]) == 0){
				printf("failure\n");
				return 1;
			}
		}
		if(SLInsert(l, &arr[5]) == 1){
			printf("failure\n");
			return 1;
		}

		nodePtr ptr = l->head;

		/*Should print 21.884 5 2.33 0 -1.11*/
		while (ptr != NULL){
			printf("%g ", *(double*) ptr->data);
			ptr = ptr->next;
		}   
		printf("\n");

		SLDestroy(l);        
		
		return 1;

	} else if (choice1 == 3){
		/*Same test for SLInsert with strings*/
		SortedListPtr l = SLCreate(compareStrings, destroyBasicTypeNoAlloc);

		char *arr[5] = {"woof","ribbit","meow","roar","hiss"};
		int i;
		for (i = 0; i < 5; i++){
			if(SLInsert(l, arr[i]) == 0){ 
				printf("failure\n");
				return 1;
			}   
		}

		nodePtr ptr = l->head;

		/*Should print woof roar ribbit meow hiss*/
		while (ptr != NULL){
			printf("%s ", *(char*) ptr->data);
			ptr = ptr->next;
		}
		printf("\n");        

		SLDestroy(l);
		
		return 1;
	} else {
		return 1;
	}

	int choice2 = atoi(argv[2]);

	switch (choice2){
		int* b;
		int c;		
		int* d;
		nodePtr p;
		SortedListIteratorPtr iter;
		SortedListIteratorPtr iter2;		

		case 1: /*SLInsert on NULL data*/
			printf("%d\n", SLInsert(list, NULL));
			break;

		case 2: /*SLInsert on NULL list*/
			d = (int*) malloc(sizeof(int));
			*d = 12;
			printf("%d\n", SLInsert(NULL, (void *)d));	
			break;

		case 3: /*Removing an existing element from beginning, middle, and end of list*/
			
			d = (int*) malloc(sizeof(int));
			*d = 12;
			SLRemove(list, (void*)d);
			*d = 6;
			SLRemove(list, (void*)d);
			*d = -4;
			SLRemove(list, (void*)d);

			p = list->head;
			while (p != NULL){
				printf("%d ", *(int *)p->data);
				p = p->next;
			}
			printf("\n"); 
			break;
		
		case 4: /*Removing a non-existing element*/

			d = (int*) malloc(sizeof(int));
                        *d = 11; 
                        SLRemove(list, (void*)d);

			p = list->head;
                        while (p != NULL){
                                printf("%d ", *(int *)p->data);
                                p = p->next;
                        }   
                        printf("\n"); 
			break;

		case 5: /*SLNextItem on a NULL iterator*/

			printf("%c\n", (int *)SLNextItem(NULL));
			break;

		case 6: /*Iterate on empty list*/
	
			iter = SLCreateIterator(list);
			iter->curr = NULL;
			printf("%c\n", (int *)SLNextItem(iter));		
			break;

		case 7: /*Call SLNextItem when iterator points to item in list*/

			iter = SLCreateIterator(list);
			printf("%d\n", *(int *)SLNextItem(iter));
			break;

		case 8: /*Create multiple iterators and call SLNextItem on both*/

			iter = SLCreateIterator(list);
			iter2 = SLCreateIterator(list);

			printf("%d ", *(int *)SLNextItem(iter));
			printf("%d\n", *(int *)SLNextItem(iter2));
			break;

		case 9: /*Create iterator, remove head of list, create new iterator, call SLNextItem on both*/

			iter = SLCreateIterator(list);
			SLRemove(list, list->head->data);
			
			iter2 = SLCreateIterator(list);

			printf("%d ", *(int *)SLNextItem(iter));
                        printf("%d\n", *(int *)SLNextItem(iter2));
                        break;


		case 10: /*Call SLNextItem on a destroyed iterator*/

			iter = SLCreateIterator(list);
			SLDestroyIterator(iter);

			printf("%c\n", (int *)SLNextItem(iter));
			break;

		case 11: /*Remove head item with iterator pointing to it*/

			iter = SLCreateIterator(list);
			SLRemove(list, list->head->data);

			printf("%d\n", *(int *)SLNextItem(iter));
			break;

		case 12: /*Remove end item with iterator pointing to it*/

			iter = SLCreateIterator(list);
			SLNextItem(iter);
			SLNextItem(iter);
			SLNextItem(iter);
			SLNextItem(iter);
			d = (int*) malloc(sizeof(int));
                        *d = -4; 
                        SLRemove(list, (void*)d);
			
			printf("%d\n", *(int *)SLNextItem(iter));
                        break;

		case 13: /*Remove element with iterator pointing to element before it*/

			iter = SLCreateIterator(list);
			SLNextItem(iter);
			SLNextItem(iter);

			d = (int*) malloc(sizeof(int));
                        *d = 0; 
                        SLRemove(list, (void*)d);

			printf("%d ", *(int *)SLNextItem(iter));
                        printf("%d\n", *(int *)SLNextItem(iter));
                        break;

		case 14: /*Add element with iterator pointing to element before it*/

			iter = SLCreateIterator(list);
                        SLNextItem(iter);
                        SLNextItem(iter);

                        d = (int*) malloc(sizeof(int));
                        *d = 2; 
                        SLInsert(list, (void*)d);

                        printf("%d ", *(int *)SLNextItem(iter));
                        printf("%d ", *(int *)SLNextItem(iter));
                        printf("%d\n", *(int *)SLNextItem(iter));
			break;

		default: return 1;
	}
	
	return 0;
}
