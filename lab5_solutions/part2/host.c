#define _GNU_SOURCE
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define BUF_SIZE 4096

static void die(const char *msg)
{
    perror(msg);
    exit(1);
}

static void usage_and_exit(char *argv0)
{
    fprintf(stderr, "usage: %s <host-name>\n", argv0);
    fprintf(stderr, "   ex) %s www.google.com\n", argv0);
    exit(1);
}

int main(int argc, char **argv)
{
    /*
     * Parse arguments and determine output file name.
     */

    if (argc != 2)
        usage_and_exit(argv[0]);

    char *server_name = argv[1];

    /*
     * Obtain socket address structure from server name and port number.
     */

    struct addrinfo hints, *info;
    memset(&hints, 0, sizeof(hints));

    hints.ai_socktype = SOCK_STREAM; // Stream socket for TCP connections
    hints.ai_protocol = IPPROTO_TCP; // TCP protocol

    int aerr;
    if ((aerr = getaddrinfo(server_name, NULL, &hints, &info)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(aerr));
        exit(1);
    }

    struct addrinfo *info0 = info;

    while (info) {
        if (info->ai_addr->sa_family == AF_INET) {
            char buf[INET_ADDRSTRLEN];

            struct sockaddr_in *addr = (struct sockaddr_in *)info->ai_addr;

            if (inet_ntop(AF_INET, &addr->sin_addr, buf, sizeof(buf)) == NULL)
                die("inet_ntop");

            printf("%s has address %s\n", server_name, buf);
        } else if (info->ai_addr->sa_family == AF_INET6) {
            char buf[INET6_ADDRSTRLEN];

            struct sockaddr_in6 *addr = (struct sockaddr_in6 *)info->ai_addr;

            if (inet_ntop(AF_INET6, &addr->sin6_addr, buf, sizeof(buf)) == NULL)
                die("inet_ntop");

            printf("%s has IPv6 address %s\n", server_name, buf);

        } else {
            printf("Other\n");
        }
        info = info->ai_next;
    }

    freeaddrinfo(info0);

    return 0;
}
