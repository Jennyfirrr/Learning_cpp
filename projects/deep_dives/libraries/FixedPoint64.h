//======================================================================================================
// [FIXED-POINT ARITHMETIC LIBRARY - 64-BIT FRACTIONAL PART]
//======================================================================================================
// Q64.64 format: sign-magnitude with __uint128_t magnitude, separate sign flag
// integer range: 0 to ~18.4 quintillion (full uint64_t range in the integer part)
// fractional precision: ~19 decimal digits
// mul uses partial products since theres no native 256-bit type
//======================================================================================================
#ifndef SST_FIXED_POINT_64_H
#define SST_FIXED_POINT_64_H

#define SST_FP64_FRAC_BITS 64

#include <stdint.h>
#include <assert.h>
#include <math.h>

//======================================================================================================
// [FIXED-POINT NUMBER REPRESENTATION]
//======================================================================================================
typedef struct {
    __uint128_t magnitude;
    int32_t sign; // 0 = positive/zero, 1 = negative
} SST_FP64;
static_assert(sizeof(SST_FP64) == 32, "SST_FP64 must be 32 bytes");
//======================================================================================================
// [DOUBLE PRECISION FIXED-POINT ARITHMETIC]
//======================================================================================================
static inline SST_FP64 SST_FP64_FromDouble(double input) {
    int32_t neg      = (input < 0.0);
    double abs_input = input * (1.0 - 2.0 * neg);
    SST_FP64 result;

    double int_part  = floor(abs_input);
    double frac_part = abs_input - int_part;
    __uint128_t hi   = (__uint128_t)(uint64_t)int_part << SST_FP64_FRAC_BITS;
    __uint128_t lo   = (__uint128_t)(uint64_t)(frac_part * 18446744073709551616.0);
    result.magnitude = hi + lo;
    result.sign      = neg & (result.magnitude != 0);
    return result;
}

static inline double SST_FP64_ToDouble(SST_FP64 value) {
    uint64_t int_part  = (uint64_t)(value.magnitude >> SST_FP64_FRAC_BITS);
    uint64_t frac_part = (uint64_t)value.magnitude;
    double mag         = (double)int_part + (double)frac_part / 18446744073709551616.0;
    return mag * (1.0 - 2.0 * value.sign);
}
//======================================================================================================
// [GUARDS]
//======================================================================================================
static inline SST_FP64 SST_FP64_Min(SST_FP64 a, SST_FP64 b) {
    int diff_sign = a.sign ^ b.sign;
    int a_lt_same = ((a.magnitude < b.magnitude) ^ a.sign) & (a.magnitude != b.magnitude);
    int a_lt      = (a.sign & diff_sign) | (a_lt_same & !diff_sign);
    a_lt &= !((a.magnitude == 0) & (b.magnitude == 0));
    return a_lt ? a : b;
}

