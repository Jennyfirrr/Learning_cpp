#include <stdio.h>
#include "FixedPoint32.h"

int main() {
    // Test addition
    SST_FP32 a   = SST_FP32_FromDouble(1.5);
    SST_FP32 b   = SST_FP32_FromDouble(2.5);
    SST_FP32 sum = SST_FP32_AddSat(a, b);
    printf("1.5 + 2.5 = %f\n", SST_FP32_ToDouble(sum)); // Should print 4.0

    // Test subtraction
    SST_FP32 diff = SST_FP32_SubSat(a, b);
    printf("1.5 - 2.5 = %f\n", SST_FP32_ToDouble(diff)); // Should print -1.0

    // Test saturation
    SST_FP32 max_val = {.raw_value = INT64_MAX};
    SST_FP32 min_val = {.raw_value = INT64_MIN};

    SST_FP32 overflow_sum = SST_FP32_AddSat(max_val, a);
    printf("INT64_MAX + 1.5 (saturated) = %f\n", SST_FP32_ToDouble(overflow_sum)); // Should print max value

    SST_FP32 underflow_diff = SST_FP32_SubSat(min_val, a);
    printf("INT64_MIN - 1.5 (saturated) = %f\n", SST_FP32_ToDouble(underflow_diff)); // Should print min value

    SST_FP32 mul = SST_FP32_Mul(a, b);
    printf("1.5 * 2.5 = %f\n", SST_FP32_ToDouble(mul)); // Should print 3.75
    // Test division
    SST_FP32 div = SST_FP32_DivNoAssert(a, b);
    printf("1.5 / 2.5 = %f\n", SST_FP32_ToDouble(div)); // Should print 0.6
    // Test square root
    SST_FP32 sqrt_a = SST_FP32_Sqrt(a);
    printf("sqrt(1.5) = %f\n", SST_FP32_ToDouble(sqrt_a)); // Should print approximately 1.22474487139
    // Test power
    SST_FP32 pow_ab = SST_FP32_Pow(a, b);
    printf("1.5 ^ 2.5 = %f\n", SST_FP32_ToDouble(pow_ab)); // Should print approximately 2.75567596063
    // Test floor, ceil, and round
    SST_FP32 floor_a = SST_FP32_Floor(a);
    printf("floor(1.5) = %f\n", SST_FP32_ToDouble(floor_a)); // Should print 1.0
    // Test ceil
    SST_FP32 ceil_a = SST_FP32_Ceil(a);
    printf("ceil(1.5) = %f\n", SST_FP32_ToDouble(ceil_a)); // Should print 2.0
    // Test round
    SST_FP32 round_a = SST_FP32_Round(a);
    printf("round(1.5) = %f\n", SST_FP32_ToDouble(round_a)); // Should print 2.75567596063
    // Test mod
    SST_FP32 mod_ab = SST_FP32_Mod(a, b);
    printf("1.5 mod 2.5 = %f\n", SST_FP32_ToDouble(mod_ab)); // Should print 1.5
    // Test lerp
    SST_FP32 t       = SST_FP32_FromDouble(0.5);
    SST_FP32 lerp_ab = SST_FP32_Lerp(a, b, t);
    printf("lerp(1.5, 2.5, 0.5) = %f\n", SST_FP32_ToDouble(lerp_ab)); // Should print 2.75567596063
    // Test sign, abs, and negate
    SST_FP32 sign_a = SST_FP32_Sign(a);
    printf("sign(1.5) = %f\n", SST_FP32_ToDouble(sign_a)); // Should print 1.0
    SST_FP32 abs_diff = SST_FP32_Abs(diff);
    printf("abs(-1.0) = %f\n", SST_FP32_ToDouble(abs_diff)); // Should print 1.0
    SST_FP32 neg_a = SST_FP32_Negate(a);
    printf("negate(1.5) = %f\n", SST_FP32_ToDouble(neg_a)); // Should print -1.5
    // Test is zero
    SST_FP32 zero = {.raw_value = 0};
    printf("is_zero(0) = %d\n", SST_FP32_IsZero(zero)); // Should print 1 (true)
    SST_FP32 non_zero = {.raw_value = 12345};
    printf("is_zero(12345) = %d\n", SST_FP32_IsZero(non_zero)); // Should print 0 (false)
    // Test comparison
    printf("1.5 < 2.5 = %d\n", SST_FP32_LessThan(a, b)); // Should print 1 (true)
    // Test equality
    printf("1.5 == 1.5 = %d\n", SST_FP32_Equal(a, a)); // Should print 1 (true)
    printf("1.5 == 2.5 = %d\n", SST_FP32_Equal(a, b)); // Should print 0 (false)
    // Test not equal
    printf("1.5 != 2.5 = %d\n", SST_FP32_NotEqual(a, b)); // Should print 1 (true)
    // Test greater than
    printf("2.5 > 1.5 = %d\n", SST_FP32_GreaterThan(b, a)); // Should print 1 (true)
    // Test greater than or equal
    printf("1.5 >= 1.5 = %d\n", SST_FP32_GreaterThanOrEqual(a, a)); // Should print 1 (true)
    printf("1.5 >= 2.5 = %d\n", SST_FP32_GreaterThanOrEqual(a, b)); // Should print 0 (false)
    // Test less than or equal
    printf("1.5 <= 1.5 = %d\n", SST_FP32_LessThanOrEqual(a, a)); // Should print 1 (true)
    printf("1.5 <= 2.5 = %d\n", SST_FP32_LessThanOrEqual(a, b)); // Should print 1 (true)
    // Test sign of negative value
    SST_FP32 neg_b      = SST_FP32_Negate(b);
    SST_FP32 sign_neg_b = SST_FP32_Sign(neg_b);
    printf("sign(-2.5) = %f\n", SST_FP32_ToDouble(sign_neg_b)); // Should print -1.0
    // Test abs of negative value
    SST_FP32 abs_neg_b = SST_FP32_Abs(neg_b);
    printf("abs(-2.5) = %f\n", SST_FP32_ToDouble(abs_neg_b)); // Should print 2.75567596063
    // Test negate of negative value
    SST_FP32 neg_neg_b = SST_FP32_Negate(neg_b);
    printf("negate(-2.5) = %f\n", SST_FP32_ToDouble(neg_neg_b)); // Should print 2.5
    // Test floor of negative value
    SST_FP32 floor_neg_b = SST_FP32_Floor(neg_b);
    printf("floor(-2.5) = %f\n", SST_FP32_ToDouble(floor_neg_b)); // Should print -3.0
    // Test ceil of negative value
    SST_FP32 ceil_neg_b = SST_FP32_Ceil(neg_b);
    printf("ceil(-2.5) = %f\n", SST_FP32_ToDouble(ceil_neg_b)); // Should print -2.0
    // Test round of negative value
    SST_FP32 round_neg_b = SST_FP32_Round(neg_b);
    printf("round(-2.5) = %f\n", SST_FP32_ToDouble(round_neg_b)); // Should print -2.0
    // Test mod with negative value
    SST_FP32 mod_neg_b = SST_FP32_Mod(neg_b, a);
    printf("-2.5 mod 1.5 = %f\n", SST_FP32_ToDouble(mod_neg_b)); // Should print 1.0
    // Test lerp with negative
    SST_FP32 lerp_neg_b = SST_FP32_Lerp(a, neg_b, t);
    printf("lerp(1.5, -2.5, 0.5) = %f\n", SST_FP32_ToDouble(lerp_neg_b)); // Should print -
    // Test sign of is_zero
    SST_FP32 sign_zero = SST_FP32_Sign(zero);
    printf("sign(0) = %f\n", SST_FP32_ToDouble(sign_zero));

    return 0;
}
