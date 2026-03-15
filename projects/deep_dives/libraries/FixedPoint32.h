//======================================================================================================
// [FIXED-POINT ARITHMETIC LIBRARY - 32-BIT FRACTIONAL PART]
//======================================================================================================
// sign-magnitude representation: full uint64_t range for magnitude, separate sign flag
// no two's complement asymmetry (no INT64_MIN edge case), mul/div just XOR signs
//======================================================================================================
#ifndef SST_FIXED_POINT_32_H
#define SST_FIXED_POINT_32_H

#define SST_FP32_FRAC_BITS 32

#include <stdint.h>
#include <assert.h>
#include <math.h>

//======================================================================================================
// [FIXED-POINT NUMBER REPRESENTATION]
//======================================================================================================
typedef struct {
    uint64_t magnitude;
    int32_t sign; // 0 = positive/zero, 1 = negative
} SST_FP32;
static_assert(sizeof(SST_FP32) == 16, "SST_FP32 must be 16 bytes");
//======================================================================================================
// [DOUBLE PRECISION FIXED-POINT ARITHMETIC]
//======================================================================================================
static inline SST_FP32 SST_FP32_FromDouble(double input) {
    int32_t neg = (input < 0.0);
    double abs_input = input * (1.0 - 2.0 * neg);
    SST_FP32 result;
    result.magnitude = (uint64_t)(abs_input * (1ULL << SST_FP32_FRAC_BITS));
    result.sign = neg & (result.magnitude != 0);
    return result;
}

static inline double SST_FP32_ToDouble(SST_FP32 value) {
    double mag = (double)value.magnitude / (1ULL << SST_FP32_FRAC_BITS);
    return mag * (1.0 - 2.0 * value.sign);
}
//======================================================================================================
// [GUARDS]
//======================================================================================================
static inline SST_FP32 SST_FP32_Min(SST_FP32 a, SST_FP32 b) {
    int diff_sign = a.sign ^ b.sign;
    int a_lt_same = ((a.magnitude < b.magnitude) ^ a.sign) & (a.magnitude != b.magnitude);
    int a_lt = (a.sign & diff_sign) | (a_lt_same & !diff_sign);
    // handle both zero
    a_lt &= !((a.magnitude == 0) & (b.magnitude == 0));
    uint64_t mask = -(uint64_t)a_lt;
    return (SST_FP32){
        .magnitude = (a.magnitude & mask) | (b.magnitude & ~mask),
        .sign = (a.sign & a_lt) | (b.sign & !a_lt)
    };
}

static inline SST_FP32 SST_FP32_Max(SST_FP32 a, SST_FP32 b) {
    int diff_sign = a.sign ^ b.sign;
    int a_gt_same = ((a.magnitude > b.magnitude) ^ a.sign) & (a.magnitude != b.magnitude);
    int a_gt = ((!a.sign) & diff_sign) | (a_gt_same & (!diff_sign));
    uint64_t mask = -(uint64_t)a_gt;
    return (SST_FP32){
        .magnitude = (a.magnitude & mask) | (b.magnitude & ~mask),
        .sign = (a.sign & a_gt) | (b.sign & !a_gt)
    };
}
//======================================================================================================
// [FIXED-POINT ARITHMETIC OPERATIONS]
//======================================================================================================
static inline SST_FP32 SST_FP32_AddSat(SST_FP32 a, SST_FP32 b) {
    int diff = a.sign ^ b.sign;

    // same sign: add magnitudes, check unsigned overflow
    uint64_t sum = a.magnitude + b.magnitude;
    uint64_t sum_overflow = (sum < a.magnitude);
    uint64_t sat_sum = sum | -(uint64_t)sum_overflow;

    // different sign: subtract (never overflows)
    int a_ge = (a.magnitude >= b.magnitude);
    uint64_t a_ge_mask = -(uint64_t)a_ge;
    uint64_t diff_val = ((a.magnitude - b.magnitude) & a_ge_mask) |
                        ((b.magnitude - a.magnitude) & ~a_ge_mask);

    uint64_t diff_mask = -(uint64_t)diff;
    uint64_t result_mag = (sat_sum & ~diff_mask) | (diff_val & diff_mask);

    int diff_sign_result = (a.sign & a_ge) | (b.sign & (1 - a_ge));
    int result_sign = (a.sign & !diff) | (diff_sign_result & diff);
    result_sign &= (result_mag != 0);

    return (SST_FP32){.magnitude = result_mag, .sign = result_sign};
}

static inline SST_FP32 SST_FP32_SubSat(SST_FP32 a, SST_FP32 b) {
    SST_FP32 neg_b = {.magnitude = b.magnitude, .sign = b.sign ^ (b.magnitude != 0)};
    return SST_FP32_AddSat(a, neg_b);
}

