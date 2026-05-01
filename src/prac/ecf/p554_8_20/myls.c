#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char** argv, char** envp) {
    if (execve("/bin/ls", argv, envp) < 0) {
        printf("%s: Command not found.\n", argv[0]);
        exit(EXIT_SUCCESS);
    }
    return 0;
}