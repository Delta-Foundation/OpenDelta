#include "../kernel-lib/screen.h"
#include "../kernel-lib/system.h"
#include "../libc/include/stdbase.h" 

tree_t * tree_create(void) {
    tree_t * out = kmalloc(sizeof(tree_t), 0, (unsigned int *)&out);
    out->nodes = 0;
    out->root = NULL;
    return out;
}

tree_node_t * tree_node_create(void * value) {
    tree_node_t * out = kmalloc(sizeof(tree_node_t), 0, (unsigned int *)&out);
    out->value = value;
    out->children = list_create();
    out->parent = NULL;
    out->word = NULL;
    out->count = 0;
    out->left = NULL;
    out->right = NULL;
    return out;
}

void tree_set_root(tree_t * tree, void * value) {
    tree_node_t * root = tree_node_create(value);
    tree->root = root;
    tree->nodes = 1;
}

void shm_install(void) {
    tree_t * shm_tree = NULL;
    
    prints("[info]: installing shared memory layer...\n", WHITE);
    shm_tree = tree_create();
    tree_set_root(shm_tree, NULL);
}

