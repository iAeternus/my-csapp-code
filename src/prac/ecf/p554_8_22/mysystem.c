#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int mysystem(char* command) {
    if (command == NULL) {
        return 1;
    }

    pid_t pid;
    int status;

    if ((pid = fork()) < 0) {
        return -1;
    }

    if (pid == 0) {
        execl("/bin/sh", "sh", "-c", command, NULL);

        _exit(127);
    }

    if (waitpid(pid, &status, 0) < 0) {
        return -1;
    }

    if (WIFEXITED(status)) {
        return WEXITSTATUS(status);
    }

    return status;
}

int main() {
    printf("%d\n", mysystem("exit 8"));
    printf("%d\n", mysystem("nonexistent_command"));
    printf("%d\n", mysystem("/bin/ls"));
    return 0;
}