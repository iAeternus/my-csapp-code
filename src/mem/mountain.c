#define _POSIX_C_SOURCE 200809L
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MINBYTES (1 << 14) // 16KB
#define MAXBYTES (1 << 26) // 128MB
#define MAXSTRIDE 12
#define MAXELEMS (MAXBYTES / sizeof(long))

#define WARMUP 2
#define REPEAT 5
#define LOOP 50

long data[MAXELEMS];

static inline long long nano_time() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);

    return (long long)ts.tv_sec * 1000000000LL + ts.tv_nsec;
}

static inline int min(int a, int b) {
    return a < b ? a : b;
}

void init_data(long* data, int n) {
    for (int i = 0; i < n; i++) {
        data[i] = i;
    }
}

// 4 * 4 循环展开
void test(int elems, int stride) {
    long i, sx2 = stride * 2, sx3 = stride * 3, sx4 = stride * 4;
    long acc0 = 0, acc1 = 0, acc2 = 0, acc3 = 0;
    long length = elems;
    long limit = length - sx4;
    volatile long sink;

    for (i = 0; i < limit; i += sx4) {
        acc0 += data[i];
        acc1 += data[i + stride];
        acc2 += data[i + sx2];
        acc3 += data[i + sx3];
    }

    for (; i < length; i += stride) {
        acc0 += data[i];
    }
    sink = (acc0 + acc1) + (acc2 + acc3); // 防优化
}

double run(int size, int stride) {
    int elems = size / sizeof(long);
    long long best = LLONG_MAX;

    // warm-up
    for (int t = 0; t < WARMUP; t++) {
        for (int i = 0; i < LOOP; i++) {
            test(elems, stride);
        }
    }

    // measure
    for (int t = 0; t < REPEAT; t++) {
        long long start = nano_time();
        for (int i = 0; i < LOOP; i++) {
            test(elems, stride);
        }
        long long end = nano_time();

        best = min(best, end - start);
    }

    double seconds = best / 1e9;

    // 实际访问数据量（stride 越大访问越少）
    double bytes = (double)size / stride * LOOP;

    return (bytes / seconds) / (1024 * 1024); // MB/s
}

void export_csv(const char* filename) {
    FILE* fp = fopen(filename, "w");
    if (!fp) {
        perror("fopen");
        exit(1);
    }

    fprintf(fp, "size,stride,bandwidth_MBps\n");

    for (int size = MAXBYTES; size >= MINBYTES; size >>= 1) {
        for (int stride = 1; stride <= MAXSTRIDE; stride++) {
            double bw = run(size, stride);
            fprintf(fp, "%d,%d,%.4f\n", size, stride, bw);
        }
    }

    fclose(fp);
}

int main() {
    init_data(data, MAXELEMS);
    export_csv("results/memory_mountain.csv");
    printf("Done: results/memory_mountain.csv\n");
    return 0;
}