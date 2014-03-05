#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>/*functions to go through directories*/
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include "uthash.h"
#include "tokenizer.c"


typedef struct record{
	char* fileName;
	int frequency;
	struct record *next;
	struct record *prev;

}record, *recordPtr;

struct my_struct{
	const char* word; 
	recordPtr list;
	UT_hash_handle hh;
};

void split(FILE*, char*);
recordPtr updateRecordList(recordPtr, char*);
recordPtr resortRecordList(recordPtr, recordPtr);

struct my_struct* words; 

int name_sort(struct my_struct* a, struct my_struct* b)
{
	return strcmp(a->word, b->word);
}

void sort_by_name()
{
	HASH_SORT(words, name_sort);
}

/*returns a 1 if the file is a directory*/ 
int validFile(const char* parent, char* name)
{
	struct stat statbuf; 
	char* path = malloc(strlen(name) + strlen(parent) + 2);
	//	printf("parent is %s name is %s\n", parent, name);
	sprintf(path, "%s%s", parent, name);
	stat(path, &statbuf);
	return S_ISDIR(statbuf.st_mode); 
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

	/*if user enter the name of a directory or file that does not exist*/
	if(stat(dir, &statbuf) == -1)
	{
		printf("dir is %s\n", dir);
		return 0;
	}

	//printf("DIR IS %s\n", dir);
	if(S_ISREG(statbuf.st_mode))
	{
		//	printf("in here and file name is %s\n", dir);
		currFile = fopen(dir, "r");
		if(currFile != NULL)
		{
			//printf("FILE OPENED PROPERLY\n");
			/*FILE IS NOW OPEN. WRITE CODE TO INDEX FILE HERE*/
			split(currFile, dir);
			/*for(s=words; s != NULL; s=s->hh.next) {
				printf("<list> %s\n", s->word);
				for (temp = s->list; temp != NULL; temp = temp->next){
					printf("(%s, %d) ", temp->fileName, temp->frequency);
				}
				printf("\n</list>\n");
			}*/
		}
		return 0;
	}

	if(S_ISDIR(statbuf.st_mode)){
		directory = opendir(dir);

		while((file = readdir(directory)) != NULL)
		{
			name = file->d_name;
			if(name[0] == '.' || name[strlen(name)-1] == '~')
			{
				continue; 
			}
			printf("%s\n", file->d_name);
			char* nextFile = malloc(strlen(dir) + strlen(name) + 1);
			sprintf(nextFile, "%s/%s", dir, name);
			readFiles(nextFile);
			free(nextFile);				

		}
	}

	return 1;


}

/*This function will start the process of Tokenizing the string from the file
 *  *  *  * *It will return a 1 if successfully split and a 0 otherwise.
 *   *   *   * */ 
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
	while(token != NULL)
	{
		HASH_FIND(hh, words, token, strlen(token), s);
		if(s)
		{
			z = s->list; 
			s->list = updateRecordList(z, fptr);
		}else{
			printf("not in s\n");
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


recordPtr updateRecordList(recordPtr head, char* fname)
{
	recordPtr temp = head;
	recordPtr curr = head;

	while (curr){
		if (strcmp(fname, curr->fileName) == 0){
			curr->frequency++;
			head = resortRecordList(head, curr);
			return head;
		}

		temp = curr;
		curr = curr->next;
	}

	recordPtr newRecord = (recordPtr)malloc(sizeof(struct record));
	newRecord->frequency = 1;
	newRecord->fileName = fname;
	newRecord->prev = temp;
	newRecord->next = NULL;
	temp->next = newRecord;
	return head;
}


recordPtr resortRecordList(recordPtr head, recordPtr curr)
{
	if (curr == head){
		return head;
	}

	recordPtr temp = curr->prev;

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

int main(int argc, char** argv)
{
	/*argv[1] is the inverted-index file name. It gives the name of the file you should create to hold your inverted index*/
	/*argv[2] gives the name or directory that your program should index*/

	if(argc != 3)
	{
		printf("Need 3 arguements\n");
		return 0;
	}

	struct my_struct* s;
	recordPtr temp;
	char* dir = argv[2];
	FILE* output;
	if(dir == NULL)
	{
		printf("directory is NULL\n");
		return 0;
	}	
	readFiles(dir);
	sort_by_name();

	
	output = fopen(argv[1], "a+");

	for(s=words; s != NULL; s=s->hh.next) {
	  fprintf(output, "<list> %s\n", s->word);
	  for (temp = s->list; temp != NULL; temp = temp->next){
	  fprintf(output,"<%s, %d>", temp->fileName, temp->frequency);
	  }
	  fprintf(output,"\n</list>\n");
	  }
}

