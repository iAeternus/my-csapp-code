#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>

void* copy_elements(void* ele_src[], int ele_cnt, size_t ele_size);
void* copy_elements_safe(void* ele_src[], int ele_cnt, size_t ele_size);

int main() {
    // 恶意参数
    size_t ele_cnt = 1048577;
    size_t ele_size = 4096;
    printf("ele_cnt = %zu, ele_size = %zu\n", ele_cnt, ele_size);

    char** src = (char**)malloc(ele_cnt * sizeof(char*));
    if (!src) {
        printf("malloc src failed\n");
        return 1;
    }

    for (size_t i = 0; i < ele_cnt; i++) {
        src[i] = (char*)malloc(ele_size);
        if (!src[i]) {
            printf("malloc src[%zu] failed\n", i);
            return 1;
        }
        memset(src[i], 0xAA, ele_size);
    }

    printf("\n[TEST] copy_elements (unsafe)\n");
    void* result1 = copy_elements((void**)src, ele_cnt, ele_size);
    if (result1 == NULL) {
        printf("copy_elements failed\n");
    } else {
        printf("copy_elements success (possible memory corruption)\n");
        free(result1);
    }

    printf("\n[TEST] copy_elements_safe\n");
    void* result2 = copy_elements_safe((void**)src, ele_cnt, ele_size);
    if (result2 == NULL) {
        printf("copy_elements_safe correctly detected overflow\n");
    } else {
        printf("copy_elements_safe success\n");
        free(result2);
    }

    for (size_t i = 0; i < ele_cnt; i++) {
        free(src[i]);
    }
    free(src);

    return 0;
}

void* copy_elements(void* ele_src[], int ele_cnt, size_t ele_size) {
    void* result = malloc(ele_cnt * ele_size);
    if (result == NULL) {
        return NULL;
    }
    char* next = (char*)result;
    int i;
    for (i = 0; i < ele_cnt; ++i) {
        memcpy(next, ele_src[i], ele_size);
        next += ele_size;
    }
    return result;
}

void* copy_elements_safe(void* ele_src[], int ele_cnt, size_t ele_size) {
    uint64_t required_size = ele_cnt * (uint64_t)ele_size;
    size_t request_size = (size_t)required_size;
    if (required_size != request_size) {
        return NULL; // 溢出
    }
    void* result = malloc(request_size);
    if (result == NULL) {
        return NULL;
    }
    char* next = (char*)result;
    int i;
    for (i = 0; i < ele_cnt; ++i) {
        memcpy(next, ele_src[i], ele_size);
        next += ele_size;
    }
    return result;
}