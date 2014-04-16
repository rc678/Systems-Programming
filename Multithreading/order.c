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

typedef struct custInfo{
	char* name;
	double creditLimit;
	char* address;
	char* state;
	char* zip; 
}custInfo, *iPtr; 

void createQueues(char*);
struct my_struct* cat;


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
	struct my_struct* s;

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
		char* c = malloc(len * sizeof(char));
		strcpy(c, line);
		HASH_FIND(hh, cat, c, strlen(c), s);
		if(s)
		{
			break;
		}else{
			s = (struct my_struct*)malloc(sizeof(struct my_struct));
			s->category = c;
			QueuePtr temp = (QueuePtr)malloc(sizeof(Queue));
			s->q = temp;
			printf("c is %s\n", c);
			HASH_ADD_KEYPTR(hh, cat, c, strlen(c), s);
		}
	}

	fclose(cg);
}

/*makes a data structure based on the database.txt*/
iPtr* createDatabase(char* database){
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

	iPtr* db = malloc(lineNum * sizeof(custInfo));

	rewind(dbase);
	char* in;
	char line[1000];	
	char* token;
	int counter = 0;
	int len;
	int index = 0;
	iPtr temp = NULL;
	double credit = 0.0;

	while(in = fgets(line, 1000, dbase))
	{

		if(in == NULL)
		{
			break;
		}

		len = strlen(line);
		line[len-1] = '\0';
		token = strtok(line, "|");

		while(token != NULL)
		{
			printf("token is %s\n", token);
			if(counter == 0)
			{	
				len = strlen(token);
				temp = (iPtr) malloc(sizeof(custInfo));
				char* name = malloc(len *sizeof(char));
				strcpy(name, token);
				temp->name = name;

			}
			if(counter == 2)
			{
				credit = atoi(token);
				temp->creditLimit = credit;
			}
			if(counter == 3)
			{
				len = strlen(token);
				char* street = malloc(len * sizeof(char));
				strcpy(street, token);
				temp->address = street;
			}
			if(counter == 4)
			{
				len = strlen(token);
				char* state = malloc(len * sizeof(char));
				strcpy(state, token);
				temp->state = state;
			}
			if(counter == 5)
			{
				len = strlen(token);
				char* zip = malloc(len * sizeof(char));
				strcpy(zip, token);
				temp->zip = zip;
				db[index] = temp;
				index++; 
				counter = -1;
			}
			counter++;
			token = strtok(NULL, "|");		
		}/*end of inner while*/	
	}/*end of outer while*/
	/*int i;
	for(i = 0; i < lineNum;i++)
	{
		printf("in array%s\n", db[i]->name);
	} */	
	return db;
}

/*producer thread that will read in data file with book orders and puts them in the queue*/ 
void* producer(void* file)
{

	FILE* f = fopen(file, "r");
	if(f == NULL)
	{
		printf("cannot open file\n");
		exit(EXIT_FAILURE);
	}

	char* in; 
	char line[1000]; 

	while(in = fgets(line, 1000, f))
	{
		printf("line is %s\n", in);
	}

	return NULL;		
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
	
	iPtr* db = NULL;
	char* database = argv[1];
	char* order = argv[2];
	char* categories = argv[3];

	if(database == NULL || order == NULL || categories == NULL){
		printf("argument is null\n");
		return 0;
	}

	db = createDatabase(database);
	createQueues(categories);	
	pthread_t producerReturn;

	pthread_create(&producerReturn, NULL, producer, (void *) order);
	return 0;
}
