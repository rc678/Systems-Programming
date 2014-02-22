#include <stdio.h>
#include <stdlib.h>
#include "sorted-list.h"


SortedListPtr SLCreate(CompareFuncT cf, DestructFuncT df)
{

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
		free(list);
		return;
	}

	temp = curr->next;

	/*Free all nodes until there is only one non-NULL node left pointed to by curr*/
	while (temp != NULL){
		(*list->df)(curr->data);
		free(curr);
		curr = temp;
		temp = curr->next;
	}   

	(*list->df)(curr->data);
	free(curr); /*Free last node*/
	free(list); /*Free the sorted list object created by SLCreate*/
}

int SLInsert(SortedListPtr list, void* newObj)
{	

	/*return 0 if a NULL object is being inserted*/
	if (newObj == NULL){
		return 0;
	}

	/*return 0 if list is Null pointer*/
	if (list == NULL){
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
	 * 	and return 1 since this is a duplicate*/
	if (compareValue == 0){ 
		(*list->df)(newNode->data);
		free(newNode);
		return 0;
	}    

	/*If the new data is greater than the data in list->head, set the new node as the new head
	 * 	of the list and return 1. No further comparisons are required*/
	if (compareValue > 0){ 
		newNode->next = ptr;
		list->head = newNode;
		newNode->numPtrs++;
		newNode->isInList = 1;
		return 1;
	}    

	/*If there is only one object in the list and it is greater than the new data, add the new
	 * 	node to the end of the list and return 1*/
	if (ptr->next == NULL){
		ptr->next = newNode;
		newNode->numPtrs++;
		newNode->isInList = 1;
		return 1;
	}   

	compareValue = (*list->cf)(newObj, ptr->next->data); /*now compare the data with the second node*/

	/*Move down the list until the new data is greater than the data in ptr->next node. Then
	 * 	add the new node to the list between ptr and ptr->next*/
	while (compareValue <= 0){ 

		ptr = ptr->next;

		/*Return 0 and free the newNode if a duplicate is found*/
		if (compareValue == 0){ 
			(*list->df)(newNode->data);
			free(newNode);
			return 0;
		}   

		/*If the end of the list is reached, break the loop and and the newNode
		 * 		to the end automatically to avoid a segmentation fault*/
		if (ptr->next == NULL){
			break;
		}   

		compareValue = (*list->cf)(newObj, ptr->next->data);
	}   

	newNode->next = ptr->next;
	ptr->next = newNode;
	newNode->numPtrs++;
	newNode->isInList = 1;
	return 1;
}

int SLRemove(SortedListPtr list, void* newObj)
{
	/*return 0 as failure for null list*/
	if (list == NULL){
		return 0;
	}

	nodePtr prev = list->head; /*temp pointer to node when removing the FOLLOWING data in the list*/
	
	/*return 0 as failure for empty list*/
	if (prev == NULL){
		return 0;
	}
	
	int compareValue = (*list->cf)(newObj, prev->data); /*compare the new object with the data in head*/

	/*If the data in head matches the data to be removed, remove the head of the list*/
	if(compareValue == 0){
		list->head = prev->next; /*Set the head of the list to the next node, which may be NULL*/
		prev->numPtrs--;
		prev->isInList = 0; /*Since the node is being removed, set this boolean back to 0*/
		
		/*free the node if necessary*/
		if (prev->numPtrs == 0){
			(*list->df)(prev->data);
			free(prev);
		}
		return 1; /*return 1 to indicate succes*/
	}
	
	nodePtr curr = prev->next; /*pointer to node to be removed for any other case*/
	
	/*traverse the sorted list until curr is a NULL pointer. If a match is found, remove curr from
	sorted list and free if necessary. Return 1 upon success. This covers the case for removal of a
	middle node or an end node*/
	while (curr != NULL){
		compareValue = (*list->cf)(newObj, curr->data);
		
		if (compareValue == 0){
			curr->numPtrs--;
			curr->isInList = 0;
			prev->next = curr->next;
			if (curr->numPtrs == 0){
				(*list->df)(curr->data);
				free(curr);
			}
			return 1;
		}

		prev = curr;
		curr = curr->next;
	}

	return 0; /*Return 0 for failure if no match is found in the list*/
}

SortedListIteratorPtr SLCreateIterator(SortedListPtr list)
{
	SortedListIteratorPtr itPtr = (SortedListIteratorPtr)malloc(sizeof(struct SortedListIterator));
	
	/*Return NULL pointer for a null list*/
	if (list == NULL){
		return NULL;
	}	

	/*Return NULL pointer for an empty list*/
	if(list->head == NULL)
	{
		return NULL;
	}

	/*point the iterator to the head of a non-empty list*/
	itPtr->curr = list->head;
	list->head->numPtrs++;
	return itPtr;
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
	 * 	simply free the iterator and return*/
	if (temp2 == NULL && temp1->numPtrs > 1){ 
		free(iter);
		temp1->numPtrs--;
		return;
	}    

	/*While iter->curr points to a non-NULL node, move the iterator down the list, freeing each node
	 *  	along the way, until a node is found to be part of another iterator or the sorted list*/
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
	
	/*Simply return if iter is a NULL pointer*/
        if (iter == NULL){
                return NULL;
        }   

        nodePtr ptr = iter->curr; /*node pointer to current node*/

        /*If the first node pointed to by iterator is NULL, return NULL*/
        if (ptr == NULL){
                return NULL;
        }   

	ptr->numPtrs--; /*The iterator will no longer be pointing to this node*/

	/*Iterate through the list until a node that has not ben removed from the sorted list is found*/
	while(ptr->isInList == 0){

		ptr = ptr->next;

		/*If the end of the list is reach without success, return NULL*/
		if (ptr == NULL){
			return NULL;
		}
	}
	
	currData = ptr->data;
	if (ptr->next != NULL){
		ptr->next->numPtrs++; /*Now the iterator points to the next item*/
	}
	iter->curr = ptr->next;
	return currData;
}
	
