#include "indexer.c"
#include "uthash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_LEN 100

void freeLists(recordPtr);

/*Searches a term in the query and adjusts the output list using "logical and"*/
recordPtr SA(char* word, recordPtr List)
{
        recordPtr head = List;
        struct my_struct* s;
        recordPtr ptr;
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

/*/*Searches a term in the query and adjusts the output list using "logical or"*/
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
void printSAList(recordPtr list)
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
	for(s = words; s != NULL; s = s->hh.next)
	{
		freeLists(s->list);
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
	int count;
	int numTerms;
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
		args = malloc(count* sizeof(char*));
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
		}
		if(strcmp(args[0], "sa") == 0)
                {
                        printSAList(saFileList);
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
