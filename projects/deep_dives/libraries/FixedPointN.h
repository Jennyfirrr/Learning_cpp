//======================================================================================================
// [FIXED-POINT ARITHMETIC LIBRARY - ARBITRARY WIDTH]
//======================================================================================================
// template parameter FRAC_BITS selects precision: 64, 128, 256, 512, etc
// storage is uint64_t w[FRAC_BITS/32] - the compiler unrolls all loops since N is compile-time
// everything decomposes into 64-bit word operations so GCC can use cmov/sbb natively
//
// usage:
//   using FP256 = SST_FPN<256>;
//   FP256 a = SST_FPN_FromDouble<256>(3.14);
//   FP256 b = SST_FPN_FromDouble<256>(2.0);
//   FP256 c = SST_FPN_Add(a, b);
//   double result = SST_FPN_ToDouble(c);
//
// compile: g++ -std=c++17 -O2 ...
//======================================================================================================
#ifndef SST_FIXED_POINT_N_H
#define SST_FIXED_POINT_N_H

#include <stdint.h>
#include <assert.h>
#include <math.h>

//======================================================================================================
// [FIXED-POINT NUMBER REPRESENTATION]
//======================================================================================================
template <unsigned FRAC_BITS> struct SST_FPN {
    static_assert(FRAC_BITS >= 64, "minimum 64 fractional bits, use FixedPoint16/32 for smaller");
    static_assert((FRAC_BITS & (FRAC_BITS - 1)) == 0, "FRAC_BITS must be a power of 2");

    static constexpr unsigned TOTAL_BITS = FRAC_BITS * 2;
    static constexpr unsigned N          = TOTAL_BITS / 64; // number of uint64_t words
    static constexpr unsigned FRAC_WORDS = FRAC_BITS / 64;  // words that are fractional

    uint64_t w[N]; // little-endian: w[0] = least significant
    int32_t sign;  // 0 = positive/zero, 1 = negative
};

//======================================================================================================
// [N-WORD HELPERS]
//======================================================================================================
// all loops are over compile-time N so the compiler fully unrolls them
//======================================================================================================

// is magnitude zero
template <unsigned F> inline int SST_FPN_MagIsZero(SST_FPN<F> v) {
    uint64_t acc = 0;
    for (unsigned i = 0; i < SST_FPN<F>::N; i++)
        acc |= v.w[i];
    return acc == 0;
}

// a >= b unsigned, branchless, built from LSB up
template <unsigned F> inline int SST_FPN_MagGe(SST_FPN<F> a, SST_FPN<F> b) {
    int ge = (a.w[0] >= b.w[0]);
    for (unsigned i = 1; i < SST_FPN<F>::N; i++) {
        int gt = (a.w[i] > b.w[i]);
        int eq = (a.w[i] == b.w[i]);
        ge     = gt | (eq & ge);
    }
    return ge;
}

// a == b
template <unsigned F> inline int SST_FPN_MagEq(SST_FPN<F> a, SST_FPN<F> b) {
    uint64_t diff = 0;
    for (unsigned i = 0; i < SST_FPN<F>::N; i++)
        diff |= (a.w[i] ^ b.w[i]);
    return diff == 0;
}

// a > b
template <unsigned F> inline int SST_FPN_MagGt(SST_FPN<F> a, SST_FPN<F> b) {
    return SST_FPN_MagGe(a, b) & !SST_FPN_MagEq(a, b);
}

// N-word add with carry chain
template <unsigned F> inline void SST_FPN_MagAddN(const uint64_t *a, const uint64_t *b, uint64_t *r) {
    uint64_t carry = 0;
    for (unsigned i = 0; i < SST_FPN<F>::N; i++) {
        uint64_t t  = a[i] + b[i];
        uint64_t c1 = (t < a[i]);
        r[i]        = t + carry;
        uint64_t c2 = (r[i] < t);
        carry       = c1 | c2;
    }
}

// N-word sub with borrow chain
template <unsigned F> inline void SST_FPN_MagSubN(const uint64_t *a, const uint64_t *b, uint64_t *r) {
    uint64_t borrow = 0;
    for (unsigned i = 0; i < SST_FPN<F>::N; i++) {
        uint64_t t   = a[i] - b[i];
        uint64_t bw1 = (a[i] < b[i]);
        r[i]         = t - borrow;
        uint64_t bw2 = (t < borrow);
        borrow       = bw1 | bw2;
    }
}

