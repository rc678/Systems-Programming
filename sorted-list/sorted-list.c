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
                printf("%d\n", newNode->numPtrs);
                newNode->numPtrs++;
                return 1;
        }   

        compareValue = (*list->cf)(newObj, ptr->data);

        if (compareValue == 0){ 
                free(newNode);
                return 0;
        }    

        if (compareValue > 0){ 
                newNode->next = ptr;
                list->head = newNode;
                newNode->numPtrs++;
                return 1;
        }    

        if (ptr->next == NULL){
                ptr->next = newNode;
                newNode->numPtrs++;
                return 1;
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
	nodePtr temp;
	SortedListIteratorPtr p = SLCreateIterator(list);
	nodePtr prev = p->curr;
	void* compareObj = SLNextItem(p); 
	int compare = (*list->cf)(newObj,compareObj);

	printf("iterator points to  %d\n", *(int*)p->curr->data);
	if(p == NULL)/*list is empty*/	
	{
		printf("list is empty\n");
		return 0;
	}

	if(compare == 0)/*newObj is equal to head, so removes the head of the list. Remember to check if the list has only one item*/
	{
		if(list->head->next == NULL)/*if there is only one node in the list*/
		{
			printf("deleting the head if it is the only item in the list\n");
			list->head->numPtrs--;
			if(list->head->numPtrs == 0){
				free(p->curr);
				list->head = NULL;
				/*destroy iterator?*/
			}
			return 1;
		}
		printf("deleting the head is the list has more than one element\n");
		list->head->numPtrs--;
		list->head = list->head->next;
		if(list->head->numPtrs == 0)
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

	while(p->curr != NULL)/*traverses Linked List to find object to delete*/
	{
		compare = (*list->cf)(newObj, compareObj);
		if(compare == 0){
			printf("deleting node if it is not the head\n");
			if(p->curr->next == NULL)
			{
				p->curr->numPtrs--;;
				temp->next = NULL;
				printf("prev is %d\n", *(int*)prev->data);
				if(p->curr->numPtrs == 0)
				{
					free(prev);
					return 1;
				} 
			}
			prev->numPtrs--;
			temp->next = prev->next;
			printf("prev data is %d and numPtrs is %d\n", *(int*)prev->data, p->curr->numPtrs);
			if(prev->numPtrs == 1)/*if there are no pointers pointing to the node*/
			{
				printf("LALAALA\n");
				free(prev);
				return 1;
			}
			return 1;
			/*destory iterator?*/	
		}
		if(prev == NULL && compare != 0)
		{
			printf("not found in the list\n");
			return 0;
		}
		compareObj = SLNextItem(p);
		printf("compareObj is %d prev is %d and numPtrs on iter is %d\n", *(int*)compareObj, *(int*)prev->data, p->curr->numPtrs);
		temp = prev;
		prev = prev->next;
		if(compareObj == NULL)/*if not found in the list*/
		{
			printf("list is empty\n");
			return 0;
		}
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
	nodePtr temp1 = iter->curr;
        nodePtr temp2;

        if (temp1 == NULL){
                free(iter);
                return;
        }   

        temp2 = temp1->next;

        if (temp2 == NULL && temp1,->numPtrs > 1){ 
                free(iter);
                return;
        }    

        while (temp2 != NULL)
        {   
                temp1->numPtrs--;
                if (temp1->numPtrs > 0){ 
                        free(iter);
                        return;
                }   
                    
                free(temp1);
                temp1 = temp2;
                temp2 = temp1->next;
        }   
            
        free(temp1);
        free(iter);
}

void* SLNextItem(SortedListIteratorPtr iter)
{
	void* currData; 
	nodePtr temp;

	if(iter == NULL)
	{
		printf("list is empty\n");
		return NULL;
	}
	iter->curr->numPtrs--;
	if(iter->curr->numPtrs == 0)
	{
		free(iter->curr);
	}
	
	if(iter->curr->next == NULL)/*if you are at the end of the list*/
	{
		return iter->curr->data;	
	}else{/*if there is more than one item in the list*/
		if(iter->curr->numPtrs != 0){/*if the current node is not the one that needs to be removed*/
			currData = iter->curr->data;
			iter->curr = iter->curr->next;
			iter->curr->numPtrs++;
			return currData;	
		}
	}


	return NULL;
}
