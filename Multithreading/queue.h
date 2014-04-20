/*definition of a node for the queues*/
typedef struct qNode{
	double price;
	double credRemain;
	int customerID;
	int noMoreOrders;
	char* bookTitle;
	const char* category;
	struct qNode *next;
}qNode, *qNodePtr;

/*struct that stores the queue as a linked list*/
typedef struct Queue{
	qNodePtr head;
	qNodePtr tail;
	int numOrders;
	pthread_mutex_t mut;
	pthread_cond_t nonEmpty;
	pthread_cond_t nonFull;
}Queue, *QueuePtr;

/*adds item to the front of the queue and returns the queue*/
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

/*returns the item from the front of the queue*/
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
