/*
 * subscriber for the PSB service
 *
 * usage: ./subscriber channel
 *
 * connects to the PSB service and subscribes to channel in argv[1]
 * displays each message received from the service until the user
 * types ^C
 */

#include "BXP/bxp.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>
#include <signal.h>

#define UNUSED __attribute__((unused))
#define HOST "localhost"
#define PORT 19998
#define SERVICE "PSB"

/*
 * global data
 */
char *channel = NULL;
unsigned long sid = 0UL;
BXPConnection bxpc = NULL;

/*
 * SIGINT signal handler
 */
void onint(UNUSED int sig) {
    char query[128], resp[128];
    unsigned len;
    sprintf(query, "Unsubscribe|%lu", sid);
    bxp_call(bxpc, query, strlen(query)+1, resp, sizeof resp, &len);
    exit(EXIT_SUCCESS);
}

/*
 * function running in thread receiving messages over the channel
 */
void *svcRoutine(void *args) {
    BXPService *bxps = (BXPService *)args;
    BXPEndpoint bxpep;
    char query[4096], *resp = "1";
    unsigned qlen;

    while ((qlen = bxp_query(bxps, &bxpep, query, 4096)) > 0) {
        printf("%s\n", query);
	bxp_response(bxps, &bxpep, resp, 2);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    char query[1024];
    char resp[1024];
    int n;
    unsigned len;
    char *host;
    char *service;
    unsigned short port;
    int ifEncrypt = 1;
    static char usage[] = "usage: %s channel\n";
    char ipaddr[16];
    unsigned short svcPort;
    BXPService *bxps;
    pthread_t svcThread;
    int id=1000;

    host = HOST;
    service = SERVICE;
    port = PORT;
    if (argc != 2) {
        fprintf(stderr, usage, argv[0]);
        return EXIT_FAILURE;
    }
    channel = argv[1];
    if (signal(SIGINT, onint) == SIG_ERR) {
        fprintf(stderr, "Unable to establish SIGINT handler\n");
        return EXIT_FAILURE;
    }
    assert(bxp_init(0, ifEncrypt));
    bxp_details(ipaddr, &svcPort);
    /* create service to receive messages */
    if ((bxps = bxp_offer("PSB-SUBSCRIBER")) == NULL) {
        fprintf(stderr, "Unable to create BXP service to receive messages\n");
	exit(EXIT_FAILURE);
    }
    if (pthread_create(&svcThread, NULL, svcRoutine, (void *)bxps)) {
        fprintf(stderr, "Unable to create thread to receive events\n");
	exit(EXIT_FAILURE);
    }
    if ((bxpc = bxp_connect(host, port, service, 0, ifEncrypt)) == NULL) {
        fprintf(stderr, "Failure to connect to %s at %s:%05u\n",
                service, host, port);
        exit(EXIT_FAILURE);
    }
    sprintf(query, "Subscribe|%s|1|%s|PSB-SUBSCRIBER|%u", channel, ipaddr, (unsigned)svcPort);
    n = strlen(query) + 1;
    if (! bxp_call(bxpc, query, n, resp, sizeof(resp), &len)) {
        fprintf(stderr, "bxp_call() failed\n");
        return EXIT_FAILURE;
    }
    if (resp[0] != '1') {
        fprintf(stderr, "PSB server returned ERR\n");
        return EXIT_FAILURE;
    }
    printf("local id %d registered as server id %s\n", id, resp+1);
    sscanf(resp+1, "%lu", &sid);
    pthread_join(svcThread, NULL);
    bxp_disconnect(bxpc);
    return EXIT_SUCCESS;
}
