#include <stdio.h>
#include <stdlib.h>
#include "sorted-list.h"


SortedListPtr SLCreate(CompareFuncT cf)
{

	SortedListPtr list; 
	if(cf != NULL)
	{
		list = (SortedListPtr)malloc(sizeof(struct SortedList));
		list->head = NULL;
		list->cf = cf;
	}
	return NULL;
}

void SLDestroy(SortedListPtr list)
{
	
}

int SLInsert(SortedListPtr list, void* newObj)
{	

	if(list->head == NULL)/*if the Linked List is empty. Creates a node and sets it as the head*/
	{
		nodePtr temp = malloc(sizeof(node));
		temp->data = newObj;
		temp->counter++;
		temp->next = NULL;
		list->head = temp;
		return 1;
	}
	return 0;
}

int SLRemove(SortedListPtr list, void* newObj)
{
	return 0;
}

SortedListIteratorPtr SLCreateIterator(SortedListPtr list)
{
	SortedListIteratorPtr itPtr = (SortedListIteratorPtr)malloc(sizeof(struct SortedListIterator));

	if(list->head != NULL)
	{
		itPtr->curr = list->head;
		list->head->counter++;
		return itPtr; 
			
	}		
	return NULL;
}

void SLDestoryIterator(SortedListIteratorPtr iter)
{

}

void* SLNextItem(SortedListIteratorPtr iter)
{
	return NULL;
}
