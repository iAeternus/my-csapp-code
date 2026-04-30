#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void unix_error(char* msg) {
    fprintf(stderr, "%s: %s\n", msg, strerror(errno));
    exit(EXIT_SUCCESS);
}

void handler(int sig) {
    return;
}

unsigned int snooze(unsigned int secs) {
    unsigned int rc = sleep(secs);
    printf("Slept for %d of %d secs\n", secs - rc, secs);
    return rc;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s <secs>\n", argv[0]);
        exit(EXIT_SUCCESS);
    }

    if (signal(SIGINT, handler) == SIG_ERR) {
        unix_error("signal error\n");
    }

    snooze(atoi(argv[1]));

    return 0;
}