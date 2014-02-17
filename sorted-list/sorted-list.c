#include <stdio.h>
#include <stdlib.h>
#include "sorted-list.h"


SortedListPtr SLCreate(CompareFuncT cf)
{

	printf("in create\n");
	SortedListPtr list; 
	if(cf != NULL)
	{
		list = (SortedListPtr)malloc(sizeof(struct SortedList));
		list->head = NULL;
		list->cf = cf;
		return list;
	}
	return NULL;
}

void SLDestroy(SortedListPtr list)
{
	nodePtr curr = list->head;
        nodePtr temp = curr->next;
        while (temp != NULL){
                free(curr);
                curr = temp;
                temp = curr->next;
        }   

        free(curr);
        free(list);	
}

int SLInsert(SortedListPtr list, void* newObj)
{	
	printf("in sorted list and new object is %d\n", *(int*) newObj);
        nodePtr newNode = (nodePtr)malloc(sizeof(node));
        newNode->data = newObj;
        nodePtr ptr = list->head;
        int compareValue;

        if (ptr == NULL){
                list->head = newNode;
                newNode->numPtrs++;
                return 1;
        }   

        compareValue = (*list->cf)(newObj, ptr->data);

        if (compareValue > 0){ 
                newNode->next = ptr;
                list->head = newNode;
                newNode->numPtrs++;
                return 1;
        }    

        if (ptr->next == NULL){
                if (compareValue == 0){ 
                	free(newNode);
                        return 0;
                } else {
                        ptr->next = newNode;
                        newNode->numPtrs++;
                        return 1;
                }   
        }   

        compareValue = list->cf(newObj, ptr->next->data);

        while (compareValue <= 0){ 

                ptr = ptr->next;
                if (compareValue == 0){
                	free(newNode);
                        printf("duplicate value\n");
                        return 0;
                }   

                if (ptr->next == NULL){
                        break;
                }   

                compareValue = list->cf(newObj, ptr->next->data);
        }   

        newNode->next = ptr->next;
        ptr->next = newNode;
        newNode->numPtrs++;
        return 1;
}

int SLRemove(SortedListPtr list, void* newObj)
{
	printf("in remove and newObj is %d and the head is %d\n", *(int*)newObj, *(int*)list->head->data);
	if(list == NULL || newObj == NULL)
	{
		return 0;
	}
	nodePtr prev = NULL;
	int compare = (*list->cf)(newObj,list->head->data);
	SortedListIteratorPtr p = SLCreateIterator(list);/*p points to head of list*/
	void* compareObj;

	printf("list head numPTrs is %d\n", list->head->numPtrs);
	if(p == NULL)/*list is empty*/	
	{
		printf("list is empty\n");
		return 0;
	}

	printf("here2\n");
	if(compare == 0)/*newObj is equal to head, so removes the head of the list. Remember to check if the list has only one item*/
	{
		if(list->head->next == NULL)/*if there is only one node in the list*/
		{
			printf("deleting the head if it is the only item in the list\n");
			list->head->numPtrs--;
			if(list->head->numPtrs == 1){
				free(p->curr);
				/*destroy iterator?*/
			}
			return 1;
		}
		printf("deleting the head is the list has more than one element\n");
		list->head->numPtrs--;
		list->head = list->head->next;
		if(list->head->numPtrs == 1)
		{
			free(p->curr);
			/*destory iterator?*/
		}
		return 1;
	}

	if(list->head->next == NULL)/*list only has head and newObj is not the head*/
	{
		return 0;
	}
	prev = p->curr;/*prev is at the head*/
	compareObj = p->curr->next->data;
	while(p->curr != NULL)/*traverses Linked List to find object to delete*/
	{
		printf("in loop\n");
		compare = (*list->cf)(newObj, compareObj);/*compares curr to newObject*/
		if(compare == 0){
			printf("deleting node if it is not the head\n");
			prev->next->numPtrs--;
			prev->next = p->curr->next;
			if(p->curr->numPtrs == 0)/*if there are no pointers pointing to the node*/
			{
				free(p->curr);
				return 1;
			}
			return 1;/*still 1 even though i didnt free b/c it is "removed" from the list.*/ 
			/*destory iterator?*/	
		}
		prev = prev->next;
		compareObj = SLNextItem(p);
		if(compareObj == NULL)/*if not found in the list*/
		{
			printf("not in list\n");
			return 0;
		}
		/*do i still have to increment the iterator somehow?*/

	}
	return 0;
}

SortedListIteratorPtr SLCreateIterator(SortedListPtr list)
{
	SortedListIteratorPtr itPtr = (SortedListIteratorPtr)malloc(sizeof(struct SortedListIterator));
	if(list->head == NULL)
	{
		printf("list is empty. Cannot make iterator\n");
		return NULL;
	}

	if(list->head != NULL)
	{
		itPtr->curr = list->head;
		list->head->numPtrs++;
		return itPtr; 

	}		
	return NULL;
}

void SLDestoryIterator(SortedListIteratorPtr iter)
{

}

void* SLNextItem(SortedListIteratorPtr iter)
{
		void* nextItem;
	nodePtr temp;

	if(iter == NULL)
	{
		printf("list is empty\n");
		return NULL;
	}


	if(iter->curr->next == NULL)/*if you are at the end of the list*/
	{
		return NULL;	
	}else{/*if there is more than one item in the list*/
		iter->curr->numPtrs--;
		temp = iter->curr->next;
		iter->curr = temp;
		nextItem = iter->curr->data;
		iter->curr->numPtrs++;
		return nextItem;
	}


	return nextItem;
}
