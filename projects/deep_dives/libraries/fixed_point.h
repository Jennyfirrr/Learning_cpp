//======================================================================================================
// [FIXED-POINT ARITHMETIC LIBRARY]
//======================================================================================================
#ifndef SST_FIXED_POINT_H
#define SST_FIXED_POINT_H

#define SST_FP_FRAC_BITS 32

#include <stdint.h>
#include <assert.h>

//======================================================================================================
// [FIXED-POINT NUMBER REPRESENTATION]
//======================================================================================================
// so apparently these are considered C or something, idk, but you have to do structs like this to avoid error high lighting, it works with stadnard cpp formatting, but the errors annoyed me and i couldnt fix them, so one thing about this is that it may be better to use this as a 32 bit integer, that way the casting for mulitplication and division dont have to be pushed to 128 bit integers, which allows a single regsiter to hold each value, but idk im just thinking
//======================================================================================================
typedef struct {
    int64_t raw_value;
} SST_FP;
static_assert(sizeof(SST_FP) == 8, "SST_FP must be 8 bytes");

//======================================================================================================
// [DOUBLE PRECISION FIXED-POINT ARITHMETIC]
//======================================================================================================
static inline SST_FP SST_FP_FromDouble(double input) {
    SST_FP result;
    result.raw_value = (int64_t)(input * (1LL << SST_FP_FRAC_BITS));
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
    result.raw_value = ((__int128)a.raw_value * b.raw_value) >> SST_FP_FRAC_BITS;
    return result;
}

static inline SST_FP SST_FP_Div(SST_FP a, SST_FP b) {
    assert(b.raw_value != 0); // prevent division by zero
    SST_FP result;
    result.raw_value = ((__int128_t)a.raw_value << SST_FP_FRAC_BITS) / b.raw_value;
    return result;
}

//======================================================================================================
//======================================================================================================
//======================================================================================================
//======================================================================================================
//======================================================================================================
//======================================================================================================
#endif // SST_FIXED_POINT_H
