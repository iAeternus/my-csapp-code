#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAXARGS 128
#define MAXLINE 128

extern char** environ;

void eval(char* cmdline);
int parseline(char* buf, char** argv);
int builtin_command(char** argv);

int main() {
    char cmdline[MAXLINE];

    while (1) {
        printf("> ");
        fgets(cmdline, MAXLINE, stdin);
        if (feof(stdin)) {
            exit(EXIT_SUCCESS);
        }

        eval(cmdline);
    }

    return 0;
}

static void unix_error(char* msg) {
    fprintf(stderr, "%s: %s\n", msg, strerror(errno));
    exit(EXIT_SUCCESS);
}

void eval(char* cmdline) {
    char* argv[MAXARGS];
    char buf[MAXLINE];
    int bg; // is background job
    pid_t pid;

    strcpy(buf, cmdline);
    bg = parseline(buf, argv);
    if (argv[0] == NULL) {
        return; // empty lines
    }

    if (!builtin_command(argv)) {
        if ((pid = fork()) == 0) {
            if (execvp(argv[0], argv) < 0) {
                printf("%s: Command not found.\n", argv[0]);
                exit(EXIT_SUCCESS);
            }
        }

        if (!bg) {
            int status;
            if (waitpid(pid, &status, 0) < 0) {
                unix_error("waitfg: waitpid error");
            }
        } else {
            printf("%d %s", pid, cmdline);
        }
    }
}

int builtin_command(char** argv) {
    if (!strcmp(argv[0], "exit")) {
        exit(EXIT_SUCCESS);
    }
    if (!strcmp(argv[0], "&")) {
        return 1;
    }
    return 0;
}

int parseline(char* buf, char** argv) {
    char* delim; // Points to first space delimiter
    int argc;
    int bg;

    buf[strlen(buf) - 1] = ' ';
    while (*buf && (*buf == ' ')) buf++;

    argc = 0;
    while ((delim = strchr(buf, ' '))) {
        argv[argc++] = buf;
        *delim = '\0';
        buf = delim + 1;
        while (*buf && (*buf == ' ')) buf++;
    }
    argv[argc] = NULL;

    if (argc == 0) {
        return 1;
    }

    if ((bg = (*argv[argc - 1] == '&')) != 0) {
        argv[--argc] = NULL;
    }

    return bg;
}