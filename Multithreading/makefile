all: order

order: uthash.h queue.h order.c
	gcc -lpthread -o order order.c
clean: 
	rm -f order
	rm -f *.o
