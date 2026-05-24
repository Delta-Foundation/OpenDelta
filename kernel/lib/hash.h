#ifndef HASH_H
#define HASH_H

#define HASHSIZE 101

struct nlist {
    struct nlist *next;
    char *name;
    char *defn;
};

extern unsigned hash(char *s);
extern struct nlist *lookup(char *s);
extern struct nlist *install(char *name, char *defn);

#endif
