#include <stdio.h>

typedef union {
    int i;
    unsigned char bytes[4];
} U;

int main() {
    U u;
    u.i = 0x12345678;

    printf("int = 0x%x\n", u.i);

    for (int j = 0; j < 4; j++) {
        printf("byte[%d] = 0x%x\n", j, u.bytes[j]);
    }

    // int = 0x12345678
    // byte[0] = 0x78
    // byte[1] = 0x56
    // byte[2] = 0x34
    // byte[3] = 0x12

    return 0;
}
