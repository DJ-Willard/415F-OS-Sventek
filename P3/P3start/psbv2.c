#include "BXP/bxp.h" // for bxp
#include "ADTs/arrayqueue.h" //adt for array queue
#include <valgrind/valgrind.h> // memory cheack
#include <pthread.h> // for p threads
#include <assert.h> // for assert
#include <time.h> //nanosleep
#include <sys/time.h> // for time struct
#include <signal.h> // sig handlers
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <stdbool.h>
#include <stdlib.h>

#define USAGE "Usage: ./psbv? [-f <filename>]"
#define UNUSED __attribute__((unused))
#define SERVICE "PSB"
#define HOST "localhost"
#define PORT 19998


volatile bool killprog = false;
volatile bool doneR = false;
volatile bool doneF = false;

pthread_mutex_t lockR = PTHREAD_MUTEX_INITIALIZER;

//form lab 4 SIGINT handler
static void onint(UNUSED int sig)
{
 	killprog = true;
	printf("^C signal received, Shutting Down\n");
}


//thread fuction to ensure clean exist(recieves request issues response)
void *request(UNUSED void *args)
{
	BXPService *svc = (BXPService *)args;
	BXPEndpoint ep;
	char query[10000];
	char response[10001];
	unsigned rlen = 0;
	unsigned qlen = 0;
	char *words[25];
	char out[BUFSIZ];
	char back[128];
	BXPConnection bxpc;
	unsigned port;
	unsigned backlen;

	while((qlen = bxp_query(svc, &ep, query, 10000))> 0)
	{
		(void) extractWords(query, "|", words);
		sscanf(words[1], "%u", &port);
		bxpc = bxp_connect(words[0], (unsigned short)port, words[2], 1,1);
		strcpy(out, words[3]);
		bxp_call(bxpc, out, strlen(out)+1, back, sizeof back, &backlen);
		bxp_disconnect(bxpc);
		//the response to send back would be created by sprintf(response," 1%s " , query);
		//
		sprintf(response, "%s", query);
		rlen = strlen(response) +1;
		bxp_response(svc, &ep, response, rlen);
	}
	return NULL;
}
//lab 7 callback server extract
static int extractWords(char *buf, char *sep, char *words[])
{
	int i;
	char *p;

	for(p = strtok(buf, sep), i = 0; p != NULL; p = strtok(NULL, sep), i++)
	{
		words[i] = p;
	}
	words[i] = NULL;
	return i;
}


int main(int argc, char *argv[])
{
	//varribles for BXP
	BXPService svc;
	char *channels[1000];
	int port = PORT;
	//varibles for file handling
	FILE *fd = NULL;
	int opt;
	char buf[BUFSIZ];
	int i = 0;
	bool clearchannel = false;
	// singal and thread varibles
	struct timespec ms20 = {0,20000000};
	pthread_t requestThread;	

	//step 1 open file and read then close
	//Check the arguments provided; if “-f <filename>” use getopt
	while((opt = getopt(argc, argv, "f:")) != -1)
	{
		switch(opt)
		{
			case 'f':

					fd = fopen(argv[2], "r");
					//open failure of the initialization file
					if (fd == NULL)
					{
						fprintf(stderr, "%s: unable to open file - %s\n", argv[0], argv[optind]);
						goto cleanup;
					}
					break;
			default:
					if(optopt != 'f')
						fprintf(stderr, "%s: illegal option '-%c'!\n", argv[0], optopt);
					else
					{
						fprintf(stderr, "%s: illegal Usage.'-%c' [HERE] Requires an File Name \n", argv[0], optopt);
						fprintf(stderr, "%s\n",USAGE);
					}
					goto cleanup;
		}
	}

	//error handling: Except for initialization failure,open failure of the initialization file, and 
	//receipt of an INT signal, PSBv1 should never exit.
	signal(SIGINT,onint);

	//<filename> contains a set of channel names, one per line
	//open this file, read each channel name, and print “Creating publish/subscribe channel: %s\n”
	while(fgets(buf, BUFSIZ, fd) != NULL)
	{

	 	channels[i] = strdup(buf);
		fprintf(stdout, "Creating publish/ subscride channel: %s\n", channels[i]);
		i++;

	}
	//after you have processed the entire file, you need to close it.
	fclose(fd);
	clearchannel = true;
	fd = NULL;
	//Initialize the BXP runtime 1 so that it can create and accept encrypted connection requests
	assert(bxp_init(port,1));
	assert((svc = bxp_offer(SERVICE)));
	//Create a thread that will receive requests from client applications.
	assert(! pthread_create(&requestThread,NULL, request, (void *) svc));

//wait for CTRL-C form lab 4
	while(!killprog)
	{
		nanosleep(&ms20, NULL);
	}
	goto cleanup;

//cleanup close open file.
	cleanup:
		if(fd != NULL)
			fclose(fd);
		if(clearchannel)
		{
			for(;i >= 0; i--)
			{
				free(channels[i]);
			}
		}
		(void)pthread_cancel(requestThread);
		pthread_join(requestThread,NULL);
		return EXIT_FAILURE;
}
