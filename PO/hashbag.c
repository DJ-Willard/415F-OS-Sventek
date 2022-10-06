/*
 * implementation of a hashbag
 */
#include "hashbag.h" /* the .h file NOT in /usr/local/include/ADTs */
#include <stdlib.h>

/*
 * needed data structures
 */

// reference to memory entries 
typedef struct mentry
{
    void *key;
    void *value;
} Mentry;

// struct for the link list aspect of the hash map and cnt tracker
typedef struct node 
{
    stuct node *next;
    int cnt; //multipicity tracker
    Mentry entry;
} Node;

// struct of each instance
typdedef struct b_data
{
    //bag data goes in (used data in the methods)
    long (*hash)(void *, long);
    int(*cmpF)(void *, void *);
    long size;
    long capacity;
    //long changes;
    //double load;
    double loadFactor;
    Node **buckets;
    void (*freeK)(void *k)
    void (*freeV)(void *v)
}BData;

//Helper Fuction
static void purge (Bdata *bd)
{
    long i;

    for(i=0L; i<bd->capacity; i++)
    {
        Node *p;
        Node *q;

        p=bd->buckets[i];
        while(p !=NULL)
        {
            bd->freeK((p->entry).key);
            bd->freeV((p->entry).value);
            q=p->next;
            free(p);
            p=q;
        }
        bd->buckets[i]=NULL;
    }
}

//todo//
static void b_destroy(const Bag *b) 
{
    BData *bd = (BData *)b->self;
    purge(bd);
    free(bd->buckets);
    free(bd);
    free((void *)b);
}
//todo//
static void b_clear(const Bag *b) 
{
    BData *bd = (BData *)b->self;
    purge(bd);
    bd->size = 0;
    bd->load = 0.0;
    bd->changes = 0;
}
//todo
static bool b_add(const Bag *b, void *member)
{
    return false;
}
//todo
static bool b_contains(const Bag *b, void *member) 
{
    return false;
}
//todo
static bool b_isEmpty(const Bag *b) 
{
    BData *bd = (BData *md)b->self;
    return (bd->size == 0L);
}
//todo
static bool b_remove(const Bag *b, void *member) 
{
    BData *bd = (BData *)b->self;
    long i;
    Node *entry = findKey(bd, key, &i)
    int status = (entry != NULL);

    if(status)
    {
        Node *p, *c;
        for(p=NULL, c = bd->buckets[i]; c!=entry; p=c, c=c->next)
            ;
        if(p==NULL)
            bd->buckets[i] = entry->next;
        else
            p->next = entry->next;
        bd->size--;
        bd->load -= bd->increment;
        bd->changes++;
        bd->freeK((entry->entry).key);
        bd->freeV((entry->entry).value);
        free(entry);
        }
    return status;
}
//todo
static long b_size(const Bag *b) 
{
    BData *bd = (BData *)b->self;
    return (bd->size == 0L);
}
//todo
static long b_count(const Bag *b, void *member) 
{
    return 0L;
}
//todo
static void **b_toArray(const Bag *b, long *len) 
{
    return NULL;
}
//todo
static const Iterator *b_itCreate(const Bag *b) 
{
    BData *bd = (BData *)b->self;
    const Iterator *it = NULL;
    void **tmp = (void **)entries(bd);

    if(tmp != NULL)
    {
        it = Iterator_create(bd->size, tmp);
        if(it == NULL)
            free(tmp);
    }
    return it;
}

static const Bag *b_create(const Bag *b);

static Bag template = {
    NULL, b_create, b_destroy, b_clear, b_add, b_contains, b_count, b_remove,
    b_isEmpty, b_size, b_toArray, b_itCreate
};

// helper fuction to create a new bag dispatch table //

//todo
static const Bag *b_create(const Bag *b) 
{
    Bdata *bd = (Bdata *)m->self;

    return newbag();
}

const Bag *HashBag(int (*cmpF)(void*,void*), void (*freeV)(void*),
                   long capacity, double loadFactor, long (*hash)(void*, long)){
    return NULL;
}
