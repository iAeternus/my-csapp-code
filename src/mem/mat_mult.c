#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define MINN 16
#define MAXN 700
#define INCN 16
#define WARMUP 2
#define REPEAT 5

// 是否清缓存
#define CLEARCACHE 0

typedef double Matrix[MAXN][MAXN];

Matrix ga, gb, gc;

void check_result(Matrix c, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (c[i][j] != (double)n) {
                printf("Error: bad number (%f) in result matrix (%d,%d)\n",
                       c[i][j], i, j);
                exit(0);
            }
        }
    }
}

void reset(Matrix c, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            c[i][j] = 0.0;
        }
    }
}

void init(Matrix a, Matrix b, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            a[i][j] = 1.0;
            b[i][j] = 1.0;
        }
    }
}

void ijk(Matrix a, Matrix b, Matrix c, int n) {
    int i, j, k;
    double sum;

    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            sum = 0.0;
            for (k = 0; k < n; k++) {
                sum += a[i][k] * b[k][j];
            }
            c[i][j] += sum;
        }
    }
}

void jik(Matrix a, Matrix b, Matrix c, int n) {
    int i, j, k;
    double sum;

    for (j = 0; j < n; j++) {
        for (i = 0; i < n; i++) {
            sum = 0.0;
            for (k = 0; k < n; k++) {
                sum += a[i][k] * b[k][j];
            }
            c[i][j] += sum;
        }
    }
}

void jki(Matrix a, Matrix b, Matrix c, int n) {
    int i, j, k;
    double r;

    for (j = 0; j < n; j++) {
        for (k = 0; k < n; k++) {
            r = b[k][j];
            for (i = 0; i < n; i++) {
                c[i][j] += a[i][k] * r;
            }
        }
    }
}

void kji(Matrix a, Matrix b, Matrix c, int n) {
    int i, j, k;
    double r;

    for (k = 0; k < n; k++) {
        for (j = 0; j < n; j++) {
            r = b[k][j];
            for (i = 0; i < n; i++) {
                c[i][j] += a[i][k] * r;
            }
        }
    }
}

void kij(Matrix a, Matrix b, Matrix c, int n) {
    int i, j, k;
    double r;

    for (k = 0; k < n; k++) {
        for (i = 0; i < n; i++) {
            r = a[i][k];
            for (j = 0; j < n; j++) {
                c[i][j] += r * b[k][j];
            }
        }
    }
}

void ikj(Matrix a, Matrix b, Matrix c, int n) {
    int i, j, k;
    double r;

    for (i = 0; i < n; i++) {
        for (k = 0; k < n; k++) {
            r = a[i][k];
            for (j = 0; j < n; j++) {
                c[i][j] += r * b[k][j];
            }
        }
    }
}

static inline long long nano_time() {
    static LARGE_INTEGER freq;
    static int initialized = 0;

    if (!initialized) {
        QueryPerformanceFrequency(&freq);
        initialized = 1;
    }

    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);

    return (long long)(now.QuadPart * 1000000000LL / freq.QuadPart);
}

double run(void (*f)(Matrix, Matrix, Matrix, int), int n) {
    long long best = LLONG_MAX;

    // warm up
    for (int t = 0; t < WARMUP; t++) {
        reset(gc, n);
        f(ga, gb, gc, n);
    }

    for (int t = 0; t < REPEAT; t++) {
        reset(gc, n);

        long long start = nano_time();
        f(ga, gb, gc, n);
        long long end = nano_time();

        if (end - start < best) {
            best = end - start;
        }
    }

    check_result(gc, n);

    return (double)best / (n * n * n);
}

int main() {
    int n;

    init(ga, gb, MAXN);

    FILE* fp = fopen("results/mat_mult.csv", "w");
    if (!fp) {
        perror("fopen");
        return 1;
    }

    fprintf(fp, "n,ijk,jik,jki,kji,kij,ikj\n");

    for (n = MINN; n <= MAXN; n += INCN) {
        double r1 = run(ijk, n);
        double r2 = run(jik, n);
        double r3 = run(jki, n);
        double r4 = run(kji, n);
        double r5 = run(kij, n);
        double r6 = run(ikj, n);

        fprintf(fp, "%d,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f\n",
                n, r1, r2, r3, r4, r5, r6);
    }

    fclose(fp);

    printf("Results written to results/mat_mult.csv\n");

    return 0;
}