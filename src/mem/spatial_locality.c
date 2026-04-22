#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <time.h>

#define N 2000000
#define REPEAT 50

typedef struct {
    int vel[3];
    int acc[3];
    char padding[64];
} Point;

typedef void (*clear_func)(Point*, int);

Point p[N];

void clear1(Point* p, int n) {
    int i, j;
    for (i = 0; i < n; i++) {
        for (j = 0; j < 3; j++) {
            p[i].vel[j] = 0;
        }
        for (j = 0; j < 3; j++) {
            p[i].acc[j] = 0;
        }
    }
}

void clear2(Point* p, int n) {
    int i, j;
    for (i = 0; i < n; i++) {
        for (j = 0; j < 3; j++) {
            p[i].vel[j] = 0;
            p[i].acc[j] = 0;
        }
    }
}

void clear3(Point* p, int n) {
    int i, j;
    for (j = 0; j < 3; j++) {
        for (i = 0; i < n; i++) {
            p[i].vel[j] = 0;
        }
        for (i = 0; i < n; i++) {
            p[i].acc[j] = 0;
        }
    }
}

static inline long long nano_time() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);

    return (long long)ts.tv_sec * 1000000000LL + ts.tv_nsec;
}

void bench_test_clear(const char* path, long min_size, long max_size) {
    FILE* fp = fopen(path, "w");
    if (!fp) {
        perror("fopen");
        return;
    }

    clear_func funcs[] = {clear1, clear2, clear3};
    const char* names[] = {"clear1", "clear2", "clear3"};
    int func_count = sizeof(funcs) / sizeof(funcs[0]);

    fprintf(fp, "size");
    for (int i = 0; i < func_count; i++) {
        fprintf(fp, ",%s", names[i]);
    }
    fprintf(fp, "\n");

    for (long size = min_size; size <= max_size; size <<= 1) {
        double times[3] = {0};

        for (int f = 0; f < func_count; f++) {
            for (int trial = 0; trial < 3; trial++) {
                for (long i = 0; i < size; i++) {
                    p[i].vel[0] = i;
                }

                long long start = nano_time();

                for (int r = 0; r < REPEAT; r++) {
                    funcs[f](p, size);
                }

                long long end = nano_time();

                times[f] += (end - start);

                volatile int sink = p[0].vel[0];
                (void)sink;
            }
        }

        fprintf(fp, "%ld", size);
        for (int i = 0; i < func_count; i++) {
            double avg_ms = (times[i] / 3.0) / 1e6;
            fprintf(fp, ",%.6f", avg_ms);
        }
        fprintf(fp, "\n");

        printf("size=%ld done\n", size);
    }

    fclose(fp);
    printf("Benchmark finished -> %s\n", path);
}

int main() {
    bench_test_clear("results/spatial_locality.csv", 1024, N);
    return 0;
}
