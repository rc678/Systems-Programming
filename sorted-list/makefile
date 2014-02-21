COMPILER = gcc
all: main

main: main.o sorted-list.o 
	gcc -o main main.o sorted-list.o
main.o: main.c
	gcc -c main.c
sorted-list.o:
	gcc -c sorted-list.c
	ar -cvq libsl.a sorted-list.o
	gcc -o sl -lsl main.c 
clean:
	rm -rf *.o libsl.a
