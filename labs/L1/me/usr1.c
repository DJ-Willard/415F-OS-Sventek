#include <stdlib.h>
#include "p1fxns.h"
#include <signal.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>

#define UNUSED __attribute__((unused))

volatile bool USR1_seen = false; // complier will sometimes put in register V make  it to pull form memory

void onusr1(UNSED int sig) // singal handler
{
	USR1_seen = true;
}

int main(UNUSED int argc, UNUSED char *argv[])
{
	struct timespec m20 = {0, 2000000};
	
	if (signal(SIGUSR1, onusr1) == SIG_ERR)
		return EXIT_FAILURE;
	while(! USR1_seen)
		nanosleep(&ms20, NULL);
	printf("USER1 recieved")
	return EXIT_SUCCESS
}