#include "uthash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_LEN 100

typedef struct record{
	char* fileName;
	int frequency;
	struct record* next;
	struct record* prev;
}record, *recordPtr;

typedef struct tempNode{
	char* string;
	int isWord;
	int frequency;
	struct tempNode* next; 
}tempNode, *tempNodePtr;

struct my_struct{
	const char* word;
	recordPtr list;
	UT_hash_handle hh;
};

/*function protypes*/
void freeLists(recordPtr);

struct my_struct* words;

/*Searches all the terms in the query and prints their file names. "Logical and"*/
recordPtr SA(char* word, recordPtr List)
{
	recordPtr head = List;	

	return head;
}

/*Searches all subsets of the term and prints their file names. "Logical or"*/
recordPtr SO(char* word, recordPtr fileList)
{
	recordPtr head = fileList;
	struct my_struct* s;
	struct my_struct* tmp;
	recordPtr ptr = NULL; 
	recordPtr temp;
	recordPtr t;
	printf("word in SO is %s\n", word);

	HASH_FIND(hh, words, word, strlen(word), s);
	if(!s){
		printf("word not in the hashtable\n");
		return head; 
	}

	if(head == NULL)
	{
		printf("HEAD IS NULL\n");
		head = s->list;
		return head; 
	}
	for(temp = s->list; temp != NULL; temp = temp->next)/*checks to see if nodes in hashtable list are in linked list*/
	{
		ptr = head;
		while(ptr != NULL)
		{
			if(strcmp(ptr->fileName, temp->fileName) == 0)
			{
				break;
			}
			ptr = ptr->next;
		}
		if(ptr == NULL)/*inserts node to head of list if not in list*/
		{
			t = (recordPtr)malloc(sizeof(record));
			t->fileName = temp->fileName;
			t->frequency = 1;
			t->prev = NULL;
			t->next = head;
			head->prev = t;
			head = t;
		}
	} 
	return head;
}

/*prints the linked list when SO is entered by the user*/
void printSOList(recordPtr list)
{
	recordPtr ptr = list;
	while(ptr != NULL)
	{
		printf("%s\n", ptr->fileName);
		ptr = ptr->next;
	}
}

/*frees the hashtable*/
void freeHash()
{
	struct my_struct* s;
	struct my_struct* tmp;

	for(s = words; s != NULL; s = s->hh.next)
	{
		freeLists(s->list);
	}
	HASH_ITER(hh,words, s, tmp){
		HASH_DEL(words,s);
		free(s);
	}
}

/*traverses linked lists and frees them*/
void freeLists(recordPtr list)
{
	recordPtr curr;
	for(curr = list; curr != NULL;){
		recordPtr temp = curr;
		curr = curr->next;
		free(temp);
	}
}

/*adds the temporary linked list to the hashtable*/
void addToHash(tempNodePtr head)
{
	if(head == NULL)
	{
		printf("file is empty\n");
		exit(-1);
	}
	tempNodePtr ptr = head;
	recordPtr list;
	char* str;
	char* fileName; 
	int len; 
	int counter;
	int fileNum = 0;
	while(ptr != NULL)
	{
		if(ptr->isWord == 1)/*mallocs string and will put into hashtable after counter is 2*/
		{
			counter = 0; 
			len = strlen(ptr->string);
			str = malloc(len * sizeof(char));
			strcpy(str, ptr->string);
			list = (recordPtr)malloc(sizeof(record));
			ptr = ptr->next;
			continue;/*should skip everything*/
		}
		if(counter == 1)/*copies the fileName into the node*/
		{
			len = strlen(ptr->string);
			fileName = malloc(len * sizeof(char));
			strcpy(fileName, ptr->string);
			list->fileName = fileName;
		}
		if(counter == 2)
		{
			counter = 1;
			list = (recordPtr)malloc(sizeof(list));
			continue;/*should skip everything*/
		}
		counter++;
		ptr = ptr->next;
	}
}

