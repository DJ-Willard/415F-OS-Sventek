#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#define UNUSED __attribute__((unused))

volatile bool USR1_seen = false;

/*
 * SIGUSR1 handler
 */
void onusr1(UNUSED int sig) {
	USR1_seen = true;
}

int main(UNUSED int argc, UNUSED char *argv[]) {
	struct timespec ms20 = {0, 20000000};

	if (signal(SIGUSR1, onusr1) == SIG_ERR) {
		fprintf(stderr, "Can't establish SIGUSR1 handler\n");
		return EXIT_FAILURE;
	}
	while (! USR1_seen)
		(void)nanosleep(&ms20, NULL);
	printf("SIGUSR1 received\n");
	return EXIT_SUCCESS;
}
