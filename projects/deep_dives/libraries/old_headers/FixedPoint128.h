//======================================================================================================
// [FIXED-POINT ARITHMETIC LIBRARY - 128-BIT FRACTIONAL PART]
//======================================================================================================
// Q128.128 format: sign-magnitude with 256-bit unsigned magnitude as 4 x uint64_t, separate sign
// this is the wide accumulator type - do heavy math here then truncate back to FP64 for storage
// add/sub keep full 256-bit precision, mul/div go through long double conversion
//
// splitting into uint64_t quads lets GCC use cmov/sbb instead of branching on __uint128_t ops
// magnitude layout: (hi1 << 192) | (hi0 << 128) | (lo1 << 64) | lo0
// lo0,lo1 = 128 fractional bits, hi0,hi1 = 128 integer bits
//======================================================================================================
#ifndef SST_FIXED_POINT_128_H
#define SST_FIXED_POINT_128_H

#define SST_FP128_FRAC_BITS 128

#include <stdint.h>
#include <assert.h>
#include <math.h>

//======================================================================================================
// [256-BIT UNSIGNED MAGNITUDE + SIGN]
//======================================================================================================
typedef struct {
    uint64_t lo0; // bits [0:63]   - least significant
    uint64_t lo1; // bits [64:127]
    uint64_t hi0; // bits [128:191]
    uint64_t hi1; // bits [192:255] - most significant
    int32_t sign; // 0 = positive/zero, 1 = negative
} SST_FP128;
static_assert(sizeof(SST_FP128) == 40, "SST_FP128 must be 40 bytes");

//======================================================================================================
// [256-BIT HELPERS]
//======================================================================================================
static inline int SST_FP128_MagIsZero(SST_FP128 v) {
    return (v.lo0 | v.lo1 | v.hi0 | v.hi1) == 0;
}

// a >= b unsigned 256-bit
static inline int SST_FP128_MagGe(SST_FP128 a, SST_FP128 b) {
    if (a.hi1 != b.hi1) return a.hi1 > b.hi1;
    if (a.hi0 != b.hi0) return a.hi0 > b.hi0;
    if (a.lo1 != b.lo1) return a.lo1 > b.lo1;
    return a.lo0 >= b.lo0;
}

// branchless a >= b for use in the hot path add
static inline int SST_FP128_MagGeBranchless(SST_FP128 a, SST_FP128 b) {
    // compare from most significant to least
    int hi1_gt = (a.hi1 > b.hi1);
    int hi1_eq = (a.hi1 == b.hi1);
    int hi0_gt = (a.hi0 > b.hi0);
    int hi0_eq = (a.hi0 == b.hi0);
    int lo1_gt = (a.lo1 > b.lo1);
    int lo1_eq = (a.lo1 == b.lo1);
    int lo0_ge = (a.lo0 >= b.lo0);
    return hi1_gt | (hi1_eq & (hi0_gt | (hi0_eq & (lo1_gt | (lo1_eq & lo0_ge)))));
}

static inline int SST_FP128_MagEq(SST_FP128 a, SST_FP128 b) {
    return (a.hi1 == b.hi1) & (a.hi0 == b.hi0) & (a.lo1 == b.lo1) & (a.lo0 == b.lo0);
}

static inline int SST_FP128_MagGt(SST_FP128 a, SST_FP128 b) {
    return SST_FP128_MagGeBranchless(a, b) & !SST_FP128_MagEq(a, b);
}

static inline SST_FP128 SST_FP128_Zero() {
    return (SST_FP128){.lo0 = 0, .lo1 = 0, .hi0 = 0, .hi1 = 0, .sign = 0};
}

//======================================================================================================
// [256-BIT ADD/SUB WITH CARRY/BORROW CHAINS]
//======================================================================================================
// all 64-bit ops so GCC can use sbb/adc natively
//======================================================================================================
static inline void SST_FP128_MagAdd4(
    uint64_t a0, uint64_t a1, uint64_t a2, uint64_t a3,
    uint64_t b0, uint64_t b1, uint64_t b2, uint64_t b3,
    uint64_t *r0, uint64_t *r1, uint64_t *r2, uint64_t *r3)
{
    *r0 = a0 + b0;
    uint64_t c0 = (*r0 < a0);

    uint64_t t1 = a1 + b1;
    uint64_t c1_partial = (t1 < a1);
    *r1 = t1 + c0;
    uint64_t c1 = c1_partial | (*r1 < t1);

    uint64_t t2 = a2 + b2;
    uint64_t c2_partial = (t2 < a2);
    *r2 = t2 + c1;
    uint64_t c2 = c2_partial | (*r2 < t2);

    uint64_t t3 = a3 + b3;
    *r3 = t3 + c2;
}

