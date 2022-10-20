/*
 * timeit - creates a gettimeofday sandwich around a routine that copies
 *          stdin to stdout; displays the elapsed time as a number of
 *          milliseconds
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#define UNUSED __attribute__((unused))

void fcopy(FILE *in, FILE *out) {
	char buf[BUFSIZ];
	while (fgets(buf, sizeof buf, in) != NULL)
		fputs(buf, out);
}

int main(UNUSED int argc, UNUSED char *argv[]) {
	struct timeval t1, t2;
	long long musecs;

	gettimeofday(&t1, NULL);
	fcopy(stdin, stdout);
	gettimeofday(&t2, NULL);
	musecs = 1000000 * (t2.tv_sec - t1.tv_sec) +(t2.tv_usec - t1.tv_usec);
	fprintf(stderr, "Elapsed time:  %Ld.%03d ms\n", musecs/1000, (int)(musecs%1000));  //LD is long long format ld is long
	return EXIT_SUCCESS;

}
