#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {

	execvp(argv[1], argv+1);
	fprintf(stderr, "execvp() of %s failed\n", argv[1]);
	return EXIT_FAILURE;
}
