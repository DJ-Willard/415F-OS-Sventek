#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>
#define UNUSED __attribute__((unused))

/*p2 use p pthread make device diver no heap statically allocated memery.
kernal no heap. 
 * data type on free list
 last in fist out list
 */
typedef struct ticket {
	struct ticket *next;
	int value;
} Ticket;

/*
 * data type for arguments to threads
 */
typedef struct args {
	int id;
	bool ifBlocking;
} Args;

/*
 * Ticket storage and freelist
 */
#define NTICKETS 10
//need a mutex to prevetn threads form pulling and outing tickets
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
Ticket tickets[NTICKETS];
// pointer to all ticket not allocated
// lock protects free list
Ticket *freeList = NULL;

/*
 * global data used by threads to shut down
 used by sig handler and the threads have referance
 */
volatile bool shutdown = false;

/*
 * this function initializes the value member of each Ticket, and threads each
 * ticket onto the freelist
last in first out queue. 
 */
void initializeFreeList(void) {
	int i;

	for (i = 0; i < NTICKETS; i++) {
		tickets[i].value = i;
		tickets[i].next = freeList;
		freeList = tickets + i;
	}
}

/*
 * nonblocking function to get ticket from free list
 * returns pointer to ticket, if successful; NULL otherwise
 nonblocking means you get a tickets or it doesnt. no block
 blocking causes the thread to wait for ticket.
 get ticket or get null
 */
Ticket *getTicket(void) {
	Ticket *answer;
	pthread_mutex_lock(&lock);
	answer = freeList;
	if (answer != NULL)
		freeList = answer->next;
	pthread_mutex_unlock(&lock);
	return answer;
}

/*
 * put ticket back onto free list
 put ticket on free list
 
 */
void putTicket(Ticket *ticket) {
	pthread_mutex_lock(&lock);
	ticket->next = freeList;
	freeList = ticket;
	pthread_mutex_unlock(&lock);
}

/*
 * function that runs in each thread
 *
 * while shutdown is false
 *     computes a random delay in the range of [100, 1000] ms
 *     obtains a ticket
 *     sleeps for the delay
 *     returns ticket (if successfully obtained)
 */
void *th_fxn(void *args) {
	Args *arg = (Args *)args;
	while(! shutdown) {
		long delay = 100000 + 20000 * (random() % 50);
		Ticket *ticket = getTicket();
		if (ticket == NULL)
			fprintf(stderr, "%d: No free tickets\n", arg->id);
		else
			fprintf(stderr, "%d: Obtained ticket %d\n", arg->id, ticket->value);
		usleep(delay);
		if (ticket != NULL) {
			fprintf(stderr, "%d: Returned ticket %d\n", arg->id, ticket->value);
			putTicket(ticket);
		}
	}
	return NULL;
}

/*
 * SIGINT handler
 cause other threads to shut down.
 */
void onint(UNUSED int sig) {
	shutdown = true;
}

int main(int argc, char *argv[]) {
	int i, nthreads = 0;
	pthread_t threads[50];
	Args myargs[50];

	/* obtain # of threads to create */
	if (argc != 2) {
		fprintf(stderr, "usage: %s number_of_threads\n", argv[0]);
		return EXIT_FAILURE;
	}
	nthreads = atoi(argv[1]);
	if (nthreads == 0) {
		fprintf(stderr, "usage: %s number_of_threads\n", argv[0]);
		return EXIT_FAILURE;
	}
	initializeFreeList();
	/* establish SIGINT handler */
	signal(SIGINT, onint);
	/* create the threads */
	for (i = 0; i < nthreads; i++) {
		myargs[i].id = i;
		pthread_create(threads+i, NULL, th_fxn, (void *)(myargs+i));
	}
	/* wait for all of them to terminate */
	for (i = 0; i < nthreads; i++)
		pthread_join(threads[i], NULL);
	return EXIT_SUCCESS;
}
