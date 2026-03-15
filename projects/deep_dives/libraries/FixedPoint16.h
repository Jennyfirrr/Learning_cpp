//======================================================================================================
// [FIXED-POINT ARITHMETIC LIBRARY 16 FRACTIONAL BITS]
//======================================================================================================
// sign-magnitude representation: full uint32_t range for magnitude, separate sign flag
// no two's complement asymmetry (no INT_MIN edge case), mul/div just XOR signs
//======================================================================================================
#ifndef SST_FIXED_POINT_H
#define SST_FIXED_POINT_H

#define SST_FP_FRAC_BITS 16

#include <stdint.h>
#include <assert.h>
#include <math.h>

//======================================================================================================
// [FIXED-POINT NUMBER REPRESENTATION]
//======================================================================================================
typedef struct {
    uint32_t magnitude;
    int32_t sign; // 0 = positive/zero, 1 = negative
} SST_FP;
static_assert(sizeof(SST_FP) == 8, "SST_FP must be 8 bytes");

//======================================================================================================
// [DOUBLE PRECISION FIXED-POINT ARITHMETIC]
//======================================================================================================
static inline SST_FP SST_FP_FromDouble(double input) {
    int32_t neg = (input < 0.0);
    double abs_input = input * (1.0 - 2.0 * neg); // branchless abs
    SST_FP result;
    result.magnitude = (uint32_t)(abs_input * (1ULL << SST_FP_FRAC_BITS));
    result.sign = neg & (result.magnitude != 0);
    return result;
}

static inline double SST_FP_ToDouble(SST_FP value) {
    double mag = (double)value.magnitude / (1ULL << SST_FP_FRAC_BITS);
    return mag * (1.0 - 2.0 * value.sign); // branchless negate
}

//======================================================================================================
// [GUARDS]
//======================================================================================================
static inline SST_FP SST_FP_Min(SST_FP a, SST_FP b) {
    // a < b? different signs: negative is smaller; same sign positive: smaller mag; same sign negative: larger mag
    int diff_sign = a.sign ^ b.sign;
    int a_lt_same = (a.magnitude < b.magnitude) ^ a.sign; // flips comparison when negative
    int a_lt = (a.sign & diff_sign) | (a_lt_same & !diff_sign);
    uint32_t mask = -(uint32_t)a_lt;
    return (SST_FP){
        .magnitude = (a.magnitude & mask) | (b.magnitude & ~mask),
        .sign = (a.sign & a_lt) | (b.sign & !a_lt)
    };
}

static inline SST_FP SST_FP_Max(SST_FP a, SST_FP b) {
    int diff_sign = a.sign ^ b.sign;
    int a_gt_same = (a.magnitude > b.magnitude) ^ a.sign;
    int a_gt = ((!a.sign) & diff_sign) | (a_gt_same & (!diff_sign));
    uint32_t mask = -(uint32_t)a_gt;
    return (SST_FP){
        .magnitude = (a.magnitude & mask) | (b.magnitude & ~mask),
        .sign = (a.sign & a_gt) | (b.sign & !a_gt)
    };
}
//======================================================================================================
// [FIXED-POINT ARITHMETIC OPERATIONS]
//======================================================================================================
static inline SST_FP SST_FP_Add(SST_FP a, SST_FP b) {
    int diff = a.sign ^ b.sign;

    // same sign path: add magnitudes
    uint32_t sum = a.magnitude + b.magnitude;

    // different sign path: subtract smaller from larger
    int a_ge = (a.magnitude >= b.magnitude);
    uint32_t a_ge_mask = -(uint32_t)a_ge;
    uint32_t diff_val = ((a.magnitude - b.magnitude) & a_ge_mask) |
                        ((b.magnitude - a.magnitude) & ~a_ge_mask);

    // select magnitude
    uint32_t diff_mask = -(uint32_t)diff;
    uint32_t result_mag = (sum & ~diff_mask) | (diff_val & diff_mask);

    // select sign
    int diff_sign_result = (a.sign & a_ge) | (b.sign & (1 - a_ge));
    int result_sign = (a.sign & !diff) | (diff_sign_result & diff);
    result_sign &= (result_mag != 0);

    return (SST_FP){.magnitude = result_mag, .sign = result_sign};
}

static inline SST_FP SST_FP_Sub(SST_FP a, SST_FP b) {
    SST_FP neg_b = {.magnitude = b.magnitude, .sign = b.sign ^ (b.magnitude != 0)};
    return SST_FP_Add(a, neg_b);
}

