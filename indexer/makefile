all: index

tokenizer.o: tokenizer.c
	gcc -c tokenizer.c
sorted-list.o: sorted-list.c
	gcc -c sorted-list.c
indexer.o: indexer.c
	gcc -c indexer.c
index: indexer.o
	gcc -o index indexer.o
clean:
	rm -f index
	rm -f *.o
