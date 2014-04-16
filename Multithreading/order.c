#include "uthash.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

typedef struct qNode{
	char* bookTitle;
	int customerID;
	double price;
	char* category;
	struct qNode *next;
}qNode, *qNodePtr;

typedef struct Queue{
	qNodePtr head;
	qNodePtr tail;
	int numOrders;
	pthread_mutex_t lock;
	pthread_cond_t nonEmpty;
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

struct myStruct{
	int id;
	iPtr info;
	UT_hash_handle hh;
};

void createQueues(char*);
struct my_struct* cat = NULL;
struct myStruct* cData = NULL; 
int numConsumers = 0;
int revenue = 0;

QueuePtr enqueue(QueuePtr q, qNodePtr newNode)
{
	if (q->head == NULL){
		q->head = newNode;
		q->tail = newNode;
		q->numOrders++;
		return q;
	}

	q->tail->next = newNode;
	q->tail = newNode;
	q->numOrders++;
	return q;
}

qNodePtr dequeue(QueuePtr q)
{
	if (q->head == NULL){
		return NULL;
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
			numConsumers++;
			HASH_ADD_KEYPTR(hh, cat, c, strlen(c), s);
		}
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

	char* in;
	char line[1000];	
	char* token;
	int counter = 0;
	int len;
	int id = 0;
	iPtr temp = NULL;
	double credit = 0.0;
	struct myStruct* s = NULL;

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
			if(counter == 0)
			{	
				len = strlen(token);
				temp = (iPtr) malloc(sizeof(custInfo));
				char* name = malloc(len *sizeof(char));
				strcpy(name, token);
				temp->name = name;
			}
			if(counter == 1)
			{
				id = atoi(token);
			}
			if(counter == 2)
			{
				credit = atof(token);
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
				counter = -1;
				HASH_FIND_INT(cData, &id, s);
				if(s)
				{
					break;
				}else{
					s = (struct myStruct*) malloc(sizeof(struct myStruct));
					s->info = temp;
					HASH_ADD_INT(cData, id, s);
				}
			}
			counter++;
			token = strtok(NULL, "|");		
		}/*end of inner while*/	
	}/*end of outer while*/
}

/*producer thread that will read in data file with book orders and puts them in the queue*/ 
void* producer(void* file)
{

	FILE* f = fopen(file, "r");
	struct my_struct* s;
	if(f == NULL)
	{
		printf("cannot open file\n");
		exit(EXIT_FAILURE);
	}

	char* in; 
	char line[1000]; 
	int len = 0;
	int counter = 0;
	char* book;
	char* cate;
	double cost;
	int id;
	char* token;
	qNodePtr temp;

	while(in = fgets(line, 1000, f))
	{
		if(in == NULL)
		{
			printf("input is null\n");
			break;
		}

		len = strlen(line);
		line[len-1] = '\0';
		token = strtok(line, "|");

		while(token != NULL)
		{
			if(counter == 0)
			{
				len = strlen(token);
				temp = (qNodePtr) malloc(sizeof(qNode));
				char* book = malloc(len * sizeof(char));
				strcpy(book, token);
				temp->bookTitle = book;
			}
			if(counter == 1)
			{
				cost = atof(token);
				temp->price = cost;
			}
			if(counter == 2)
			{
				id = atoi(token);
				temp->customerID = id;	
			}
			if(counter == 3)
			{
				len = strlen(token);
				cate = malloc(len * sizeof(char));
				strcpy(cate, token);
				temp->category = cate;
				counter = -1;
				HASH_FIND(hh, cat, cate, strlen(cate), s);
				if(!s)
				{
					break;
				}else{/*if category in the hashtable, add to queue*/
					if(s->q->numOrders < 10)
					{
						QueuePtr list = s->q;
						list = enqueue(list, temp);
						s->q = list;
						pthread_mutex_unlock(&s->q->lock);
						pthread_cond_signal(&s->q->nonEmpty);
					}else{/*wait for the consumer*/
						printf("waiting for consumer\n");
						pthread_mutex_lock(&s->q->lock);	
					}
				}
			}
			counter++;
			token = strtok(NULL, "|");	
		}/*end of inner while*/

		pthread_cond_broadcast(&s->q->nonEmpty);/*double check this later*/
		/*REMEMBER TO FREE MEMORY*/

	}/*end of outer while*/


	fclose(f);
	return NULL;		
}/*end of producer function*/ 

void *consumer(void *param)
{
        struct Queue *queue = (struct Queue *)param;
    
        while (queue->numOrders < 10) 
        {   
                qNodePtr order = dequeue(queue);
                int cust = order->customerID;
                struct myStruct *s;
                HASH_FIND_INT(cData, &cust, s);
                iPtr customer = s->info;
                double bookprice = order->price;
                double credit = customer->creditLimit;
                if (credit >= bookprice)
                {   
                        customer->creditLimit -= bookprice;
                        revenue += bookprice;
                        printf("Order Confirmation:\nBook Title: %s\nPrice: %g\n", order->bookTitle, order->price);     
                        printf("Customer Name: %s\n\tAddress: %s\n", customer->name, customer->address);
                        printf("\tState: %s\n\tZip Code: %s\n", customer->state, customer->zip);
                } else {
                        printf("Order Rejection\n\tCustomer Name: %s\n\tBook Title: %s\n", customer->name, order->bookTitle);
                        printf("\tBook Price: %g\n\tCustomer's Remaining Credit Limit: %g\n", order->price, customer->creditLimit);
                }   
        }           
}/*end of consumer function*/

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

	struct my_struct *s, *tmp;
	createDatabase(database);
	createQueues(categories);	
	pthread_t producerReturn;
	pthread_t consumerReturn[numConsumers];
        int i = 0;

	pthread_create(&producerReturn, NULL, producer, order);
	
	HASH_ITER(hh, cat, s, tmp)
        {
                pthread_create(&consumerReturn[i], NULL, consumer, s->q);
                i++;
        }
        
	return 0;
}
