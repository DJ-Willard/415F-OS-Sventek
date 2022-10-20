#include <unistd.h>
#include <stdlib.h>
#include "p1fxns.h"
#include <errno.h>

int int main(int argc, char const *argv[])
{
//Obtain the number of processes to create (nprocesses), the number of processor cores upon which to run those programs (ncores), and the command line to execute in each of the processes (command) from the environment variables and command arguments to TH; note that the command line is just as you would present it to bash).
//1) Note the current time (start)
//2) Launch ‘nprocesses’ processes executing ‘command’ using fork(), execvp(), and any other required system calls. To make things simpler, assume that the programs will run in the same environment as used by TH.
//3) After all of the processes are running, wait for each process to terminate.
//4) Note the current time (stop)
//5) Compute the elapsed time (stop - start) that it took for all of the processes to complete their processing and display on standard output1
//6) Exit

/*The launching of each processes executing ‘command’ will look something like this in pseudocode:
prepare argument structure 
for i in 0 .. nprocesses-1 
pid[i] = fork(); 
if (pid[i] == 0) 
execvp(args[0], args) 
for i in 0 .. nprocesses-1 wait(pid[i])*/
//While this may appear to be simple, there are many things that can go wrong. You should spend significant time reading the entire man page for the fork(), execvp(), and wait() system calls.
	
}