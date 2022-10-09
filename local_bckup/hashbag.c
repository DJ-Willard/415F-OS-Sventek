/*
 * implementation of a hashbag
 */
#include "hashbag.h" /* the .h file NOT in /usr/local/include/ADTs */
#include <stdlib.h>

//defineing terms

//#define DEFAULT_CAPACITY 16 // given
#define MAX_CAPACITY 134217728L
//#define DEFAULT_LOAD_FACTOR 0.75  // given
#define TRIGGER 100 // number of changes to trigger a load check for book



/*
 * needed data structures
 */

//Referanced pg 379, 291-324 chatper 12 in SventekCaDS21F

// reference to memory needed for entries 
typedef struct bentry
{
    void *elemt;
} Bentry;

// struct for the link list aspect of the hash map and cnt tracker
typedef struct node 
{
    struct node *next;
    Bentry entry;
} Node;

// struct of each instance
//bag data goes in (used data in the methods)
typedef struct b_data
{
    long (*hash)(void *, long);
    int (*cmpF)(void *, void *);
    long size;
    long capacity;
    long changes; //measure changes before load check
    double load;
    double loadFactor;
    double increment;
    Node **buckets;
    void (*freeV)(void *e);  // given fuction void point to elemt.
}BData;

//Helper Fuction//  purge values in hash map
// calls freeV on each entry then frees storage of Bentry.
static void purge (BData *bd)
{
    long i;

    for(i=0L; i<bd->capacity; i++)
    {
        Node *p;
        Node *q;

        p=bd->buckets[i];
        while(p !=NULL)
        {
            bd->freeV((p->entry).elemt);
            q=p->next;
            free(p);
            p=q;
        }
        bd->buckets[i]=NULL;
    }
}

//given// method destroy b; for each elemt in the bag the constuctor-specified FreeV fuction
// on elemt, the storage is returned to heap. no reeturn value. 
static void b_destroy(const Bag *b) 
{
    BData *bd = (BData *)b->self;
    purge(bd);
    free(bd->buckets);
    free(bd);
    free((void *)b);
}
//given// clears all elemt form the bag b
// b will then be emtpy no return.
static void b_clear(const Bag *b) 
{
    BData *bd = (BData *)b->self;
    purge(bd);
    bd->size = 0;
    bd->load = 0.0;
    bd->changes = 0;
}
//local fuction// fuction find element form the hash fuction and into the link list
static Node *findElemt(BData *bd, void *member, long *bucket)
{
    long i = bd->hash(member, bd->capacity);
    Node *p;

    *bucket = i;
    for(p = bd->buckets[i]; p != NULL; p = p->next)
    {
        if(bd->cmpF((p->entry).elemt, member) == 0)
        {
            break;
        }
    }
    return p;
}

//helper fuction// resizes hash table if tigger needed in add
static  void resize(BData *bd)
{
    int N;
    Node *p;
    Node *q;
    Node **array;
    long i;
    long j;

    N= 2 * bd->capacity;
    
    if(N > MAX_CAPACITY)
    {
        N = MAX_CAPACITY;
    }
    
    if(N == bd->capacity)
    {
        return;
    }
    
    array = (Node **)malloc(N * sizeof(Node *));

    if(array == NULL)
    {
        return;
    }

    for(j = 0; j < N; j++)
    {
        array[j] = NULL;
    }

    // redistorbite entries into the new set of buckets
    for(i = 0; i < bd->capacity; i++)
    {
        for(p = bd->buckets[i]; p != NULL; p = q)
        {
            q = p->next;
            j = bd->hash((p->entry).elemt,N);
            p->next = array[j];
        }
    }
    free(bd->buckets);
    bd->buckets = array;
    bd->capacity = N;
    bd->load /= 2.0;
    bd->changes = 0;
    bd->increment = 1.0/(double)N;
}

//helper fuction// insert new (elemt, mutli) into table
static bool insertEntry(BData *bd, void *elemt, long i)
{
    Node *p = (Node *)malloc(sizeof(Node));
    bool status = (p != NULL);

    if (status)
    {
        (p->entry).elemt = elemt;
        p->next = bd->buckets[i];
        bd->buckets[i] = p;
        bd->size++;
        bd->load += bd->increment;
        bd->changes ++;
    }
    return status;
}


//given// referance put new pg382
// adds value to bag returns true if succesful.
static bool b_add(const Bag *b, void *member)
{
    BData *bd = (BData *)b->self;
    long i;
    Node *p;
    int status = false;

    if(bd->changes > TRIGGER)
    {
        bd->changes = 0;
        if(bd->load > bd->loadFactor)
        {
            resize(bd);
        }
    }

    p = findElemt(bd, member, &i);
    if(p != NULL)
    {
        bd->freeV((p->entry).elemt);
        //do i need to creat or increment 
        (p->entry).elemt = member;
        status = true;
    }
    else
    {
     status = insertEntry(bd, member, i);   
    }

    return status;
}
//given// reutns 1 if true (value is in  the bag) else 0 if not
static bool b_contains(const Bag *b, void *member) 
{
    BData *bd = (BData *)b->self;
    long bucket;

    return (findElemt(bd, member, &bucket) != NULL);
}
//given// 1 if empty else 0
static bool b_isEmpty(const Bag *b) 
{
    BData *bd = (BData *)b->self;
    return (bd->size == 0L);
}

