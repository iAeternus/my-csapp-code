/*
linux> ./hostinfo baidu.com
110.242.74.102
111.63.65.103
124.237.177.164
111.63.65.247
*/
#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#define MAXLINE 8192

int main(int argc, char** argv) {
    struct addrinfo *p, *listp, hints;
    char buf[MAXLINE];
    int rc, flags;

    if (argc != 2) {
        fprintf(stderr, "usage: %s <domain name>\n", argv[0]);
        exit(EXIT_SUCCESS);
    }

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET; // Ipv4
    hints.ai_socktype = SOCK_STREAM;
    if ((rc = getaddrinfo(argv[1], NULL, &hints, &listp)) < 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(rc));
        exit(EXIT_FAILURE);
    }

    flags = NI_NUMERICHOST; // 展示IP地址而不是域名
    for (p = listp; p; p = p->ai_next) {
        if ((rc = getnameinfo(p->ai_addr, p->ai_addrlen, buf, MAXLINE, NULL, 0, flags)) < 0) {
            fprintf(stderr, "getnameinfo error: %s\n", gai_strerror(rc));
            exit(EXIT_SUCCESS);
        } else {
            printf("%s\n", buf);
        }
    }

    freeaddrinfo(listp);

    return 0;
}