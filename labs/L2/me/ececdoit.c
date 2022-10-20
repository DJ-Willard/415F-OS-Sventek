#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

//#define USUSED __attribute__((unused))

int int main(int argc, char const *argv[])
{
	//char *args[] = {"echo", "hello","World",NULL};
	
	//fallows path execve you would have to have the path
	execvp(argv[1],args+1);
	
	//returns -1 if executes next statement
	fprintf(stderr, "execvp() of %s falied\n", argv[1]);

	return EXIT_FAILURE;
}