//given// removes one instance of the valuje form the bag,  uses freeV 
// returns true value was presnt 
static bool b_remove(const Bag *b, void *member) 
{
    BData *bd = (BData *)b->self;
    long i;
    Node *entry = findElemt(bd, member, &i);
    int status = (entry != NULL);

    if(status)
    {
        Node *p, *c;
        // find in the singly link list
        for(p=NULL, c = bd->buckets[i]; c!=entry; p=c, c=c->next)
        {
            ;
        }
        if(p==NULL)
            bd->buckets[i] = entry->next;
        else
            p->next = entry->next;
        bd->size--;
        bd->load -= bd->increment;
        bd->changes++;
        bd->freeV((entry->entry).elemt);
        free(entry);
        }
    return status;
}

//given// returns the number of elements in the bag
static long b_size(const Bag *b) 
{
    BData *bd = (BData *)b->self;
    return bd->size;
}

//given returns the multicity of values in s 
//if not pesent in bag 0L return
static long b_count(const Bag *b, void *member) 
{
    BData *bd = (BData *)b->self;
    long cnt;
    
	return 0L;
}


//helper function for generation an array of BEntry * form a bag
//returns pointer to array or NULL on malloc faliure
static void **elemts(BData *bd)
{
    void **tmp = NULL;
    
    if(bd-> size > 0L)
    {
        size_t nbytes = bd->size *sizeof(void *);
        tmp = (void **)malloc(nbytes);
        
        if(tmp != NULL)
        {
            long i;
            long n = 0L;

            for(i = 0L; i < bd->capacity; i++)
            {
                Node *p = bd->buckets[i];
                while(p != NULL)
                {
                    tmp[n++] = (p->entry).elemt;
                    p = p->next;
                }
            }
        }
    }

    return tmp;
}

//helper fuction// generating an array of BEntry *form the bag
// returns pointer to the arry or null if malloc falis
static Bentry **entries(BData *bd)
{
    Bentry **tmp = NULL;

    if(bd->size > 0L)
    {
        size_t nbytes = bd->size * sizeof(Bentry *);
        tmp = (Bentry **)malloc(nbytes);

        if(tmp != NULL)
        {
            long i;
            long n = 0L;

            for(i = 0L; i < bd->capacity; i++)
            {
                Node *p = bd->buckets[i];

                while(p != NULL)
                {
                    tmp[n++] = &(p->entry);
                    p = p->next;
                }
            }
        }
    }

    return tmp;
}

//given// Returns  the elemt of the bag as an array of void* pointers in arbitrart order
// returns a pointer of the array or null if error
// returns the number of elemt in the array in *len
static void **b_toArray(const Bag *b, long *len) 
{
    BData *bd = (BData *)b->self;
    Bentry **tmp = entries(bd);

    if(tmp != NULL)
    {
        *len = bd->size;
    }

    return tmp;
    
}

//given// creates a generic iterator to b;
// returns pointer to the iterator instance  or Null if failure.
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
//given to create instance of bag
static const Bag *b_create(const Bag *b);

static Bag template = {
    NULL, b_create, b_destroy, b_clear, b_add, b_contains, b_count, b_remove,
    b_isEmpty, b_size, b_toArray, b_itCreate
};

// helper fuction to create a new bag dispatch table // form book 386
static const Bag *newBag(int (*cmpF)(void*,void*), void (*freeV)(void*),
                   long capacity, double loadFactor, long (*hash)(void*, long))
{
    Bag *b= (Bag *)malloc(sizeof(Bag));
    long N;
    double LF; //load factor
    Node  **array;
    long  i;

    if (b != NULL)
    {
        BData *bd = (BData *)malloc(sizeof(BData));

        if(bd != NULL)
        {
            N = ((capacity > 0) ? capacity : DEFAULT_CAPACITY);
            N = (N > MAX_CAPACITY) ? MAX_CAPACITY : N;
            LF = ((loadFactor > .0000001) ? loadFactor : DEFAULT_LOAD_FACTOR);
            array = (Node **)malloc(N * sizeof(Node *));

            if(array != NULL)
            {
                bd->capacity = N;
                bd->size = 0L;
                bd->changes = 0L;
                bd->loadFactor = LF;
                bd->load = 0.0;
                bd->increment = 1.0/(double)N;
                bd->freeV = freeV;
                bd->buckets = array;

                for(i=0; i <N; i++)
                {
                    array[i] = NULL;
                }
                *b= template;
                b->self = bd;
            }
            else
            {
                free(bd);
                free(b);
                b = NULL;
            }
        }
        else
        {
            free(b);
            b = NULL;
        }
    }
    return b;
}

//given
static const Bag *b_create(const Bag *b) 
{
    BData *bd = (BData *)b->self;

    return newBag(bd->cmpF, bd->freeV, bd->capacity, bd->loadFactor, bd->hash);
}
//given
const Bag *HashBag(int (*cmpF)(void*,void*), void (*freeV)(void*),
                   long capacity, double loadFactor, long (*hash)(void*, long)){
    return newBag(cmpF, freeV, capacity, loadFactor, hash);
}
