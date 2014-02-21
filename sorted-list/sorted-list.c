#include <stdio.h>
#include <stdlib.h>
#include "sorted-list.h"


SortedListPtr SLCreate(CompareFuncT cf, DestructFuncT df)
{

	printf("in create\n");
	SortedListPtr list; 
	
	/*Malloc space for a SortedListPtr, initialize it, and return it as long as cf and df are non-NULL*/
	if(cf != NULL && df != NULL)
	{
		list = (SortedListPtr)malloc(sizeof(struct SortedList));
		list->head = NULL;
		list->cf = cf;
		list->df = df;
		return list;
	}
	return NULL; /*return NULL otherwise*/
}

void SLDestroy(SortedListPtr list)
{
	/*Simply return if the list is a NULL pointer*/
	if (list == NULL){
		return;
	}
	
	nodePtr curr = list->head; /*node pointer for node getting freed*/
        nodePtr temp; /*temp node pointer for next node so the previous node can be freed*/
	
	/*If the list is empty, free the list and return*/
	if (curr == NULL){
		free(list)
		return;
	}

	temp = curr->next;

	/*Free all nodes until there is only one non-NULL node left pointed to by curr*/
        while (temp != NULL){
                free(curr);
                curr = temp;
                temp = curr->next;
        }   
        
        free(curr); /*Free last node*/
        free(list); /*Free the sorted list object created by SLCreate*/
}

int SLInsert(SortedListPtr list, void* newObj)
{	
	printf("in sorted list and new object is %d\n", *(int*) newObj);
        
        /*return 0 if a NULL object is being inserted*/
        if (newObj == NULL){
        	return 0;
        }

        nodePtr newNode = (nodePtr)malloc(sizeof(node));
        newNode->data = newObj;
        nodePtr ptr = list->head;
        int compareValue;

	/*Add the new object and return 1 automatically if the sorted list is empty*/ 
	if (ptr == NULL){
                list->head = newNode;
                newNode->numPtrs++; 
                newNode->isInList = 1; 
                return 1;
        }   

        compareValue = (*list->cf)(newObj, ptr->data); /*compare the new object with the data in head*/

	/*If the new data is the same as the data in list->head, free the newly malloced object
	and return 1 since this is a duplicate*/
        if (compareValue == 0){ 
                free(newNode);
                return 0;
        }    

	/*IF the new data is greater than the data in list->head, set the new node as the new head
	of the list and return 1. No further comparisons are required*/
        if (compareValue > 0){ 
                newNode->next = ptr;
                list->head = newNode;
                newNode->numPtrs++;
                newNode->isInList = 1;
                return 1;
        }    

	/*If there is only one object in the list and it is greater than the new data, add the new
	node to the end of the list and return 1*/
        if (ptr->next == NULL){
                ptr->next = newNode;
                newNode->numPtrs++;
                newNode->isInList = 1;
                return 1;
        }   

        compareValue = list->cf(newObj, ptr->next->data); /*now compare the data with the second node*/

	/*Move down the list until the new data is greater than the data in ptr->next node. Then
	add the new node to the list between ptr and ptr->next*/
        while (compareValue <= 0){ 

                ptr = ptr->next;
                
                /*Return 0 and free the newNode if a duplicate is found*/
                if (compareValue == 0){ 
                        free(newNode);
                        return 0;
                }   

		/*If the end of the list is reached, break the loop and and the newNode
		to the end automatically to avoid a segmentation fault*/
                if (ptr->next == NULL){
                        break;
                }   

                compareValue = list->cf(newObj, ptr->next->data);
        }   

        newNode->next = ptr->next;
        ptr->next = newNode;
        newNode->numPtrs++;
        newNode->isInList = 1;
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

void SLDestroyIterator(SortedListIteratorPtr iter)
{
	/*Simply return if iter is a NULL pointer*/
	if (iter == NULL){
		return;
	}
	
	nodePtr temp1 = iter->curr; /*node pointer to node being freed*/
        nodePtr temp2; /*temp node pointer for next node so the previous node can be freed*/

	/*If the first node pointed to by iterator is NULL, free the iterator struct and return*/
        if (temp1 == NULL){
                free(iter);
                return;
        }   

        temp2 = temp1->next;

	/*If the iterator points to a single node list but the node has other pointers pointing to it,
	simply free the iterator and return*/
        if (temp2 == NULL && temp1->numPtrs > 1){ 
                free(iter);
                temp1->numPtrs--;
                return;
        }    

 	/*While iter->curr points to a non-NULL node, move the iterator down the list, freeing each node
 	along the way, until a node is found to be part of another iterator or the sorted list*/
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
        
        temp1->numPtrs--;    
        
        /*free last node only if nothing else points to it*/
	if (temp1->numPtrs == 0){
        	free(temp1);
	}
        free(iter); /*free iterator*/
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
