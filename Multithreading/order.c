#include "uthash.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

typedef struct qNode{
	double price;
	double credRemain;
	int customerID;
	int noMoreOrders;
	char* bookTitle;
	const char* category;
	struct qNode *next;
}qNode, *qNodePtr;

typedef struct Queue{
	qNodePtr head;
	qNodePtr tail;
	int numOrders;
	pthread_mutex_t mut;
	pthread_cond_t nonEmpty;
	pthread_cond_t nonFull;
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
	QueuePtr successes;
	QueuePtr failures; 
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
		return q;
	}

	q->tail->next = newNode;
	q->tail = q->tail->next;
	return q;
}

qNodePtr dequeue(QueuePtr q)
{
	if (q->head == NULL){
		return NULL;
	}

	if (q->head->next == NULL){
		qNodePtr temp = q->head;
		q->head = NULL;
		q->tail = NULL;
		return temp;
	}

	qNodePtr temp = q->head;
	q->head = q->head->next;
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
	int idIns = 0;
	iPtr temp = NULL;
	double credit = 0.0;
	struct myStruct* s = NULL;
	struct myStruct* tmp = NULL;
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
				len = strlen(token)+1;
				temp = (iPtr) malloc(sizeof(custInfo));
				char* name = malloc(len *sizeof(char));
				strcpy(name, token);
				temp->name = name;
			}
			if(counter == 1)
			{
				idIns = atoi(token);
			}
			if(counter == 2)
			{
				credit = atof(token);
				temp->creditLimit = credit;
			}
			if(counter == 3)
			{
				len = strlen(token) + 1;
				char* street = malloc(len * sizeof(char));
				strcpy(street, token);
				temp->address = street;
			}
			if(counter == 4)
			{
				len = strlen(token) + 1;
				char* state = malloc(len * sizeof(char));
				strcpy(state, token);
				temp->state = state;
			}
			if(counter == 5)
			{
				len = strlen(token) + 1;
				char* zip = malloc(len * sizeof(char));
				strcpy(zip, token);
				temp->zip = zip;
				temp->successes = (QueuePtr)malloc(sizeof(Queue));
				temp->failures = (QueuePtr)malloc(sizeof(Queue));
				int i = idIns;
				counter = -1;
				HASH_FIND_INT(cData, &i, s);
				if(s)
				{
					break;
				}else{
					s = (struct myStruct*) malloc(sizeof(struct myStruct));
					s->id = i;
					s->info = temp;
					HASH_ADD_INT(cData, id , s);
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
	struct my_struct* tmp;
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
				len = strlen(token) + 1;
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
				len = strlen(token) + 1;
				cate = malloc(len * sizeof(char));
				strcpy(cate, token);
				temp->category = cate;
				counter = 0;

				struct my_struct *p;
				qNodePtr curr;
				/*for(p = cat; p != NULL; p = p->hh.next){
					if(p->q->head == NULL){
						printf("queue is empty\n");
						break;
					}
					printf("category: %s\n", p->category);
					for(curr = p->q->head; curr != NULL; curr = curr->next){
						printf("Node is %s\n",curr->bookTitle);
					}
				}*/


				HASH_FIND(hh, cat, cate, strlen(cate), s);
				pthread_mutex_lock(&s->q->mut);
				if(!s)
				{
					free(book);
					free(cate);
					pthread_mutex_unlock(&s->q->mut);
					break;
				}else{/*if category in the hashtable, add to queue*/
					if(s->q->numOrders == 10)
					{
						pthread_cond_signal(&s->q->nonEmpty);
						printf("***Producer waits because queue is full***\n");
						pthread_cond_wait(&s->q->nonFull,&s->q->mut);
						printf("***Producer resumes because queue has space available***\n");
					}
			
					QueuePtr list = s->q;
					list = enqueue(list, temp);
					s->q = list;
					s->q->numOrders++;
					pthread_mutex_unlock(&s->q->mut);
					pthread_cond_signal(&s->q->nonEmpty);
					break;
				}
			}
			counter++;
			token = strtok(NULL, "|");	
		}/*end of inner while*/
		

		/*REMEMBER TO FREE MEMORY*/

	}/*end of outer while*/


	HASH_ITER(hh, cat, s, tmp){
		pthread_mutex_lock(&s->q->mut);
		temp = (qNodePtr)malloc(sizeof(qNode));
		temp->noMoreOrders = 1;
		temp->category = s->category;
		s->q = enqueue(s->q, temp);
		pthread_mutex_unlock(&s->q->mut);
                pthread_cond_signal(&s->q->nonEmpty);
	}

	fclose(f);
	return 0;		
}/*end of producer function*/ 