static inline SST_FP64 SST_FP64_Max(SST_FP64 a, SST_FP64 b) {
    int diff_sign = a.sign ^ b.sign;
    int a_gt_same = ((a.magnitude > b.magnitude) ^ a.sign) & (a.magnitude != b.magnitude);
    int a_gt      = ((!a.sign) & diff_sign) | (a_gt_same & (!diff_sign));
    return a_gt ? a : b;
}
//======================================================================================================
// [FIXED-POINT ARITHMETIC OPERATIONS]
//======================================================================================================
static inline SST_FP64 SST_FP64_AddSat(SST_FP64 a, SST_FP64 b) {
    int diff = a.sign ^ b.sign;

    // split into 64-bit halves so GCC can use cmov/sbb instead of branching on __uint128_t
    uint64_t a_lo = (uint64_t)a.magnitude, a_hi = (uint64_t)(a.magnitude >> 64);
    uint64_t b_lo = (uint64_t)b.magnitude, b_hi = (uint64_t)(b.magnitude >> 64);

    // same sign path: add magnitudes
    uint64_t sum_lo       = a_lo + b_lo;
    uint64_t carry        = (sum_lo < a_lo);
    uint64_t sum_hi       = a_hi + b_hi + carry;
    uint64_t sum_overflow = (sum_hi < a_hi) | ((sum_hi == a_hi) & carry & (b_hi != 0));
    // simpler overflow: the 128-bit add overflowed if the high part wrapped
    sum_overflow = (sum_hi < a_hi + carry) | ((a_hi + carry < a_hi) & (sum_hi <= b_hi));
    // just check carry out of the full 128-bit add
    __uint128_t sum_full = a.magnitude + b.magnitude;
    sum_overflow         = (sum_full < a.magnitude);
    uint64_t sat_mask    = -(uint64_t)sum_overflow;
    uint64_t sat_lo      = sum_lo | sat_mask;
    uint64_t sat_hi      = sum_hi | sat_mask;

    // different sign path: both subtractions computed, mask selects
    int a_ge         = (a_hi > b_hi) | ((a_hi == b_hi) & (a_lo >= b_lo));
    uint64_t ge_mask = -(uint64_t)a_ge;

    uint64_t borrow_ab = (a_lo < b_lo);
    uint64_t dab_lo    = a_lo - b_lo;
    uint64_t dab_hi    = a_hi - b_hi - borrow_ab;

    uint64_t borrow_ba = (b_lo < a_lo);
    uint64_t dba_lo    = b_lo - a_lo;
    uint64_t dba_hi    = b_hi - a_hi - borrow_ba;

    uint64_t diff_lo = (dab_lo & ge_mask) | (dba_lo & ~ge_mask);
    uint64_t diff_hi = (dab_hi & ge_mask) | (dba_hi & ~ge_mask);

    // select between same-sign (sat_sum) and diff-sign (diff) paths
    uint64_t d_mask = -(uint64_t)diff;
    uint64_t r_lo   = (sat_lo & ~d_mask) | (diff_lo & d_mask);
    uint64_t r_hi   = (sat_hi & ~d_mask) | (diff_hi & d_mask);

    // select sign
    int diff_sign_result = (a.sign & a_ge) | (b.sign & (1 - a_ge));
    int result_sign      = (a.sign & (!diff)) | (diff_sign_result & diff);
    result_sign &= ((r_lo | r_hi) != 0);

    __uint128_t result_mag = ((__uint128_t)r_hi << 64) | r_lo;
    return (SST_FP64){.magnitude = result_mag, .sign = result_sign};
}

static inline SST_FP64 SST_FP64_SubSat(SST_FP64 a, SST_FP64 b) {
    SST_FP64 neg_b = {.magnitude = b.magnitude, .sign = b.sign ^ (b.magnitude != 0)};
    return SST_FP64_AddSat(a, neg_b);
}
//======================================================================================================
// [MULTIPLY - PARTIAL PRODUCT APPROACH]
//======================================================================================================
// split each __uint128_t into two uint64 halves and do 4 partial multiplies
// then shift right by 64 frac bits - we need bits [191:64] of the full 256-bit product
// sign is just XOR of the two sign flags
//======================================================================================================
static inline SST_FP64 SST_FP64_Mul(SST_FP64 a, SST_FP64 b) {
    SST_FP64 result;

    uint64_t a_lo = (uint64_t)a.magnitude;
    uint64_t a_hi = (uint64_t)(a.magnitude >> 64);
    uint64_t b_lo = (uint64_t)b.magnitude;
    uint64_t b_hi = (uint64_t)(b.magnitude >> 64);

    __uint128_t ll = (__uint128_t)a_lo * b_lo;
    __uint128_t lh = (__uint128_t)a_lo * b_hi;
    __uint128_t hl = (__uint128_t)a_hi * b_lo;
    __uint128_t hh = (__uint128_t)a_hi * b_hi;

    // we want bits [191:64] of the 256-bit result
    __uint128_t mid     = lh + hl + (ll >> 64);
    __uint128_t shifted = hh + (mid >> 64);

    __uint128_t result_mag = (shifted << 64) | ((uint64_t)mid);

    // overflow: if shifted has bits above 127, saturate
    int overflow        = (shifted >> 64) != 0;
    __uint128_t of_mask = -(__uint128_t)overflow;
    result.magnitude    = (result_mag & ~of_mask) | ((__uint128_t)(-1) & of_mask);

    result.sign = (a.sign ^ b.sign) & (result.magnitude != 0);
    return result;
}
//======================================================================================================
// [DIVISION]
//======================================================================================================
// goes through long double since theres no 256-bit / 128-bit hardware divide
// long double on x86-64 is 80-bit extended precision (~18-19 significant digits)
// sign is just XOR of sign flags
//======================================================================================================
static inline SST_FP64 SST_FP64_DivNoAssert(SST_FP64 a, SST_FP64 b) {
    SST_FP64 result;
    if (b.magnitude == 0) {
        result.magnitude = (__uint128_t)(-1);
        result.sign      = a.sign;
        return result;
    }
    long double fa = (long double)SST_FP64_ToDouble(a);
    long double fb = (long double)SST_FP64_ToDouble(b);
    // compute absolute quotient through doubles, apply sign separately
    if (fa < 0)
        fa = -fa;
    if (fb < 0)
        fb = -fb;
    long double fq = fa / fb;
    result         = SST_FP64_FromDouble((double)fq);
    result.sign    = (a.sign ^ b.sign) & (result.magnitude != 0);
    return result;
}