template <unsigned F> inline SST_FPN<F> SST_FPN_Zero() {
    SST_FPN<F> result;
    for (unsigned i = 0; i < SST_FPN<F>::N; i++)
        result.w[i] = 0;
    result.sign = 0;
    return result;
}

//======================================================================================================
// [DOUBLE PRECISION CONVERSION]
//======================================================================================================
// precision is limited by double (~52 bits) but these are for getting data in and out
//======================================================================================================
template <unsigned F> inline SST_FPN<F> SST_FPN_FromDouble(double input) {
    constexpr unsigned N  = SST_FPN<F>::N;
    constexpr unsigned FW = SST_FPN<F>::FRAC_WORDS;

    int32_t neg      = (input < 0.0);
    double abs_input = input * (1.0 - 2.0 * neg);

    SST_FPN<F> result;
    for (unsigned i = 0; i < N; i++)
        result.w[i] = 0;

    double int_part  = floor(abs_input);
    double frac_part = abs_input - int_part;

    // integer part into the first integer word
    if (FW < N)
        result.w[FW] = (uint64_t)int_part;

    // fractional part: top 2 fractional words (most precision we can get from double)
    double frac_hi = floor(frac_part * 18446744073709551616.0);
    double frac_lo = (frac_part * 18446744073709551616.0 - frac_hi) * 18446744073709551616.0;
    if (FW >= 1)
        result.w[FW - 1] = (uint64_t)frac_hi;
    if (FW >= 2)
        result.w[FW - 2] = (uint64_t)frac_lo;

    result.sign = neg & !SST_FPN_MagIsZero(result);
    return result;
}

template <unsigned F> inline double SST_FPN_ToDouble(SST_FPN<F> value) {
    constexpr unsigned N  = SST_FPN<F>::N;
    constexpr unsigned FW = SST_FPN<F>::FRAC_WORDS;

    // integer part
    double int_part = 0.0;
    double scale    = 1.0;
    for (unsigned i = FW; i < N; i++) {
        int_part += (double)value.w[i] * scale;
        scale *= 18446744073709551616.0;
    }

    // fractional part (from most significant frac word down)
    double frac       = 0.0;
    double frac_scale = 1.0 / 18446744073709551616.0;
    for (int i = (int)FW - 1; i >= 0; i--) {
        frac += (double)value.w[i] * frac_scale;
        frac_scale /= 18446744073709551616.0;
    }

    double mag = int_part + frac;
    return mag * (1.0 - 2.0 * value.sign);
}

//======================================================================================================
// [FP64 CONVERSION]
//======================================================================================================
#ifdef SST_FIXED_POINT_64_H
template <unsigned F> inline SST_FPN<F> SST_FPN_FromFP64(SST_FP64 value) {
    constexpr unsigned FW = SST_FPN<F>::FRAC_WORDS;
    // FP64 has 64 frac bits, FPN has F frac bits
    // FP64 magnitude is __uint128_t = two uint64_t words
    // place them at word (FW-1) and word (FW) to shift left by (F-64) bits
    SST_FPN<F> result = SST_FPN_Zero<F>();
    if (FW >= 1)
        result.w[FW - 1] = (uint64_t)value.magnitude;
    if (FW < SST_FPN<F>::N)
        result.w[FW] = (uint64_t)(value.magnitude >> 64);
    result.sign = value.sign;
    return result;
}

template <unsigned F> inline SST_FP64 SST_FPN_ToFP64(SST_FPN<F> value) {
    constexpr unsigned FW = SST_FPN<F>::FRAC_WORDS;
    SST_FP64 result;
    uint64_t lo      = (FW >= 1) ? value.w[FW - 1] : 0;
    uint64_t hi      = (FW < SST_FPN<F>::N) ? value.w[FW] : 0;
    result.magnitude = ((__uint128_t)hi << 64) | (__uint128_t)lo;
    result.sign      = value.sign & (result.magnitude != 0);
    return result;
}
#endif