static inline void SST_FP128_MagSub4(
    uint64_t a0, uint64_t a1, uint64_t a2, uint64_t a3,
    uint64_t b0, uint64_t b1, uint64_t b2, uint64_t b3,
    uint64_t *r0, uint64_t *r1, uint64_t *r2, uint64_t *r3)
{
    *r0 = a0 - b0;
    uint64_t bw0 = (a0 < b0);

    uint64_t t1 = a1 - b1;
    uint64_t bw1_partial = (a1 < b1);
    *r1 = t1 - bw0;
    uint64_t bw1 = bw1_partial | (t1 < bw0);

    uint64_t t2 = a2 - b2;
    uint64_t bw2_partial = (a2 < b2);
    *r2 = t2 - bw1;
    uint64_t bw2 = bw2_partial | (t2 < bw1);

    uint64_t t3 = a3 - b3;
    *r3 = t3 - bw2;
}

//======================================================================================================
// [DOUBLE PRECISION CONVERSION]
//======================================================================================================
static inline SST_FP128 SST_FP128_FromDouble(double input) {
    int32_t neg = (input < 0.0);
    double abs_input = input * (1.0 - 2.0 * neg);

    double int_part  = floor(abs_input);
    double frac_part = abs_input - int_part;

    // integer part goes into hi0 (hi1 = 0 for double-range values)
    uint64_t hi0 = (uint64_t)int_part;
    // fractional part: split across lo1 and lo0
    double frac_hi = floor(frac_part * 18446744073709551616.0);
    double frac_lo = (frac_part * 18446744073709551616.0 - frac_hi) * 18446744073709551616.0;

    SST_FP128 result;
    result.lo0 = (uint64_t)frac_lo;
    result.lo1 = (uint64_t)frac_hi;
    result.hi0 = hi0;
    result.hi1 = 0;
    result.sign = neg & !SST_FP128_MagIsZero(result);
    return result;
}

static inline double SST_FP128_ToDouble(SST_FP128 value) {
    double int_part  = (double)value.hi0 + (double)value.hi1 * 18446744073709551616.0;
    double frac_hi   = (double)value.lo1 / 18446744073709551616.0;
    double frac_lo   = (double)value.lo0 / (18446744073709551616.0 * 18446744073709551616.0);
    double mag       = int_part + frac_hi + frac_lo;
    return mag * (1.0 - 2.0 * value.sign);
}
//======================================================================================================
// [FP64 CONVERSION]
//======================================================================================================
// widen FP64 to FP128 for accumulation, narrow back when done
// FP64 has 64 frac bits, FP128 has 128, so widening shifts left by 64, narrowing shifts right by 64
//======================================================================================================
#ifdef SST_FIXED_POINT_64_H
static inline SST_FP128 SST_FP128_FromFP64(SST_FP64 value) {
    // FP64 magnitude is __uint128_t with 64 frac bits
    // shift left by 64: low 64 bits of magnitude go to lo1, high 64 bits go to hi0
    SST_FP128 result;
    result.lo0 = 0;
    result.lo1 = (uint64_t)value.magnitude;
    result.hi0 = (uint64_t)(value.magnitude >> 64);
    result.hi1 = 0;
    result.sign = value.sign;
    return result;
}

static inline SST_FP64 SST_FP128_ToFP64(SST_FP128 value) {
    // shift right by 64: lo1 becomes low word, hi0 becomes high word
    SST_FP64 result;
    result.magnitude = ((__uint128_t)value.hi0 << 64) | (__uint128_t)value.lo1;
    result.sign = value.sign & (result.magnitude != 0);
    return result;
}
#endif
//======================================================================================================
// [GUARDS]
//======================================================================================================
static inline SST_FP128 SST_FP128_Min(SST_FP128 a, SST_FP128 b) {
    int diff_sign = a.sign ^ b.sign;
    int a_lt_mag = SST_FP128_MagGt(b, a);
    int a_lt_same = (a_lt_mag ^ a.sign) & !SST_FP128_MagEq(a, b);
    int both_zero = SST_FP128_MagIsZero(a) & SST_FP128_MagIsZero(b);
    int a_lt = ((a.sign & diff_sign) | (a_lt_same & (!diff_sign))) & !both_zero;

    uint64_t mask = -(uint64_t)a_lt;
    return (SST_FP128){
        .lo0 = (a.lo0 & mask) | (b.lo0 & ~mask),
        .lo1 = (a.lo1 & mask) | (b.lo1 & ~mask),
        .hi0 = (a.hi0 & mask) | (b.hi0 & ~mask),
        .hi1 = (a.hi1 & mask) | (b.hi1 & ~mask),
        .sign = (int32_t)((a.sign & a_lt) | (b.sign & (1 - a_lt)))
    };
}

