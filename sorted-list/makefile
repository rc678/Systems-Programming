all: sl

sorted-list.o: sorted-list.c sorted-list.h
	gcc -c sorted-list.c
main.o: main.c sorted-list.h
	gcc -c main.c
libsl.a: sorted-list.o
	ar -cvq libsl.a sorted-list.o
sl: main.o libsl.a
	gcc -o sl main.o libsl.a
clean:
	rm -f sl
	rm -f *.o
	rm -rf *.o libsl.a