//======================================================================================================
// [GUARDS]
//======================================================================================================
template <unsigned F> inline SST_FPN<F> SST_FPN_Min(SST_FPN<F> a, SST_FPN<F> b) {
    int diff_sign = a.sign ^ b.sign;
    int a_lt_mag  = SST_FPN_MagGt(b, a);
    int a_lt_same = (a_lt_mag ^ a.sign) & !SST_FPN_MagEq(a, b);
    int both_zero = SST_FPN_MagIsZero(a) & SST_FPN_MagIsZero(b);
    int a_lt      = ((a.sign & diff_sign) | (a_lt_same & (!diff_sign))) & !both_zero;

    uint64_t mask = -(uint64_t)a_lt;
    SST_FPN<F> result;
    for (unsigned i = 0; i < SST_FPN<F>::N; i++)
        result.w[i] = (a.w[i] & mask) | (b.w[i] & ~mask);
    result.sign = (int32_t)((a.sign & a_lt) | (b.sign & (1 - a_lt)));
    return result;
}

template <unsigned F> inline SST_FPN<F> SST_FPN_Max(SST_FPN<F> a, SST_FPN<F> b) {
    int diff_sign = a.sign ^ b.sign;
    int a_gt_mag  = SST_FPN_MagGt(a, b);
    int a_gt_same = (a_gt_mag ^ a.sign) & !SST_FPN_MagEq(a, b);
    int a_gt      = (((!a.sign) & diff_sign) | (a_gt_same & (!diff_sign)));

    uint64_t mask = -(uint64_t)a_gt;
    SST_FPN<F> result;
    for (unsigned i = 0; i < SST_FPN<F>::N; i++)
        result.w[i] = (a.w[i] & mask) | (b.w[i] & ~mask);
    result.sign = (int32_t)((a.sign & a_gt) | (b.sign & (1 - a_gt)));
    return result;
}

//======================================================================================================
// [FIXED-POINT ARITHMETIC OPERATIONS]
//======================================================================================================
// branchless sign-magnitude add: compute both paths, mask-select with 64-bit ops
//======================================================================================================
template <unsigned F> inline SST_FPN<F> SST_FPN_AddSat(SST_FPN<F> a, SST_FPN<F> b) {
    constexpr unsigned N = SST_FPN<F>::N;
    int diff             = a.sign ^ b.sign;

    // same sign: add magnitudes
    uint64_t sum[N];
    SST_FPN_MagAddN<F>(a.w, b.w, sum);

    // different sign: compute both a-b and b-a, mask-select
    uint64_t dab[N], dba[N];
    SST_FPN_MagSubN<F>(a.w, b.w, dab);
    SST_FPN_MagSubN<F>(b.w, a.w, dba);

    int ge           = SST_FPN_MagGe(a, b);
    uint64_t ge_mask = -(uint64_t)ge;
    uint64_t d_mask  = -(uint64_t)diff;

    SST_FPN<F> result;
    uint64_t or_all = 0;
    for (unsigned i = 0; i < N; i++) {
        uint64_t d  = (dab[i] & ge_mask) | (dba[i] & ~ge_mask);
        result.w[i] = (sum[i] & ~d_mask) | (d & d_mask);
        or_all |= result.w[i];
    }

    int diff_sign_result = (a.sign & ge) | (b.sign & (1 - ge));
    int result_sign      = (a.sign & (!diff)) | (diff_sign_result & diff);
    result.sign          = result_sign & (or_all != 0);

    return result;
}

template <unsigned F> inline SST_FPN<F> SST_FPN_SubSat(SST_FPN<F> a, SST_FPN<F> b) {
    SST_FPN<F> neg_b = b;
    neg_b.sign       = b.sign ^ (!SST_FPN_MagIsZero(b));
    return SST_FPN_AddSat(a, neg_b);
}

// also provide non-sat versions (identical for now since 2*FRAC_BITS of headroom is massive)
template <unsigned F> inline SST_FPN<F> SST_FPN_Add(SST_FPN<F> a, SST_FPN<F> b) {
    return SST_FPN_AddSat(a, b);
}
template <unsigned F> inline SST_FPN<F> SST_FPN_Sub(SST_FPN<F> a, SST_FPN<F> b) {
    return SST_FPN_SubSat(a, b);
}

