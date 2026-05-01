#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

#define BUF_SIZE 1024
#define TIMEOUT 5

volatile sig_atomic_t timed_out;

void handler(int sig) {
    timed_out = 1;
}

// 5s后超时，若用户5s内不键入一个输入行，返回NULL，否则返回指向输入行的指针
char* tfgets(char* s, int n, FILE* stream) {
    struct sigaction sa, old_sa;

    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    sigaction(SIGALRM, &sa, &old_sa);
    alarm(TIMEOUT);
    char* ret = fgets(s, n, stream);
    alarm(0);
    sigaction(SIGALRM, &old_sa, NULL);

    if (timed_out) {
        return NULL;
    }
    return ret;
}

int main() {
    char buf[BUF_SIZE];

    if (tfgets(buf, BUF_SIZE, stdin) != NULL) {
        printf("%s", buf);
    } else {
        fprintf(stderr, "fgets failed or EOF\n");
    }

    return 0;
}
