#include <stddef.h>
#include <stdio.h>

int main() {
    size_t n = 5;
    int arr[] = {1, 2, 3, 4, 5};

    for (size_t i = n - 1; i < n; i--) {
        printf("%d ", arr[i]);
    }
    putchar('\n');

    return 0;
}