//======================================================================================================
// [FIXED-POINT ARITHMETIC LIBRARY 16 FRACTIONAL BITS]
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
    int32_t raw_value;
} SST_FP;
static_assert(sizeof(SST_FP) == 4, "SST_FP must be 4 bytes");

//======================================================================================================
// [DOUBLE PRECISION FIXED-POINT ARITHMETIC]
//======================================================================================================
static inline SST_FP SST_FP_FromDouble(double input) {
    SST_FP result;
    result.raw_value = (int32_t)(input * (1LL << SST_FP_FRAC_BITS));
    return result;
}

static inline double SST_FP_ToDouble(SST_FP value) {
    return (double)value.raw_value / (1LL << SST_FP_FRAC_BITS);
}

//======================================================================================================
// [GUARDS]
//======================================================================================================
static inline SST_FP SST_FP_Min(SST_FP a, SST_FP b) {
    int32_t mask = (a.raw_value - b.raw_value) >> 31;
    return (SST_FP){.raw_value = (a.raw_value & mask) | (b.raw_value & ~mask)};
}

static inline SST_FP SST_FP_Max(SST_FP a, SST_FP b) {
    int32_t mask = (a.raw_value - b.raw_value) >> 31;
    return (SST_FP){.raw_value = (b.raw_value & mask) | (a.raw_value & ~mask)};
}
//======================================================================================================
// [FIXED-POINT ARITHMETIC OPERATIONS]
//======================================================================================================
static inline SST_FP SST_FP_Add(SST_FP a, SST_FP b) {
    SST_FP result;
    result.raw_value = a.raw_value + b.raw_value;
    return result;
}

static inline SST_FP SST_FP_Sub(SST_FP a, SST_FP b) {
    SST_FP result;
    result.raw_value = a.raw_value - b.raw_value;
    return result;
}

static inline SST_FP SST_FP_Mul(SST_FP a, SST_FP b) {
    SST_FP result;
    result.raw_value = ((int64_t)a.raw_value * b.raw_value) >> SST_FP_FRAC_BITS;
    return result;
}

static inline SST_FP SST_FP_Div(SST_FP a, SST_FP b) {
    assert(b.raw_value != 0); // prevent division by zero
    SST_FP result;
    result.raw_value = ((int64_t)a.raw_value << SST_FP_FRAC_BITS) / b.raw_value;
    return result;
}
//======================================================================================================
// [SATURATING ARITHMETIC OPERATIONS]
//======================================================================================================
static inline SST_FP SST_FP_AddSat(SST_FP a, SST_FP b) {
    SST_FP result;
    int32_t sum = a.raw_value + b.raw_value;

    int32_t overflow = (a.raw_value ^ sum) & (b.raw_value ^ sum);

    int32_t sat = (a.raw_value >> 31) ^ INT32_MAX;

    int32_t mask     = overflow >> 31;
    result.raw_value = (sat & mask) | (sum & ~mask);
    return result;
}

static inline SST_FP SST_FP_SubSat(SST_FP a, SST_FP b) {
    SST_FP result;
    int32_t diff = a.raw_value - b.raw_value;

    int32_t overflow = (a.raw_value ^ b.raw_value) & (a.raw_value ^ diff);

    int32_t sat = (a.raw_value >> 31) ^ INT32_MAX;

    int32_t mask     = overflow >> 31;
    result.raw_value = (sat & mask) | (diff & ~mask);
    return result;
}

static inline SST_FP SST_FP_MulSat(SST_FP a, SST_FP b) {
    SST_FP result;
    int64_t wide    = (int64_t)a.raw_value * b.raw_value;
    int64_t shifted = wide >> SST_FP_FRAC_BITS;

    int32_t hi           = (int32_t)(shifted >> 31);
    int32_t sign_a       = a.raw_value >> 31;
    int32_t sign_b       = b.raw_value >> 31;
    int32_t product_sign = sign_a ^ sign_b;
    int32_t sat          = (product_sign ^ INT32_MAX);

    int overflow = (hi != 0) & (hi != -1);
    int32_t mask = -(int32_t)overflow;

    result.raw_value = (sat & mask) | ((int32_t)shifted & ~mask);
    return result;
}

static inline SST_FP SST_FP_DivNoAssert(SST_FP a, SST_FP b) {
    int32_t safe_b = b.raw_value | (!b.raw_value);
    SST_FP result;
    int64_t wide = ((int64_t)a.raw_value << SST_FP_FRAC_BITS) / safe_b;

    int32_t hi            = (int32_t)(wide >> 31);
    int32_t sign_a        = a.raw_value >> 31;
    int32_t sign_b        = b.raw_value >> 31;
    int32_t quotient_sign = sign_a ^ sign_b;
    int32_t sat           = (quotient_sign ^ INT32_MAX);

    int overflow = (hi != 0) & (hi != -1);
    int32_t mask = -(int32_t)overflow;

    result.raw_value = (sat & mask) | ((int32_t)wide & ~mask);
    return result;
}

