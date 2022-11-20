#include "BXP/bxp.h" // for bxp
#include <pthread.h> // for p threads
#include <assert.h> // for assert
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <stdbool.h>
#include <stdlib.h>

#define USAGE "Usage: ./psbv? [-f <filename>]"
#define UNUSED __attribute__((unused))
#define SERVICE "echo"

volatile bool shutdown = false;

//form lab 4 SIGINT handler
static void onint(UNUSED int sig)
{
	printf("^C signal received, Shutting Down\n");
	shutdown = true;
}
/* parses words form file.
int extractwords(char *buf, char *sep, char *words[])
{
	int i;
	char *p;

	for(p = strtok(buf, sep), i = 0; p != NULL; p = strtok(NULL, sep), i++)|
	{
		words[i] = p;
	}
	words[i] =NULL;
	return i;
}
*/


int main(int argc, char const *argv[])
{
	//varribles for BXP
	BXPConnection bxp;
	BXPEndpoint ep;
	BxpService svc;
	char channels[10000];
	char query[10000];
	char response[10001];
	unsigned qlen;
	unsigned rlen;
	//varibles for file handling
	FILE *fd = NULL;
	int opt;
	bool FFlag = false;
	char buf[BUFSIZ];

	int i = 0;



	//step 1 open file and read then close
	//Check the arguments provided; if “-f <filename>” use getopt
	while((opt = getopt(argc, argv, "f:")) != -1)
	{
		switch(opt)
		{
			case 'f':
					FFlag = true;
					break;
			default:
					if(optopt != 'f')
						fprintf(stderr, "%s: illegal option '-%c'!\n", argv[0], optopt);
					else
						fprintf(stderr, "%s: illegal Usage.'-%c' [HERE] Requires an File Name \n", argv[0], optopt);
						fprintf(stderr, "%s\n",USAGE);
					goto cleanup;

		}
	}

	
	//error handling //
	//Except for initialization failure, 
	//open failure of the initialization file, and 
	//receipt of an INT signal, PSBv1 should never exit.


	//open failure of the initialization file
	if(FFlag)
	{
		fd = fopen(argv[optind], "r");
		//open failure of the initialization file
		if (fd == NULL)
		{
			fprintf(stderr, "%s: unable to open file - %s\n", argv[0], argv[optind]);
			goto cleanup;
		}
	}

	//<filename> contains aset of channel names, one per line
	//open this file, read each channel name, and print “Creating publish/subscribe channel: %s\n”
	
	while(fgets(buf, BUFSIZ, fd) != NULL)
	{
		strcpy(channel, buf);
		fprintf(stdout, "Creating publish/ subscride channel: %s\n", channel)
	}
	//after you have processed the entire file, you need to close it.
	fclose(fd);

	//Initialize the BXP runtime 1 so that it can create and accept encrypted connection requests
	assert(bxp_init(0,1))
	assert()




	//Create a thread that will receive requests from client applications.

	//Respond to each such request by echoing back the received request along with a statusbyte; 
	//the first byte of the response is ‘1’ for success, ‘0’ for failure

	//the response to send back would be created by sprintf(response," 1%s " , query);

	//wait for CTRL-C form lab 4
	while(!shutdown)
	{
		nanosleep(&ms20, NULL);
	}
	return EXIT_SUCCESS;


	//cleanup close open file.
	cleanup:
		if(fd != NULL)
			fclose(fd);
		return EXIT_FAILURE;
}