static inline SST_FP64 SST_FP64_DivWithAssert(SST_FP64 a, SST_FP64 b) {
    assert(b.magnitude != 0);
    return SST_FP64_DivNoAssert(a, b);
}

//======================================================================================================
// [FIXED-POINT COMPARISON OPERATIONS]
//======================================================================================================
static inline int SST_FP64_Equal(SST_FP64 a, SST_FP64 b) {
    int both_zero = (a.magnitude == 0) & (b.magnitude == 0);
    return both_zero | ((a.magnitude == b.magnitude) & (a.sign == b.sign));
}

static inline int SST_FP64_NotEqual(SST_FP64 a, SST_FP64 b) {
    return !SST_FP64_Equal(a, b);
}

static inline int SST_FP64_LessThan(SST_FP64 a, SST_FP64 b) {
    int both_zero   = (a.magnitude == 0) & (b.magnitude == 0);
    int diff_sign   = a.sign ^ b.sign;
    int diff_result = a.sign & !both_zero;
    int same_result = ((a.magnitude < b.magnitude) ^ a.sign) & (a.magnitude != b.magnitude);
    return (diff_result & diff_sign) | (same_result & !diff_sign);
}

static inline int SST_FP64_LessThanOrEqual(SST_FP64 a, SST_FP64 b) {
    return SST_FP64_LessThan(a, b) | SST_FP64_Equal(a, b);
}

static inline int SST_FP64_GreaterThan(SST_FP64 a, SST_FP64 b) {
    return SST_FP64_LessThan(b, a);
}

static inline int SST_FP64_GreaterThanOrEqual(SST_FP64 a, SST_FP64 b) {
    return !SST_FP64_LessThan(a, b);
}
//======================================================================================================
// [FIXED-POINT UTILITY FUNCTIONS]
//======================================================================================================
static inline SST_FP64 SST_FP64_Negate(SST_FP64 value) {
    return (SST_FP64){.magnitude = value.magnitude, .sign = value.sign ^ (value.magnitude != 0)};
}

static inline int SST_FP64_IsZero(SST_FP64 value) {
    return value.magnitude == 0;
}

static inline SST_FP64 SST_FP64_Abs(SST_FP64 value) {
    return (SST_FP64){.magnitude = value.magnitude, .sign = 0};
}

static inline SST_FP64 SST_FP64_Sign(SST_FP64 value) {
    __uint128_t one = (__uint128_t)1 << SST_FP64_FRAC_BITS;
    int is_nonzero  = (value.magnitude != 0);
    __uint128_t mag = is_nonzero ? one : 0;
    return (SST_FP64){.magnitude = mag, .sign = value.sign & is_nonzero};
}
//======================================================================================================
// [FIXED-POINT MATH FUNCTIONS]
//======================================================================================================
static inline SST_FP64 SST_FP64_Sqrt(SST_FP64 value) {
    assert(value.sign == 0 || value.magnitude == 0);
    return SST_FP64_FromDouble(sqrt(SST_FP64_ToDouble(value)));
}

static inline SST_FP64 SST_FP64_InvSqrt(SST_FP64 value) {
    assert(value.sign == 0 && value.magnitude != 0);
    double x = SST_FP64_ToDouble(value);
    return SST_FP64_FromDouble(1.0 / sqrt(x));
}

static inline SST_FP64 SST_FP64_Sin(SST_FP64 value) {
    return SST_FP64_FromDouble(sin(SST_FP64_ToDouble(value)));
}

static inline SST_FP64 SST_FP64_Cos(SST_FP64 value) {
    return SST_FP64_FromDouble(cos(SST_FP64_ToDouble(value)));
}

static inline SST_FP64 SST_FP64_Tan(SST_FP64 value) {
    return SST_FP64_FromDouble(tan(SST_FP64_ToDouble(value)));
}

static inline SST_FP64 SST_FP64_Atan2(SST_FP64 y, SST_FP64 x) {
    return SST_FP64_FromDouble(atan2(SST_FP64_ToDouble(y), SST_FP64_ToDouble(x)));
}

static inline SST_FP64 SST_FP64_Exp(SST_FP64 value) {
    return SST_FP64_FromDouble(exp(SST_FP64_ToDouble(value)));
}

