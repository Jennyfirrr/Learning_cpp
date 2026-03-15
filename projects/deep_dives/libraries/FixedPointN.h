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
#pragma GCC unroll 65534
    for (unsigned i = 0; i < SST_FPN<F>::N; i++)
        acc |= v.w[i];
    return acc == 0;
}

// a >= b unsigned, branchless, built from LSB up
template <unsigned F> inline int SST_FPN_MagGe(SST_FPN<F> a, SST_FPN<F> b) {
    int ge = (a.w[0] >= b.w[0]);
#pragma GCC unroll 65534
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
#pragma GCC unroll 65534
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
#pragma GCC unroll 65534
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
#pragma GCC unroll 65534
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
#pragma GCC unroll 65534
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
    #pragma GCC unroll 65534
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

    // double only has ~52 bits of mantissa, so we only need the most significant
    // non-zero words. processing all words at large N causes scale to hit inf
    // and frac_scale to hit 0, producing NaN from 0*inf

    // integer part: find highest non-zero integer word, convert from there down
    // (at most 2 words contribute meaningful precision to a double)
    double int_part = 0.0;
    int top_int = -1;
    for (int i = (int)N - 1; i >= (int)FW; i--) {
        if (value.w[i] != 0) { top_int = i; break; }
    }
    if (top_int >= 0) {
        double scale = 1.0;
        for (int i = (int)FW; i < top_int; i++) scale *= 18446744073709551616.0;
        // convert top 2 words (more than enough for double precision)
        int start = (top_int > (int)FW) ? (top_int - 1) : top_int;
        double s = scale;
        for (int i = (int)FW; i < start; i++) s *= 18446744073709551616.0;
        // just do it simply: accumulate from FW up to top_int
        double sc = 1.0;
        for (int i = (int)FW; i <= top_int; i++) {
            int_part += (double)value.w[i] * sc;
            sc *= 18446744073709551616.0;
        }
    }

    // fractional part: only top 2 fractional words matter for double precision
    double frac = 0.0;
    if (FW >= 1) {
        frac += (double)value.w[FW - 1] / 18446744073709551616.0;
        if (FW >= 2)
            frac += (double)value.w[FW - 2] / (18446744073709551616.0 * 18446744073709551616.0);
    }

    double mag = int_part + frac;
    return mag * (1.0 - 2.0 * value.sign);
}

//======================================================================================================
// [STRING CONVERSION - FULL PRECISION]
//======================================================================================================
// not limited by double's 52-bit mantissa - these preserve every bit
// uses single-word multiply/divmod by 10 across the word array
// decimal_places controls fractional digits (max meaningful ≈ FRAC_BITS * 0.301)
//======================================================================================================

// helper: multiply N-word array in-place by a single uint64_t, return overflow
template <unsigned N_WORDS>
inline uint64_t SST_FPN_MulSingle(uint64_t *a, uint64_t b) {
    uint64_t carry = 0;
    for (unsigned i = 0; i < N_WORDS; i++) {
        __uint128_t prod = (__uint128_t)a[i] * b + carry;
        a[i]  = (uint64_t)prod;
        carry = (uint64_t)(prod >> 64);
    }
    return carry;
}

// helper: divmod N-word array in-place by a single uint64_t, return remainder
template <unsigned N_WORDS>
inline uint64_t SST_FPN_DivModSingle(uint64_t *a, uint64_t b) {
    uint64_t rem = 0;
    for (int i = (int)N_WORDS - 1; i >= 0; i--) {
        __uint128_t cur = ((__uint128_t)rem << 64) | a[i];
        a[i] = (uint64_t)(cur / b);
        rem  = (uint64_t)(cur % b);
    }
    return rem;
}

// helper: is N-word array all zeros
template <unsigned N_WORDS>
inline int SST_FPN_ArrayIsZero(const uint64_t *a) {
    uint64_t acc = 0;
    for (unsigned i = 0; i < N_WORDS; i++) acc |= a[i];
    return acc == 0;
}

// max meaningful decimal digits for the fractional part
template <unsigned F>
inline constexpr unsigned SST_FPN_MaxDecimalDigits() {
    return (unsigned)((uint64_t)F * 301 / 1000) + 1; // ceil(FRAC_BITS * log10(2))
}

