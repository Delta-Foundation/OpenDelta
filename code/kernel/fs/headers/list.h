#ifndef _DLTFS_LIST_H
#define _DLTFS_LIST_H

#include "./fs.h"

typedef struct node {
	struct node * next;
	struct node * prev;
	void * value;
	void * owner;
} __attribute__((packed)) node_t;

typedef struct {
	node_t * head;
	node_t * tail;
	size_t length;
} __attribute__((packed)) list_t;

#define foreach(item, list)	for(node_t *(item) = (node_t*)(list); (item); (item) = (item)->next)

void listAppend(list_t *list, node_t *item);
node_t *listInsert(list_t *list, void *item);
list_t *listCreate(void);
node_t *listFind(list_t *list, void *value);

#endif
