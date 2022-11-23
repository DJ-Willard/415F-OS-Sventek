/* Master-worker pattern: m makes workers and waits till complete
 * they share data
 * main init all data; all data must be made before workers
 * 
 * producer/consumer program using BoundedBuffer class from project 2
 *
 * usage: ./prodcons [-b] [-n <buffer_size>] [-d <msecs>]
 *
 * main:
 *   create bounded buffer holding <buffer_size> lines; default size is 10
 *   create consumer thread
 *   create producer thread
 *   wait for producer and consumer threads to terminate
 *
 * producer thread:
 *   for each line from stdin
 *       duplicate line on heap
 *       delay thread by `msecs'; default is 10 ms
 *       if -b specified
 *           call blockingPut of duplicate to bounded buffer
 *       else
 *           call nonBlockingPut of duplicate to bounded buffer
 *           if failed
 *               free duplicate
 *   prodDone = true
 *   return NULL
 *
 * consumer thread:
 *   while not prodDone
 *       delay thread by 2 * `msecs'; default is 20 ms
 *       blockingGet of line from bounded buffer
 *       print line on stdout
 *       free line
 *   while nonblockingGet of line is true
 *       print line on stdout
 *       free line
 *   return NULL
 *
 * expectations:
 *     if -b is specified, all lines read from stdin will be written to stdout
 *     if -b is not specified, appoximately half of the lines read from stdin
 *         will not be buffered, since the consumer is delaying by twice as
 *         much time as the producer between each interaction with the
 *         bounded buffer
 */
#include "BoundedBuffer.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#define UNUSED __attribute__((unused))

long delay = 10 * 1000L;  /* contains the number of micro seconds for delay */
bool isBlocking = false;	/* if -b was specified */
BoundedBuffer *bb = NULL;	/* buffer used by producer and consumer */
bool prodDone = false;	  /* set to true when producer has finished */

void *producer(UNUSED void *args) {
    char buf[BUFSIZ];

    while (fgets(buf, sizeof buf, stdin) != NULL) {
        char *s = strdup(buf);
	usleep(delay); // millisecs by usleep instead of nano seconds.  in pthread documentation in project.
	if (isBlocking)
            bb->blockingWrite(bb, (void *)s);
	else if (! bb->nonblockingWrite(bb, (void *)s))
            free(s);
    }
    prodDone = true;
    return NULL;
}

void *consumer(UNUSED void *args) {
    char *s;

    while (! prodDone) {
        usleep(2 * delay);
        bb->blockingRead(bb, (void **)&s);
	fputs(s, stdout);
	free(s);
    }
    while (bb->nonblockingRead(bb, (void **)&s)) {
        fputs(s, stdout);
	free(s);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    int opt;
    int size = 10;	/* size of bounded buffer, default is 10 */
    pthread_t prod_id, cons_id;

    opterr = 0;
    while ((opt = getopt(argc, argv, "bn:d:")) != -1) {
        switch(opt) {
            case 'b': isBlocking = true; break;
	    case 'n': size = atoi(optarg); break;
            case 'd': delay = 1000 * atoi(optarg); break;
            default: fprintf(stderr, "Illegal option: -%c\nusage: %s [-b] [-n <buffer_size>] [-d <msecs_delay>]\n", optopt, argv[0]); return EXIT_FAILURE;
	}
    }
    bb = BoundedBuffer_create(size);
    pthread_create(&cons_id, NULL, consumer, NULL);
    pthread_create(&prod_id, NULL, producer, NULL);
    pthread_join(prod_id, NULL);
    pthread_join(cons_id, NULL);
    return EXIT_SUCCESS;
}
