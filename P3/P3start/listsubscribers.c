/*
 * list the current subscribers to a particular channel in the PSB service
 *
 * usage: ./listsubscribers channel
 *
 * connects to the PSB service on localhost, invokes the ListSubscribers
 * command, and displays the output on stdout
 */

#include "BXP/bxp.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#define UNUSED __attribute__((unused))
#define HOST "localhost"
#define PORT 19998
#define SERVICE "PSB"

BXPConnection bxpc;

int main(int argc, char *argv[]) {
    char query[BUFSIZ];
    char resp[BUFSIZ+1];
    int n;
    unsigned len;
    char *host;
    char *service;
    unsigned short port;
    int ifEncrypt = 1;
    static char usage[] = "usage: %s channel\n";

    host = HOST;
    service = SERVICE;
    port = PORT;
    if (argc != 2) {
        fprintf(stderr, usage, argv[0]);
	return EXIT_FAILURE;
    }
    assert(bxp_init(0, ifEncrypt));
    if ((bxpc = bxp_connect(host, port, service, 0, ifEncrypt)) == NULL) {
        fprintf(stderr, "Failure to connect to %s at %s:%05u\n",
                service, host, port);
        exit(EXIT_FAILURE);
    }
    sprintf(query, "ListSubscribers|%s", argv[1]);
    n = strlen(query) + 1;
    if (! bxp_call(bxpc, query, n, resp, sizeof(resp), &len)) {
        fprintf(stderr, "bxp_call() failed\n");
    } else if (resp[0] != '1') {
        fprintf(stderr, "PSB server returned ERR\n");
    } else
        printf("%s\n", resp+1);
    bxp_disconnect(bxpc);
    return EXIT_SUCCESS;
}
