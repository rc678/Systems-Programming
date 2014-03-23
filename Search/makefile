all: search

tokenizer.o: tokenizer.c uthash.h
	gcc -c tokenizer.c
indexer.o: indexer.c uthash.h
	gcc -c tokenizer.c indexer.c
search.o: search.c uthash.h 
	gcc -c indexer.c search.c
search: search.o
	gcc -o search search.o 
clean:
	rm -f search
	rm -f *.o