static inline SST_FP SST_FP_Mul(SST_FP a, SST_FP b) {
    uint64_t wide = (uint64_t)a.magnitude * b.magnitude;
    uint32_t result_mag = (uint32_t)(wide >> SST_FP_FRAC_BITS);
    int result_sign = (a.sign ^ b.sign) & (result_mag != 0);
    return (SST_FP){.magnitude = result_mag, .sign = result_sign};
}

static inline SST_FP SST_FP_Div(SST_FP a, SST_FP b) {
    assert(b.magnitude != 0);
    uint64_t wide = ((uint64_t)a.magnitude << SST_FP_FRAC_BITS) / b.magnitude;
    uint32_t result_mag = (uint32_t)wide;
    int result_sign = (a.sign ^ b.sign) & (result_mag != 0);
    return (SST_FP){.magnitude = result_mag, .sign = result_sign};
}
//======================================================================================================
// [SATURATING ARITHMETIC OPERATIONS]
//======================================================================================================
static inline SST_FP SST_FP_AddSat(SST_FP a, SST_FP b) {
    int diff = a.sign ^ b.sign;

    // same sign: add magnitudes, check unsigned overflow
    uint32_t sum = a.magnitude + b.magnitude;
    uint32_t sum_overflow = (sum < a.magnitude);
    uint32_t sat_sum = sum | -(uint32_t)sum_overflow;

    // different sign: subtract (never overflows)
    int a_ge = (a.magnitude >= b.magnitude);
    uint32_t a_ge_mask = -(uint32_t)a_ge;
    uint32_t diff_val = ((a.magnitude - b.magnitude) & a_ge_mask) |
                        ((b.magnitude - a.magnitude) & ~a_ge_mask);

    uint32_t diff_mask = -(uint32_t)diff;
    uint32_t result_mag = (sat_sum & ~diff_mask) | (diff_val & diff_mask);

    int diff_sign_result = (a.sign & a_ge) | (b.sign & (1 - a_ge));
    int result_sign = (a.sign & !diff) | (diff_sign_result & diff);
    result_sign &= (result_mag != 0);

    return (SST_FP){.magnitude = result_mag, .sign = result_sign};
}

static inline SST_FP SST_FP_SubSat(SST_FP a, SST_FP b) {
    SST_FP neg_b = {.magnitude = b.magnitude, .sign = b.sign ^ (b.magnitude != 0)};
    return SST_FP_AddSat(a, neg_b);
}

static inline SST_FP SST_FP_MulSat(SST_FP a, SST_FP b) {
    uint64_t wide = (uint64_t)a.magnitude * b.magnitude;
    uint64_t shifted = wide >> SST_FP_FRAC_BITS;

    uint32_t hi = (uint32_t)(shifted >> 32);
    int overflow = (hi != 0);
    uint32_t of_mask = -(uint32_t)overflow;
    uint32_t result_mag = ((uint32_t)shifted & ~of_mask) | (UINT32_MAX & of_mask);

    int result_sign = (a.sign ^ b.sign) & (result_mag != 0);
    return (SST_FP){.magnitude = result_mag, .sign = result_sign};
}

static inline SST_FP SST_FP_DivNoAssert(SST_FP a, SST_FP b) {
    int b_zero = (b.magnitude == 0);
    uint32_t safe_b = b.magnitude | (uint32_t)b_zero;
    uint64_t wide = ((uint64_t)a.magnitude << SST_FP_FRAC_BITS) / safe_b;

    uint32_t hi = (uint32_t)(wide >> 32);
    int overflow = (hi != 0) | b_zero;
    uint32_t of_mask = -(uint32_t)overflow;
    uint32_t result_mag = ((uint32_t)wide & ~of_mask) | (UINT32_MAX & of_mask);

    // b zero: use a's sign; otherwise XOR signs
    int result_sign = ((a.sign ^ b.sign) & !b_zero) | (a.sign & b_zero);
    result_sign &= (result_mag != 0);
    return (SST_FP){.magnitude = result_mag, .sign = result_sign};
}