// convert to decimal string, returns number of chars written (excluding null terminator)
// buf must be large enough: sign + integer digits + '.' + decimal_places + '\0'
// if decimal_places is 0, uses max meaningful precision
template <unsigned F>
inline unsigned SST_FPN_ToString(SST_FPN<F> value, char *buf, unsigned buf_size, unsigned decimal_places = 0) {
    constexpr unsigned N  = SST_FPN<F>::N;
    constexpr unsigned FW = SST_FPN<F>::FRAC_WORDS;
    constexpr unsigned IW = N - FW;

    if (decimal_places == 0) decimal_places = SST_FPN_MaxDecimalDigits<F>();

    unsigned pos = 0;

    // sign
    if (value.sign && !SST_FPN_MagIsZero(value)) {
        if (pos < buf_size - 1) buf[pos++] = '-';
    }

    // integer part: repeated divmod by 10, collect digits reversed
    uint64_t int_words[IW];
    for (unsigned i = 0; i < IW; i++) int_words[i] = value.w[FW + i];

    char int_digits[IW * 20 + 1];
    unsigned n_int_digits = 0;

    if (SST_FPN_ArrayIsZero<IW>(int_words)) {
        int_digits[n_int_digits++] = '0';
    } else {
        while (!SST_FPN_ArrayIsZero<IW>(int_words)) {
            uint64_t rem = SST_FPN_DivModSingle<IW>(int_words, 10);
            int_digits[n_int_digits++] = '0' + (char)rem;
        }
    }

    // write integer digits (reversed)
    for (int i = (int)n_int_digits - 1; i >= 0 && pos < buf_size - 1; i--)
        buf[pos++] = int_digits[i];

    // fractional part: repeated multiply by 10, overflow digit is the next decimal digit
    if (decimal_places > 0 && pos < buf_size - 1) {
        buf[pos++] = '.';

        uint64_t frac_words[FW];
        for (unsigned i = 0; i < FW; i++) frac_words[i] = value.w[i];

        for (unsigned d = 0; d < decimal_places && pos < buf_size - 1; d++) {
            uint64_t digit = SST_FPN_MulSingle<FW>(frac_words, 10);
            buf[pos++] = '0' + (char)digit;
        }
    }

    buf[pos] = '\0';
    return pos;
}

