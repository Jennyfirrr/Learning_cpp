//======================================================================================================
// [FIXED-POINT ARITHMETIC LIBRARY]
//======================================================================================================
#ifndef SST_FIXED_POINT_H
#define SST_FIXED_POINT_H

#define SST_FP_FRAC_BITS 16

#include <stdint.h>
#include <assert.h>

//======================================================================================================
// [FIXED-POINT NUMBER REPRESENTATION]
//======================================================================================================
// so apparently these are considered C or something, idk, but you have to do structs like this to avoid error high lighting, it works with stadnard cpp formatting, but the errors annoyed me and i couldnt fix them, so one thing about this is that it may be better to use this as a 32 bit integer, that way the casting for mulitplication and division dont have to be pushed to 64 bit integers, which allows a single regsiter to hold each value, but idk im just thinking
//======================================================================================================
typedef struct {
    int32_t raw_value;
} SST_FP;
static_assert(sizeof(SST_FP) == 4, "SST_FP must be 4 bytes");

//======================================================================================================
// [DOUBLE PRECISION FIXED-POINT ARITHMETIC]
//======================================================================================================
// im gonna add more types here
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
    result.raw_value = ((__int64_t)a.raw_value << SST_FP_FRAC_BITS) / b.raw_value;
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

//======================================================================================================
//======================================================================================================
//======================================================================================================
//======================================================================================================
//======================================================================================================
//======================================================================================================
#endif // SST_FIXED_POINT_H
