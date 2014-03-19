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
	char* token;
	char* args[10];
	int i = 0;
	int j = 0;	
	int tokenCounter = 0;
	
	while(1)
	{
		fputs("Enter search query\n", stdout);
		fflush(stdout);
		fgets(line, BUFFER_LEN, stdin);
		token = strtok(line, " ");
		while(token != NULL)
		{
			args[i] = token;
			i++;
			tokenCounter++;
			token = strtok(NULL, " ");
		}
		i = 0;
		tokenCounter = 0;
		/*prints arguements*/
		for(j = 0; j < tokenCounter; j++)
		{
			printf("args is %s", args[j]);
		}
		if(strcmp(args[0], "sa"))
		{
			SA();
		}else if(strcmp(args[0], "so")){
			SO();
		}else if(strcmp(args[0], "q")){
			break;
		}else{
			printf("not valid input\n");
			continue;
		}
		memset(&args[0], 0, sizeof(args));
		
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