static inline SST_FP32 SST_FP32_Mul(SST_FP32 a, SST_FP32 b) {
    __uint128_t wide = (__uint128_t)a.magnitude * b.magnitude;
    __uint128_t shifted = wide >> SST_FP32_FRAC_BITS;

    // overflow if shifted doesnt fit in uint64_t
    uint64_t hi = (uint64_t)(shifted >> 64);
    int overflow = (hi != 0);
    uint64_t of_mask = -(uint64_t)overflow;
    uint64_t result_mag = ((uint64_t)shifted & ~of_mask) | (UINT64_MAX & of_mask);

    int result_sign = (a.sign ^ b.sign) & (result_mag != 0);
    return (SST_FP32){.magnitude = result_mag, .sign = result_sign};
}

static inline SST_FP32 SST_FP32_DivNoAssert(SST_FP32 a, SST_FP32 b) {
    int b_zero = (b.magnitude == 0);
    uint64_t safe_b = b.magnitude | (uint64_t)b_zero;
    __uint128_t wide = ((__uint128_t)a.magnitude << SST_FP32_FRAC_BITS) / safe_b;

    uint64_t hi = (uint64_t)(wide >> 64);
    int overflow = (hi != 0) | b_zero;
    uint64_t of_mask = -(uint64_t)overflow;
    uint64_t result_mag = ((uint64_t)wide & ~of_mask) | (UINT64_MAX & of_mask);

    int result_sign = ((a.sign ^ b.sign) & !b_zero) | (a.sign & b_zero);
    result_sign &= (result_mag != 0);
    return (SST_FP32){.magnitude = result_mag, .sign = result_sign};
}

static inline SST_FP32 SST_FP32_DivWithAssert(SST_FP32 a, SST_FP32 b) {
    assert(b.magnitude != 0);
    return SST_FP32_DivNoAssert(a, b);
}

//======================================================================================================
// [FIXED-POINT COMPARISON OPERATIONS]
//======================================================================================================
static inline int SST_FP32_Equal(SST_FP32 a, SST_FP32 b) {
    int both_zero = (a.magnitude == 0) & (b.magnitude == 0);
    return both_zero | ((a.magnitude == b.magnitude) & (a.sign == b.sign));
}

static inline int SST_FP32_NotEqual(SST_FP32 a, SST_FP32 b) {
    return !SST_FP32_Equal(a, b);
}

static inline int SST_FP32_LessThan(SST_FP32 a, SST_FP32 b) {
    int both_zero = (a.magnitude == 0) & (b.magnitude == 0);
    int diff_sign = a.sign ^ b.sign;
    int diff_result = a.sign & !both_zero;
    int same_result = ((a.magnitude < b.magnitude) ^ a.sign) & (a.magnitude != b.magnitude);
    return (diff_result & diff_sign) | (same_result & !diff_sign);
}

static inline int SST_FP32_LessThanOrEqual(SST_FP32 a, SST_FP32 b) {
    return SST_FP32_LessThan(a, b) | SST_FP32_Equal(a, b);
}

static inline int SST_FP32_GreaterThan(SST_FP32 a, SST_FP32 b) {
    return SST_FP32_LessThan(b, a);
}

static inline int SST_FP32_GreaterThanOrEqual(SST_FP32 a, SST_FP32 b) {
    return !SST_FP32_LessThan(a, b);
}
//======================================================================================================
// [FIXED-POINT UTILITY FUNCTIONS]
//======================================================================================================
static inline SST_FP32 SST_FP32_Negate(SST_FP32 value) {
    return (SST_FP32){.magnitude = value.magnitude, .sign = value.sign ^ (value.magnitude != 0)};
}

static inline int SST_FP32_IsZero(SST_FP32 value) {
    return value.magnitude == 0;
}

static inline SST_FP32 SST_FP32_Abs(SST_FP32 value) {
    return (SST_FP32){.magnitude = value.magnitude, .sign = 0};
}

static inline SST_FP32 SST_FP32_Sign(SST_FP32 value) {
    uint64_t one = 1ULL << SST_FP32_FRAC_BITS;
    uint64_t is_nonzero = (value.magnitude != 0);
    return (SST_FP32){.magnitude = one & -(uint64_t)is_nonzero, .sign = value.sign & (int)is_nonzero};
}
//======================================================================================================
// [FIXED-POINT MATH FUNCTIONS]
//======================================================================================================
// this one doesnt work
//======================================================================================================
static inline SST_FP32 SST_FP32_Sqrt(SST_FP32 value) {
    assert(value.sign == 0 || value.magnitude == 0);
    return SST_FP32_FromDouble(sqrt(SST_FP32_ToDouble(value)));
}

static inline SST_FP32 SST_FP32_InvSqrt(SST_FP32 value) {
    assert(value.sign == 0 && value.magnitude != 0);
    double x = SST_FP32_ToDouble(value);
    return SST_FP32_FromDouble(1.0 / sqrt(x));
}

static inline SST_FP32 SST_FP32_Sin(SST_FP32 value) {
    return SST_FP32_FromDouble(sin(SST_FP32_ToDouble(value)));
}

static inline SST_FP32 SST_FP32_Cos(SST_FP32 value) {
    return SST_FP32_FromDouble(cos(SST_FP32_ToDouble(value)));
}

static inline SST_FP32 SST_FP32_Tan(SST_FP32 value) {
    return SST_FP32_FromDouble(tan(SST_FP32_ToDouble(value)));
}

