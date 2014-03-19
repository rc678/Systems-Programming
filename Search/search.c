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
	int len = 0;
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
		/*prints arguements*/
		/*for(j = 0; j < tokenCounter; j++)
		{
			printf("args is %s and j is %d\n", args[j], j);
		}*/
		len = strlen(args[0]);
		char dest[len];
		strncpy(dest, args[0], sizeof args[0]);
		dest[len-1] = '\0';
		/*for(j = 0; j < 3; j++)
		{
			printf("char in arg[0] is %c\n", dest[j]);
		}*/
		if((strcmp(dest, "sa") == 0))
		{
			printf("calls sa\n");
			SA();
			tokenCounter = 0;
		}else if(strcmp(dest, "so") == 0){
			
			printf("calls so\n");
			SO();
			tokenCounter = 0;
		}else if(strcmp(dest, "q") == 0){
			break;
		}else{
			printf("not valid input\n");
			tokenCounter = 0;
			continue;
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
