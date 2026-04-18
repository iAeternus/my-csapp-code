#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <windows.h>

typedef int data_t;

typedef struct {
    long len;
    data_t* data;
} Vec;

Vec* new_vec(long len) {
    Vec* result = (Vec*)malloc(sizeof(Vec));
    data_t* data = NULL;
    if (!result) {
        return NULL;
    }
    result->len = len;
    if (len > 0) {
        data = (data_t*)calloc(len, sizeof(data_t));
        if (!data) {
            free(result);
            return NULL;
        }
    }
    result->data = data;
    return result;
}

Vec* from_array(data_t arr[], long len) {
    Vec* v = new_vec(len);
    for (long i = 0; i < len; i++) {
        v->data[i] = arr[i];
    }
    return v;
}

void destroy_vec(Vec** v) {
    free((*v)->data);
    free(*v);
    *v = NULL;
}

int get_vec_element(Vec* v, long index, data_t* dest) {
    if (index < 0 || index >= v->len) {
        return 0;
    }
    *dest = v->data[index];
    return 1;
}

long vec_length(Vec* v) {
    return v->len;
}

data_t* get_vec_start(Vec* v) {
    return v->data;
}

#define IDENT 1
#define OP *

typedef void (*combine_func)(Vec*, data_t*);

// 计算向量元素的乘积
void combine1(Vec* v, data_t* dest) {
    long i;
    *dest = IDENT;
    for (i = 0; i < vec_length(v); i++) {
        data_t val;
        get_vec_element(v, i, &val);
        *dest = *dest OP val;
    }
}

// 消除循环的低效率
void combine2(Vec* v, data_t* dest) {
    long i;
    long length = vec_length(v);

    *dest = IDENT;
    for (i = 0; i < length; i++) {
        data_t val;
        get_vec_element(v, i, &val);
        *dest = *dest OP val;
    }
}

// 消除循环中的过程调用，并没有性能提升
void combine3(Vec* v, data_t* dest) {
    long i;
    long length = vec_length(v);
    data_t* data = get_vec_start(v);

    *dest = IDENT;
    for (i = 0; i < length; i++) {
        *dest = *dest OP data[i];
    }
}

// 消除不必要的内存引用
void combine4(Vec* v, data_t* dest) {
    long i;
    long length = vec_length(v);
    data_t* data = get_vec_start(v);
    data_t acc = IDENT;

    for (i = 0; i < length; i++) {
        acc = acc OP data[i];
    }
    *dest = acc;
}

// 2x1 循环展开
void combine5(Vec* v, data_t* dest) {
    long i;
    long length = vec_length(v);
    long limit = length - 1;
    data_t* data = get_vec_start(v);
    data_t acc = IDENT;

    for (i = 0; i < limit; i += 2) {
        acc = (acc OP data[i])OP data[i + 1];
    }

    for (; i < length; i++) {
        acc = acc OP data[i];
    }
    *dest = acc;
}

// 1x2 循环展开
void combine6(Vec* v, data_t* dest) {
    long i;
    long length = vec_length(v);
    long limit = length - 1;
    data_t* data = get_vec_start(v);
    data_t acc = IDENT;

    for (i = 0; i < limit; i += 2) {
        // data[i] OP data[i + 1] 与 acc OP 无关，流水线并行度更高
        acc = acc OP(data[i] OP data[i + 1]);
    }

    for (; i < length; i++) {
        acc = acc OP data[i];
    }
    *dest = acc;
}

// 2x2循环展开
void combine7(Vec* v, data_t* dest) {
    long i;
    long length = vec_length(v);
    long limit = length - 1;
    data_t* data = get_vec_start(v);
    data_t acc0 = IDENT;
    data_t acc1 = IDENT;

    // 同时计算两个元素
    for (i = 0; i < limit; i += 2) {
        acc0 = acc0 OP data[i];
        acc1 = acc1 OP data[i + 1];
    }

    for (; i < length; i++) {
        acc0 = acc0 OP data[i];
    }
    *dest = acc0 OP acc1;
}

void test_combine() {
    data_t arr[] = {2, 5, 7, 14};
    long len = 4;
    Vec* v = from_array(arr, len);
    data_t result;

    combine1(v, &result);
    assert(result == 980);

    combine2(v, &result);
    assert(result == 980);

    combine3(v, &result);
    assert(result == 980);

    combine4(v, &result);
    assert(result == 980);

    combine5(v, &result);
    assert(result == 980);

    combine6(v, &result);
    assert(result == 980);

    combine7(v, &result);
    assert(result == 980);

    destroy_vec(&v);

    printf("All Tests Passed\n");
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

void bench_test_combine(long min_size, long max_size) {
    FILE* fp = fopen("combine_bench.csv", "w");
    if (!fp) {
        perror("fopen");
        return;
    }

    static combine_func funcs[] = {combine1, combine2, combine3,
                                   combine4, combine5, combine6, combine7};
    static const char* names[] = {"combine1", "combine2", "combine3",
                                  "combine4", "combine5", "combine6", "combine7"};

    int func_count = sizeof(funcs) / sizeof(funcs[0]);

    // CSV 表头
    fprintf(fp, "size");
    for (int i = 0; i < func_count; i++) {
        fprintf(fp, ",%s", names[i]);
    }
    fprintf(fp, "\n");

    // 数据规模增长
    for (long size = min_size; size <= max_size; size <<= 1) {
        Vec* v = new_vec(size);

        for (long i = 0; i < size; i++) {
            v->data[i] = (i % 10) + 1;
        }

        data_t result;
        double times[func_count];

        for (int i = 0; i < func_count; i++) {
            times[i] = 0;
        }

        // 每个函数测 3 次
        for (int trial = 0; trial < 3; trial++) {
            for (int f = 0; f < func_count; f++) {
                long long start = nano_time();
                funcs[f](v, &result);
                long long end = nano_time();

                times[f] += (end - start);

                // 防止被优化掉
                volatile data_t sink = result;
                (void)sink;
            }
        }

        // 写入 CSV
        fprintf(fp, "%ld", size);
        for (int i = 0; i < func_count; i++) {
            double avg_ms = (times[i] / 3.0) / 1e6;
            fprintf(fp, ",%.3f", avg_ms);
        }
        fprintf(fp, "\n");

        destroy_vec(&v);

        printf("size=%ld done\n", size);
    }

    fclose(fp);
    printf("Benchmark finished -> combine_bench.csv\n");
}

int main() {
    test_combine();
    bench_test_combine(1000, 100000000);
    return 0;
}