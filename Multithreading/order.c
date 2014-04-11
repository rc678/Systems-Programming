#include "uthash.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

typedef struct qNode{
	char* bookTitle;
	int customerID;
	char* category;
	struct qNode *next;
}qNode, *qNodePtr;

typedef struct Queue{
	qNodePtr head;
	qNodePtr tail;
	int numOrders;
}Queue, *QueuePtr;

struct my_struct{
        const char* category;
        QueuePtr q;
        UT_hash_handle hh; 
};

struct my_struct* categories;

qNodePtr enqueue(QueuePtr q, qNodePtr newNode)
{
	if (q->head == NULL){
		q->head = newNode;
		q->tail = newNode;
		q->numOrders++;
		return q->head;
	}
	
	q->tail->next = newNode;
	q->tail = newNode;
	q->numOrders++;
	return q->head;
}

qNodePtr dequeue(QueuePtr q)
{
	if (q->head == NULL){
		return q->head;
	}
	qNodePtr temp = q->head;
	q->head = q->head->next;
	q->numOrders--;
	return temp;
}

/*reads categories.txt and adds a queue for each category to the hashtable*/
void createQueues(char* categories)
{
	FILE* cg;
	cg = fopen(categories, "r");
	char* line;
	char category[100];
	int len;
	struct my_struct *s;

	if (cg == NULL){
		printf("category file does not exist\n");
		exit(-1);
	}

	if (fgetc(cg) == EOF){
		printf("category file is empty\n");
		exit(-1);
	}
	rewind(cg);

	while(line = fgets(category, 100, cg))
	{
		if (line == NULL){
			return;
		}
		
		len = strlen(line);
                line[len-1] = '\0';
		char* c = malloc(sizeof(char));
		strcpy(c, line);
		s = (struct my_struct*)malloc(sizeof(struct my_struct));
		s->category = c;
		QueuePtr temp = (QueuePtr)malloc(sizeof(Queue));
		s->q = temp;
		HASH_ADD_KEYPTR(hh, categories, c, strlen(c), s);
	}

	fclose(cg);
}

/*makes a data structure based on the database.txt*/
void createDatabase(char* database){
	FILE* dbase;

	dbase = fopen(database, "r");
	int size = 0;
	char* input;
	int lineNum = 0;

	if(dbase == NULL)
	{
		printf("database file does not exist\n");
		exit(-1);
	}

	int c;

	if (fgetc(dbase) == EOF){
		printf("database file is empty\n");
		exit(-1);
	}
	rewind(dbase);

	/*counts the number of lines in the database.txt*/
	do{
		c = fgetc(dbase);
		if(c == '\n')
		{
			lineNum++;
		}
	}while(c != EOF);

	printf("%d\n", lineNum); 	

	qNode db[lineNum];

	rewind(dbase);
	char* in;
	char line[1000];	

	while(in = fgets(line, 1000, dbase))
	{

		if(in == NULL)
		{
			break;
		}

		printf("%s\n", in);
	}	

}

int main(int argc, char** argv)
{

	if(argc < 4)
	{
		printf("Too few arguements\n");
		return 0;
	}

	if(argc > 4)
	{
		printf("Too many arguements\n");
		return 0;
	}

	char* database = argv[1];
	char* order = argv[2];
	char* categories = argv[3];

	if(database == NULL || order == NULL || categories == NULL){
		printf("argument is null\n");
		return 0;
	}
	createDatabase(database);	

	return 0;
}

