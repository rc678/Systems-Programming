#include "uthash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#define BUFFER_LEN 100

typedef struct record{
	char* fileName;
	int frequency;
	struct record* next;
	struct record* prev;
}record, *recordPtr;

struct my_struct{
	const char* word;
	recordPtr list;
	UT_hash_handle hh;
};

/*function prototypes*/
void freeLists(recordPtr);
recordPtr copyList(recordPtr);

struct my_struct* words;

/*Searches a term in the query and adjusts the output list using "logical and"*/
recordPtr SA(char* word, recordPtr List)
{
	recordPtr head = List; /*pointer to list that will be adjusted*/
        struct my_struct* s; /*pointer to hash table*/
        recordPtr ptr = NULL; /*pointer for list traversak*/
        recordPtr temp; /*pointer for list in hash table*/

	/*Do nothing if word is not in hash table*/
        HASH_FIND(hh, words, word, strlen(word), s); 
        if(!s){
                return head; 
        }   

	/*Set the list to the list in the hash table for the word if this is the first term that is actually found*/
        if (List == NULL)
        {   
                head = copyList(s->list); /*copy the list so the hash table list isnt altered*/
		temp = head;
		
		/*Set the frequency to 1 for each record in the list*/
                while (temp != NULL)
                {   
                        temp->frequency = 1;
			temp = temp->next;
                }   
   		
                return head;
        }   

	/*If the list has already been initialized, traverse it and increment the frequency for any record whose
	file name is also in the list for the word in the hash table*/
        for (temp = s->list; temp != NULL; temp = temp->next)
        {    
                ptr = head;
                while (ptr != NULL)
                {  
                        if (strcmp(ptr->fileName, temp->fileName) == 0)
                        {   
                                ptr->frequency++;
                        }
			ptr = ptr->next;   
                }   
   	 
	}   

	return head;
}


