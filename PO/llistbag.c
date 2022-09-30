/*
 * implementation of a llistbag
 */
#include "llistbag.h" /* the .h file NOT in /usr/local/include/ADTs */
#include <stdlib.h>

/*
 * needed data structures
 */

static void b_destroy(const Bag *b) {
}

static void b_clear(const Bag *b) {
}

static bool b_add(const Bag *b, void *member) {
    return false;
}

static bool b_contains(const Bag *b, void *member) {
    return false;
}

static bool b_isEmpty(const Bag *b) {
    return true;
}

static bool b_remove(const Bag *b, void *member) {
    return false;
}

static long b_size(const Bag *b) {
    return 0L;
}

static long b_count(const Bag *b, void *member) {
    return 0L;
}

static void **b_toArray(const Bag *b, long *len) {
    return NULL;
}

const Iterator *b_itCreate(const Bag *b) {
    return NULL;
}

static const Bag *b_create(const Bag *b);

static Bag template = {
    NULL, b_create, b_destroy, b_clear, b_add, b_contains, b_count, b_remove,
    b_isEmpty, b_size, b_toArray, b_itCreate
};

static const Bag *b_create(const Bag *b) {
    return NULL;
}

const Bag *LListBag(int (*cmpF)(void*,void*), void (*freeV)(void*)) {
    return NULL;
}
