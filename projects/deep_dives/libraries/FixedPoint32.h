//======================================================================================================
// [FIXED-POINT ARITHMETIC LIBRARY - 32-BIT FRACTIONAL PART]
//======================================================================================================
#ifndef SST_FIXED_POINT_32_H
#define SST_FIXED_POINT_32_H

#define SST_FP32_FRAC_BITS 32

#include <stdint.h>
#include <assert.h>
//======================================================================================================
// [FIXED-POINT NUMBER REPRESENTATION]
//======================================================================================================
typedef struct {
    int64_t raw_value;
} SST_FP32;
static_assert(sizeof(SST_FP32) == 8, "SST_FP32 must be 8 bytes");
//======================================================================================================
// [DOUBLE PRECISION FIXED-POINT ARITHMETIC]
//======================================================================================================
static inline SST_FP32 SST_FP32_FromDouble(double input) {
    SST_FP32 result;
    result.raw_value = (int64_t)(input * (1LL << SST_FP32_FRAC_BITS));
    return result;
}

static inline double SST_FP32_ToDouble(SST_FP32 value) {
    return (double)value.raw_value / (1LL << SST_FP32_FRAC_BITS);
}
//======================================================================================================
// [FIXED-POINT ARITHMETIC OPERATIONS]
//======================================================================================================
static inline SST_FP32 SST_FP32_AddSat(SST_FP32 a, SST_FP32 b) {
    SST_FP32 result;
    int64_t sum = a.raw_value + b.raw_value;

    int64_t overflow = (a.raw_value ^ b.raw_value) & (b.raw_value ^ sum);

    int64_t sat = (a.raw_value >> 63) ^ INT64_MAX;

    int64_t mask     = overflow >> 63;
    result.raw_value = (sat & mask) | (sum & ~mask);
    return result;
}

static inline SST_FP32 SST_FP32_SubSat(SST_FP32 a, SST_FP32 b) {
    SST_FP32 result;
    int64_t diff = a.raw_value - b.raw_value;

    int64_t overflow = (a.raw_value ^ b.raw_value) & (a.raw_value ^ diff);

    int64_t sat = (a.raw_value >> 63) ^ INT64_MAX;

    int64_t mask     = overflow >> 63;
    result.raw_value = (sat & mask) | (diff & ~mask);
    return result;
}

static inline SST_FP32 SST_FP32_Mul(SST_FP32 a, SST_FP32 b) {
    SST_FP32 result;
    __int128_t wide    = (__int128_t)a.raw_value * b.raw_value;
    __int128_t shifted = wide >> SST_FP32_FRAC_BITS;

    int64_t hi           = (int64_t)(shifted >> 63);
    int64_t sign_a       = a.raw_value >> 63;
    int64_t sign_b       = b.raw_value >> 63;
    int64_t product_sign = sign_a ^ sign_b;
    int64_t sat          = (product_sign ^ INT64_MAX);

    int overflow = (hi != 0) & (hi != -1);
    int64_t mask = -(int64_t)overflow;

    result.raw_value = (sat & mask) | ((int64_t)shifted & ~mask);
    return result;
}

static inline SST_FP32 SST_FP32_DivNoAssert(SST_FP32 a, SST_FP32 b) {
    int64_t safe_b = b.raw_value | (!b.raw_value);
    SST_FP32 result;
    __int128_t wide = ((__int128_t)a.raw_value << SST_FP32_FRAC_BITS) / safe_b;

    int64_t hi            = (int64_t)(wide >> 63);
    int64_t sign_a        = a.raw_value >> 63;
    int64_t sign_b        = b.raw_value >> 63;
    int64_t quotient_sign = sign_a ^ sign_b;
    int64_t sat           = (quotient_sign ^ INT64_MAX);

    int overflow = (hi != 0) & (hi != -1);
    int64_t mask = -(int64_t)overflow;

    result.raw_value = (sat & mask) | ((int64_t)wide & ~mask);
    return result;
}

static inline SST_FP32 SST_FP32_DivWithAssert(SST_FP32 a, SST_FP32 b) {
    assert(b.raw_value != 0);

    int64_t is_zero   = !b.raw_value;
    int64_t zero_mask = -(int64_t)is_zero;
    int64_t inf_sat   = (a.raw_value >> 63) ^ INT64_MAX;

    int64_t safe_b = b.raw_value | is_zero;

    SST_FP32 result;
    __int128_t wide = ((__int128_t)a.raw_value << SST_FP32_FRAC_BITS) / safe_b;

    int64_t hi            = (int64_t)(wide >> 63);
    int64_t sign_a        = a.raw_value >> 63;
    int64_t sign_b        = b.raw_value >> 63;
    int64_t quotient_sign = sign_a ^ sign_b;
    int64_t sat           = (quotient_sign ^ INT64_MAX);

    int overflow          = (hi != 0) & (hi != -1);
    int64_t mask          = -(int64_t)overflow;
    int64_t combined_mask = zero_mask | mask;

    int64_t sat_value = (inf_sat & zero_mask) | (sat & mask & ~zero_mask);

    result.raw_value = (sat_value & combined_mask) | ((int64_t)wide & ~combined_mask);
    return result;
}

//======================================================================================================
// [FIXED-POINT COMPARISON OPERATIONS]
//======================================================================================================
static inline int SST_FP32_Equal(SST_FP32 a, SST_FP32 b) {
    return a.raw_value == b.raw_value;
}

static inline int SST_FP32_NotEqual(SST_FP32 a, SST_FP32 b) {
    return a.raw_value != b.raw_value;
}

static inline int SST_FP32_LessThan(SST_FP32 a, SST_FP32 b) {
    return a.raw_value < b.raw_value;
}

static inline int SST_FP32_LessThanOrEqual(SST_FP32 a, SST_FP32 b) {
    return a.raw_value <= b.raw_value;
}

static inline int SST_FP32_GreaterThan(SST_FP32 a, SST_FP32 b) {
    return a.raw_value > b.raw_value;
}

static inline int SST_FP32_GreaterThanOrEqual(SST_FP32 a, SST_FP32 b) {
    return a.raw_value >= b.raw_value;
}
//======================================================================================================
// [FIXED-POINT UTILITY FUNCTIONS]
//======================================================================================================
static inline SST_FP32 SST_FP32_Negate(SST_FP32 value) {
    SST_FP32 result;
    result.raw_value = -value.raw_value;
    return result;
}

static inline int SST_FP32_IsZero(SST_FP32 value) {
    return value.raw_value == 0;
}

static inline SST_FP32 SST_FP32_Abs(SST_FP32 value) {
    SST_FP32 result;

    int64_t mask     = value.raw_value >> 63;
    int64_t diff     = value.raw_value + mask;
    result.raw_value = diff - mask;

    return result;
}

static inline SST_FP32 SST_FP32_Sign(SST_FP32 value) {
    int64_t mask = value.raw_value >> 63;
    return (SST_FP32){.raw_value = (mask | (!!value.raw_value))};
}
//======================================================================================================
// [FIXED-POINT MATH FUNCTIONS]
//======================================================================================================
//======================================================================================================
//======================================================================================================

//======================================================================================================
#endif // SST_FIXED_POINT_32_H
