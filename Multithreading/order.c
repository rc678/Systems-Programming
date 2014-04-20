#include "uthash.h" /*Copyright (c) 2005-2014, Troy D. Hanson  http://troydhanson.github.com/uthash*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "queue.h"

/*struct for global cat hashtable*/
struct my_struct{
	const char* category; /*category string as key*/
	QueuePtr q; /*Queue struct as value*/
	UT_hash_handle hh; 
};

/*holds a single customer's information*/
typedef struct custInfo{
	char* name;
	double creditLimit; /*holds either customer's initial credit limit or current credit limit (depending on where it is used)*/
	char* address;
	char* state;
	char* zip;
	QueuePtr successes; /*queue of succesful orders for this customer*/
	QueuePtr failures; /*queue of rejected orders for this customer*/
}custInfo, *iPtr; 

/*struct for global cData hashtable*/
struct myStruct{
	int id; /*customer ID as key*/
	iPtr info; /*custInfo struct as value*/
	UT_hash_handle hh;
};

struct my_struct* cat = NULL; /*global hash table for category Queues*/
struct myStruct* cData = NULL; /*global hash table for customers*/
int numConsumers = 0; /*number of consumer threads, one for each category*/
double revenue = 0; /*total profit from all successful book orders*/

/*reads input category file and adds a queue for each category to cat hashtable*/
void createQueues(char* categories)
{
	FILE* cg;
	cg = fopen(categories, "r");
	char* line;
	char category[100];
	int len;
	struct my_struct* s;

	if (cg == NULL){
		printf("Category names input file does not exist\n");
		exit(-1);
	}

	if (fgetc(cg) == EOF){
		printf("Category file is empty\n");
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
		if(s) /*if category has already been added, ignore and read next category*/
		{
			break;
		}else{
			s = (struct my_struct*)malloc(sizeof(struct my_struct));
			s->category = c;
			QueuePtr temp = (QueuePtr)malloc(sizeof(Queue));
			s->q = temp;
			numConsumers++;
			HASH_ADD_KEYPTR(hh, cat, c, strlen(c), s); /*Add Queue to cat hashtable with category string as key*/
		}
	}

	fclose(cg);
}

/*Create global cData hashtable from information in input database file*/
void createDatabase(char* database){
	FILE* dbase;

	dbase = fopen(database, "r");
	int size = 0;
	char* input;
	int lineNum = 0;

	if(dbase == NULL)
	{
		printf("Database input file does not exist\n");
		exit(-1);
	}

	int c;

	if (fgetc(dbase) == EOF){
		printf("Database file is empty- No customers for order processing\n");
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
	/*reads file line by line*/
	while(in = fgets(line, 1000, dbase))
	{

		if(in == NULL)
		{
			break;
		}

		len = strlen(line);
		line[len-1] = '\0';
		token = strtok(line, "|");

		/*read each customers information assuming given format*/
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
			if(counter == 5) /*Customer's information has been read. Add to hashtable*/
			{
				len = strlen(token) + 1;
				char* zip = malloc(len * sizeof(char));
				strcpy(zip, token);
				temp->zip = zip;
				temp->successes = (QueuePtr)malloc(sizeof(Queue)); /*allocate memory for successful orders for later use*/
				temp->failures = (QueuePtr)malloc(sizeof(Queue)); /*allocate memory for rejected orders for later use*/
				int i = idIns;
				counter = -1;
				HASH_FIND_INT(cData, &i, s);
				if(s) /*If customer is already in database, ignore and read next customer in input file*/
				{
					break;
				}else{
					s = (struct myStruct*) malloc(sizeof(struct myStruct));
					s->id = i;
					s->info = temp;
					HASH_ADD_INT(cData, id , s); /*Add customer to database hashtable using customer ID as key*/
				}
			}
			counter++;
			token = strtok(NULL, "|");		
		}/*end of inner while*/	
	}/*end of outer while*/
}

/*producer thread function reads in book order input file and enqueues each order in the appropriate Queue
 * where the corresponding consumer thread can process it*/ 
void* producer(void* file)
{

	FILE* f = fopen(file, "r");
	struct my_struct* s;
	struct my_struct* tmp;
	if(f == NULL)
	{
		printf("Book order input file does not exist\n");
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

	/*reads the book order input file line by line until EOF*/
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

		/*Reads single book order assuming given format*/
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
			if(counter == 3) /*a full book order has been read*/
			{
				len = strlen(token) + 1;
				cate = malloc(len * sizeof(char));
				strcpy(cate, token);
				temp->category = cate;
				counter = 0;
				struct my_struct *p;
				HASH_FIND(hh, cat, cate, strlen(cate), s);
				if(!s) /*if category is not in hashtable, ignore order and continue to next*/
				{
					free(cate);
					counter = 0;
					printf("Invalid category in book orders input file. Order ignored\n");
					break;
				}else{/*if category in the hashtable, add to queue*/
					pthread_mutex_lock(&s->q->mut); /*lock mutex from use by consumer while adding order*/
					/*If Queue for category already has 10 orders, wait for consumer to process an order*/
					if(s->q->numOrders == 10)
					{
						pthread_cond_signal(&s->q->nonEmpty); /*shout to consumer that there are orders to be processed*/
						printf("***Producer waits because queue is full***\n");
						pthread_cond_wait(&s->q->nonFull,&s->q->mut); /*wait until there is space for new order*/
						printf("***Producer resumes because queue has space available***\n");
					}
			
					QueuePtr list = s->q;
					list = enqueue(list, temp); /*add new order to end of queue*/
					s->q = list;
					s->q->numOrders++;
					pthread_mutex_unlock(&s->q->mut); /*allow consumer access to queue again*/
					pthread_cond_signal(&s->q->nonEmpty); /*shout to consumer that there are orders to be processed*/
					break;
				}
			}
			counter++;
			token = strtok(NULL, "|");	
		}/*end of inner while*/
		
	}/*end of outer while*/
	
	/*Once all orders have been processed, add a poison order to each Queue in the hashtable*/
	HASH_ITER(hh, cat, s, tmp){
		pthread_mutex_lock(&s->q->mut);
		temp = (qNodePtr)malloc(sizeof(qNode));
		temp->noMoreOrders = 1;
		temp->category = s->category;
		s->q = enqueue(s->q, temp);
		s->q->numOrders++;
	        pthread_cond_signal(&s->q->nonEmpty);
		pthread_mutex_unlock(&s->q->mut);
	}

	fclose(f); /*close file and exit producer function*/
	return 0;		
}/*end of producer function*/ 