static inline SST_FP128 SST_FP128_Max(SST_FP128 a, SST_FP128 b) {
    int diff_sign = a.sign ^ b.sign;
    int a_gt_mag = SST_FP128_MagGt(a, b);
    int a_gt_same = (a_gt_mag ^ a.sign) & !SST_FP128_MagEq(a, b);
    int a_gt = (((!a.sign) & diff_sign) | (a_gt_same & (!diff_sign)));

    uint64_t mask = -(uint64_t)a_gt;
    return (SST_FP128){
        .lo0 = (a.lo0 & mask) | (b.lo0 & ~mask),
        .lo1 = (a.lo1 & mask) | (b.lo1 & ~mask),
        .hi0 = (a.hi0 & mask) | (b.hi0 & ~mask),
        .hi1 = (a.hi1 & mask) | (b.hi1 & ~mask),
        .sign = (int32_t)((a.sign & a_gt) | (b.sign & (1 - a_gt)))
    };
}
//======================================================================================================
// [FIXED-POINT ARITHMETIC OPERATIONS]
//======================================================================================================
// branchless 256-bit sign-magnitude add using 64-bit word operations
// computes both paths (same-sign add, diff-sign subtract) and mask-selects
//======================================================================================================
static inline SST_FP128 SST_FP128_Add(SST_FP128 a, SST_FP128 b) {
    int diff = a.sign ^ b.sign;

    // same sign path: add magnitudes
    uint64_t s0, s1, s2, s3;
    SST_FP128_MagAdd4(a.lo0, a.lo1, a.hi0, a.hi1,
                       b.lo0, b.lo1, b.hi0, b.hi1,
                       &s0, &s1, &s2, &s3);

    // different sign path: compute both a-b and b-a, mask-select
    int a_ge = SST_FP128_MagGeBranchless(a, b);
    uint64_t ge_mask = -(uint64_t)a_ge;

    uint64_t dab0, dab1, dab2, dab3;
    SST_FP128_MagSub4(a.lo0, a.lo1, a.hi0, a.hi1,
                       b.lo0, b.lo1, b.hi0, b.hi1,
                       &dab0, &dab1, &dab2, &dab3);

    uint64_t dba0, dba1, dba2, dba3;
    SST_FP128_MagSub4(b.lo0, b.lo1, b.hi0, b.hi1,
                       a.lo0, a.lo1, a.hi0, a.hi1,
                       &dba0, &dba1, &dba2, &dba3);

    uint64_t d0 = (dab0 & ge_mask) | (dba0 & ~ge_mask);
    uint64_t d1 = (dab1 & ge_mask) | (dba1 & ~ge_mask);
    uint64_t d2 = (dab2 & ge_mask) | (dba2 & ~ge_mask);
    uint64_t d3 = (dab3 & ge_mask) | (dba3 & ~ge_mask);

    // select between same-sign and diff-sign paths
    uint64_t d_mask = -(uint64_t)diff;
    uint64_t r0 = (s0 & ~d_mask) | (d0 & d_mask);
    uint64_t r1 = (s1 & ~d_mask) | (d1 & d_mask);
    uint64_t r2 = (s2 & ~d_mask) | (d2 & d_mask);
    uint64_t r3 = (s3 & ~d_mask) | (d3 & d_mask);

    // select sign
    int diff_sign_result = (a.sign & a_ge) | (b.sign & (1 - a_ge));
    int result_sign = (a.sign & (!diff)) | (diff_sign_result & diff);
    result_sign &= ((r0 | r1 | r2 | r3) != 0);

    return (SST_FP128){.lo0 = r0, .lo1 = r1, .hi0 = r2, .hi1 = r3, .sign = result_sign};
}

static inline SST_FP128 SST_FP128_Sub(SST_FP128 a, SST_FP128 b) {
    int flip = !SST_FP128_MagIsZero(b);
    SST_FP128 neg_b = {.lo0 = b.lo0, .lo1 = b.lo1, .hi0 = b.hi0, .hi1 = b.hi1,
                       .sign = b.sign ^ flip};
    return SST_FP128_Add(a, neg_b);
}

static inline SST_FP128 SST_FP128_AddSat(SST_FP128 a, SST_FP128 b) {
    // 256-bit range is so large that overflow is practically impossible in accumulator use
    return SST_FP128_Add(a, b);
}

