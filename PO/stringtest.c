#include "hashbag.h"
#include "llistbag.h"
#include "sort.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int ncmp(void *x1, void *x2) {
    return ((long)x1 - (long) x2);
}

int scmp(void *x1, void *x2) {
    return strcmp((char *)x1, (char *)x2);
}

long nhash(void *x, long N) {
    return ((long)x) % N;
}

#define A 31L
long shash(void *x, long N) {
    int i;
    long sum = 0L;
    char *s = (char *)x;

    for (i = 0; s[i] != '\0'; i++)
        sum = A * sum + (long)s[i];
    return sum % N;
}

int findLong(long v, long array[], long size) {
    long i;
    for (i = 0L; i < size; i++)
        if (array[i] == v)
            return 1;
    return 0;
}

int findStr(char *v, char *array[], long size) {
    long i;
    for (i = 0L; i < size; i++)
        if (strcmp(v, array[i]) == 0)
            return 1;
    return 0;
}

const Bag *create(int dohash, void (*freeV)(void*), int (*cmp)(void*, void*),
		  long cap, double lf, long (*hash)(void*, long)) {
    const Bag *b;
    if (dohash)
        b = HashBag(cmp, freeV, cap, lf, hash);
    else
        b = LListBag(cmp, freeV);
    return b;
}

#define HASH 1
#define LLIST 2
#define USAGE "usage: %s -h|-l test# . . .\n"

