#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
		pid_t id;
		int status;

/* parent */	id = fork();
/* both */	switch(id) {
/* parent */	case -1: fprintf(stderr, "fork() failed\n");
			 return EXIT_FAILURE;
/* child */	case 0: execvp(argv[1], argv+1);
			fprintf(stderr, "Child: execvp() of %s failed\n", argv[1]);
			return EXIT_FAILURE;
/* parent */	default:
			 wait(&status);
			 printf("Parent: child exit status is %d\n", status);
			 return EXIT_SUCCESS;
		}
}
