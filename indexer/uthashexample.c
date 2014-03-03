#include <string.h>  /* strcpy */
#include <stdlib.h>  /* malloc */
#include <stdio.h>   /* printf */
#include "uthash.h"

typedef struct node{
	int data;
	struct node* next; 
}node, *nodeptr;

struct my_struct {
    char name[10];             /* key (string is WITHIN the structure) */
    nodeptr list;
    UT_hash_handle hh;         /* makes this structure hashable */
};


int main(int argc, char *argv[]) {
    const char **n, *names[] = { "joe", "bob", "betty", NULL };
    struct my_struct *s, *tmp, *users = NULL;
    int i=100;

    for (n = names; *n != NULL; n++) {
        s = (struct my_struct*)malloc(sizeof(struct my_struct));
        strncpy(s->name, *n,10);
	nodeptr temp = (nodeptr)malloc(sizeof(node));
	temp->data = i++;
	temp->next = NULL; 
        s->list = temp;
	HASH_ADD_STR( users, name, s );
    }

    HASH_FIND_STR( users, "bob", s);
    if (s) printf("bob's data in list is %d\n", s->list->data);

    /* free the hash table contents */
    HASH_ITER(hh, users, s, tmp) {
      HASH_DEL(users, s);
      free(s);
    }
    return 0;
}