static inline SST_FP SST_FP_DivWithAssert(SST_FP a, SST_FP b) {
    assert(b.raw_value != 0);

    int32_t is_zero   = !b.raw_value;
    int32_t zero_mask = -(int32_t)is_zero;
    int32_t inf_sat   = (a.raw_value >> 31) ^ INT32_MAX;

    int32_t safe_b = b.raw_value | is_zero;

    SST_FP result;
    int64_t wide = ((int64_t)a.raw_value << SST_FP_FRAC_BITS) / safe_b;

    int32_t hi            = (int32_t)(wide >> 31);
    int32_t sign_a        = a.raw_value >> 31;
    int32_t sign_b        = b.raw_value >> 31;
    int32_t quotient_sign = sign_a ^ sign_b;
    int32_t sat           = (quotient_sign ^ INT32_MAX);

    int overflow          = (hi != 0) & (hi != -1);
    int32_t mask          = -(int32_t)overflow;
    int32_t combined_mask = zero_mask | mask;

    int32_t sat_value = (inf_sat & zero_mask) | (sat & mask & ~zero_mask);

    result.raw_value = (sat_value & combined_mask) | ((int32_t)wide & ~combined_mask);
    return result;
}
//======================================================================================================
// [FIXED-POINT COMPARISON OPERATIONS]
//======================================================================================================
static inline int SST_FP_Equal(SST_FP a, SST_FP b) {
    return a.raw_value == b.raw_value;
}

static inline int SST_FP_NotEqual(SST_FP a, SST_FP b) {
    return a.raw_value != b.raw_value;
}

static inline int SST_FP_LessThan(SST_FP a, SST_FP b) {
    return a.raw_value < b.raw_value;
}

static inline int SST_FP_LessThanOrEqual(SST_FP a, SST_FP b) {
    return a.raw_value <= b.raw_value;
}

static inline int SST_FP_GreaterThan(SST_FP a, SST_FP b) {
    return a.raw_value > b.raw_value;
}

static inline int SST_FP_GreaterThanOrEqual(SST_FP a, SST_FP b) {
    return a.raw_value >= b.raw_value;
}

static inline int SST_FP_IsZero(SST_FP a) {
    return a.raw_value == 0;
}

static inline int SST_FP_IsNegative(SST_FP a) {
    return a.raw_value < 0;
}

static inline int SST_FP_IsPositive(SST_FP a) {
    return a.raw_value > 0;
}

static inline int SST_FP_Sign(SST_FP a) {
    int32_t mask = a.raw_value >> 31;
    return (mask | (!!a.raw_value));
}

static inline SST_FP SST_FP_Abs(SST_FP a) {
    SST_FP result;

    int32_t mask     = a.raw_value >> 31;
    int32_t diff     = a.raw_value ^ mask;
    result.raw_value = diff - mask;

    return result;
}

static inline SST_FP SST_FP_Negate(SST_FP value) {
    SST_FP result;
    int32_t is_min = !(value.raw_value ^ INT32_MIN);

    result.raw_value = (-value.raw_value & ~(-(int32_t)is_min)) | (INT32_MAX & (-(int32_t)is_min));
    return result;
}

static inline SST_FP SST_FP_SignFP(SST_FP value) {
    int32_t is_pos = (value.raw_value > 0);
    int32_t is_neg = (value.raw_value < 0);
    int32_t one    = 1 << SST_FP_FRAC_BITS;
    int32_t raw    = (one & -is_pos) | (-one & -is_neg);
    return (SST_FP){.raw_value = raw};
}
//======================================================================================================
// [FIXED-POINT MATH FUNCTIONS]
//======================================================================================================
static inline SST_FP SST_FP_Sqrt(SST_FP value) {
    assert(value.raw_value >= 0);
    double x = SST_FP_ToDouble(value);
    double y = sqrt(x);
    return SST_FP_FromDouble(y);
}

static inline SST_FP SST_FP_InvSqrt(SST_FP value) {
    assert(value.raw_value > 0);
    double x = SST_FP_ToDouble(value);
    double y = 1.0 / sqrt(x);
    return SST_FP_FromDouble(y);
}

static inline SST_FP SST_FP_Sin(SST_FP value) {
    double x = SST_FP_ToDouble(value);
    double y = sin(x);
    return SST_FP_FromDouble(y);
}

static inline SST_FP SST_FP_Cos(SST_FP value) {
    double x = SST_FP_ToDouble(value);
    double y = cos(x);
    return SST_FP_FromDouble(y);
}

