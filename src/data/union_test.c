#include <stdio.h>

typedef union {
    int i;
    float f;
} U;

int main() {
    U u;

    u.i = 0x40490fdb;   // 这是一段比特（IEEE754中约等于3.1415926）
    printf("as int   = 0x%x\n", u.i);
    printf("as float = %f\n", u.f);

    // as int   = 0x40490fdb                                                                                                                        
    // as float = 3.141593

    return 0;
}