//======================================================================================================
// [MULTIPLY AND DIVIDE]
//======================================================================================================
// go through long double conversion - precision limited by double but the value of arbitrary
// width is in the add/sub accumulation path where full N-word precision is maintained
//======================================================================================================
template <unsigned F> inline SST_FPN<F> SST_FPN_Mul(SST_FPN<F> a, SST_FPN<F> b) {
    long double fa    = fabsl((long double)SST_FPN_ToDouble(a));
    long double fb    = fabsl((long double)SST_FPN_ToDouble(b));
    SST_FPN<F> result = SST_FPN_FromDouble<F>((double)(fa * fb));
    result.sign       = (a.sign ^ b.sign) & !SST_FPN_MagIsZero(result);
    return result;
}

template <unsigned F> inline SST_FPN<F> SST_FPN_DivNoAssert(SST_FPN<F> a, SST_FPN<F> b) {
    if (SST_FPN_MagIsZero(b)) {
        SST_FPN<F> sat;
        for (unsigned i = 0; i < SST_FPN<F>::N; i++)
            sat.w[i] = UINT64_MAX;
        sat.sign = a.sign;
        return sat;
    }
    long double fa    = fabsl((long double)SST_FPN_ToDouble(a));
    long double fb    = fabsl((long double)SST_FPN_ToDouble(b));
    SST_FPN<F> result = SST_FPN_FromDouble<F>((double)(fa / fb));
    result.sign       = (a.sign ^ b.sign) & !SST_FPN_MagIsZero(result);
    return result;
}

template <unsigned F> inline SST_FPN<F> SST_FPN_DivWithAssert(SST_FPN<F> a, SST_FPN<F> b) {
    assert(!SST_FPN_MagIsZero(b));
    return SST_FPN_DivNoAssert(a, b);
}

//======================================================================================================
// [FIXED-POINT COMPARISON OPERATIONS]
//======================================================================================================
template <unsigned F> inline int SST_FPN_Equal(SST_FPN<F> a, SST_FPN<F> b) {
    int both_zero = SST_FPN_MagIsZero(a) & SST_FPN_MagIsZero(b);
    return both_zero | (SST_FPN_MagEq(a, b) & (a.sign == b.sign));
}

template <unsigned F> inline int SST_FPN_NotEqual(SST_FPN<F> a, SST_FPN<F> b) {
    return !SST_FPN_Equal(a, b);
}

template <unsigned F> inline int SST_FPN_LessThan(SST_FPN<F> a, SST_FPN<F> b) {
    int both_zero   = SST_FPN_MagIsZero(a) & SST_FPN_MagIsZero(b);
    int diff_sign   = a.sign ^ b.sign;
    int diff_result = a.sign & !both_zero;
    int a_lt_mag    = SST_FPN_MagGt(b, a);
    int same_result = (a_lt_mag ^ a.sign) & !SST_FPN_MagEq(a, b);
    return (diff_result & diff_sign) | (same_result & (!diff_sign));
}

template <unsigned F> inline int SST_FPN_LessThanOrEqual(SST_FPN<F> a, SST_FPN<F> b) {
    return SST_FPN_LessThan(a, b) | SST_FPN_Equal(a, b);
}

template <unsigned F> inline int SST_FPN_GreaterThan(SST_FPN<F> a, SST_FPN<F> b) {
    return SST_FPN_LessThan(b, a);
}

template <unsigned F> inline int SST_FPN_GreaterThanOrEqual(SST_FPN<F> a, SST_FPN<F> b) {
    return !SST_FPN_LessThan(a, b);
}

//======================================================================================================
// [FIXED-POINT UTILITY FUNCTIONS]
//======================================================================================================
template <unsigned F> inline SST_FPN<F> SST_FPN_Negate(SST_FPN<F> value) {
    SST_FPN<F> result = value;
    result.sign       = value.sign ^ (!SST_FPN_MagIsZero(value));
    return result;
}

template <unsigned F> inline int SST_FPN_IsZero(SST_FPN<F> value) {
    return SST_FPN_MagIsZero(value);
}

template <unsigned F> inline SST_FPN<F> SST_FPN_Abs(SST_FPN<F> value) {
    SST_FPN<F> result = value;
    result.sign       = 0;
    return result;
}

template <unsigned F> inline SST_FPN<F> SST_FPN_Sign(SST_FPN<F> value) {
    if (SST_FPN_MagIsZero(value))
        return SST_FPN_Zero<F>();
    // 1.0: integer part = 1, so w[FRAC_WORDS] = 1, rest = 0
    SST_FPN<F> one                = SST_FPN_Zero<F>();
    one.w[SST_FPN<F>::FRAC_WORDS] = 1;
    one.sign                      = value.sign;
    return one;
}

