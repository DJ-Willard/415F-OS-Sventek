#include <unistd.h>// for  execvp get opt
#include <stdlib.h>// for EXIT status
#include "p1fxns.h"// professor helpful functions adt
#include <sys/time.h> // time handler
#include <time.h> // for nano sleep
#include <sys/wait.h> // for wait and pause
#include <signal.h> //for signal
#include <fcntl.h> // file manipulate
#include <stdio.h> //for testing

#define UNUSED __attribute__((unused))
#define USEAGE "usage: ./THv? [-q <msec>] [-p <nprocesses>] [-c <ncores>] -l 'command line' \n"
#define SIZE 4096 // max bash output size
int volatile npro = 0;
bool volatile USR1_seen = false;

void onusr2(UNUSED int sig)
{

}

void onusr1(UNUSED int sig)
{
	USR1_seen = true;
}

void onchld(UNUSED int sig)
{
	pid_t pid;
	int status;

	while((pid = waitpid(-1,&status,(WNOHANG|WUNTRACED|WUNTRACED))) > 0) //-1 any child, NOHANG means no hang for any child alive
	{
		if(WIFEXITED(status) || WIFSIGNALED(status)) // marco in other file
		{
			npro--;
			kill(pid, SIGUSR2);
		}
		if(WIFSTOPPED(status))
		{
			kill(pid, SIGUSR2);
		}
		if(WIFCONTINUED(status))
		{
			kill(pid, SIGUSR2);
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
	char times[SIZE] = ""; //the foramted stings
	char timed[SIZE] = "";
	char sec[SIZE] = "";
	char dsecs[SIZE] = ""; // decial part of secounds
	struct timespec ms20 = {0,20000000};
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
	char copies[SIZE] = "";
	char pcessor[SIZE] = "";
	char finout[SIZE] = "";	
	//fork and exec var
	pid_t pid;
	int status;
	int pidcnt;
	// general var  
	int i, j, w, cnt, cmdlen;


//Start)Obtain the number of processes to create (nprocesses), the number of processor cores upon which to run those programs (ncores), and the command line to execute in each of the processes (command) from the environment variables and command arguments to TH; 
	//note that the command line is just as you would present it to bash).
	i = 0;//iterrators
	fli = 0;//*
	j = 0; //iterrators
	pidcnt = 0;
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

//store pid values here
	int *pidarr = (int *)malloc(npro*sizeof(int));

	signal(SIGCHLD, onchld);
	if (signal(SIGUSR1, onusr1) == SIG_ERR) 
	{
		p1strcat(errout,"Can't establish SIGUSR1 handler\n");
		p1perror(2, errout);
		return EXIT_FAILURE;
	}
	for (i = 0; i < npro; i++) 
	{
		pid = fork();
		switch(pid) 
		{
			case -1: //in perent fork fail
					p1strcat(errout, "Parent: fork() call failed Parent: error \n");
					p1perror(2, errout);
					for(i = 0; i <cnt +1; i++)
					{
						free(args[i]);
					}
					free(args);
					free(pidarr);
					return EXIT_FAILURE;
					break;
			case 0: // child process prepare to exec
//1)Immediately after each process is created using fork(), the child process waits on the 
	//SIGUSR1 signal before calling execvp().  form lab 2
					while (! USR1_seen) //in the run state
					{
						(void)nanosleep(&ms20, NULL);
					}
					execvp(args[0], args);
					p1strcat(errout, "Child: Unable to exec ");
					p1strcat(errout, args[0]);
					p1strcat(errout, "Child: error \n");
					p1perror(2, errout);
					return EXIT_FAILURE;
			 		break;
			default:  // in parent process
					pidarr[pidcnt] = pid;
					pidcnt++;
					p1strcat(stdout,"Parent: waiting for ");
					p1itoa(pid,pidVstr);
					p1strpack(pidVstr, -8, '_', buf);
					p1strcat(stdout, buf);
					p1strcat(stdout, " to complete\n");
					p1putstr(1, stdout);
		}
	}
//2) Create all nprocesses 
//3) Note the current time (start) 
	gettimeofday(&t1,NULL); 

//4) The TH parent process sends each child process a SIGUSR1 signal to wake it up.  Each 
	//process will then wake up and invoke execvp() to run the workload process. 
	for(i = 0; i < pidcnt ; i++)
	{
		printf("sigusr1 sent\n");
		kill(pidarr[i],SIGUSR1);
	}
//5)After all of the processes have been awakened and are executing, the TH sends each process a SIGSTOP signal to suspend it.
	for(i = 0; i < pidcnt ; i++)
	{
		printf("sigustop sent\n");
		kill(pidarr[i],SIGSTOP);
	}
//6) After all of the processes have been suspended, the TH sends each process a SIGCONT signal to resume it.
	for(i = 0; i < pidcnt ; i++)
	{
		printf("sigCont sent\n");
		kill(pidarr[i],SIGCONT);
	}
//7)Once all processes are back up and running, the TH waits for each process to terminate.
	(void) wait(&status);
//8) Note the current time (stop) 
	gettimeofday(&t2,NULL);
//9)Compute the elapsed time (stop - start) that it took for all of the processes to complete their 
	//processing and display on standard output 
	musecs = 1000000 *(t2.tv_sec-t1.tv_sec) + (t2.tv_usec-t1.tv_usec);
	p1strcat(finout, "The elased time to execute ");
	p1itoa(npro, copies);
	p1strcat(finout,copies);
	p1strcat(finout, " compies of [");
	p1strcat(finout, args[0]);
	p1strcat(finout, "] on ");
	p1itoa(ncor,pcessor);
	p1strcat(finout,pcessor);
	p1strcat(finout," processors is ");
	p1itoa((int)(musecs/1000000), sec);
	p1itoa((int)(musecs%1000000), dsecs);
	p1strpack(sec,-7,'0', times);
	p1strpack(sec,3,'0', timed);
	p1strcat(finout, times);
	p1strcat(finout, ".");
	p1strcat(finout, timed);
	p1strcat(finout, "sec\n");
	p1putstr(1,finout);

//10) Exit //must catch failing point in ERRCK //FREE	
	for(i = 0; i <cnt +1; i++)
	{
		free(args[i]);
	}
	free(args);
	free(pidarr);
	return EXIT_SUCCESS;
}