#include "indexer.c"
#include "uthash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_LEN 100

/*Searches all the terms in the query and prints their file names. "Logical and"*/
void SA()
{

}

/*Searches all subsets of the term and prints their file names. "Logical or"*/
void SO()
{

}

int main(int argc, char** argv)
{
	char* dir = argv[1];
	indexFiles(dir);/*successfull indexes files and puts them into the hashtable words*/
	struct my_struct* s;
	struct my_struct* tmp;
	recordPtr temp;
	int counter = 0;
	char line[BUFFER_LEN];
	char** args;
	char* arg; 
	size_t i = 0;
	int j = 0;	
	int tokenCounter = 0;
	int len = 0;
	int count;
	char* token;
	char* input;  
	char* q = (char*)malloc(2 * sizeof(char*));
	q = "q";
	char* sa = (char*)malloc(3 * sizeof(char*));
	sa = "sa";
	char* so = (char*)malloc(3 * sizeof(char*));
	so = "so"; 
	
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
			i++;
			tokenCounter++;
			printf("token is %s\n", token);
			token = strtok(NULL, " ");
		}
		for(j=0; j < 10; j++)/*assuming less than 10 entries*/
		{
			if(args[j] == NULL)
			{
				break;
			}
			printf("arg[j] is %s\n", args[j]);
		}
		if((strcmp(args[0], q) == 0))
		{
			printf("it is q\n");
			return 1;
		}
		if((strcmp(args[0],sa)) == 0)
		{
			printf("it is sa\n");
			SA();
		}
		if((strcmp(args[0],so)) == 0)
		{
			printf("it is so\n");
			SO();
		} 
	}
	/*Testing purposes. Prints hashtable*/
	/*for(s = words; s != NULL; s = s->hh.next)
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
	}*/
	return 1;
}