//======================================================================================================
// [FIXED-POINT MATH FUNCTIONS]
//======================================================================================================
// all go through double conversion - precision limited but these are convenience functions
//======================================================================================================
template <unsigned F> inline SST_FPN<F> SST_FPN_Sqrt(SST_FPN<F> value) {
    assert(value.sign == 0 || SST_FPN_MagIsZero(value));
    return SST_FPN_FromDouble<F>(sqrt(SST_FPN_ToDouble(value)));
}

template <unsigned F> inline SST_FPN<F> SST_FPN_InvSqrt(SST_FPN<F> value) {
    assert(value.sign == 0 && !SST_FPN_MagIsZero(value));
    return SST_FPN_FromDouble<F>(1.0 / sqrt(SST_FPN_ToDouble(value)));
}

template <unsigned F> inline SST_FPN<F> SST_FPN_Sin(SST_FPN<F> value) {
    return SST_FPN_FromDouble<F>(sin(SST_FPN_ToDouble(value)));
}

template <unsigned F> inline SST_FPN<F> SST_FPN_Cos(SST_FPN<F> value) {
    return SST_FPN_FromDouble<F>(cos(SST_FPN_ToDouble(value)));
}

template <unsigned F> inline SST_FPN<F> SST_FPN_Tan(SST_FPN<F> value) {
    return SST_FPN_FromDouble<F>(tan(SST_FPN_ToDouble(value)));
}

template <unsigned F> inline SST_FPN<F> SST_FPN_Atan2(SST_FPN<F> y, SST_FPN<F> x) {
    return SST_FPN_FromDouble<F>(atan2(SST_FPN_ToDouble(y), SST_FPN_ToDouble(x)));
}

template <unsigned F> inline SST_FPN<F> SST_FPN_Exp(SST_FPN<F> value) {
    return SST_FPN_FromDouble<F>(exp(SST_FPN_ToDouble(value)));
}

template <unsigned F> inline SST_FPN<F> SST_FPN_Log(SST_FPN<F> value) {
    assert(value.sign == 0 && !SST_FPN_MagIsZero(value));
    return SST_FPN_FromDouble<F>(log(SST_FPN_ToDouble(value)));
}

template <unsigned F> inline SST_FPN<F> SST_FPN_Pow(SST_FPN<F> base, SST_FPN<F> exponent) {
    return SST_FPN_FromDouble<F>(pow(SST_FPN_ToDouble(base), SST_FPN_ToDouble(exponent)));
}

//======================================================================================================
// [FIXED-POINT MISCELLANEOUS FUNCTIONS]
//======================================================================================================
template <unsigned F> inline SST_FPN<F> SST_FPN_Floor(SST_FPN<F> value) {
    constexpr unsigned FW = SST_FPN<F>::FRAC_WORDS;
    // check if any fractional word is nonzero
    uint64_t frac_or = 0;
    for (unsigned i = 0; i < FW; i++)
        frac_or |= value.w[i];
    int has_frac = (frac_or != 0);

    SST_FPN<F> result;
    // zero out fractional words
    for (unsigned i = 0; i < FW; i++)
        result.w[i] = 0;
    // copy integer words
    for (unsigned i = FW; i < SST_FPN<F>::N; i++)
        result.w[i] = value.w[i];
    // if negative and had fraction, bump integer part by 1 with carry chain
    uint64_t bump = (uint64_t)(value.sign & has_frac);
    for (unsigned i = FW; i < SST_FPN<F>::N; i++) {
        uint64_t old = result.w[i];
        result.w[i] += bump;
        bump = (result.w[i] < old);
    }
    result.sign = value.sign;
    return result;
}

