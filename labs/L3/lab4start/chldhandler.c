/*
 * chldhandler - shows how to use a SIGCHLD handler to harvest child processes
 *
 * Creates 5 subprocesses executing "./cpubound -s 5"
 * onchld handler decrements `nprocesses' when it receives a child process
 *     termination event, and then sends a SIGUSR2 signal to this process
 *     to wake up the pause() call
 */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h> // fork and exec
#include <sys/types.h> // time stuff
#include <sys/wait.h>
#define UNUSED __attribute__((unused))

#define NUMBER 5
char *args[] = { "./cpubound", "-s", "5", NULL }; // must have a null at the end so execvp know when to stop
volatile int nprocesses = 0;

/* in chapter 8 in reads
 * SIGUSR2 handler  // to send a signal to self when child dies
 */  //like hard ware interput but software
void onusr2(UNUSED int sig)
{}

/*
 * SIGCHLD handler
 */
void onchld(UNUSED int sig)
{
	pid_t pid;
	int status;

	while((pid = waitpid(-1, &status, WNOHANG)) > 0) //-1 any child, NOHANG means no hang for any child alive
	{
		if(WIFEXITED(status) || WIFSIGNALED(status)) // marco in other file
		{
			nprocesses--;
			kill(getpid(), SIGUSR2); // gives our pid then sigal condition varible
		}
	}
}


int main(UNUSED int argc, UNUSED char *argv[]) {
	int i;
	int status;

/*
 * establish SIGCHLD and SIGUSR2 handlers
 */
	signal(SIGCHLD, onchld);
	signal(SIGUSR2, onusr2);
	for (i = 0; i < NUMBER; i++) {
		int pid = fork();
		switch(pid) {
		case -1: fprintf(stderr, "Parent: fork() failed\n");
			 goto wait_for_children;
		case 0:  execvp(args[0], args);
			 fprintf(stderr, "Child: execvp() failed\n");
			 exit(EXIT_FAILURE);
		default: nprocesses++; // we will need more to track pids
		}
	}
// we take advatage of pause
wait_for_children:
	while (nprocesses > 0)
	{// this lets you know when each is dead
		pause(); // acts like broadcast
	}
	for (i = 0; i < nprocesses; i++)
		wait(&status);
	return EXIT_SUCCESS;
}
