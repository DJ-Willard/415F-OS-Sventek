/*
Authorship Statement
Daniel Willard
dwillar4
CIS 415 project 3
This is my own work expect form bxp code form lab 7 in my request thread and extract words function form lab 7
and the adts form the school libary. the sortalpha function neccessity was explained by adam case implemented by me, 
*/
#include "BXP/bxp.h" // for bxp
#include "ADTs/arrayqueue.h" //adt for array queue 
#include "ADTs/hashcskmap.h" //hash map for channels Recommened by adam for v3
#include "ADTs/queue.h" //adt for  queue 
#include "ADTs/stringADT.h" // for publish
#include "ADTs/iterator.h" // 
#include "sort.h" //alphabetical sort givenS
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

//marcos
#define USAGE "Usage: ./psbv? [-f <filename>]"
#define UNUSED __attribute__((unused))
#define SERVICE "PSB"
#define HOST "localhost"
#define PORT 19998
#define MAX_CHANNELS

//global varibles
unsigned long SVID = 0;
unsigned long CHID = 0;

//volatile data
volatile bool killprog = false;
volatile bool doneR = false;
volatile bool doneF = false;

// pthread data
pthread_mutex_t lockP = PTHREAD_MUTEX_INITIALIZER;

//data structure of requests of PSB
typedef struct chData{
	char *name; // channel name
	unsigned long  chid; //channel id 
	unsigned long  **subs; // list of
	int numsubs;
	bool cancalled; // list to die.
	//lock conditional will be needed
}CHData;

typedef struct subData{
	unsigned long *svid;
	unsigned long *clid;
	char *host;
	char *service;
	unsigned short *port;
}SUBData;

//helper function to free data in structure
void freeCHData(void *chData)
{
	CHData *tmp = (CHData *)chData;
	if(tmp->name != NULL)
		free(tmp->name);
	if(tmp->subs != NULL)
		for(int i = 0; i < tmp->numsubs; i++)
		{
			free(tmp->subs[i]);
		}
	free(chData);
}

int sortAlpha(void *v1, void *v2)
{
	char *a1 = (char *) v1;
	char *a2 = (char *) v2;

	return strcmp(a1, a2);	
}

//adt's for subscriptions and channels
//ADT queues intances
const CSKMap *subTracker;
const Queue *CHQueue;

