#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#define UNUSED __attribute__((unused))

volatile int USR1_count = 0;
#define DEFAULT_COUNT 1

/*
 * SIGUSR1 handler
 */
void onusr1(UNUSED int sig) {
	USR1_count++;
}

int main(UNUSED int argc, UNUSED char *argv[]) {
	struct timespec ms20 = {0, 20000000};
	int count = DEFAULT_COUNT;

	if (argc > 1)
		count = atoi(argv[1]);
	if (signal(SIGUSR1, onusr1) == SIG_ERR) {
		fprintf(stderr, "Can't establish SIGUSR1 handler\n");
		return EXIT_FAILURE;
	}
	while (USR1_count < count)
		(void)nanosleep(&ms20, NULL);
	printf("SIGUSR1 received %d times\n", count);
	return EXIT_SUCCESS;
}