// parse decimal string to fixed-point
// accepts optional sign, integer digits, optional '.', fractional digits
template <unsigned F>
inline SST_FPN<F> SST_FPN_FromString(const char *str) {
    constexpr unsigned N  = SST_FPN<F>::N;
    constexpr unsigned FW = SST_FPN<F>::FRAC_WORDS;
    constexpr unsigned IW = N - FW;

    SST_FPN<F> result = SST_FPN_Zero<F>();
    unsigned i = 0;

    // sign
    int neg = 0;
    if (str[i] == '-') { neg = 1; i++; }
    else if (str[i] == '+') { i++; }

    // integer part: left to right, result = result * 10 + digit
    uint64_t int_words[IW];
    for (unsigned w = 0; w < IW; w++) int_words[w] = 0;

    while (str[i] >= '0' && str[i] <= '9') {
        SST_FPN_MulSingle<IW>(int_words, 10);
        __uint128_t sum = (__uint128_t)int_words[0] + (uint64_t)(str[i] - '0');
        int_words[0] = (uint64_t)sum;
        uint64_t carry = (uint64_t)(sum >> 64);
        for (unsigned w = 1; w < IW && carry; w++) {
            sum = (__uint128_t)int_words[w] + carry;
            int_words[w] = (uint64_t)sum;
            carry = (uint64_t)(sum >> 64);
        }
        i++;
    }

    for (unsigned w = 0; w < IW; w++) result.w[FW + w] = int_words[w];

    // fractional part: collect digits, process right to left
    // for each digit (right to left): place digit in integer word, divmod (FW+1) words by 10
    // this converts 0.12345 by computing (((((0+5)/10)+4)/10)+3)/10...
    if (str[i] == '.') {
        i++;

        const char *frac_start = &str[i];
        unsigned n_frac = 0;
        while (str[i] >= '0' && str[i] <= '9') { n_frac++; i++; }

        uint64_t frac_words[FW + 1];
        for (unsigned w = 0; w <= FW; w++) frac_words[w] = 0;

        for (int d = (int)n_frac - 1; d >= 0; d--) {
            frac_words[FW] = (uint64_t)(frac_start[d] - '0');
            SST_FPN_DivModSingle<FW + 1>(frac_words, 10);
        }

        for (unsigned w = 0; w < FW; w++) result.w[w] = frac_words[w];
    }

    result.sign = neg & !SST_FPN_MagIsZero(result);
    return result;
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
    #pragma GCC unroll 65534
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
    #pragma GCC unroll 65534
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
    #pragma GCC unroll 65534
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
// [MULTIPLY - SCHOOLBOOK PARTIAL PRODUCTS]
//======================================================================================================
// splits each N-word magnitude into N single uint64_t words, does N^2 partial multiplies
// each partial is uint64_t * uint64_t -> __uint128_t (one hardware mul instruction)
// the compiler fully unrolls both loops since N is compile-time, so this is straight-line
// multiply-accumulate with no branches
//
// full product is 2N words, we shift right by FRAC_WORDS to get back to Q(F.F)
//======================================================================================================
template <unsigned F> inline SST_FPN<F> SST_FPN_Mul(SST_FPN<F> a, SST_FPN<F> b) {
    constexpr unsigned N  = SST_FPN<F>::N;
    constexpr unsigned FW = SST_FPN<F>::FRAC_WORDS;

    // 2N-word product
    uint64_t p[2 * N] = {0};

    // schoolbook: N^2 partial products, each one mul + add + carry propagation
    // pragma unroll forces GCC to flatten these into straight-line mul/add/carry
#pragma GCC unroll 65534
    for (unsigned i = 0; i < N; i++) {
        uint64_t carry = 0;
#pragma GCC unroll 65534
        for (unsigned j = 0; j < N; j++) {
            __uint128_t prod = (__uint128_t)a.w[i] * b.w[j]
                             + (__uint128_t)p[i + j]
                             + carry;
            p[i + j] = (uint64_t)prod;
            carry    = (uint64_t)(prod >> 64);
        }
        // propagate remaining carry (fixed-trip, compiler unrolls)
#pragma GCC unroll 65534
        for (unsigned k = i + N; k < 2 * N; k++) {
            __uint128_t s = (__uint128_t)p[k] + carry;
            p[k]  = (uint64_t)s;
            carry = (uint64_t)(s >> 64);
        }
    }

    // extract result: shift right by FW words
    // overflow: any word above position FW + N - 1 means we overflowed
    uint64_t overflow = 0;
    #pragma GCC unroll 65534
    for (unsigned i = FW + N; i < 2 * N; i++) overflow |= p[i];
    uint64_t of_mask = -(uint64_t)(overflow != 0);

    SST_FPN<F> result;
    #pragma GCC unroll 65534
    for (unsigned i = 0; i < N; i++) {
        result.w[i] = (p[FW + i] & ~of_mask) | (UINT64_MAX & of_mask);
    }

    result.sign = (a.sign ^ b.sign) & !SST_FPN_MagIsZero(result);
    return result;
}

//======================================================================================================
// [DIVISION - BRANCHLESS LONG DIVISION]
//======================================================================================================
// computes (a_magnitude << FRAC_BITS) / b_magnitude using bit-by-bit long division
// the dividend is 2N words (a shifted left by FRAC_BITS), divisor is N words
// each iteration produces 1 bit of quotient: trial-subtract the divisor from the remainder,
// use branchless mask-select to keep or discard, shift quotient bit in
//
// total iterations: N * 64 (one per bit of quotient)
// each iteration: N-word compare + N-word conditional subtract + 2N-word shift = O(N) work
// all branchless - compare produces a mask, subtract is always computed, mask selects result
//======================================================================================================

// helper: shift a 2N-word value left by 1 bit
template <unsigned F>
inline void SST_FPN_ShiftLeft1_2N(uint64_t *v) {
    constexpr unsigned W = 2 * SST_FPN<F>::N;
#pragma GCC unroll 65534
    for (int i = (int)W - 1; i > 0; i--) {
        v[i] = (v[i] << 1) | (v[i - 1] >> 63);
    }
    v[0] <<= 1;
}

// helper: is N-word a >= N-word b (branchless, from LSB up)
template <unsigned F>
inline int SST_FPN_NWordGe(const uint64_t *a, const uint64_t *b) {
    constexpr unsigned N = SST_FPN<F>::N;
    int ge = (a[0] >= b[0]);
#pragma GCC unroll 65534
    for (unsigned i = 1; i < N; i++) {
        int gt = (a[i] > b[i]);
        int eq = (a[i] == b[i]);
        ge = gt | (eq & ge);
    }
    return ge;
}

// helper: N-word conditional subtract: r = ge ? (a - b) : a
// branchless: always computes both, mask-selects
template <unsigned F>
inline void SST_FPN_CondSub(uint64_t *a, const uint64_t *b, int ge) {
    constexpr unsigned N = SST_FPN<F>::N;
    uint64_t mask = -(uint64_t)ge;

    uint64_t diff[N];
    uint64_t borrow = 0;
#pragma GCC unroll 65534
    for (unsigned i = 0; i < N; i++) {
        uint64_t t   = a[i] - b[i];
        uint64_t bw1 = (a[i] < b[i]);
        diff[i]      = t - borrow;
        uint64_t bw2 = (t < borrow);
        borrow       = bw1 | bw2;
    }

#pragma GCC unroll 65534
    for (unsigned i = 0; i < N; i++) {
        a[i] = (diff[i] & mask) | (a[i] & ~mask);
    }
}

template <unsigned F> inline SST_FPN<F> SST_FPN_DivNoAssert(SST_FPN<F> a, SST_FPN<F> b) {
    constexpr unsigned N  = SST_FPN<F>::N;
    constexpr unsigned FW = SST_FPN<F>::FRAC_WORDS;

    // branchless zero-divisor saturation: if b is zero, safe_b = 1, result gets masked to MAX
    int b_zero = SST_FPN_MagIsZero(b);
    uint64_t bz_mask = -(uint64_t)b_zero;

    // make safe divisor: if b is zero, set LSB to 1 so division executes without UB
    uint64_t divisor[N];
#pragma GCC unroll 65534
    for (unsigned i = 0; i < N; i++) divisor[i] = b.w[i];
    divisor[0] |= (uint64_t)b_zero; // 0 becomes 1, nonzero stays the same

    // dividend = a_magnitude << FRAC_BITS
    // in a 2N-word array: lower FW words are 0, upper N words are a.w shifted up by FW
    uint64_t remainder[2 * N] = {0};
#pragma GCC unroll 65534
    for (unsigned i = 0; i < N; i++) remainder[FW + i] = a.w[i];

    // quotient accumulates bit by bit
    uint64_t quotient[N] = {0};

    // long division: produce N*64 bits of quotient, one bit per iteration
    // each iteration: shift remainder left 1, compare top N words against divisor,
    // conditionally subtract, shift quotient bit in
    constexpr unsigned TOTAL_QBITS = N * 64;
    // NOTE: do NOT unroll this outer loop - at 512+ bits it's 1024+ iterations and GCC chokes
    // the loop counter branch is fixed-trip (branch predictor hits 100%), not data-dependent
    // all the actual math inside (compare, cond-subtract, quotient bit) stays branchless
    for (unsigned bit = 0; bit < TOTAL_QBITS; bit++) {
        // shift remainder left by 1
        SST_FPN_ShiftLeft1_2N<F>(remainder);

        // compare top N words of remainder against divisor
        int ge = SST_FPN_NWordGe<F>(&remainder[N], divisor);

        // conditionally subtract divisor from top N words
        SST_FPN_CondSub<F>(&remainder[N], divisor, ge);

        // shift quotient left by 1 and OR in the new bit
        unsigned word_idx = N - 1 - (bit / 64);
        unsigned bit_idx  = 63 - (bit % 64);
        quotient[word_idx] |= ((uint64_t)ge << bit_idx);
    }

    // build result: quotient is the magnitude, saturate if b was zero
    SST_FPN<F> result;
#pragma GCC unroll 65534
    for (unsigned i = 0; i < N; i++) {
        result.w[i] = (quotient[i] & ~bz_mask) | (UINT64_MAX & bz_mask);
    }

    // sign: XOR inputs, but if b was zero use a's sign
    int normal_sign = (a.sign ^ b.sign) & !SST_FPN_MagIsZero(result);
    int zero_sign   = a.sign;
    result.sign = (normal_sign & (!b_zero)) | (zero_sign & b_zero);

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
    // branchless: compute +/-1.0, then mask to zero if input is zero
    int is_nonzero       = !SST_FPN_MagIsZero(value);
    uint64_t nz_mask     = -(uint64_t)is_nonzero;
    SST_FPN<F> result    = SST_FPN_Zero<F>();
    result.w[SST_FPN<F>::FRAC_WORDS] = 1 & nz_mask;
    result.sign          = value.sign & is_nonzero;
    return result;
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
    #pragma GCC unroll 65534
    for (unsigned i = 0; i < FW; i++)
        frac_or |= value.w[i];
    int has_frac = (frac_or != 0);

    SST_FPN<F> result;
    // zero out fractional words
    #pragma GCC unroll 65534
    for (unsigned i = 0; i < FW; i++)
        result.w[i] = 0;
    // copy integer words
    #pragma GCC unroll 65534
    for (unsigned i = FW; i < SST_FPN<F>::N; i++)
        result.w[i] = value.w[i];
    // if negative and had fraction, bump integer part by 1 with carry chain
    uint64_t bump = (uint64_t)(value.sign & has_frac);
    #pragma GCC unroll 65534
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
    #pragma GCC unroll 65534
    for (unsigned i = 0; i < FW; i++)
        frac_or |= value.w[i];
    int has_frac = (frac_or != 0);

    SST_FPN<F> result;
    #pragma GCC unroll 65534
    for (unsigned i = 0; i < FW; i++)
        result.w[i] = 0;
    #pragma GCC unroll 65534
    for (unsigned i = FW; i < SST_FPN<F>::N; i++)
        result.w[i] = value.w[i];
    // if positive and had fraction, bump integer part by 1
    uint64_t bump = (uint64_t)((!value.sign) & has_frac);
    #pragma GCC unroll 65534
    for (unsigned i = FW; i < SST_FPN<F>::N; i++) {
        uint64_t old = result.w[i];
        result.w[i] += bump;
        bump = (result.w[i] < old);
    }
    uint64_t int_or = 0;
    #pragma GCC unroll 65534
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
    #pragma GCC unroll 65534
    for (unsigned i = 0; i < FW; i++)
        result.w[i] = 0;
    #pragma GCC unroll 65534
    for (unsigned i = FW; i < SST_FPN<F>::N; i++)
        result.w[i] = value.w[i];
    uint64_t bump = (uint64_t)round_up;
    #pragma GCC unroll 65534
    for (unsigned i = FW; i < SST_FPN<F>::N; i++) {
        uint64_t old = result.w[i];
        result.w[i] += bump;
        bump = (result.w[i] < old);
    }
    uint64_t int_or = 0;
    #pragma GCC unroll 65534
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
    #pragma GCC unroll 65534
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
    constexpr unsigned N = SST_FPN<F>::N;

    // always compute the polynomial: t = (x - edge0) / (edge1 - edge0), result = t*t*(3 - 2*t)
    SST_FPN<F> t     = SST_FPN_DivNoAssert(SST_FPN_SubSat(x, edge0), SST_FPN_SubSat(edge1, edge0));
    SST_FPN<F> three = SST_FPN_FromDouble<F>(3.0);
    SST_FPN<F> two   = SST_FPN_FromDouble<F>(2.0);
    SST_FPN<F> poly  = SST_FPN_Mul(SST_FPN_Mul(t, t), SST_FPN_SubSat(three, SST_FPN_Mul(two, t)));

    // clamp conditions
    int below = SST_FPN_LessThanOrEqual(x, edge0);   // -> 0.0
    int above = SST_FPN_GreaterThanOrEqual(x, edge1); // -> 1.0

    // 1.0 constant
    SST_FPN<F> one                = SST_FPN_Zero<F>();
    one.w[SST_FPN<F>::FRAC_WORDS] = 1;

    // mask-select: below -> zero, above -> one, else -> poly
    uint64_t bm = -(uint64_t)below;
    uint64_t am = -(uint64_t)above;
    uint64_t pm = ~bm & ~am; // middle region

    SST_FPN<F> result;
    #pragma GCC unroll 65534
    for (unsigned i = 0; i < N; i++)
        result.w[i] = (poly.w[i] & pm) | (one.w[i] & am);
    // sign: poly could be negative from numerical noise, but clamped regions are non-negative
    result.sign = poly.sign & (int)(pm != 0);
    return result;
}

//======================================================================================================
//======================================================================================================
#endif // SST_FIXED_POINT_N_H
