#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>/*functions to go through directories*/
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include "uthash.h"
#include "tokenizer.c"

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
	DIR* directory;
	struct dirent* file; 
	struct stat statbuf;
	char* name; 
	stat(dir, &statbuf);
	FILE* currFile; 

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
			split(currFile);
			
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

int main(int argc, char** argv)
{
	/*argv[1] is the inverted-index file name. It gives the name of the file you should create to hold your inverted index*/
	/*argv[2] gives the name or directory that your program should index*/

	if(argc != 3)
	{
		printf("Need 3 arguements\n");
		return 0;
	}

	char* dir = argv[2];
	if(dir == NULL)
	{
		printf("directory is NULL\n");
		return 0;
	}	
	readFiles(dir);

}
