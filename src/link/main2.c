#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>

int x[2] = {1, 2};
int y[2] = {3, 4};
int z[2];

int main() {
    void* handle;
    void (*addvec)(int*, int*, int*, int);
    char* error;

    handle = dlopen("./libvector.so", RTLD_LAZY); // 加载动态库
    if (!handle) {
        fprintf(stderr, "%s\n", dlerror());
        exit(EXIT_FAILURE);
    }

    addvec = dlsym(handle, "addvec"); // 函数名字符串获取函数指针
    if ((error = dlerror()) != NULL) {
        fprintf(stderr, "%s\n", dlerror());
        exit(EXIT_FAILURE);
    }

    addvec(x, y, z, 2); // 调用
    printf("z = [%d, %d]\n", z[0], z[1]); // z = [4, 6]

    if (dlclose(handle) < 0) {
        fprintf(stderr, "%s\n", dlerror());
        exit(EXIT_FAILURE);
    }

    return 0;
}
