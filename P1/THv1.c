#include <unistd.h>// for  execvp get opt
#include <stdlib.h>// for EXIT status
#include "p1fxns.h"// professor helpful functions adt
#include <sys/time.h> // time handler
#include <sys/wait.h> // for wait and pause
#include <signal.h> //for signal
#include <fcntl.h> // file manipulate
#include <stdio.h> //for testing

#define UNUSED __attribute__((unused))
#define USEAGE "usage: ./THv? [-q <msec>] [-p <nprocesses>] [-c <ncores>] -l 'command line' \n"
#define SIZE 4096 // max bash output size
int volatile npro = 0;


//#define TH_QUANTUM_MSEC 5000 //5 secound defaulf
//#define TH_NPROCESSES 6 // 3 process per core
//#define TH_NCORES 2 //defaulf form vbox set up
//defined via export in bash form 2.6.1 will fail catch with these

//SIG) signals for waiting form lab 3
//in chapter 8 in reads
//SIGUSR2 handler  // to send a signal to self when child dies
//like hard ware interput but software

/*make
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
			npro--;
		}
	}
}


int main( int argc, char *argv[])
{
	//get opt extern vars
	extern char *optarg;
	extern int optind;
	//timme var
	struct timeval t1;
	struct timeval t2;
	long long musecs;
	char time[SIZE] = ""; //formatedsting
	char sec[SIZE] = ""; //the foramted sting
	char dsecs[SIZE] = ""; // decial part of secounds
	//get opt var / err var
	int npro;
	int ncor;
	int q; 
	int opt; // get opt iterator
	int fli; // flag l iterator
	int Qflag, Pflag, Cflag, Lflag;
	char *qf, *pf, *cf , *cmdLine;
	//file / command var;
	char buf[SIZE] = "";
	char pidVstr[SIZE] = "";
	char errout[SIZE] = "";
	char stdout[SIZE] = "";
	char word[SIZE] = "";
	char 
	//fork and exec var
	pid_t pid;
	int status;
	// general var  
	int i, j, w, cnt, cmdlen;


//Start)Obtain the number of processes to create (nprocesses), the number of processor cores upon which to run those programs (ncores), and the command line to execute in each of the processes (command) from the environment variables and command arguments to TH; 
	//note that the command line is just as you would present it to bash).
	i = 0;//iterrators
	fli = 0;//*
	j = 0; //iterrators
	cmdlen = 0;
	npro = -1;
	ncor = -1;
	q = -1;
	musecs = 0;
	qf = NULL;
	pf = NULL;
	cf = NULL;
	cmdLine = NULL;
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
						p1putstr(2,"-q flag requires an int no int assigned: ");
						p1putstr(2, optarg);
						p1putstr(2, "\n");
						return EXIT_FAILURE;	
					}
					break;
			// process option
			case 'p': 
					Pflag =1;
					npro = p1atoi(optarg);
					if(npro == 0)
					{
						p1putstr(2,"-p flag requires an int no int assigned: ");
						p1putstr(2, optarg);
						p1putstr(2, "\n");
						return EXIT_FAILURE;	
					}
					break;
			//core option
			case 'c': 
					Cflag =1;
					ncor = p1atoi(optarg);
					if(ncor == 0)
					{
						p1putstr(2,"-c flag requires an int no int assigned: ");
						p1putstr(2, optarg);
						p1putstr(2, "\n");
						return EXIT_FAILURE;	
					}
					printf("%d\n",ncor);
					break;
			//comand line option
			case 'l': 
					Lflag = 1;
					if(p1atoi(optarg) != 0)
					{
						p1putstr(2,"-l flag requires an command string an int was assigned: ");
						p1putstr(2, optarg);
						p1putstr(2, "\n");
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
		else
		{
			p1putstr(2,"TH_QUANTUM_MSEC enviroment varible undefined please define -q or TH_QUANTUM_MSEC\n");
		}	
	}
	if(Pflag == 0)
	{
		if((pf = getenv("TH_NPROCESSES")) != NULL)
		{
			npro = p1atoi(pf);
		}	
		else
		{
			p1putstr(2,"TH_NPROCESSES enviroment varible undefined please define -p or TH_NPROCESSES\n");
		}
	}
	if(Cflag == 0)
	{
		if((cf = getenv("TH_NCORES")) != NULL)
		{
			ncor = p1atoi(cf);
		}
		else
		{
			p1putstr(2,"TH_NCORES enviroment varible undefined please define -c or TH_NCORES\n");
		}
	}
	//ERRHLDR) command line -l check tricky part parse command first value in cmd
	if(Lflag == 1)
	{
		//if optind is less then argc there statment outside of ' '
		if(optind != argc)
		{
			p1putstr(2,USEAGE);
			p1putstr(2,"please ensure all of command string is in 'command'\n");
			p1putint(2,(argc - optind));
			p1putstr(2,": number of agrs outside of ' '\n");
			p1putstr(2,argv[optind]);
			p1putstr(2,": was first illegal statment.\n");
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
	if(argc == 1)
	{
		p1putstr(2,USEAGE);
		return EXIT_FAILURE;
	} 

	//get number of words
	w = 0;
	cnt = 0;
	cmdlen = p1strlen(cmdLine);

	for(i=0; i < cmdlen; i++)
	{
		if(w != (fli = p1getword(cmdLine,i,word)))
		{
			w = fli;
			cnt++;
		}
	}
// set args with null termination
	char **args = (char **)malloc((cnt + 1) * sizeof(char *));
	w = 0;
	for(i=0; i < cmdlen; i++)
	{
		if(w != (fli = p1getword(cmdLine,i,word)))
		{
			w = fli;
			args[j] = p1strdup(word);
			j++;
		}
	}
	args[j] = NULL;
//1) Note the current time (start)
	gettimeofday(&t1,NULL);
//2) Launch ‘nprocesses’ processes executing ‘command’ using fork(), execvp(), and any other required system calls. To make things simpler, assume that the programs will run in the same environment as used by TH.
	//ripped form lab 4 childhlder/ lecture 4 and editied
	signal(SIGCHLD, onchld);
	for (i = 0; i < npro; i++) 
	{
		pid = fork();
		switch(pid) 
		{
			case -1: //in perent fork fail
					p1strcat(errout, "Parent: fork() call failed Parent: error \n");
					p1perror(2, errout);
					goto wait_for_children;
					for(i = 0; i <cnt +1; i++)
					{
						free(args[i]);
					}
					free(args);
					return EXIT_FAILURE;
					break;
			case 0: // child process prepare to exec
					execvp(args[0], args);
					p1strcat(errout, "Child: Unable to exec ");
					p1strcat(errout, args[0]);
					p1strcat(errout, "Child: error \n");
					p1perror(2, errout);
					return EXIT_FAILURE;
			 		break;
			default:  // in parent process
					p1strcat(stdout,"Parent: waiting for ");
					p1itoa(pid,pidVstr);
					p1strcat(stdout, p1strpack(pidVstr, -8, ' ', buf));
					p1strcat(stdout, " to complete\n");
					p1putstr(1, stdout);
					(void) wait(&status); // wiat for child
		}
	}
//4) Note the current time (stop)

//5) Compute the elapsed time (stop - start) that it took for all of the processes to complete their processing and display on standard output1	
	//time
	//sec
	//decs
	musecs = 1000000 *(t2.tv_sec-t1.tv_sec) + (t2.tv_usec-t1.tv_usec);
	p1strcat(time, "The elased time to execute: ");



	/// museces = musecs/1000.int musecs%1000
//6) Exit //must catch failing point is ERRCK
	for(i = 0; i <cnt +1; i++)
	{
		free(args[i]);
	}
	free(args);
	return EXIT_SUCCESS;
//3) After all of the processes are running, wait for each process to terminate.
// we take advatage of pause
wait_for_children:
	while (npro > 0)
	{// this lets you know when each is dead
		pause(); // acts like broadcast
	}
}