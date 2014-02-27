#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>/*functions to go through directories*/

/*Recursively indexes all the files in a directory*/
int readFiles(char* dir)
{
	DIR* directory = opendir(dir);
	struct dirent* file; 

	while((file = readdir(directory)) != NULL)
	{
		printf("%s\n", file->d_name);
	}

	return 0;
	
	
}

int main(int argc, char** argv)
{
	/*argv[1] is the inverted-index file name. It gives the name of the file you should create to hold your inverted index*/
	/*argv[2] gives the name or directory that your program should index*/
	
	if(argc != 3)
	{
		printf("Need 3 arguements");
	}

	char* dir = argv[2];
	readFiles(dir);
		
}
