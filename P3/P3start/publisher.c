/*
 * publisher for the PSB service
 *
 * usage: ./publisher [-d secs] [-n name] channel
 *
 * reads each line from standard input and publishes "%s: %s", `name', line to
 * the `channel' in the PSB service; if -d is specified, it will wait
 * `secs' seconds between each publish call
 */

#include "BXP/bxp.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#define UNUSED __attribute__((unused))
#define HOST "localhost"
#define PORT 19998
#define SERVICE "PSB"

BXPConnection bxpc;

void onint(UNUSED int sig) {
    fprintf(stderr, "\nTerminated by ^C\n");
    bxp_disconnect(bxpc);
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {
    char buf[BUFSIZ];
    char query[2*BUFSIZ];
    char resp[128];
    int n;
    unsigned len;
    char *host;
    char *service;
    unsigned short port;
    char name[128];
    long delay = 0;
    char *channel = NULL;
    int opt;
    int ifEncrypt = 1;
    static char usage[] = "usage: %s [-d secs] [-n name] channel\n";

    host = HOST;
    service = SERVICE;
    port = PORT;
    sprintf(name, "%d", getpid());
    opterr = 0; /* tells getopt to NOT print an error message */
    while ((opt = getopt(argc, argv, "d:n:")) != -1) {
        switch (opt) {
        case 'n':
            strcpy(name, optarg);
            break;
        case 'd':
	    sscanf(optarg, "%ld", &delay);
            break;
        default:
            fprintf(stderr, usage, argv[0]);
            return EXIT_FAILURE;
        }
    }
    if (optind != (argc-1)) {
        fprintf(stderr, usage, argv[0]);
	return EXIT_FAILURE;
    }
    channel = argv[optind];
    if (signal(SIGINT, onint) == SIG_ERR) {
        fprintf(stderr, "Unable to establish SIGINT handler\n");
        return EXIT_FAILURE;
    }
    assert(bxp_init(0, ifEncrypt));
    if ((bxpc = bxp_connect(host, port, service, 0, ifEncrypt)) == NULL) {
        fprintf(stderr, "Failure to connect to %s at %s:%05u\n",
                service, host, port);
        exit(EXIT_FAILURE);
    }
    delay *= 1000000;		/* convert delay to microseconds */
    while (fgets(buf, sizeof(buf), stdin) != NULL) {
        long slen = strlen(buf);
	buf[slen-1] = '\0';
	if (delay > 0)
            usleep(delay);
        sprintf(query, "Publish|%s|%s:%s", channel, name, buf);
        n = strlen(query) + 1;
        if (! bxp_call(bxpc, query, n, resp, sizeof(resp), &len)) {
            fprintf(stderr, "bxp_call() failed\n");
            break;
        }
        if (resp[0] != '1') {
            fprintf(stderr, "PSB server returned ERR\n");
            continue;
        }
    }
    bxp_disconnect(bxpc);
    return EXIT_SUCCESS;
}