void *consumer(void *param)
{
	struct Queue *queue = (struct Queue *)param;
	int ordersProcessed = 0;
	const char* cgry;

	while (1) 
	{  
		pthread_mutex_lock(&queue->mut); 
		if(queue->numOrders == 0)
		{
			pthread_cond_signal(&queue->nonFull);
			printf("***Consumer waits if the queue is empty***\n");
			pthread_cond_wait(&queue->nonEmpty, &queue->mut);
			printf("***Consumer resumes because queue has orders ready for processing***\n");
		}
		
		qNodePtr order = dequeue(queue);
		if(order->noMoreOrders == 1){
			cgry = order->category;
			break;
		}
		queue->numOrders--;
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
			printf("Order Confirmation:\nBook Title: %s\nPrice: %g\nCustomer Name: %s\n\tAddress: %s\n\tState: %s\n\tZip Code: %s\n", order->bookTitle, order->price, customer->name, customer->address, customer->state, customer->zip);    
			qNodePtr succ = (qNodePtr)malloc(sizeof(qNode));
			succ->price = order->price;
			succ->bookTitle = order->bookTitle;
			succ->credRemain = customer->creditLimit;
			customer->successes = enqueue(customer->successes, succ); 
			ordersProcessed++;
		} else {
			printf("Order Rejection\n\tCustomer Name: %s\n\tBook Title: %s\n\tBook Price: %g\n\tCustomer's Remaining Credit Limit: %g\n", customer->name, order->bookTitle, order->price, customer->creditLimit);
			qNodePtr fail = (qNodePtr)malloc(sizeof(qNode));
			fail->price = order->price;
			fail->bookTitle = order->bookTitle;
			customer->failures = enqueue(customer->failures, fail);	
			ordersProcessed++;
		} 
		pthread_mutex_unlock(&queue->mut);
		pthread_cond_signal(&queue->nonFull);
	}

	printf("%d orders processed for consumer thread %s\n", ordersProcessed, cgry);
	pthread_mutex_unlock(&queue->mut);
	return 0;
}/*end of consumer function*/

int main(int argc, char** argv)
{

	if(argc < 4)
	{
		printf("Too few arguments\n");
		return 0;
	}

	if(argc > 4)
	{
		printf("Too many arguments\n");
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

	HASH_ITER(hh, cat, s, tmp)
        {   
                pthread_mutex_init(&s->q->mut, NULL);
                pthread_cond_init(&s->q->nonEmpty, NULL);
                pthread_cond_init(&s->q->nonFull, NULL);
                s->q->numOrders = 0;
        }   

	pthread_create(&producerReturn, NULL, producer, order);

	HASH_ITER(hh, cat, s, tmp)
	{
		pthread_create(&consumerReturn[i], NULL, consumer, s->q);
		i++;
	}
	
	pthread_join(producerReturn, NULL);
	for(i = 0; i < numConsumers; i++)
	{
		pthread_join(consumerReturn[i], NULL);
	}

	printf("\nAll orders have been processed. The following is the final report:\n");
	
	struct myStruct *y;
	struct myStruct *m;
	qNodePtr curr;
	HASH_ITER(hh, cData, y, m)
	{
		printf("\n=== BEGIN CUSTOMER INFO ===\n### BALANCE ###\n");
		printf("Customer name: %s\nCustomer ID number: %d\n", y->info->name, y->id);
		printf("Remaining credit balance after all purchases: $%g\n", y->info->creditLimit);	
		printf("### SUCCESSFUL ORDERS ###\n");
		for(curr = y->info->successes->head; curr != NULL; curr = curr->next)
		{
			printf("%s|$%g|$%g\n", curr->bookTitle, curr->price, curr->credRemain);
		}
		printf("### REJECTED ORDERS ###\n");
		for(curr = y->info->failures->head; curr != NULL; curr = curr->next)
                {
                        printf("%s|$%g\n", curr->bookTitle, curr->price);
                }
		printf("=== END OF CUSTOMER INFO ===\n");
	}
	return 0;
}