//form lab 4 SIGINT handler
static void onint(UNUSED int sig)
{
 	killprog = true;
	printf("^C signal received, Shutting Down\n");
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
//helper function 
// bool createCH(char *name)
// {
// 	CHData *chData = (CHData *)malloc(sizeof (CHData));
// 	if(chData == NULL)
// 		return false;

// 	chData->chid = CHID;
// 	CHID++;
// 	chData->name = name;
// 	if(CHQueue->enqueue(CHQueue, (void *) chData))
// 		return true;
// 	return false;
// }

//helper thread to publish 
// void *publishCh(UNUSED void *args)
// {
// 	return NULL;
// }

//thread fuction to ensure clean exist(recieves request issues response)
void *request(UNUSED void *args)
{
	BXPService *svc = (BXPService *)args;
	BXPEndpoint ep;
	char query[10000];
	char response[10001];
	char reply[10000];
	char queryDble[10000];
	char *words[25];
	char *CHlist[BUFSIZ];
	char test[BUFSIZ];
	unsigned rlen = 0;
	unsigned qlen = 0;
	int arglen = 0; 
	long chQlen =0;

	//subTracker =  HashCSKMap(0L, (double) 0, freeCHData);
	CHQueue = Queue_create(freeCHData);

	while((qlen = bxp_query(svc, &ep, query, 10000))> 0)
	{

		assert(query != NULL);
		strcpy(queryDble, query);
		(void) extractWords(queryDble, "|", words);

		for(int i = 0; words[i] != NULL; i++)
		{
			arglen = i;
		}
		strcpy(test, words[0]);



		switch(test[0])
		{
			case 'C': 
					if(strcmp(words[0],"CreateChannel") == 0)
					{
						if(arglen == 1)
						{
							CHData *chData = (CHData *)malloc(sizeof (CHData));
							chData->chid = CHID;
							CHID++;
							if(words[1][strlen(words[1])-1] == '\n')
								words[1][strlen(words[1])-1] = '\0';
							chData->name = words[1];
							if(CHQueue->enqueue(CHQueue, (void *) chData))
								sprintf(response, "1%08lu",chData->chid);
						}
						break;
					}

					sprintf(response, "0%s", query);
					break;
			case 'D': 
					if(strcmp(words[0],"DestroyChannel") == 0)
					{
						if(arglen == 1)
							sprintf(response, "1%s", query);
						break;
					}

					sprintf(response, "0%s", query);
					break;
			case 'L': 
					if(words[0][strlen(words[0])-1] == '\n')
						words[0][strlen(words[0])-1] = '\0';
					if(strcmp(words[0],"ListChannels") == 0)
					{
						if(arglen == 0)
						{
							CHData **array;
							array = (CHData **) CHQueue->toArray(CHQueue, &chQlen);

							for(int i = 0; i < chQlen; i++)
							{
								CHlist[i] = (char *)(array[i]->name);
							}
							
							sort((void *) CHlist[chQlen], chQlen, sortAlpha);
							for(int i = 0; i < chQlen; i++)
							{
								strcat(reply, CHlist[i]);
								strcat(reply, ", ");
							}
							free(array);
							sprintf(response, "1%s", reply);		
						}
						break;
					}

					if(strcmp(words[0],"ListSubscribers") == 0)
					{
						if(arglen == 1)
							sprintf(response, "1%s", query);
						break;
					}

					if(strcmp(words[0],"ListSubscribers") != 0 && strcmp(words[0],"ListChannels") != 0)
						sprintf(response, "0%s", query);
					break;
			case 'P':
					if(strcmp(words[0],"Publish") == 0)
					{
						if(arglen == 2)
							sprintf(response, "1%s", query);
						break;
					}
					sprintf(response, "0%s", query);
					break;
			case 'S': 
					if(strcmp(words[0],"Subscribe") == 0)
					{
							if(arglen == 1)
								sprintf(response, "1%s", query);
							break;
					}
					sprintf(response, "0%s", query);
					break;
			case 'U': 
					if(strcmp(words[0],"Unsubscribe") == 0)
					{
							if(arglen == 5)
								sprintf(response, "1%s", query);
							break;
					}
					sprintf(response, "0%s", query);
					break;
			default:
					sprintf(response, "0%s", query);
		}

		rlen = strlen(response) +1;
		bxp_response(svc, &ep, response, rlen);
	}
	return NULL;
}


int main(int argc, char *argv[])
{
	//varribles for BXP
	BXPService svc;
	int port = PORT;
	//varibles for file handling
	FILE *fd = NULL;
	char *channels[BUFSIZ];
	char buf[BUFSIZ];
	int opt;
	int i = 0;
	int j = 0;
	bool clearchannel = false;
	bool Fflag = false;
	// singal and thread varibles
	struct timespec ms20 = {0,20000000};
	pthread_t requestThread;
	//pthread_t publishChThread;	

	CHQueue = Queue_create(freeCHData);

	//step 1 open file and read then close
	//Check the arguments provided; if “-f <filename>” use getopt
	while((opt = getopt(argc, argv, "f:")) != -1)
	{
		switch(opt)
		{
			case 'f':
					Fflag = true;
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

	assert(bxp_init(port,1));
	assert((svc = bxp_offer(SERVICE)));
	//error handling: Except for initialization failure,open failure of the initialization file, and 
	//receipt of an INT signal, PSBv1 should never exit.
	signal(SIGINT,onint);

	//<filename> contains a set of channel names, one per line
	//open this file, read each channel name, and print “Creating publish/subscribe channel: %s\n”
	if(Fflag)
	{
		while(fgets(buf, BUFSIZ, fd) != NULL)
		{
			channels[i] = strdup(buf);
			fprintf(stdout, "Creating publish/ subscride channel: %s\n", channels[i]);
			i++;
		}
		fclose(fd);
		for(int i = 0; i < j; i++)
		{
			CHData *chData = (CHData *)malloc(sizeof (CHData));
			chData->chid = CHID;
			CHID++;
			chData->name = channels[i];
			CHQueue->enqueue(CHQueue, (void *) chData);
		}
		clearchannel = true;
		fd = NULL;
	}

	//Initialize the BXP runtime 1 so that it can create and accept encrypted connection requests
	//assert(bxp_init(port,1));
	//assert((svc = bxp_offer(SERVICE)));
	//Create a thread that will receive requests from client applications.
	assert(! pthread_create(&requestThread,NULL, request, (void *) svc));
	//assert(! pthread_create(&publishChThread, NULL, publishCh, NULL));


//wait for CTRL-C form lab 4
	while(!killprog)
	{
		nanosleep(&ms20, NULL);
	}
	goto cleanup;

//cleanup close open file.
	cleanup:
		if(CHQueue != NULL)
			CHQueue->destroy(CHQueue);
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