/*reads the output file from indexer and puts them in the hashtable*/
void indexFiles(char* dir)
{
	printf("dir is %s\n", dir);
	FILE* outputFile;

	outputFile = fopen(dir,"r");

	if(outputFile == 0)
	{
		printf("the file does not exist\n");
		exit(-1);
	}

	struct my_struct* s;
	tempNodePtr head = NULL; 
	tempNodePtr tail = NULL;
	tempNodePtr temp = NULL;	

	char* input;
	char* token;
	char line[1000]; /*size of line*/
	int len;
	int isWord = 0;
	int freq; 	
	char* str; 

	while(input = fgets(line, 1000, outputFile))
	{
		printf("line is %s\n", line);
		if(input == NULL)
		{
			break;
		}
		len = strlen(line);
		line[len-1] = '\0';
		token = strtok(line, " ");
		printf("token before while is %s\n", token);
		while(token != NULL)
		{
			printf("token is %s\n", token);
			if((token == NULL)|| (strcmp(token, "</list>") == 0))/*goes to next token if null or </*list>*/
			{
				break;
			}
			if(strcmp(token, "<list>") == 0)/*goes to next token if <list>*/
			{
				printf("reading list\n");
				isWord = 1;
				token = strtok(NULL, " ");
			}
			if(isWord == 1){/*makes nodes for words*/
				isWord = 0;
				str = malloc(sizeof(strlen(token)));
				strcpy(str, token);
				printf("str is %s\n", str);
				if(head == NULL)
				{
					head = (tempNodePtr)malloc(sizeof(tempNode));
					head->string = str;
					head->isWord = 1;
					head->frequency = 0;
					head->next = NULL;
					tail = head;
				}else{
					temp = (tempNodePtr)malloc(sizeof(tempNode));
					temp->string = str;
					temp->isWord = 1;
					temp->frequency = 0;
					temp->next = NULL;
					tail->next = temp;
					tail = temp;
				}
				token = strtok(NULL, " ");
			}else{/*makes nodes for files and frequency*/
				int l = strlen(token);/*PROPER WAY TO COPY STRINGS*/
				str = malloc(len* sizeof(char));
				strcpy(str,token);
				temp = (tempNodePtr)malloc(sizeof(tempNode));
				temp->string = str;
				temp->isWord = 0;
				temp->frequency = 0;
				temp->next = NULL;
				tail->next = temp;
				tail = temp;
			}
			token = strtok(NULL, " ");
		}/*end of inner while*/

	}/*end of outer while*/
	temp = head;
	while(temp != NULL)
	{
		printf("list is: %s\n", temp->string);
		temp = temp->next;
	}
	addToHash(head);
}

int main(int argc, char** argv)
{
	char* dir = argv[1];
	indexFiles(dir);/*successfull indexes files and puts them into the hashtable words*/
	struct my_struct* s;
	struct my_struct* tmp;
	int counter = 0;
	char line[BUFFER_LEN];
	char** args;
	char* arg; 
	int i = 0;
	int j = 0;	
	int len = 0;
	char* token;
	char* input;  

	recordPtr soFileList = NULL;/*will be used to keep track of the files for SO list*/
	recordPtr saFileList = NULL; /*will be used to keep track of the files for SA filelist*/
	recordPtr t = NULL;
	recordPtr temp = NULL;
	int isQ = 0;
	/*infinite loop that does the constant querying*/
	while(1)
	{
		fputs("Enter search query\n", stdout);
		fflush(stdout);
		input = fgets(line, BUFFER_LEN, stdin);
		if(input == NULL)
		{
			break;
		}
		len = strlen(line);
		line[len-1] = '\0';
		printf("input is %s\n", line);
		args = malloc(10* sizeof(char*));
		token = strtok(line," ");
		i = 0;
		while(token != NULL)
		{

			len = strlen(token) + 1;
			arg = (char*)malloc(len * sizeof(char));
			strcpy(arg, token);
			args[i] = arg;
			if(strcmp(args[0],"q") == 0)
			{
				printf("it is q\n");
				/*anything u want to do outside of the loop, make a method and call here*/
				isQ = 1;
				break;
			}
			if((strcmp(args[0],"so") == 0) && i > 0)
			{
				printf("calls so\n");
				soFileList = SO(args[i], soFileList);
				t = soFileList;
			}
			if((strcmp(args[0],"sa") == 0) && i > 0)
			{
				printf("calls sa\n");
				saFileList = SA(args[i], saFileList);
			}
			i++;
			printf("token is %s\n", token);
			token = strtok(NULL, " ");
		}/*end of inner while*/
		if(isQ == 1)
		{
			break;
		}
		if(strcmp(args[0],"so") == 0)
		{
			printSOList(soFileList);
			freeLists(soFileList);
		}
		/* test to cheeck what is in the array*/
		/*for(j=0; j < 10; j++)
		  {
		  if(args[j] == NULL)
		  {
		  break;
		  }
		  printf("arg[j] is %s\n", args[j]);
		  }*/
	}/*end of outer while loop*/

	/*Testing purposes. Prints hashtable*/
	for(s = words; s != NULL; s = s->hh.next)
	{
		printf( "<list> %s\n", s->word);
		for (temp = s->list; temp != NULL; temp = temp->next){
			if(counter == 5)
			{
				counter = 0;
				printf("\n");
			}
			printf("%s %d ", temp->fileName, temp->frequency);
			counter++;
		}
		counter = 0;
		printf("\n</list>\n");
	}

	/*space to free memory*/
	freeHash();
	freeLists(soFileList);
	freeLists(saFileList);
	free(args);
	return 1;
}