int main(int argc, char *argv[]) {
    int i;
    int which = 0;
    int opt, dohash;

    opterr = 0;
    while ((opt = getopt(argc, argv, "hl")) != -1) {
        switch(opt) {
        case 'h': which = HASH; break;
	case 'l': which = LLIST; break;
        default:
            fprintf(stderr, "%s: illegal option, '-%c'\n", argv[0], optopt);
	    fprintf(stderr, USAGE, argv[0]);
	    return EXIT_FAILURE;
	    break;
	}
    }
    if (which == 0) {
        fprintf(stderr, "%s: you must specify either -h or -l\n", argv[0]);
	fprintf(stderr, USAGE, argv[0]);
	return EXIT_FAILURE;
    }
    dohash = (which == HASH);

    for (i = optind; i < argc; i++) {
        int test;
        sscanf(argv[i], "%d", &test);
        switch(test) {
          case 1: {
            printf("Test creation and destruction of a bag ... ");
	    const Bag *b = create(dohash, free, scmp, 0L, 0.0, shash);
            if (b != NULL) {
                printf("success\n");
                b->destroy(b);
            } else
                printf("failure\n");
            break;
          }
          case 2: {
            printf("Test addition of a single value ... ");
	    const Bag *b = create(dohash, free, scmp, 0L, 0.0, shash);
            if (b->add(b, (void *)strdup("42")))
                printf("success\n");
            else
                printf("failure\n");
            b->destroy(b);
            break;
          }
          case 3: {
            printf("Test size method ... ");
	    const Bag *b = create(dohash, free, scmp, 0L, 0.0, shash);
            char *st = "failure\n";
            if (b->size(b) == 0L) {
                (void)b->add(b, (void *)strdup("42"));
                if (b->size(b) == 1L)
                    st = "success\n";
            }
            printf(st);
            b->destroy(b);
            break;
          }
          case 4: {
            printf("Test addition of a duplicate value ... ");
	    const Bag *b = create(dohash, free, scmp, 0L, 0.0, shash);
            (void)b->add(b, (void *)strdup("42"));
            (void)b->add(b, (void *)strdup("42"));
            if (b->size(b) == 2L)
                printf("success\n");
            else
                printf("failure\n");
            b->destroy(b);
            break;
          }
          case 5: {
            printf("Test isEmpty() on an empty bag ... ");
	    const Bag *b = create(dohash, free, scmp, 0L, 0.0, shash);
            if (b->isEmpty(b))
                printf("success\n");
            else
                printf("failure\n");
            b->destroy(b);
            break;
          }
          case 6: {
            printf("Test isEmpty() on a non-empty bag ... ");
	    const Bag *b = create(dohash, free, scmp, 0L, 0.0, shash);
            (void)b->add(b, (void *)strdup("42"));
            if (! b->isEmpty(b))
                printf("success\n");
            else
                printf("failure\n");
            b->destroy(b);
            break;
          }
          case 7: {
            printf("Test contains() on an empty bag ... ");
	    const Bag *b = create(dohash, free, scmp, 0L, 0.0, shash);
            if (! b->contains(b, (void *)"42"))
                printf("success\n");
            else
                printf("failure\n");
            b->destroy(b);
            break;
          }
          case 8: {
            printf("Test contains() on a non-empty bag and value is present ... ");
	    const Bag *b = create(dohash, free, scmp, 0L, 0.0, shash);
            (void)b->add(b, (void *)strdup("42"));
            if (b->contains(b, (void *)"42"))
                printf("success\n");
            else
                printf("failure\n");
            b->destroy(b);
            break;
          }
          case 9: {
            printf("Test contains() on a non-empty bag and value is not present ... ");
	    const Bag *b = create(dohash, free, scmp, 0L, 0.0, shash);
            (void)b->add(b, (void *)strdup("42"));
            if (! b->contains(b, (void *)"99"))
                printf("success\n");
            else
                printf("failure\n");
            b->destroy(b);
            break;
          }
          case 10: {
            printf("Test remove() on an empty bag ... ");
	    const Bag *b = create(dohash, free, scmp, 0L, 0.0, shash);
            if (! b->remove(b, (void *)"99"))
                printf("success\n");
            else
                printf("failure\n");
            b->destroy(b);
            break;
          }
          case 11: {
            printf("Test remove() on a non-empty bag and value is present ... ");
	    const Bag *b = create(dohash, free, scmp, 0L, 0.0, shash);
            (void)b->add(b, (void *)strdup("42"));
            if (b->remove(b, (void *)"42"))
                printf("success\n");
            else
                printf("failure\n");
            b->destroy(b);
            break;
          }
          case 12: {
            printf("Test remove() on a non-empty bag and value is not present ... ");
	    const Bag *b = create(dohash, free, scmp, 0L, 0.0, shash);
            (void)b->add(b, (void *)strdup("42"));
            if (! b->remove(b, (void *)"99"))
                printf("success\n");
            else
                printf("failure\n");
            b->destroy(b);
            break;
          }
          case 13: {
            printf("Test addition of multiple, unique values ... ");
	    const Bag *b = create(dohash, free, scmp, 0L, 0.0, shash);
            char *values[] = {"1", "2", "3", "4", "5"};
            int i, success = 1;
            for (i = 0; i < 5; i++) {
                if (! b->add(b, (void *)strdup(values[i]))) {
                    success = 0;
                    break;
                }
            }
            if (success && b->size(b) == 5L)
                printf("success\n");
            else
                printf("failure\n");
            b->destroy(b);
            break;
          }
          case 14: {
            printf("Test addition of multiple, identical values ... ");
	    const Bag *b = create(dohash, free, scmp, 0L, 0.0, shash);
            char *values[] = {"1", "1", "2", "3", "3", "3", "4", "5", "5", "5", "5"};
            int i, success = 1;
            for (i = 0; i < 11; i++) {
                if (! b->add(b, (void *)strdup(values[i]))) {
                    success = 0;
                    break;
                }
            }
            if (success && b->size(b) == 11L)
                printf("success\n");
            else
                printf("failure\n");
            b->destroy(b);
            break;
          }
          case 15: {
            printf("Test of count() method ... ");
	    const Bag *b = create(dohash, free, scmp, 0L, 0.0, shash);
            char *values[] = {"1", "1", "2", "3", "3", "3", "4", "5", "5", "5", "5"};
            int i, success = 1;
            for (i = 0; i < 11; i++) {
                if (! b->add(b, (void *)strdup(values[i]))) {
                    success = 0;
                    break;
                }
            }
            if (success && b->size(b) == 11L) {
                if (b->count(b, (void *)"8") == 0L &&
                    b->count(b, (void *)"2") == 1L &&
                    b->count(b, (void *)"5") == 4L)
                    printf("success\n");
                else
                    printf("failure\n");
            } else
                printf("failure\n");
            b->destroy(b);
            break;
          }
          case 16: {
            printf("Test toArray() ... ");
	    const Bag *b = create(dohash, free, scmp, 0L, 0.0, shash);
            char *values[] = {"1", "1", "2", "3", "3", "3", "4", "5", "5", "5", "5"};
            long len;
            int i, success = 0;
            char **array;
            for (i = 0; i < 11; i++)
                (void)b->add(b, (void *)strdup(values[i]));
            array = (char **)b->toArray(b, &len);
            if (array != NULL && len == 11L) {
                for (i = 0; i < 11; i++) {
                    if (findStr(array[i], values, 11L))
                        success++;
                }
             }
             if (success)
                printf("success\n");
            else
                printf("failure\n");
            free(array);
            b->destroy(b);
            break;
          }
          case 17: {
            printf("Test itCreate() ... ");
	    const Bag *b = create(dohash, free, scmp, 0L, 0.0, shash);
            char *values[] = {"1", "1", "2", "3", "3", "3", "4", "5", "5", "5", "5"};
            char *v;
            int i, success = 0;
            const Iterator *it;
            for (i = 0; i < 11; i++)
                (void)b->add(b, (void *)strdup(values[i]));
            it = b->itCreate(b);
            if (it != NULL) {
                i = 0;
                while (it->hasNext(it)) {
                    (void)it->next(it, (void **)&v);
                    if (findStr(v, values, 11L))
                        success++;
                    i++;
                }
            }
            if (it != NULL && i == 11 && success)
                printf("success\n");
            else
                printf("failure\n");
            if (it != NULL)
                it->destroy(it);
            b->destroy(b);
            break;
          }
          case 18: {
            printf("Test sorting of elements ");
	    const Bag *b = create(dohash, free, scmp, 0L, 0.0, shash);
            char *values[] = {"1", "1", "2", "3", "3", "3", "4", "5", "5", "5", "5"};
            long len;
            long i;
            char **array;
            for (i = 0; i < 11; i++)
                (void)b->add(b, (void *)strdup(values[i]));
            array = (char **)b->toArray(b, &len);
            sort((void **)array, len, scmp);
            for (i = 0; i < len; i++)
                printf("%s ", array[i]);
            printf("... success\n");
            free(array);
            b->destroy(b);
            break;
          }
          default: {
            printf("Undefined test\n");
            break;
          }
       }
    }
}