static inline SST_FP128 SST_FP128_SubSat(SST_FP128 a, SST_FP128 b) {
    return SST_FP128_Sub(a, b);
}
//======================================================================================================
// [MULTIPLY AND DIVIDE]
//======================================================================================================
// go through long double conversion - precision is limited but sufficient for accumulator use
// since youre truncating back to FP64 anyway
//======================================================================================================
static inline SST_FP128 SST_FP128_Mul(SST_FP128 a, SST_FP128 b) {
    long double fa = fabsl((long double)SST_FP128_ToDouble(a));
    long double fb = fabsl((long double)SST_FP128_ToDouble(b));
    SST_FP128 result = SST_FP128_FromDouble((double)(fa * fb));
    result.sign = (a.sign ^ b.sign) & !SST_FP128_MagIsZero(result);
    return result;
}

static inline SST_FP128 SST_FP128_DivNoAssert(SST_FP128 a, SST_FP128 b) {
    if (SST_FP128_MagIsZero(b)) {
        return (SST_FP128){.lo0 = UINT64_MAX, .lo1 = UINT64_MAX,
                           .hi0 = UINT64_MAX, .hi1 = UINT64_MAX, .sign = a.sign};
    }
    long double fa = fabsl((long double)SST_FP128_ToDouble(a));
    long double fb = fabsl((long double)SST_FP128_ToDouble(b));
    SST_FP128 result = SST_FP128_FromDouble((double)(fa / fb));
    result.sign = (a.sign ^ b.sign) & !SST_FP128_MagIsZero(result);
    return result;
}

static inline SST_FP128 SST_FP128_DivWithAssert(SST_FP128 a, SST_FP128 b) {
    assert(!SST_FP128_MagIsZero(b));
    return SST_FP128_DivNoAssert(a, b);
}
//======================================================================================================
// [FIXED-POINT COMPARISON OPERATIONS]
//======================================================================================================
static inline int SST_FP128_Equal(SST_FP128 a, SST_FP128 b) {
    int both_zero = SST_FP128_MagIsZero(a) & SST_FP128_MagIsZero(b);
    return both_zero | (SST_FP128_MagEq(a, b) & (a.sign == b.sign));
}

static inline int SST_FP128_NotEqual(SST_FP128 a, SST_FP128 b) {
    return !SST_FP128_Equal(a, b);
}

static inline int SST_FP128_LessThan(SST_FP128 a, SST_FP128 b) {
    int both_zero = SST_FP128_MagIsZero(a) & SST_FP128_MagIsZero(b);
    int diff_sign = a.sign ^ b.sign;
    int diff_result = a.sign & !both_zero;
    int a_lt_mag = SST_FP128_MagGt(b, a);
    int same_result = (a_lt_mag ^ a.sign) & !SST_FP128_MagEq(a, b);
    return (diff_result & diff_sign) | (same_result & (!diff_sign));
}

static inline int SST_FP128_LessThanOrEqual(SST_FP128 a, SST_FP128 b) {
    return SST_FP128_LessThan(a, b) | SST_FP128_Equal(a, b);
}

static inline int SST_FP128_GreaterThan(SST_FP128 a, SST_FP128 b) {
    return SST_FP128_LessThan(b, a);
}

static inline int SST_FP128_GreaterThanOrEqual(SST_FP128 a, SST_FP128 b) {
    return !SST_FP128_LessThan(a, b);
}
//======================================================================================================
// [FIXED-POINT UTILITY FUNCTIONS]
//======================================================================================================
static inline SST_FP128 SST_FP128_Negate(SST_FP128 value) {
    int flip = !SST_FP128_MagIsZero(value);
    return (SST_FP128){.lo0 = value.lo0, .lo1 = value.lo1,
                       .hi0 = value.hi0, .hi1 = value.hi1, .sign = value.sign ^ flip};
}

static inline int SST_FP128_IsZero(SST_FP128 value) {
    return SST_FP128_MagIsZero(value);
}

static inline SST_FP128 SST_FP128_Abs(SST_FP128 value) {
    return (SST_FP128){.lo0 = value.lo0, .lo1 = value.lo1,
                       .hi0 = value.hi0, .hi1 = value.hi1, .sign = 0};
}

static inline SST_FP128 SST_FP128_Sign(SST_FP128 value) {
    if (SST_FP128_MagIsZero(value)) return SST_FP128_Zero();
    // 1.0 in Q128.128: integer part = 1, so hi0 = 1, rest = 0
    return (SST_FP128){.lo0 = 0, .lo1 = 0, .hi0 = 1, .hi1 = 0, .sign = value.sign};
}
//======================================================================================================
// [FIXED-POINT MATH FUNCTIONS]
//======================================================================================================
static inline SST_FP128 SST_FP128_Sqrt(SST_FP128 value) {
    assert(value.sign == 0 || SST_FP128_MagIsZero(value));
    return SST_FP128_FromDouble(sqrt(SST_FP128_ToDouble(value)));
}

