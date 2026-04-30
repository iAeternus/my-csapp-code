#include "int_arith.h"

#include <limits.h>

static inline bool add_overflow_i32(i32 a, i32 b, i32 res) {
    return ((a ^ res) & (b ^ res)) < 0;
}

static inline bool sub_overflow_i32(i32 a, i32 b, i32 res) {
    return ((a ^ b) & (a ^ res)) < 0;
}

static inline bool mul_overflow_i32(i32 a, i32 b, i32* out) {
    i64 tmp = (i64)a * (i64)b;
    *out = (i32)tmp;
    return tmp > INT32_MAX || tmp < INT32_MIN;
}

i32 i32_add_wrapping(i32 a, i32 b) {
    return (i32)((u32)a + (u32)b);
}

i32 i32_add_overflowing(i32 a, i32 b, bool* overflow) {
    i32 res = (i32)((u32)a + (u32)b);
    *overflow = add_overflow_i32(a, b, res);
    return res;
}

bool i32_add_checked(i32 a, i32 b, i32* out) {
    bool of;
    i32 res = i32_add_overflowing(a, b, &of);
    if (of) return false;
    *out = res;
    return true;
}

i32 i32_add_saturating(i32 a, i32 b) {
    bool of;
    i32 res = i32_add_overflowing(a, b, &of);
    if (!of) return res;
    return a > 0 ? INT32_MAX : INT32_MIN;
}

i32 i32_sub_wrapping(i32 a, i32 b) {
    return (i32)((u32)a - (u32)b);
}

i32 i32_sub_overflowing(i32 a, i32 b, bool* overflow) {
    i32 res = (i32)((u32)a - (u32)b);
    *overflow = sub_overflow_i32(a, b, res);
    return res;
}

bool i32_sub_checked(i32 a, i32 b, i32* out) {
    bool of;
    i32 res = i32_sub_overflowing(a, b, &of);
    if (of) return false;
    *out = res;
    return true;
}

i32 i32_sub_saturating(i32 a, i32 b) {
    bool of;
    i32 res = i32_sub_overflowing(a, b, &of);
    if (!of) return res;
    return (a > 0) ? INT32_MAX : INT32_MIN;
}

i32 i32_mul_wrapping(i32 a, i32 b) {
    return (i32)((u32)a * (u32)b);
}

i32 i32_mul_overflowing(i32 a, i32 b, bool* overflow) {
    i32 res;
    *overflow = mul_overflow_i32(a, b, &res);
    return res;
}

bool i32_mul_checked(i32 a, i32 b, i32* out) {
    return !mul_overflow_i32(a, b, out);
}

i32 i32_mul_saturating(i32 a, i32 b) {
    i32 res;
    if (!mul_overflow_i32(a, b, &res)) {
        return res;
    }

    if ((a > 0 && b > 0) || (a < 0 && b < 0)) {
        return INT32_MAX;
    } else {
        return INT32_MIN;
    }
}

i32 i32_div_overflowing(i32 a, i32 b, bool* overflow) {
    if (b == 0) {
        *overflow = true;
        return 0;
    }

    if (a == INT32_MIN && b == -1) {
        *overflow = true;
        return INT32_MIN; // wrap 行为
    }

    *overflow = false;
    return a / b;
}

bool i32_div_checked(i32 a, i32 b, i32* out) {
    if (b == 0) return false;
    if (a == INT32_MIN && b == -1) return false;

    *out = a / b;
    return true;
}

i32 i32_div_saturating(i32 a, i32 b) {
    if (b == 0) return 0;

    if (a == INT32_MIN && b == -1) {
        return INT32_MAX;
    }

    return a / b;
}

i32 i32_mod_overflowing(i32 a, i32 b, bool* overflow) {
    if (b == 0) {
        *overflow = true;
        return 0;
    }

    *overflow = false;
    return a % b;
}

bool i32_mod_checked(i32 a, i32 b, i32* out) {
    if (b == 0) return false;

    *out = a % b;
    return true;
}

i32 i32_mod_saturating(i32 a, i32 b) {
    if (b == 0) return 0;
    return a % b;
}