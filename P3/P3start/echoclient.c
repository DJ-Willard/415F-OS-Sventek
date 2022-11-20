/*
 * echoclient for the PSB service
 *
 * usage: ./echoclient
 *
 * reads each line from standard input, sends it to PSB service on localhost,
 * receives the echo'd line, and writes it to standard output
 */

#include "BXP/bxp.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#define HOST "localhost"
#define PORT 19998
#define SERVICE "PSB"

int main(int argc, char *argv[]) {
    BXPConnection bxpc;
    char buf[250];
    char query[1024];
    char resp[251];
    unsigned len;
    char *host;
    char *service;
    unsigned short port;
    struct timeval start, stop;
    unsigned long count = 0;
    unsigned long msec;
    double mspercall;
    int opt;
    int ifEncrypt = 0;
    static char usage[] = "usage: %s [-e] [-h host] [-p port] [-s service]\n";

    host = HOST;
    service = SERVICE;
    port = PORT;
    opterr = 0; /* tells getopt to NOT print an error message */
    while ((opt = getopt(argc, argv, "eh:p:s:")) != -1)
        switch (opt) {
        case 'e':
            ifEncrypt = 1;
            break;
        case 'h':
            host = optarg;
            break;
        case 'p':
            port = atoi(optarg);
            break;
        case 's':
            service = optarg;
            break;
        default:
            fprintf(stderr, usage, argv[0]);
            return EXIT_FAILURE;
        }
    assert(bxp_init(0, ifEncrypt));
    if ((bxpc = bxp_connect(host, port, service, 0, ifEncrypt)) == NULL) {
        fprintf(stderr, "Failure to connect to %s at %s:%05u\n",
                service, host, port);
        exit(-1);
    }
    gettimeofday(&start, NULL);
    while (fgets(buf, sizeof(buf), stdin) != NULL) {
        unsigned n = strlen(buf) + 1;
        count++;
        sprintf(query, "%s", buf);
        if (! bxp_call(bxpc, query, n, resp, sizeof resp, &len)) {
            fprintf(stderr, "bxp_call() failed\n");
            break;
        }
        if (resp[0] != '1') {
            fprintf(stderr, "%s: server returned ERR - %s", argv[0], resp+1);
        } else
            fputs(resp+1, stdout);
    }
    gettimeofday(&stop, NULL);
    if (stop.tv_usec < start.tv_usec) {
        stop.tv_usec += 1000000;
        stop.tv_sec--;
    }
    msec = 1000 * (stop.tv_sec - start.tv_sec) +
           (stop.tv_usec - start.tv_usec) / 1000;
    mspercall = (double)msec / (double)count;
    fprintf(stderr, "%ld lines Echo'd in %ld.%03ld seconds, %.3fms/call\n",
            count, msec/1000, msec % 1000, mspercall);
    bxp_disconnect(bxpc);
    return EXIT_SUCCESS;
}
