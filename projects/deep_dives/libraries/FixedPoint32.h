//======================================================================================================
// [FIXED-POINT ARITHMETIC LIBRARY - 32-BIT FRACTIONAL PART]
//======================================================================================================
// these dont handle over flow
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
static inline SST_FP32 SST_FP32_Add(SST_FP32 a, SST_FP32 b) {
    SST_FP32 result;
    result.raw_value = a.raw_value + b.raw_value;
    return result;
}

static inline SST_FP32 SST_FP32_Sub(SST_FP32 a, SST_FP32 b) {
    SST_FP32 result;
    result.raw_value = a.raw_value - b.raw_value;
    return result;
}

static inline SST_FP32 SST_FP32_Mul(SST_FP32 a, SST_FP32 b) {
    SST_FP32 result;
    result.raw_value = ((__int128_t)a.raw_value * b.raw_value) >> SST_FP32_FRAC_BITS;
    return result;
}

static inline SST_FP32 SST_FP32_Div(SST_FP32 a, SST_FP32 b) {
    assert(b.raw_value != 0); // prevent division by zero
    SST_FP32 result;
    result.raw_value = ((__int128_t)a.raw_value << SST_FP32_FRAC_BITS) / b.raw_value;
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
