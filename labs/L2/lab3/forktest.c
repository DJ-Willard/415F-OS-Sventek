#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#define UNUSED __attribute__((unused))

int main(UNUSED int argc, UNUSED char *argv[]) {
		pid_t id;
		int status;

/* parent */	id = fork();
/* both */	switch(id) {
/* parent */	case -1: fprintf(stderr, "fork() failed\n");
			 return EXIT_FAILURE;
/* child */	case 0: printf("Child: my PID %d, parent PID %d\n", getpid(), getppid());
			return EXIT_SUCCESS;
/* parent */	default: printf("Parent: my PID %d, child PID %d\n", getpid(), id);
			 wait(&status);
			 printf("Parent: child exit status is %d\n", status);
			 return EXIT_SUCCESS;
		}
}
