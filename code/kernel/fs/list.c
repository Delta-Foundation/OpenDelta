#include "../lib/stdbase.h"
#include "./headers/list.h"

void listAppend(list_t *list, node_t *node)
{
    node->next = NULL;

    node->owner = list;
    if (!list->length) {
        list->head = node;
        list->tail = node;
        node->prev = NULL;
        node->next = NULL;
        list->length++;
        return;
    }

    list->tail->next = node;
    node->prev = list->tail;
    list->tail = node;
    list->length++;
}

node_t *listInsert(list_t *list, void *item) 
{
    node_t *node = malloc(sizeof(node_t));
    node->value = item;
    node->next = NULL;
    node->prev = NULL;
    node->owner = NULL;
    listAppend(list, node);

    return node;
}

node_t *listFind(list_t *list, void *value) 
{
    foreach (item, list) {
        if (item->value == value) {
            return item;
        }
    }

    return NULL;
}

list_t *listCreate(void) {
    list_t *out = kmalloc(sizeof(list_t), 0, (uint32_t*)&out);
    out->head = NULL;
    out->tail = NULL;
    out->length = 0;
    return out;
}
