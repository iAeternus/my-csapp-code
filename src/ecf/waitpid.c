#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#define N 2

void unix_error(char* msg) {
    fprintf(stderr, "%s: %s\n", msg, strerror(errno));
    exit(EXIT_SUCCESS);
}

pid_t fork_with_errhandle() {
    pid_t pid;
    if ((pid = fork()) < 0) {
        unix_error("fork error");
    }
    return pid;
}

int main() {
    int status, i;
    pid_t pids[N], retpid;

    for (i = 0; i < N; i++) {
        if ((pids[i] = fork_with_errhandle()) == 0) {
            exit(100 + i);
        }
    }

    i = 0;
    while ((retpid = waitpid(pids[i++], &status, 0)) > 0) {
        if (WIFEXITED(status)) {
            printf("child %d terminated normally with exit status=%d\n", retpid, WEXITSTATUS(status));
        } else {
            printf("child %d terminated abnormally\n", retpid);
        }
    }

    if (errno != ECHILD) {
        unix_error("waitpid error");
    }

    return 0;
}