static inline SST_FP SST_FP_DivWithAssert(SST_FP a, SST_FP b) {
    assert(b.magnitude != 0);
    return SST_FP_DivNoAssert(a, b);
}
//======================================================================================================
// [FIXED-POINT COMPARISON OPERATIONS]
//======================================================================================================
static inline int SST_FP_Equal(SST_FP a, SST_FP b) {
    // handle +0 == -0
    int both_zero = (a.magnitude == 0) & (b.magnitude == 0);
    return both_zero | ((a.magnitude == b.magnitude) & (a.sign == b.sign));
}

static inline int SST_FP_NotEqual(SST_FP a, SST_FP b) {
    return !SST_FP_Equal(a, b);
}

static inline int SST_FP_LessThan(SST_FP a, SST_FP b) {
    int both_zero = (a.magnitude == 0) & (b.magnitude == 0);
    int diff_sign = a.sign ^ b.sign;
    // different signs: a < b iff a is negative (and not both zero)
    int diff_result = a.sign & !both_zero;
    // same sign: compare magnitudes (flipped when negative)
    int same_result = ((a.magnitude < b.magnitude) ^ a.sign) & (a.magnitude != b.magnitude);
    return (diff_result & diff_sign) | (same_result & !diff_sign);
}

static inline int SST_FP_LessThanOrEqual(SST_FP a, SST_FP b) {
    return SST_FP_LessThan(a, b) | SST_FP_Equal(a, b);
}

static inline int SST_FP_GreaterThan(SST_FP a, SST_FP b) {
    return SST_FP_LessThan(b, a);
}

static inline int SST_FP_GreaterThanOrEqual(SST_FP a, SST_FP b) {
    return !SST_FP_LessThan(a, b);
}

static inline int SST_FP_IsZero(SST_FP a) {
    return a.magnitude == 0;
}

static inline int SST_FP_IsNegative(SST_FP a) {
    return a.sign & (a.magnitude != 0);
}

static inline int SST_FP_IsPositive(SST_FP a) {
    return !a.sign & (a.magnitude != 0);
}

static inline int SST_FP_Sign(SST_FP a) {
    // returns -1, 0, or 1
    int is_pos = !a.sign & (a.magnitude != 0);
    int is_neg = a.sign & (a.magnitude != 0);
    return is_pos - is_neg;
}

static inline SST_FP SST_FP_Abs(SST_FP a) {
    return (SST_FP){.magnitude = a.magnitude, .sign = 0};
}

static inline SST_FP SST_FP_Negate(SST_FP value) {
    return (SST_FP){.magnitude = value.magnitude, .sign = value.sign ^ (value.magnitude != 0)};
}

static inline SST_FP SST_FP_SignFP(SST_FP value) {
    uint32_t one = 1U << SST_FP_FRAC_BITS;
    uint32_t is_nonzero = (value.magnitude != 0);
    return (SST_FP){.magnitude = one & -(uint32_t)is_nonzero, .sign = (int32_t)(value.sign & is_nonzero)};
}
//======================================================================================================
// [FIXED-POINT MATH FUNCTIONS]
//======================================================================================================
static inline SST_FP SST_FP_Sqrt(SST_FP value) {
    assert(value.sign == 0 || value.magnitude == 0);
    double x = SST_FP_ToDouble(value);
    return SST_FP_FromDouble(sqrt(x));
}

static inline SST_FP SST_FP_InvSqrt(SST_FP value) {
    assert(value.sign == 0 && value.magnitude != 0);
    double x = SST_FP_ToDouble(value);
    return SST_FP_FromDouble(1.0 / sqrt(x));
}

static inline SST_FP SST_FP_Sin(SST_FP value) {
    return SST_FP_FromDouble(sin(SST_FP_ToDouble(value)));
}

static inline SST_FP SST_FP_Cos(SST_FP value) {
    return SST_FP_FromDouble(cos(SST_FP_ToDouble(value)));
}

static inline SST_FP SST_FP_Tan(SST_FP value) {
    return SST_FP_FromDouble(tan(SST_FP_ToDouble(value)));
}

static inline SST_FP SST_FP_Atan2(SST_FP y, SST_FP x) {
    return SST_FP_FromDouble(atan2(SST_FP_ToDouble(y), SST_FP_ToDouble(x)));
}

static inline SST_FP SST_FP_Exp(SST_FP value) {
    return SST_FP_FromDouble(exp(SST_FP_ToDouble(value)));
}

static inline SST_FP SST_FP_Log(SST_FP value) {
    assert(value.sign == 0 && value.magnitude != 0);
    return SST_FP_FromDouble(log(SST_FP_ToDouble(value)));
}