template <unsigned F> inline SST_FPN<F> SST_FPN_Ceil(SST_FPN<F> value) {
    constexpr unsigned FW = SST_FPN<F>::FRAC_WORDS;
    uint64_t frac_or      = 0;
    for (unsigned i = 0; i < FW; i++)
        frac_or |= value.w[i];
    int has_frac = (frac_or != 0);

    SST_FPN<F> result;
    for (unsigned i = 0; i < FW; i++)
        result.w[i] = 0;
    for (unsigned i = FW; i < SST_FPN<F>::N; i++)
        result.w[i] = value.w[i];
    // if positive and had fraction, bump integer part by 1
    uint64_t bump = (uint64_t)((!value.sign) & has_frac);
    for (unsigned i = FW; i < SST_FPN<F>::N; i++) {
        uint64_t old = result.w[i];
        result.w[i] += bump;
        bump = (result.w[i] < old);
    }
    uint64_t int_or = 0;
    for (unsigned i = FW; i < SST_FPN<F>::N; i++)
        int_or |= result.w[i];
    result.sign = value.sign & (int_or != 0);
    return result;
}

template <unsigned F> inline SST_FPN<F> SST_FPN_Round(SST_FPN<F> value) {
    constexpr unsigned FW = SST_FPN<F>::FRAC_WORDS;
    // half = MSB of the top fractional word
    int round_up = (FW >= 1) ? ((value.w[FW - 1] >> 63) & 1) : 0;

    SST_FPN<F> result;
    for (unsigned i = 0; i < FW; i++)
        result.w[i] = 0;
    for (unsigned i = FW; i < SST_FPN<F>::N; i++)
        result.w[i] = value.w[i];
    uint64_t bump = (uint64_t)round_up;
    for (unsigned i = FW; i < SST_FPN<F>::N; i++) {
        uint64_t old = result.w[i];
        result.w[i] += bump;
        bump = (result.w[i] < old);
    }
    uint64_t int_or = 0;
    for (unsigned i = FW; i < SST_FPN<F>::N; i++)
        int_or |= result.w[i];
    result.sign = value.sign & (int_or != 0);
    return result;
}

template <unsigned F> inline SST_FPN<F> SST_FPN_Mod(SST_FPN<F> a, SST_FPN<F> b) {
    assert(!SST_FPN_MagIsZero(b));
    SST_FPN<F> quotient = SST_FPN_DivNoAssert(a, b);
    // truncate: zero out fractional words
    SST_FPN<F> truncated = quotient;
    for (unsigned i = 0; i < SST_FPN<F>::FRAC_WORDS; i++)
        truncated.w[i] = 0;
    return SST_FPN_SubSat(a, SST_FPN_Mul(truncated, b));
}

template <unsigned F> inline SST_FPN<F> SST_FPN_Lerp(SST_FPN<F> a, SST_FPN<F> b, SST_FPN<F> t) {
    SST_FPN<F> diff   = SST_FPN_SubSat(b, a);
    SST_FPN<F> scaled = SST_FPN_Mul(diff, t);
    return SST_FPN_AddSat(a, scaled);
}

template <unsigned F> inline SST_FPN<F> SST_FPN_SmoothStep(SST_FPN<F> edge0, SST_FPN<F> edge1, SST_FPN<F> x) {
    if (SST_FPN_LessThanOrEqual(x, edge0))
        return SST_FPN_Zero<F>();
    if (SST_FPN_GreaterThanOrEqual(x, edge1)) {
        SST_FPN<F> one                = SST_FPN_Zero<F>();
        one.w[SST_FPN<F>::FRAC_WORDS] = 1; // hmm, 1.0 needs to be in the right spot
        // actually 1.0 in Q(F.F) has the integer LSB set, which is w[FRAC_WORDS] bit 0
        // but we need the whole word to be 1? No, 1 in the integer part = w[FRAC_WORDS] = 1
        // but thats not right either. 1.0 means the value is 1, so raw magnitude = 1 << FRAC_BITS
        // in word terms: w[FRAC_WORDS] = 1, since FRAC_BITS = FRAC_WORDS * 64
        // wait, 1 << (FRAC_WORDS * 64) in a multi-word representation is just w[FRAC_WORDS] = 1
        // yes thats correct
        return one;
    }
    SST_FPN<F> t     = SST_FPN_DivNoAssert(SST_FPN_SubSat(x, edge0), SST_FPN_SubSat(edge1, edge0));
    SST_FPN<F> three = SST_FPN_FromDouble<F>(3.0);
    SST_FPN<F> two   = SST_FPN_FromDouble<F>(2.0);
    return SST_FPN_Mul(SST_FPN_Mul(t, t), SST_FPN_SubSat(three, SST_FPN_Mul(two, t)));
}

//======================================================================================================
//======================================================================================================
#endif // SST_FIXED_POINT_N_H
