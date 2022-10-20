#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#define UNUSED __attribute__((unused))

int main(UNUSED int argc, UNUSED char *argv[]) {
	char *args[] = {"echo", "hello", "world!", NULL};

	execvp(args[0], args);
	fprintf(stderr, "execvp() of 'echo hello world! failed\n");
	return EXIT_FAILURE;
}
