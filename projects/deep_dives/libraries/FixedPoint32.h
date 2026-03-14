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
// [GUARDS]
//======================================================================================================
static inline SST_FP32 SST_FP32_Min(SST_FP32 a, SST_FP32 b) {
    int64_t mask = (a.raw_value - b.raw_value) >> 63;
    return (SST_FP32){.raw_value = (a.raw_value & mask) | (b.raw_value & ~mask)};
}

static inline SST_FP32 SST_FP32_Max(SST_FP32 a, SST_FP32 b) {
    int64_t mask = (a.raw_value - b.raw_value) >> 63;
    return (SST_FP32){.raw_value = (b.raw_value & mask) | (a.raw_value & ~mask)};
}
//======================================================================================================
// [FIXED-POINT ARITHMETIC OPERATIONS]
//======================================================================================================
static inline SST_FP32 SST_FP32_AddSat(SST_FP32 a, SST_FP32 b) {
    SST_FP32 result;
    int64_t sum = a.raw_value + b.raw_value;

    int64_t overflow = (a.raw_value ^ sum) & (b.raw_value ^ sum);

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
    int64_t is_min = !(value.raw_value ^ INT64_MIN);

    result.raw_value = (-value.raw_value & ~(-(int64_t)is_min)) | (INT64_MAX & (-(int64_t)is_min));
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
static inline SST_FP32 SST_FP32_Sqrt(SST_FP32 value) {
    assert(value.raw_value >= 0);

    if (value.raw_value == 0) {
        return (SST_FP32){.raw_value = 0};
    }

    int64_t x      = value.raw_value;
    int64_t result = 0;
    int64_t bit    = 1LL << (SST_FP32_FRAC_BITS + 30);

    while (bit > x) {
        bit >>= 2;
    }

    for (int i = 0; i < 2; ++i) {
        while (bit != 0) {
            if (x >= result + bit) {
                x -= result + bit;
                result += bit << 1;
            }
            result >>= 1;
            bit >>= 2;
        }
        if (i == 0) {
            if (x > (1LL << SST_FP32_FRAC_BITS)) {
                x -= result;
                result += 1LL << SST_FP32_FRAC_BITS;
            }
            x <<= SST_FP32_FRAC_BITS;
            bit = 1LL << (SST_FP32_FRAC_BITS + 30);
        }
    }

    return (SST_FP32){.raw_value = result};
}

static inline SST_FP32 SST_FP32_InvSqrt(SST_FP32 value) {
    assert(value.raw_value > 0);

    double x = SST_FP32_ToDouble(value);
    double y = 1.0 / sqrt(x);
    return SST_FP32_FromDouble(y);
}

static inline SST_FP32 SST_FP32_Sin(SST_FP32 value) {
    double x = SST_FP32_ToDouble(value);
    double y = sin(x);
    return SST_FP32_FromDouble(y);
}

static inline SST_FP32 SST_FP32_Cos(SST_FP32 value) {
    double x = SST_FP32_ToDouble(value);
    double y = cos(x);
    return SST_FP32_FromDouble(y);
}

static inline SST_FP32 SST_FP32_Tan(SST_FP32 value) {
    double x = SST_FP32_ToDouble(value);
    double y = tan(x);
    return SST_FP32_FromDouble(y);
}

static inline SST_FP32 SST_FP32_Atan2(SST_FP32 y, SST_FP32 x) {
    double dy    = SST_FP32_ToDouble(y);
    double dx    = SST_FP32_ToDouble(x);
    double angle = atan2(dy, dx);
    return SST_FP32_FromDouble(angle);
}

static inline SST_FP32 SST_FP32_Exp(SST_FP32 value) {
    double x = SST_FP32_ToDouble(value);
    double y = exp(x);
    return SST_FP32_FromDouble(y);
}

static inline SST_FP32 SST_FP32_Log(SST_FP32 value) {
    assert(value.raw_value > 0);

    double x = SST_FP32_ToDouble(value);
    double y = log(x);
    return SST_FP32_FromDouble(y);
}

static inline SST_FP32 SST_FP32_Pow(SST_FP32 base, SST_FP32 exponent) {
    double b = SST_FP32_ToDouble(base);
    double e = SST_FP32_ToDouble(exponent);
    double y = pow(b, e);
    return SST_FP32_FromDouble(y);
}
//======================================================================================================
// [FIXED-POINT MISCELLANEOUS FUNCTIONS]
//======================================================================================================

static inline SST_FP32 SST_FP32_Floor(SST_FP32 value) {
    int64_t mask = (1LL << SST_FP32_FRAC_BITS) - 1;
    int64_t raw  = value.raw_value & ~mask;

    if (value.raw_value < 0 && (value.raw_value & mask) != 0) {
        raw -= (1LL << SST_FP32_FRAC_BITS);
    }

    return (SST_FP32){.raw_value = raw};
}

static inline SST_FP32 SST_FP32_Ceil(SST_FP32 value) {
    int64_t mask = (1LL << SST_FP32_FRAC_BITS) - 1;
    int64_t raw  = value.raw_value & ~mask;

    if (value.raw_value > 0 && (value.raw_value & mask) != 0) {
        raw += (1LL << SST_FP32_FRAC_BITS);
    }

    return (SST_FP32){.raw_value = raw};
}

static inline SST_FP32 SST_FP32_Round(SST_FP32 value) {
    int64_t mask = (1LL << SST_FP32_FRAC_BITS) - 1;
    int64_t raw  = value.raw_value & ~mask;
    int64_t half = 1LL << (SST_FP32_FRAC_BITS - 1);

    if (value.raw_value >= 0) {
        if ((value.raw_value & mask) >= half) {
            raw += (1LL << SST_FP32_FRAC_BITS);
        }
    } else {
        if ((value.raw_value & mask) > half) {
            raw -= (1LL << SST_FP32_FRAC_BITS);
        }
    }

    return (SST_FP32){.raw_value = raw};
}

static inline SST_FP32 SST_FP32_Mod(SST_FP32 a, SST_FP32 b) {
    assert(b.raw_value != 0);

    int64_t safe_b     = b.raw_value | (!b.raw_value);
    SST_FP32 quotient  = SST_FP32_DivNoAssert(a, (SST_FP32){.raw_value = safe_b});
    SST_FP32 truncated = (SST_FP32){.raw_value = quotient.raw_value & ~((1LL << SST_FP32_FRAC_BITS) - 1)};
    return SST_FP32_SubSat(a, SST_FP32_Mul(truncated, (SST_FP32){.raw_value = safe_b}));
}

static inline SST_FP32 SST_FP32_Lerp(SST_FP32 a, SST_FP32 b, SST_FP32 t) {
    SST_FP32 diff   = SST_FP32_SubSat(b, a);
    SST_FP32 scaled = SST_FP32_Mul(diff, t);
    return SST_FP32_AddSat(a, scaled);
}

static inline SST_FP32 SST_FP32_SmoothStep(SST_FP32 edge0, SST_FP32 edge1, SST_FP32 x) {
    if (SST_FP32_LessThanOrEqual(x, edge0)) {
        return (SST_FP32){.raw_value = 0};
    } else if (SST_FP32_GreaterThanOrEqual(x, edge1)) {
        return (SST_FP32){.raw_value = 1LL << SST_FP32_FRAC_BITS};
    } else {
        SST_FP32 t = SST_FP32_DivNoAssert(SST_FP32_SubSat(x, edge0), SST_FP32_SubSat(edge1, edge0));
        return SST_FP32_Mul(SST_FP32_Mul(t, t), SST_FP32_SubSat((SST_FP32){.raw_value = 3LL << SST_FP32_FRAC_BITS},
                                                                SST_FP32_Mul((SST_FP32){.raw_value = 2LL << SST_FP32_FRAC_BITS}, t)));
    }
}
//======================================================================================================
//======================================================================================================

//======================================================================================================
#endif // SST_FIXED_POINT_32_H