/*Searches a term in the query and adjusts the output list using "logical or"*/
recordPtr SO(char* word, recordPtr fileList)
{
	recordPtr head = fileList;
	struct my_struct* s;
	struct my_struct* tmp;
	recordPtr ptr = NULL; 
	recordPtr temp;
	recordPtr t;

	HASH_FIND(hh, words, word, strlen(word), s);
	if(!s){
		return head; 
	}

	if(head == NULL)
	{
		temp = s->list;
		head = copyList(temp);
		return head; 
	}
	/*traverses word's list in hashtable and adds any records to the output list whose file name
	is not already in it*/
	for(temp = s->list; temp != NULL; temp = temp->next)
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

/*prints list of file names for an SA search query*/
void printSAList(recordPtr list, int numTerms)
{
	recordPtr ptr = list;
	while(ptr != NULL)
	{
		/*only print the file names that have occurred for each word in the query*/
		if (ptr->frequency == numTerms)
		{
			printf("%s ", ptr->fileName);
		}
		ptr = ptr->next;
	}
}

/*prints list of file names for a SO query*/
void printSOList(recordPtr list)
{
	recordPtr ptr = list;
	while(ptr != NULL)
	{
		printf("%s ", ptr->fileName);
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
	if(list == NULL)
	{
		return;
	}
	recordPtr p;
	recordPtr next;
	for(p = list; p != NULL; p = next)
	{
		next = p->next;
		free(p);
	}

}

/*copies a list of record pointers to a new list*/
recordPtr copyList(recordPtr head)
{
	recordPtr p = head;
	recordPtr newList = NULL;
	recordPtr tail = NULL;
	recordPtr temp;
	while(p != NULL)
	{
		if(newList == NULL)
		{
			newList = (recordPtr)malloc(sizeof(record));
			newList->fileName = p->fileName;
			newList->frequency = p->frequency;
			newList->next = NULL;
			newList->prev = NULL;
			tail = newList;
		}else{
			temp = (recordPtr)malloc(sizeof(record));
			temp->fileName = p->fileName;
			temp->frequency = p->frequency;
			temp->next = NULL;
			temp->prev = tail;
			tail->next = temp;
			tail = temp;
		}
		p = p->next;
	}
	return newList;
}

/*reads an inverted index file in the format of the output file from indexer and puts the data in a hashtable*/
void indexFiles(char* dir)
{
	FILE* outputFile;

	outputFile = fopen(dir,"r");

	if(outputFile == 0)
	{
		printf("the file does not exist\n");
		exit(-1);
	}

	recordPtr head = NULL; 
	recordPtr tail = NULL;
	recordPtr temp = NULL;	

	struct my_struct* s;
	char* input;
	char* token;
	char line[1000]; /*size of line*/
	int len;
	int freq; 	
	char* file;
	char* word; 
	int counter = 0;
	int listComplete = 0;
	int isWord = 0;
	int nodeCounter = 0;
	recordPtr add;
	recordPtr z; 
	int fileIsEmpty = 1;

	while(input = fgets(line, 1000, outputFile))
	{
		fileIsEmpty = 0;
		if(input == NULL)
		{
			break;
		}
		len = strlen(line);
		line[len-1] = '\0';
		token = strtok(line, " ");
		
		while(token != NULL)
		{
			if(strcmp(token, "</list>") == 0)/*insert into hashtable*/
			{
				/*ADD TO HASH TABLE*/
				add = copyList(head);
				char* w = malloc(sizeof(word));
				w = strcpy(w, word);
				HASH_FIND(hh, words, w, strlen(w), s);
				if(s)
				{
					break;
				}else{
					s = (struct my_struct*)malloc(sizeof(struct my_struct));
					s->word = w;
					s->list = add;
					HASH_ADD_KEYPTR(hh, words, w, strlen(w), s);
				}
				freeLists(head);
				free(word);
				head = NULL;
				nodeCounter = 0;
				counter = -1;
			}
			if((counter%2 == 0) && (counter != 0) && (counter > 0))/*even but not zero. copy frequency*/
			{
				freq = atoi(token);
				if(head == NULL)
				{
					head = (recordPtr)malloc(sizeof(record));
					head->fileName = file;
					head->frequency = freq;
					head->next = NULL;
					head->prev = NULL;
					tail = head;
					nodeCounter = 1;

				}else{
					temp = (recordPtr)malloc(sizeof(record));
					temp->fileName = file;
					temp->frequency = freq;
					temp->next = NULL;
					temp->prev = tail;
					tail->next = temp;
					tail = temp;
					nodeCounter++;
				}
				counter++;
				freq = -1;
				/*figure out way to skip next if or get next token*/
			}
			if((counter%2 == 1) && (counter > 0) && (freq != -1)){/*odd so strcpy file*/
				len = strlen(token);
				file = malloc(len *sizeof(char));
				strcpy(file, token);
				counter++;
			}	
			if(counter == 0 && (strcmp(token, "<list>") != 0)){/*strcpy word*/
				len = strlen(token);
				word = malloc(len * sizeof(char));
				strcpy(word, token);
				counter++;
			}
			if((strcmp(token, "<list>") == 0))/*goes to next token if <list>*/
			{
				counter = 0;	
			}
			freq = 0;
			token = strtok(NULL, " ");
		}/*end of inner while*/

	}/*end of outer while*/

	if (fileIsEmpty == 1){ 
		printf("Input file is empty\n");
		exit(-1);
	}   
}

int main(int argc, char** argv)
{
	if(argc < 2)
	{   
		printf("Too few arguments\n");
		return 0;
	}   

	if(argc > 2)
	{   
		printf("Too many arguments\n");
		return 0;
	}  

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
	int numTerms = 0;	/*number of terms in current search query*/
	char* token;	/*single token of input line*/
	char* input;	/*input query from user*/

	recordPtr soFileList = NULL;/*will be used to keep track of the files for SO list*/
	recordPtr saFileList = NULL; /*will be used to keep track of the files for SA filelist*/
	recordPtr t = NULL;
	recordPtr temp = NULL;
	int isQ = 0;
	
	/*infinite loop that does the constant querying until user asks to quit*/
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
				/*anything u want to do outside of the loop, make a method and call here*/
				isQ = 1;
				break;
			}
			if((strcmp(args[0],"so") == 0) && i > 0)
			{
				soFileList = SO(args[i], soFileList);
				t = soFileList;
			}
			if((strcmp(args[0],"sa") == 0) && i > 0)
			{
				numTerms++;
				saFileList = SA(args[i], saFileList);
			}
			i++;
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
			soFileList = NULL;
		}
		if(strcmp(args[0], "sa") == 0)
		{
			printSAList(saFileList, numTerms);
			freeLists(saFileList);
			saFileList = NULL;
			numTerms = 0;
		}
	}/*end of outer while loop*/

	/*space to free memory*/
	freeHash();
	freeLists(soFileList);
	freeLists(saFileList);
	free(args);
	return 1;
}
