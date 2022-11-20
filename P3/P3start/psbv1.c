#include "BXP/bxp.h"
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>

#define USAGE "Usage: ./psbv? [-f <filename>]"
#define UNUSED __attribute__((unused))
#define SERVICE "echo"





int main(int argc, char const *argv[])
{
	//varribles
	FILE *fd = NULL;
	char *name = NULL;
	int opt;
	bool FFlag = false;


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

	//error handling
	//open failure of the initialization file
	if(FFlag)
	{
		fd = fopen(argv[optind], "r");
		if (fd == NULL)
		{
			fprintf(stderr, "%s: unable to open file - %s\n", argv[0], argv[optind]);
			goto cleanup;
		}
	}



	//error handling
	//Except for initialization failure, 
	//open failure of the initialization file, and 
	//receipt of an INT signal, PSBv1 should never exit.

	//<filename> contains aset of channel names, one per line

	//open this file, read each channel name, and print “Creating publish/subscribe channel: %s\n”

	//after you have processed the entire file, you need to close it.

	//Initialize the BXP runtime 1 so that it can create and accept encrypted connection requests

	//Create a thread that will receive requests from client applications.

	//Respond to each such request by echoing back the received request along with a statusbyte; 
	//the first byte of the response is ‘1’ for success, ‘0’ for failure

	//the response to send back would be created by sprintf(response," 1%s " , query);


	//cleanup close open file.
	cleanup:
		if(fd != NULL)
			fclose(fd);
		return EXIT_FAILURE;
}