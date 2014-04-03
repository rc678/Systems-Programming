all: index

tokenizer.o: tokenizer.c uthash.h
	gcc -c tokenizer.c
indexer.o: indexer.c uthash.h
	gcc -c indexer.c
index: indexer.o
	gcc -o index indexer.o
clean:
	rm -f index
	rm -f *.o