static inline SST_FP32 SST_FP32_Atan2(SST_FP32 y, SST_FP32 x) {
    return SST_FP32_FromDouble(atan2(SST_FP32_ToDouble(y), SST_FP32_ToDouble(x)));
}

static inline SST_FP32 SST_FP32_Exp(SST_FP32 value) {
    return SST_FP32_FromDouble(exp(SST_FP32_ToDouble(value)));
}

static inline SST_FP32 SST_FP32_Log(SST_FP32 value) {
    assert(value.sign == 0 && value.magnitude != 0);
    return SST_FP32_FromDouble(log(SST_FP32_ToDouble(value)));
}

static inline SST_FP32 SST_FP32_Pow(SST_FP32 base, SST_FP32 exponent) {
    return SST_FP32_FromDouble(pow(SST_FP32_ToDouble(base), SST_FP32_ToDouble(exponent)));
}
//======================================================================================================
// [FIXED-POINT MISCELLANEOUS FUNCTIONS]
//======================================================================================================

static inline SST_FP32 SST_FP32_Floor(SST_FP32 value) {
    uint64_t frac_mask = (1ULL << SST_FP32_FRAC_BITS) - 1;
    uint64_t has_frac = (value.magnitude & frac_mask) != 0;
    uint64_t trunc = value.magnitude & ~frac_mask;
    uint64_t bump = (uint64_t)(value.sign & (int)has_frac) << SST_FP32_FRAC_BITS;
    return (SST_FP32){.magnitude = trunc + bump, .sign = value.sign};
}

static inline SST_FP32 SST_FP32_Ceil(SST_FP32 value) {
    uint64_t frac_mask = (1ULL << SST_FP32_FRAC_BITS) - 1;
    uint64_t has_frac = (value.magnitude & frac_mask) != 0;
    uint64_t trunc = value.magnitude & ~frac_mask;
    uint64_t bump = (uint64_t)((!value.sign) & (int)has_frac) << SST_FP32_FRAC_BITS;
    uint64_t result_mag = trunc + bump;
    int result_sign = value.sign & (result_mag != 0);
    return (SST_FP32){.magnitude = result_mag, .sign = result_sign};
}

static inline SST_FP32 SST_FP32_Round(SST_FP32 value) {
    uint64_t frac_mask = (1ULL << SST_FP32_FRAC_BITS) - 1;
    uint64_t half = 1ULL << (SST_FP32_FRAC_BITS - 1);
    uint64_t frac = value.magnitude & frac_mask;
    uint64_t trunc = value.magnitude & ~frac_mask;
    uint64_t bump = (uint64_t)(frac >= half) << SST_FP32_FRAC_BITS;
    uint64_t result_mag = trunc + bump;
    int result_sign = value.sign & (result_mag != 0);
    return (SST_FP32){.magnitude = result_mag, .sign = result_sign};
}

static inline SST_FP32 SST_FP32_Mod(SST_FP32 a, SST_FP32 b) {
    assert(b.magnitude != 0);
    SST_FP32 quotient = SST_FP32_DivNoAssert(a, b);
    uint64_t frac_mask = (1ULL << SST_FP32_FRAC_BITS) - 1;
    SST_FP32 truncated = {.magnitude = quotient.magnitude & ~frac_mask, .sign = quotient.sign};
    return SST_FP32_SubSat(a, SST_FP32_Mul(truncated, b));
}

static inline SST_FP32 SST_FP32_Lerp(SST_FP32 a, SST_FP32 b, SST_FP32 t) {
    SST_FP32 diff   = SST_FP32_SubSat(b, a);
    SST_FP32 scaled = SST_FP32_Mul(diff, t);
    return SST_FP32_AddSat(a, scaled);
}

static inline SST_FP32 SST_FP32_SmoothStep(SST_FP32 edge0, SST_FP32 edge1, SST_FP32 x) {
    uint64_t mask0    = -(uint64_t)SST_FP32_LessThanOrEqual(x, edge0);
    uint64_t mask1    = -(uint64_t)SST_FP32_GreaterThanOrEqual(x, edge1);
    uint64_t in_range = ~(mask0 | mask1);

    SST_FP32 clamped = SST_FP32_Max(SST_FP32_Min(x, edge1), edge0);
    SST_FP32 t = SST_FP32_DivNoAssert(SST_FP32_SubSat(clamped, edge0), SST_FP32_SubSat(edge1, edge0));

    SST_FP32 three = {.magnitude = 3ULL << SST_FP32_FRAC_BITS, .sign = 0};
    SST_FP32 two   = {.magnitude = 2ULL << SST_FP32_FRAC_BITS, .sign = 0};
    SST_FP32 poly  = SST_FP32_Mul(SST_FP32_Mul(t, t), SST_FP32_SubSat(three, SST_FP32_Mul(two, t)));

    uint64_t one = 1ULL << SST_FP32_FRAC_BITS;
    uint64_t raw = (0 & mask0) | (one & mask1) | (poly.magnitude & in_range);
    return (SST_FP32){.magnitude = raw, .sign = 0};
}
//======================================================================================================
//======================================================================================================

//======================================================================================================
#endif // SST_FIXED_POINT_32_H
