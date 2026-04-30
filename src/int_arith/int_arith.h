#ifndef INT_ARITH_H
#define INT_ARITH_H

#include "type.h"

#include <stdbool.h>

i32 i32_add_wrapping(i32 a, i32 b);
i32 i32_add_overflowing(i32 a, i32 b, bool* overflow);
bool i32_add_checked(i32 a, i32 b, i32* out);
i32 i32_add_saturating(i32 a, i32 b);

i32 i32_sub_wrapping(i32 a, i32 b);
i32 i32_sub_overflowing(i32 a, i32 b, bool* overflow);
bool i32_sub_checked(i32 a, i32 b, i32* out);
i32 i32_sub_saturating(i32 a, i32 b);

i32 i32_mul_wrapping(i32 a, i32 b);
i32 i32_mul_overflowing(i32 a, i32 b, bool* overflow);
bool i32_mul_checked(i32 a, i32 b, i32* out);
i32 i32_mul_saturating(i32 a, i32 b);

/*
 * 特殊情况：
 * - b == 0 -> overflow / checked = false
 * - INT32_MIN / -1 -> overflow
 */
i32 i32_div_overflowing(i32 a, i32 b, bool* overflow);
bool i32_div_checked(i32 a, i32 b, i32* out);
i32 i32_div_saturating(i32 a, i32 b);

/*
 * - b == 0 -> overflow / checked = false
 */
i32 i32_mod_overflowing(i32 a, i32 b, bool* overflow);
bool i32_mod_checked(i32 a, i32 b, i32* out);
i32 i32_mod_saturating(i32 a, i32 b);

#endif // INT_ARITH_H