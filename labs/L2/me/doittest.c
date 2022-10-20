#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

#define USUSED __attribute__((unused))

int int main(int argc, char const *argv[])
{
	pid_t id; // signed int

//parent
	id=fork();

//both
	switch(id)
	{
		//parent
		case -1: fprintf(stderr,"fork() failed\n");
			return EXIT_FAILURE;
		//child
		case 0: printf("Child: my PID = %d, Parent PID = %d/n", getpid(), getppid());
			return EXIT_SUCCESS;
		//parent
		default: printf("Parent:  my PID = %d, Child PID = %d\n", getpid(), id);
			wait(NULL); // tells the system i dont care if it failed (an int asks for the speficics)
			printf("Parent: child exited with status = %d\n", status);
			return EXIT_SUCCESS;
	}
}