static inline SST_FP SST_FP_Pow(SST_FP base, SST_FP exponent) {
    return SST_FP_FromDouble(pow(SST_FP_ToDouble(base), SST_FP_ToDouble(exponent)));
}
//======================================================================================================
// [FIXED-POINT MISCELLANEOUS FUNCTIONS]
//======================================================================================================
static inline SST_FP SST_FP_Floor(SST_FP value) {
    uint32_t frac_mask = (1U << SST_FP_FRAC_BITS) - 1;
    uint32_t has_frac = (value.magnitude & frac_mask) != 0;
    uint32_t trunc = value.magnitude & ~frac_mask;
    // negative with fraction: bump magnitude up (floor goes more negative)
    uint32_t bump = (uint32_t)(value.sign & has_frac) << SST_FP_FRAC_BITS;
    return (SST_FP){.magnitude = trunc + bump, .sign = value.sign};
}

static inline SST_FP SST_FP_Ceil(SST_FP value) {
    uint32_t frac_mask = (1U << SST_FP_FRAC_BITS) - 1;
    uint32_t has_frac = (value.magnitude & frac_mask) != 0;
    uint32_t trunc = value.magnitude & ~frac_mask;
    // positive with fraction: bump magnitude up (ceil goes more positive)
    uint32_t bump = (uint32_t)((!value.sign) & has_frac) << SST_FP_FRAC_BITS;
    uint32_t result_mag = trunc + bump;
    int result_sign = value.sign & (result_mag != 0);
    return (SST_FP){.magnitude = result_mag, .sign = result_sign};
}

static inline SST_FP SST_FP_Round(SST_FP value) {
    uint32_t frac_mask = (1U << SST_FP_FRAC_BITS) - 1;
    uint32_t half = 1U << (SST_FP_FRAC_BITS - 1);
    uint32_t frac = value.magnitude & frac_mask;
    uint32_t trunc = value.magnitude & ~frac_mask;
    uint32_t bump = (uint32_t)(frac >= half) << SST_FP_FRAC_BITS;
    uint32_t result_mag = trunc + bump;
    int result_sign = value.sign & (result_mag != 0);
    return (SST_FP){.magnitude = result_mag, .sign = result_sign};
}

static inline SST_FP SST_FP_Mod(SST_FP a, SST_FP b) {
    assert(b.magnitude != 0);
    SST_FP quotient = SST_FP_DivNoAssert(a, b);
    uint32_t frac_mask = (1U << SST_FP_FRAC_BITS) - 1;
    SST_FP truncated = {.magnitude = quotient.magnitude & ~frac_mask, .sign = quotient.sign};
    return SST_FP_SubSat(a, SST_FP_MulSat(truncated, b));
}

static inline SST_FP SST_FP_Lerp(SST_FP a, SST_FP b, SST_FP t) {
    SST_FP diff   = SST_FP_SubSat(b, a);
    SST_FP scaled = SST_FP_MulSat(diff, t);
    return SST_FP_AddSat(a, scaled);
}

static inline SST_FP SST_FP_SmoothStep(SST_FP edge0, SST_FP edge1, SST_FP x) {
    uint32_t mask0    = -(uint32_t)SST_FP_LessThanOrEqual(x, edge0);
    uint32_t mask1    = -(uint32_t)SST_FP_GreaterThanOrEqual(x, edge1);
    uint32_t in_range = ~(mask0 | mask1);

    SST_FP clamped = SST_FP_Max(SST_FP_Min(x, edge1), edge0);
    SST_FP t = SST_FP_DivNoAssert(SST_FP_SubSat(clamped, edge0), SST_FP_SubSat(edge1, edge0));

    SST_FP three = {.magnitude = 3U << SST_FP_FRAC_BITS, .sign = 0};
    SST_FP two   = {.magnitude = 2U << SST_FP_FRAC_BITS, .sign = 0};
    SST_FP poly  = SST_FP_MulSat(SST_FP_MulSat(t, t), SST_FP_SubSat(three, SST_FP_MulSat(two, t)));

    uint32_t one = 1U << SST_FP_FRAC_BITS;
    uint32_t raw = (0 & mask0) | (one & mask1) | (poly.magnitude & in_range);
    return (SST_FP){.magnitude = raw, .sign = 0};
}
//======================================================================================================
//======================================================================================================
#endif // SST_FIXED_POINT_H