static inline SST_FP SST_FP_Tan(SST_FP value) {
    double x = SST_FP_ToDouble(value);
    double y = tan(x);
    return SST_FP_FromDouble(y);
}

static inline SST_FP SST_FP_Atan2(SST_FP y, SST_FP x) {
    double dy    = SST_FP_ToDouble(y);
    double dx    = SST_FP_ToDouble(x);
    double angle = atan2(dy, dx);
    return SST_FP_FromDouble(angle);
}

static inline SST_FP SST_FP_Exp(SST_FP value) {
    double x = SST_FP_ToDouble(value);
    double y = exp(x);
    return SST_FP_FromDouble(y);
}

static inline SST_FP SST_FP_Log(SST_FP value) {
    assert(value.raw_value > 0);
    double x = SST_FP_ToDouble(value);
    double y = log(x);
    return SST_FP_FromDouble(y);
}

static inline SST_FP SST_FP_Pow(SST_FP base, SST_FP exponent) {
    double b = SST_FP_ToDouble(base);
    double e = SST_FP_ToDouble(exponent);
    double y = pow(b, e);
    return SST_FP_FromDouble(y);
}
//======================================================================================================
// [FIXED-POINT MISCELLANEOUS FUNCTIONS]
//======================================================================================================
static inline SST_FP SST_FP_Floor(SST_FP value) {
    int32_t mask = (1 << SST_FP_FRAC_BITS) - 1;
    return (SST_FP){.raw_value = value.raw_value & ~mask};
}

static inline SST_FP SST_FP_Ceil(SST_FP value) {
    int32_t mask     = (1 << SST_FP_FRAC_BITS) - 1;
    int32_t has_frac = (value.raw_value & mask) != 0;
    int32_t raw      = (value.raw_value & ~mask) + ((1 << SST_FP_FRAC_BITS) & -has_frac);
    return (SST_FP){.raw_value = raw};
}

static inline SST_FP SST_FP_Round(SST_FP value) {
    int32_t mask = (1 << SST_FP_FRAC_BITS) - 1;
    int32_t half = 1 << (SST_FP_FRAC_BITS - 1);
    SST_FP av    = SST_FP_Abs(value);
    int32_t frac = av.raw_value & mask;
    int32_t raw  = av.raw_value & ~mask;
    int32_t bump = (frac >= half);
    raw += (1 << SST_FP_FRAC_BITS) & -bump;
    int32_t sign = value.raw_value >> 31;
    raw          = (raw ^ sign) - sign;
    return (SST_FP){.raw_value = raw};
}

static inline SST_FP SST_FP_Mod(SST_FP a, SST_FP b) {
    assert(b.raw_value != 0);

    int32_t safe_b   = b.raw_value | (!b.raw_value);
    SST_FP quotient  = SST_FP_DivNoAssert(a, (SST_FP){safe_b});
    SST_FP truncated = (SST_FP){quotient.raw_value & ~((1 << SST_FP_FRAC_BITS) - 1)};
    return SST_FP_SubSat(a, SST_FP_MulSat(truncated, (SST_FP){safe_b}));
}

static inline SST_FP SST_FP_Lerp(SST_FP a, SST_FP b, SST_FP t) {
    SST_FP diff   = SST_FP_SubSat(b, a);
    SST_FP scaled = SST_FP_MulSat(diff, t);
    return SST_FP_AddSat(a, scaled);
}

static inline SST_FP SST_FP_SmoothStep(SST_FP edge0, SST_FP edge1, SST_FP x) {
    int32_t mask0    = -((int32_t)SST_FP_LessThanOrEqual(x, edge0));
    int32_t mask1    = -((int32_t)SST_FP_GreaterThanOrEqual(x, edge1));
    int32_t in_range = ~(mask0 | mask1);

    SST_FP clamped = SST_FP_Max(SST_FP_Min(x, edge1), edge0);

    SST_FP t = SST_FP_DivNoAssert(SST_FP_SubSat(clamped, edge0), SST_FP_SubSat(edge1, edge0));

    SST_FP poly = SST_FP_MulSat(SST_FP_MulSat(t, t),
                                SST_FP_SubSat((SST_FP){3 << SST_FP_FRAC_BITS}, SST_FP_MulSat((SST_FP){2 << SST_FP_FRAC_BITS}, t)));

    int32_t one = 1 << SST_FP_FRAC_BITS;
    int32_t raw = (0 & mask0) | (one & mask1) | (poly.raw_value & in_range);

    return (SST_FP){.raw_value = raw};
}
//======================================================================================================
//======================================================================================================
#endif // SST_FIXED_POINT_H
