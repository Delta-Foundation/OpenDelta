#include "../lib/screen.h"
#include "../lib/system.h"
#include "../lib/stdbase.h" 

tree_t *treeCreate(void) {
    tree_t *out = kmalloc(sizeof(tree_t), 0, (unsigned int *)&out);
    out->nodes = 0;
    out->root = NULL;
    return out;
}

tree_node_t *treeNodeCreate(void *value) {
    tree_node_t *out = kmalloc(sizeof(tree_node_t), 0, (unsigned int *)&out);
    out->value = value;
    out->children = listCreate();
    out->parent = NULL;
    out->word = NULL;
    out->count = 0;
    out->left = NULL;
    out->right = NULL;
    return out;
}

void treeSetRoot(tree_t *tree, void *value) {
    tree_node_t *root = treeNodeCreate(value);
    tree->root = root;
    tree->nodes = 1;
}

void shmInstall(void) {
    tree_t *shmTree = NULL;
    
    prints("[info]: installing shared memory layer...\n", WHITE);
    shmTree = treeCreate();
    treeSetRoot(shmTree, NULL);
}