static inline SST_FP64 SST_FP64_Log(SST_FP64 value) {
    assert(value.sign == 0 && value.magnitude != 0);
    return SST_FP64_FromDouble(log(SST_FP64_ToDouble(value)));
}

static inline SST_FP64 SST_FP64_Pow(SST_FP64 base, SST_FP64 exponent) {
    return SST_FP64_FromDouble(pow(SST_FP64_ToDouble(base), SST_FP64_ToDouble(exponent)));
}
//======================================================================================================
// [FIXED-POINT MISCELLANEOUS FUNCTIONS]
//======================================================================================================
static inline SST_FP64 SST_FP64_Floor(SST_FP64 value) {
    __uint128_t frac_mask = ((__uint128_t)1 << SST_FP64_FRAC_BITS) - 1;
    int has_frac          = (value.magnitude & frac_mask) != 0;
    __uint128_t trunc     = value.magnitude & ~frac_mask;
    __uint128_t bump      = (__uint128_t)(value.sign & has_frac) << SST_FP64_FRAC_BITS;
    return (SST_FP64){.magnitude = trunc + bump, .sign = value.sign};
}

static inline SST_FP64 SST_FP64_Ceil(SST_FP64 value) {
    __uint128_t frac_mask  = ((__uint128_t)1 << SST_FP64_FRAC_BITS) - 1;
    int has_frac           = (value.magnitude & frac_mask) != 0;
    __uint128_t trunc      = value.magnitude & ~frac_mask;
    __uint128_t bump       = (__uint128_t)((!value.sign) & has_frac) << SST_FP64_FRAC_BITS;
    __uint128_t result_mag = trunc + bump;
    int result_sign        = value.sign & (result_mag != 0);
    return (SST_FP64){.magnitude = result_mag, .sign = result_sign};
}

static inline SST_FP64 SST_FP64_Round(SST_FP64 value) {
    __uint128_t frac_mask  = ((__uint128_t)1 << SST_FP64_FRAC_BITS) - 1;
    __uint128_t half       = (__uint128_t)1 << (SST_FP64_FRAC_BITS - 1);
    __uint128_t frac       = value.magnitude & frac_mask;
    __uint128_t trunc      = value.magnitude & ~frac_mask;
    int do_bump            = (frac >= half);
    __uint128_t bump       = (__uint128_t)do_bump << SST_FP64_FRAC_BITS;
    __uint128_t result_mag = trunc + bump;
    int result_sign        = value.sign & (result_mag != 0);
    return (SST_FP64){.magnitude = result_mag, .sign = result_sign};
}

static inline SST_FP64 SST_FP64_Mod(SST_FP64 a, SST_FP64 b) {
    assert(b.magnitude != 0);
    SST_FP64 quotient     = SST_FP64_DivNoAssert(a, b);
    __uint128_t frac_mask = ((__uint128_t)1 << SST_FP64_FRAC_BITS) - 1;
    SST_FP64 truncated    = {.magnitude = quotient.magnitude & ~frac_mask, .sign = quotient.sign};
    return SST_FP64_SubSat(a, SST_FP64_Mul(truncated, b));
}

static inline SST_FP64 SST_FP64_Lerp(SST_FP64 a, SST_FP64 b, SST_FP64 t) {
    SST_FP64 diff   = SST_FP64_SubSat(b, a);
    SST_FP64 scaled = SST_FP64_Mul(diff, t);
    return SST_FP64_AddSat(a, scaled);
}

static inline SST_FP64 SST_FP64_SmoothStep(SST_FP64 edge0, SST_FP64 edge1, SST_FP64 x) {
    if (SST_FP64_LessThanOrEqual(x, edge0))
        return (SST_FP64){.magnitude = 0, .sign = 0};
    if (SST_FP64_GreaterThanOrEqual(x, edge1))
        return (SST_FP64){.magnitude = (__uint128_t)1 << SST_FP64_FRAC_BITS, .sign = 0};

    SST_FP64 t = SST_FP64_DivNoAssert(SST_FP64_SubSat(x, edge0), SST_FP64_SubSat(edge1, edge0));

    SST_FP64 three = {.magnitude = (__uint128_t)3 << SST_FP64_FRAC_BITS, .sign = 0};
    SST_FP64 two   = {.magnitude = (__uint128_t)2 << SST_FP64_FRAC_BITS, .sign = 0};
    return SST_FP64_Mul(SST_FP64_Mul(t, t), SST_FP64_SubSat(three, SST_FP64_Mul(two, t)));
}
//======================================================================================================
//======================================================================================================

//======================================================================================================
#endif // SST_FIXED_POINT_64_H