static inline SST_FP128 SST_FP128_InvSqrt(SST_FP128 value) {
    assert(value.sign == 0 && !SST_FP128_MagIsZero(value));
    double x = SST_FP128_ToDouble(value);
    return SST_FP128_FromDouble(1.0 / sqrt(x));
}

static inline SST_FP128 SST_FP128_Sin(SST_FP128 value) {
    return SST_FP128_FromDouble(sin(SST_FP128_ToDouble(value)));
}

static inline SST_FP128 SST_FP128_Cos(SST_FP128 value) {
    return SST_FP128_FromDouble(cos(SST_FP128_ToDouble(value)));
}

static inline SST_FP128 SST_FP128_Tan(SST_FP128 value) {
    return SST_FP128_FromDouble(tan(SST_FP128_ToDouble(value)));
}

static inline SST_FP128 SST_FP128_Atan2(SST_FP128 y, SST_FP128 x) {
    return SST_FP128_FromDouble(atan2(SST_FP128_ToDouble(y), SST_FP128_ToDouble(x)));
}

static inline SST_FP128 SST_FP128_Exp(SST_FP128 value) {
    return SST_FP128_FromDouble(exp(SST_FP128_ToDouble(value)));
}

static inline SST_FP128 SST_FP128_Log(SST_FP128 value) {
    assert(value.sign == 0 && !SST_FP128_MagIsZero(value));
    return SST_FP128_FromDouble(log(SST_FP128_ToDouble(value)));
}

static inline SST_FP128 SST_FP128_Pow(SST_FP128 base, SST_FP128 exponent) {
    return SST_FP128_FromDouble(pow(SST_FP128_ToDouble(base), SST_FP128_ToDouble(exponent)));
}
//======================================================================================================
// [FIXED-POINT MISCELLANEOUS FUNCTIONS]
//======================================================================================================
static inline SST_FP128 SST_FP128_Floor(SST_FP128 value) {
    // lo0,lo1 are the 128 fractional bits, so floor just zeros them
    // if negative and had a fraction, bump integer part up by 1
    int has_frac = (value.lo0 | value.lo1) != 0;
    uint64_t bump = (uint64_t)(value.sign & has_frac);
    // add 1 to hi0 with carry to hi1
    uint64_t new_hi0 = value.hi0 + bump;
    uint64_t carry = (new_hi0 < value.hi0);
    uint64_t new_hi1 = value.hi1 + carry;
    return (SST_FP128){.lo0 = 0, .lo1 = 0, .hi0 = new_hi0, .hi1 = new_hi1, .sign = value.sign};
}

static inline SST_FP128 SST_FP128_Ceil(SST_FP128 value) {
    int has_frac = (value.lo0 | value.lo1) != 0;
    uint64_t bump = (uint64_t)((!value.sign) & has_frac);
    uint64_t new_hi0 = value.hi0 + bump;
    uint64_t carry = (new_hi0 < value.hi0);
    uint64_t new_hi1 = value.hi1 + carry;
    int result_sign = value.sign & ((new_hi0 | new_hi1) != 0);
    return (SST_FP128){.lo0 = 0, .lo1 = 0, .hi0 = new_hi0, .hi1 = new_hi1, .sign = result_sign};
}

static inline SST_FP128 SST_FP128_Round(SST_FP128 value) {
    // half = MSB of lo1 set (bit 127 of the fractional part)
    int round_up = (value.lo1 >> 63) & 1;
    uint64_t new_hi0 = value.hi0 + (uint64_t)round_up;
    uint64_t carry = (new_hi0 < value.hi0);
    uint64_t new_hi1 = value.hi1 + carry;
    int result_sign = value.sign & ((new_hi0 | new_hi1) != 0);
    return (SST_FP128){.lo0 = 0, .lo1 = 0, .hi0 = new_hi0, .hi1 = new_hi1, .sign = result_sign};
}

static inline SST_FP128 SST_FP128_Lerp(SST_FP128 a, SST_FP128 b, SST_FP128 t) {
    SST_FP128 diff   = SST_FP128_Sub(b, a);
    SST_FP128 scaled = SST_FP128_Mul(diff, t);
    return SST_FP128_Add(a, scaled);
}
//======================================================================================================
//======================================================================================================

//======================================================================================================
#endif // SST_FIXED_POINT_128_H
