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

struct my_struct{
	const char* word;
	recordPtr list;
	UT_hash_handle hh;
};

/*function protypes*/
void freeLists(recordPtr);

struct my_struct* words;

/*Searches a term in the query and adjusts the output list using "logical and"*/
recordPtr SA(char* word, recordPtr List)
{
        recordPtr head = List;  
        struct my_struct* s;
        recordPtr ptr = NULL; 
        recordPtr temp;

        HASH_FIND(hh, words, word, strlen(word), s); 
        if(!s){
                printf("word not in the hashtable\n");
                return head; 
        }   

        if (head == NULL)
        {   
                head = s->list;
                for (temp = head; temp != NULL; temp = temp->next)
                {   
                        temp->frequency = 1;
                }   
    
                return head;
        }   

        for (temp = s->list; temp != NULL; temp = temp->next)
        {    
                ptr = head;
                while (ptr != NULL)
                {   
                        if (strcmp(ptr->fileName, temp->fileName))
                        {   
                                ptr->frequency++;
                        }   
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

/*prints output for an SA search query*/
void printSAList(recordPtr list, int numTerms)
{
        recordPtr ptr = list;
        while(ptr != NULL)
        {
                if (ptr->frequency == numTerms)
                {
                        printf("%s\n", ptr->fileName);
                }
                ptr = ptr->next;
        }
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
	int counter = NULL;
	int listComplete = 0;
	int isWord = 0;
	int nodeCounter = 0;
	recordPtr add;
	recordPtr z; 
	while(input = fgets(line, 1000, outputFile))
	{
		if(input == NULL)
		{
			break;
		}
		len = strlen(line);
		line[len-1] = '\0';
		token = strtok(line, " ");
		/*printf("token before while is %s\n", token);*/
		while(token != NULL)
		{
			if(strcmp(token, "</list>") == 0)/*insert into hashtable*/
			{
				printf("word is %s\n", word);
				/*ADD TO HASH TABLE*/
				add = copyList(head);
				char* w = malloc(sizeof(word));
				w = strcpy(w, word);
				HASH_FIND(hh, words, w, strlen(w), s);
				if(s)
				{
					printf("in the hashtable already\n");
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
				printf("token in even is %s\n", token);
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
				printf("token in odd is %s\n", token);
				len = strlen(token);
				file = malloc(len *sizeof(char));
				strcpy(file, token);
				counter++;
			}	
			if(counter == 0 && (strcmp(token, "<list>") != 0)){/*strcpy word*/
				printf("token in 0 is %s\n", token);
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
	int len = 0
	int numTerms = 0;
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
				numTerms++;
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
		if(strcmp(args[0], "sa") == 0)
                {
                        printSAList(saFileList, numTerms);
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
