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
    fprintf(stderr, "usage: %s <host-name> <port-number> <URI>\n", argv0);
    fprintf(stderr, "   ex) %s www.example.com 80 /index.html\n", argv0);
    exit(1);
}

int main(int argc, char **argv)
{
    /*
     * Parse arguments and determine output file name.
     */

    if (argc != 4)
        usage_and_exit(argv[0]);

    char *server_name = argv[1];
    char *server_port = argv[2];
    char *request_uri = argv[3];

    /*
     * Extract file name, i.e., whatever comes after the last '/' in the URI.
     */

    // This call to strrchr() returns a pointer to the last '/', if any.
    char *file_name = strrchr(request_uri, '/');
    if (!file_name) {
        fprintf(stderr, "Error: URI does not contain '/'\n");
        usage_and_exit(argv[0]);
    }

    // Skip past that '/'.
    file_name += 1;

    /*
     * Obtain socket address structure from server name and port number.
     */

    struct addrinfo hints, *info;
    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_INET;       // Only accept IPv4 addresses
    hints.ai_socktype = SOCK_STREAM; // Stream socket for TCP connections
    hints.ai_protocol = IPPROTO_TCP; // TCP protocol

    int aerr;
    if ((aerr = getaddrinfo(server_name, server_port, &hints, &info)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(aerr));
        exit(1);
    }

    /*
     * Create a socket(), connect() it to the server, and wrap in FILE *s.
     */

    int serv_fd = socket(info->ai_family, info->ai_socktype, info->ai_protocol);
    if (serv_fd < 0)
        die("socket");

    if (connect(serv_fd, info->ai_addr, info->ai_addrlen) < 0)
        die("connect");

    freeaddrinfo(info);

    FILE *serv_r = fdopen(serv_fd, "rb");
    FILE *serv_w = fdopen(dup(serv_fd), "wb");

    /*
     * Send HTTP request.
     */

    fprintf(serv_w,
        // Note that in C, adjacent string literals are concatenated
        "GET %s HTTP/1.0\r\n"
        "Host: %s:%s\r\n"
        "\r\n",
        request_uri, server_name, server_port);

    // We're done writing to the server, so let's just close write FILE * now.
    if (fclose(serv_w))
        die("sent");

    /*
     * Read the HTTP response line, and headers.
     */

    char buf[BUF_SIZE];

    // Read the first line
    if (fgets(buf, sizeof(buf), serv_r) == NULL) {
        fprintf(stderr, "Server connection terminated prematurely.\n");
        fclose(serv_r);
        exit(1);
    }

    // Check that the response is in HTTP/1.0 or HTTP/1.1.
    if (strncmp("HTTP/1.0 ", buf, 9) && strncmp("HTTP/1.1 ", buf, 9)) {
        fprintf(stderr, "Unknown response protocol: %s\n", buf);
        fclose(serv_r);
        exit(1);
    }

    // Check that the response code is 200.
    if (strncmp("200", buf + 9, 3) != 0) {
        fprintf(stderr, "%s\n", buf);
        fclose(serv_r);
        exit(1);
    }

    // If we're still here, we have a successful HTTP response (i.e., code 200).

    // Now, skip the header lines.
    do {
        if (fgets(buf, sizeof(buf), serv_r) == NULL) {
            fprintf(stderr, "Server terminated connection without sending file.\n");
            fclose(serv_r);
            exit(1);
        }
    } while (strcmp("\r\n", buf));

    /*
     * Read from the file, and write out to file_name.
     */

    // Open up file_name for writing.
    FILE *out_fp = fopen(file_name, "wb");
    if (out_fp == NULL)
        die("fopen");

    // Switch to fread()/fwrite() so that we can download binary files.
    size_t n;
    while ((n = fread(buf, 1, sizeof(buf), serv_r)) > 0) {
        if (fwrite(buf, 1, n, out_fp) != n) {
            fprintf(stderr, "Encountered error writing to %s\n", file_name);
            exit(1);
        }
    }

    /*
     * All done, clean up.
     */

    // fread() returns 0 on EOF or on error, so we need to check for errors.
    if (ferror(serv_r)) {
        fprintf(stderr, "Encountered error reading from server socket.\n");
        exit(1);
    }

    // Close FILE * for output file.
    if (fclose(out_fp))
        die("close");

    // Close FILE *serv_r as well, closing the underlying TCP connection too.
    if (fclose(serv_r))
        die("sent");

    return 0;
}
