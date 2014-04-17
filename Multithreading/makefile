all: order

order.o: order.c uthash.h
	gcc -c -lpthread order.c
order: order.o
	gcc -o order order.o
clean:
	rm -f order
	rm -f *.o
