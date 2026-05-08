#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

int main(int argc, char** argv) {
    struct stat file_stat;
    char *type, *readok;

    if (stat(argv[1], &file_stat) < 0) {
        perror("stat error");
        exit(EXIT_FAILURE);
    }

    if (S_ISREG(file_stat.st_mode)) {
        type = "regular";
    } else if (S_ISDIR(file_stat.st_mode)) {
        type = "directory";
    } else {
        type = "other";
    }

    if (file_stat.st_mode & S_IRUSR) {
        readok = "yes";
    } else {
        readok = "no";
    }

    printf("type: %s, read: %s\n", type, readok);
    return 0;
}