/*consumer thread function takes Queue struct as argument and processes its orders until it reads a poison order.
 * There is one consumer thread per category in category input file*/
void *consumer(void *param)
{
	struct Queue *queue = (struct Queue *)param; /*shared memory with producer is held in a Queue struct*/

	while (1) 
	{  
		pthread_mutex_lock(&queue->mut); 
		/*wait for producer to process orders if there are no orders to process in the queue*/
		if(queue->numOrders == 0)
		{
			pthread_cond_signal(&queue->nonFull);
			printf("***Consumer waits if the queue is empty***\n");
			pthread_cond_wait(&queue->nonEmpty, &queue->mut);
			printf("***Consumer resumes because queue has orders ready for processing***\n");
		}
		
		qNodePtr order = dequeue(queue);
		/*check if order is poison order and exit consumer loop if so*/
		if(order->noMoreOrders == 1){
			queue->numOrders--;
			break;
		}
		queue->numOrders--;
		int cust = order->customerID;
		struct myStruct *s;
		HASH_FIND_INT(cData, &cust, s);
		if (!s){
			printf("Invalid user ID in book order input file. Order ignored\n");
			pthread_mutex_unlock(&queue->mut);
			continue;
		}
		iPtr customer = s->info;
		double bookprice = order->price;
		double credit = customer->creditLimit;
		if (credit >= bookprice) /*if customer has enough funds, process/print successful order*/
		{   
			customer->creditLimit -= bookprice;
			revenue += bookprice;
			printf("Order Confirmation:\nBook Title: %s\nPrice: %g\nCustomer Name: %s\n\tAddress: %s\n\tState: %s\n\tZip Code: %s\n", order->bookTitle, order->price, customer->name, customer->address, customer->state, customer->zip);    
			qNodePtr succ = (qNodePtr)malloc(sizeof(qNode));
			succ->price = order->price;
			succ->bookTitle = order->bookTitle;
			succ->credRemain = customer->creditLimit;
			customer->successes = enqueue(customer->successes, succ); 
		} else {/*if customer does not have enough funds, process/print rejected order*/
			printf("Order Rejection\n\tCustomer Name: %s\n\tBook Title: %s\n\tBook Price: %g\n\tCustomer's Remaining Credit Limit: %g\n", customer->name, order->bookTitle, order->price, customer->creditLimit);
			qNodePtr fail = (qNodePtr)malloc(sizeof(qNode));
			fail->price = order->price;
			fail->bookTitle = order->bookTitle;
			customer->failures = enqueue(customer->failures, fail);	
		} 
		pthread_mutex_unlock(&queue->mut); /*unlock mutex after an order is processed*/
		pthread_cond_signal(&queue->nonFull); /*shout to consumer that there is room in the Queue for more orders*/
	}

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

	struct my_struct *s, *tmp;
	createDatabase(database);
	createQueues(categories);	
	pthread_t producerReturn;
	pthread_t consumerReturn[numConsumers]; /*array for consumer threads big enough to hold one for each category*/
	int i = 0;

	/*initialize mutex and condition variables for each category Queue*/
	HASH_ITER(hh, cat, s, tmp)
        {   
                pthread_mutex_init(&s->q->mut, NULL);
                pthread_cond_init(&s->q->nonEmpty, NULL);
                pthread_cond_init(&s->q->nonFull, NULL);
                s->q->numOrders = 0;
        }   

	pthread_create(&producerReturn, NULL, producer, order); /*create one producer thread with book order file as argument*/

	/*create one consumer thread for each category with corresponding Queue struct as argument*/
	HASH_ITER(hh, cat, s, tmp)
	{
		pthread_create(&consumerReturn[i], NULL, consumer, s->q);
		i++;
	}
	
	/*Join all finished threads with main*/
	pthread_join(producerReturn, NULL);
	for(i = 0; i < numConsumers; i++)
	{
		pthread_join(consumerReturn[i], NULL);
	}

	printf("\nAll orders have been processed. The following is the final report:\n");
	
	/*destroy all mutexes and condition variables*/
	HASH_ITER(hh, cat, s, tmp)
	{
		pthread_mutex_destroy(&s->q->mut);
		pthread_cond_destroy(&s->q->nonFull);
		pthread_cond_destroy(&s->q->nonEmpty);
	}

	struct myStruct *y;
	struct myStruct *m;
	qNodePtr curr;
	/*Print out final report for each customer using in formation in global cData hashtable*/
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

	printf("\nTotal Revenue: $%g\n", revenue); /*print revenue before exiting program*/
	return 0;
}
