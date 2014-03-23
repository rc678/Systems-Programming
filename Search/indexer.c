#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "uthash.h"/*Copyright (c) 2005-2014, Troy D. Hanson  http://troydhanson.github.com/uthash/ All rights reserved.*/
#include "tokenizer.c"

/*Node type struct used to store the records in a sorted linked list at each index of the hashtable*/
typedef struct record{
	char* fileName; /*stores the relative name of the file*/
	int frequency; /*stores the frequency of a distinct word in the file*/
	struct record *next;
	struct record *prev;

}record, *recordPtr;

/*Hashtable struct to store distinct words and the records associated with them*/
struct my_struct{
	const char* word; /*the distinct word that is used as the key*/
	recordPtr list; /*value is a recordptr to a sorted list of records*/
	UT_hash_handle hh; /*external macros*/
};

/*function prototypes*/
void split(FILE*, char*);
recordPtr updateRecordList(recordPtr, char*);
recordPtr resortRecordList(recordPtr, recordPtr);

/*global hashtable struct*/
struct my_struct* words; 

/*sorts all the keys in the hashtable*/
int name_sort(struct my_struct* a, struct my_struct* b)
{
	return strcmp(a->word, b->word);
}

/*calls a function that compares the keys in a hashtable and sorts them*/
void sort_by_name()
{
	HASH_SORT(words, name_sort);
}

/*Recursively indexes all the files in a directory. Returns 1 if successful in indexing and 0 if not.*/
int readFiles(char* dir)
{
	recordPtr temp;
	DIR* directory;
	struct dirent* file; 
	struct stat statbuf;
	char* name; 
	stat(dir, &statbuf);
	FILE* currFile;
	struct my_struct* s = NULL; 

	/*if the user enters the name of a directory or file that does not exist*/
	if(stat(dir, &statbuf) == -1)
	{
		return 0;
	}

	/*checks if dir is currently on a file. If it is a file, then open the file and call split.*/
	if(S_ISREG(statbuf.st_mode))
	{
		currFile = fopen(dir, "r");
		if(currFile != NULL)
		{
			split(currFile, dir);
		}
		return 0;
	}

	/*checks if dir is currently a directory*/
	if(S_ISDIR(statbuf.st_mode)){
		directory = opendir(dir);

		while((file = readdir(directory)) != NULL)
		{
			name = file->d_name;
			if(name[0] == '.' || name[strlen(name)-1] == '~')
			{
				continue; 
			}
			char* nextFile = malloc(strlen(dir) + strlen(name) + 1);
			sprintf(nextFile, "%s/%s", dir, name);
			readFiles(nextFile);
			free(nextFile);				

		}
	}

	return 1;


}

/*This function will tokenize the file, add all new distinct words to the hash table, and call functions to update
 *the lists of records when a tokenized word is already in the hashtable.
 *The arguments are a file pointer and relative path name of the file.
 */ 
void split(FILE* file, char* fileName) {

	if(file == NULL)
	{
		printf("File is NULL\n");
	}

	char* token;
	recordPtr temp; 
	recordPtr a;
	recordPtr z;  
	struct my_struct *s =  NULL;
	struct my_struct *r = NULL;

	char str[1000000];
	fgets(str, 10000, file);
	char* st = str;

	TokenizerT* t = TKCreate(st);
	token = TKGetNextToken(t);

	int len = strlen(fileName);
	char* fptr = (char*)malloc(len);
	strcpy(fptr, fileName);

	/*tokenizes the string and adds them to the hashtable called words*/
	while(token != NULL)
	{
		HASH_FIND(hh, words, token, strlen(token), s);
		if(s)/*if the token is in the hashtable*/
		{
			z = s->list; 
			s->list = updateRecordList(z, fptr);
		}else{/*if the token is not in the hashtable*/
			s = (struct my_struct*)malloc(sizeof(struct my_struct));
			s->word = token;
			recordPtr add = (recordPtr)malloc(sizeof(struct record));
			add->fileName = fptr;
			add->frequency = 1;
			add->next = NULL;
			add->prev = NULL;
			s->list = add;
			HASH_ADD_KEYPTR(hh, words, token, strlen(token), s);
		}

		token = TKGetNextToken(t);
	}
}

/*This function updates the sorted list of records for a word as necessary.
 *Arguments: pointer to the head of the list and the relative name of the file
 *Returns a record pointer to the head of the newly updated list
 */
recordPtr updateRecordList(recordPtr head, char* fname)
{
	recordPtr temp = head; /*pointer to the current head of the list*/
	recordPtr curr = head; /*pointer to traverse the list for the proper record*/

	/*Search the list for a record with a matching file name. If it is found, increment the frequency
	 * 	and call resortRecordList*/
	while (curr){
		if (strcmp(fname, curr->fileName) == 0){
			curr->frequency++;
			head = resortRecordList(head, curr);
			return head;
		}

		temp = curr;
		curr = curr->next;
	}

	/*If no matching file name is found, malloc and initialize a new record with a frequency of 1 for 
	 * 	this file and attach it to the end of the list*/
	recordPtr newRecord = (recordPtr)malloc(sizeof(struct record));
	newRecord->frequency = 1;
	newRecord->fileName = fname;
	newRecord->prev = temp;
	newRecord->next = NULL;
	temp->next = newRecord;
	return head;
}

/*This function re-sorts a list of records when one record's frequency has been incremented.
 *Arguments: pointer to the current head of the list and pointer to the incremented record
 *Returns pointer to the head of the updated list
 */
recordPtr resortRecordList(recordPtr head, recordPtr curr)
{
	/*If the incremented record is already the head, don't change anything*/
	if (curr == head){
		return head;
	}

	recordPtr temp = curr->prev; /*Pointer to traverse backwards through the list*/

	/*Move the incremented record up the list until the previous node has the same or higher frequency*/
	while (temp->prev){
		if (curr->frequency > temp->frequency){
			temp->next = curr->next;
			if(curr->next){
				curr->next->prev = temp;
			}
			curr->next = temp;
			curr->prev = temp->prev;
			temp->prev->next = curr;
			temp->prev = curr;
			temp = curr->prev;
		} else {
			break;
		}
	}

	/*If the incremented record now has the highest frequency in the list, make it the head and return it*/
	if (temp == head && curr->frequency > temp->frequency){
		temp->next = curr->next;
		if (curr->next){
			curr->next->prev = temp;
		}
		curr->next = temp;
		temp->prev = curr;
		curr->prev = NULL;
		return curr;
	}

	return head;	
}


int indexFiles(char* dir)
{
	/*argv[1] is the inverted-index file name. It gives the name of the file you should create to hold your inverted index*/
	/*argv[2] gives the name or directory that your program should index*/

	readFiles(dir);
	sort_by_name();


/*	HASH_ITER(hh, words, s, tmp) {
		HASH_DEL(words, s);
		free(s);
	}*/
	return 1;	
}/*end of main*/


