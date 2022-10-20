#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define USUSED __attribute__((unused))

int int main(int argc, char const *argv[])
{
	char *args[] = {"echo", "hello","World",NULL};
	
	//fallows path execve you would have to have the path
	execvp(args[0],args);
	
	//returns -1 if executes next statement
	fprintf(stderr, "execvp() of 'echo hello world!' falied\n");

	return EXIT_FAILURE;
}

/* this will not work

	char *args[] = {"sventek", "hello","World",NULL};
	
	//fallows path execve you would have to have the path
	execvp(args[0],args);
	
	//returns -1 if executes next statement
	fprintf(stderr, "execvp() of 'sventek hello world!' falied\n"); 

	// invoke exit to kill process _exit in section 2 to kill child exec failed

	return EXIT_FAILURE;

*/