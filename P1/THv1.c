#include <unistd.h>// for  execvp get opt
#include <stdlib.h>// for EXIT status
#include "p1fxns.h"// professor helpful functions adt
#include <sys/time.h> // time handler
#include <fcntl.h> // file manipulate
#include <stdio.h> //for testing

#define UNUSED __attribute__((unused))
#define USEAGE "usage: ./THv? [-q <msec>] [-p <nprocesses>] [-c <ncores>] -l 'command line' \n"


//#define TH_QUANTUM_MSEC 5000 //5 secound defaulf
//#define TH_NPROCESSES 6 // 3 process per core
//#define TH_NCORES 2 //defaulf form vbox set up
//defined via export in bash form 2.6.1 will fail catch with these


int main( int argc, char *argv[])
{
	extern char *optarg;
	extern int optind;
	//timme var
	struct timeval t1;
	struct timeval t2;
	long long musecs;
	//get opt var / err var
	int npro;
	int ncor;
	int q; 
	char *cmdLine; // everthing after -l
	int opt; // get opt iterator
	int Qflag, Pflag, Cflag, Lflag;
	char *qf, *pf, *cf,*lg;
	//file / command var
	char *progname;
	char buf[BUFSIZ];
	char word[4096]; //largest input bash can handle
	// file discriptor for open 2 = stderr 1=stdout 
	//fork and exec var
	pid_t pid;
	// general var  
	int i; // iterator

//Start)Obtain the number of processes to create (nprocesses), the number of processor cores upon which to run those programs (ncores), and the command line to execute in each of the processes (command) from the environment variables and command arguments to TH; 
	//note that the command line is just as you would present it to bash).
	npro = -1;
	ncor = -1;
	q = -1;
	cmdLine = NULL;
	progname = argv[0];
	Qflag = 0,Pflag = 0,Cflag = 0,Lflag = 0; // set to false


//FLAGCK) flag cheak aand asssing falg value
// use getopt extern char *optarg, int optind, opterr, optopt; 
	while((opt = getopt(argc, argv, "q:p:c:l:")) != -1)
	{
		switch(opt)
		{
			// qauntum option
			case 'q':
					Qflag = 1;
					q = p1atoi(optarg);
					if(q == 0)
					{
						p1putstr(2,"-q flag requires an int no int assigned\n");
						return EXIT_FAILURE;	
					}
					break;
			// process option
			case 'p': 
					Pflag =1;
					npro = p1atoi(optarg);
					if(npro == 0)
					{
						p1putstr(2,"-p flag requires an int no int assigned\n");
						return EXIT_FAILURE;	
					}
					break;
			//core option
			case 'c': 
					Cflag =1;
					ncor = p1atoi(optarg);
					if(ncor == 0)
					{
						p1putstr(2,"-c flag requires an int no int assigned\n");
						return EXIT_FAILURE;	
					}
					printf("%d\n",ncor);
					break;
			//comand line option
			case 'l': 
					Lflag = 1;
					if(p1atoi(optarg) != 0)
					{
						p1putstr(2,"-l flag requires an command string an int was assigned\n");
						return EXIT_FAILURE;
					}
					cmdLine = optarg;
					break;
			//? return not parsed options
			default: 
					p1putstr(2,USEAGE);
					return EXIT_FAILURE;
				break;
		}
	}

//ERRCK) error handling / value defaulting.
	if(Qflag == 0)
	{
		if((qf = getenv("TH_QUANTUM_MSEC")) != NULL)
		{
			q = p1atoi(qf);
		}	
	}
	if(Pflag == 0)
	{
		if((pf = getenv("TH_NPROCESSES")) != NULL)
		{
			npro = p1atoi(pf);
		}	
	}
	if(Cflag == 0)
	{
		if((cf = getenv("TH_NCORES")) != NULL)
		{
			ncor = p1atoi(cf);
		}
	}
	//ERRHLDR) command line -l check tricky part parse command first value in cmd
	if(Lflag == 1)
	{
		(char*)malloc(word);
		cmdLine = 
		printf("cmd: %s", cmdLine);
		for(i=0; argv[i] != NULL; i++)
		{
			printf("argv[%d] %s\n",i,argv[i]);
		}
	}
	if(Lflag == 0)
	{
		p1putstr(2,"-l flag is reqired please provide -l and a legal bash command\n");
		return EXIT_FAILURE;
	}
	if((q == -1|| npro == -1 || ncor == -1) || (cmdLine == NULL))
	{
		printf("value not set\n");
		if(Qflag == 0 || Pflag == 0 || Cflag == 0 || Lflag == 0)
		{
			p1putstr(2,"Flag[s] set default evrrionment varible[s] failed to fetch or be set\n");
			return EXIT_FAILURE;
		}
		p1putstr(2,"Defualt value[s] failed to fetch or be set\n");
		return EXIT_FAILURE;
	}
//1) Note the current time (start)
/*ettimeofday(&t1,NULL);
//2) Launch ‘nprocesses’ processes executing ‘command’ using fork(), execvp(), and any other required system calls. To make things simpler, assume that the programs will run in the same environment as used by TH.
	//ripped form lab 4 childhlder and adited
	for (i = 0; i < npro; i++) {
		pid = fork();
		switch(pid) {
		case -1: fprintf(stderr, "Parent: fork() failed\n");
			 goto wait_for_children;
		case 0:  execvp(args[0], args);
			 fprintf(stderr, "Child: execvp() failed\n");
			 exit(EXIT_FAILURE);
		default: nprocesses++; // we will need more to track pids
		}
	}
//3) After all of the processes are running, wait for each process to terminate.

//4) Note the current time (stop)
	gettimeofday(&t2,NULL);
//5) Compute the elapsed time (stop - start) that it took for all of the processes to complete their processing and display on standard output1	
	musecs= 1000000 *(t2.tv_sec-t1.tv_sec) + (t2.tv_usec-t1.tv_usec);
	/// museces = musecs/1000.int musecs%1000
//6) Exit //must catch failing point is ERRCK*/
	return EXIT_SUCCESS;
}





/*The launching of each processes executing ‘command’ will look something like this in pseudocode:
prepare argument structure 
for i in 0 .. nprocesses-1 
pid[i] = fork(); 
if (pid[i] == 0) 
execvp(args[0], args) 
for i in 0 .. nprocesses-1 wait(pid[i])*/
//While this may appear to be simple, there are many things that can go wrong. You should spend significant time reading the entire man page for the fork(), execvp(), and wait() system calls.


