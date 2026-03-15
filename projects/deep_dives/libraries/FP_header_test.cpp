//======================================================================================================
// [FIXED-POINT HEADER TEST]
//======================================================================================================
// set which lib to test by defining FP_TEST_BITS before compiling:
//   g++ -std=c++17 -DFP_TEST_BITS=16 -o fp_test FP_header_test.cpp -lm
//   g++ -std=c++17 -DFP_TEST_BITS=32 -o fp_test FP_header_test.cpp -lm
//   g++ -std=c++17 -DFP_TEST_BITS=64 -o fp_test FP_header_test.cpp -lm
//   g++ -std=c++17 -DFP_TEST_BITS=128 -o fp_test FP_header_test.cpp -lm
//
// defaults to 64 if not specified
//======================================================================================================
#ifndef FP_TEST_BITS
#define FP_TEST_BITS 64
#endif

#include <stdio.h>

//======================================================================================================
// [INCLUDE + ALIAS MACROS]
//======================================================================================================
// maps the generic names to whichever lib you picked so the test code below is write-once
//======================================================================================================
#if FP_TEST_BITS == 16
#include "FixedPoint16.h"
#define FP_T SST_FP
#define FP_FromDouble SST_FP_FromDouble
#define FP_ToDouble SST_FP_ToDouble
#define FP_AddSat SST_FP_AddSat
#define FP_SubSat SST_FP_SubSat
#define FP_Mul SST_FP_MulSat
#define FP_Div SST_FP_DivNoAssert
#define FP_Min SST_FP_Min
#define FP_Max SST_FP_Max
#define FP_Abs SST_FP_Abs
#define FP_Negate SST_FP_Negate
#define FP_Floor SST_FP_Floor
#define FP_Ceil SST_FP_Ceil
#define FP_Round SST_FP_Round
#define FP_Sqrt SST_FP_Sqrt
#define FP_Sin SST_FP_Sin
#define FP_Cos SST_FP_Cos
#define FP_Log SST_FP_Log
#define FP_Exp SST_FP_Exp
#define FP_Pow SST_FP_Pow
#define FP_LessThan SST_FP_LessThan
#define FP_GreaterThan SST_FP_GreaterThan
#define FP_Equal SST_FP_Equal
#define FP_Lerp SST_FP_Lerp
#define FP_LABEL "FP16 (Q16.16)"
#elif FP_TEST_BITS == 32
#include "FixedPoint32.h"
#define FP_T SST_FP32
#define FP_FromDouble SST_FP32_FromDouble
#define FP_ToDouble SST_FP32_ToDouble
#define FP_AddSat SST_FP32_AddSat
#define FP_SubSat SST_FP32_SubSat
#define FP_Mul SST_FP32_Mul
#define FP_Div SST_FP32_DivNoAssert
#define FP_Min SST_FP32_Min
#define FP_Max SST_FP32_Max
#define FP_Abs SST_FP32_Abs
#define FP_Negate SST_FP32_Negate
#define FP_Floor SST_FP32_Floor
#define FP_Ceil SST_FP32_Ceil
#define FP_Round SST_FP32_Round
#define FP_Sqrt SST_FP32_Sqrt
#define FP_Sin SST_FP32_Sin
#define FP_Cos SST_FP32_Cos
#define FP_Log SST_FP32_Log
#define FP_Exp SST_FP32_Exp
#define FP_Pow SST_FP32_Pow
#define FP_LessThan SST_FP32_LessThan
#define FP_GreaterThan SST_FP32_GreaterThan
#define FP_Equal SST_FP32_Equal
#define FP_Lerp SST_FP32_Lerp
#define FP_LABEL "FP32 (Q32.32)"
#elif FP_TEST_BITS == 64
#include "FixedPoint64.h"
#define FP_T SST_FP64
#define FP_FromDouble SST_FP64_FromDouble
#define FP_ToDouble SST_FP64_ToDouble
#define FP_AddSat SST_FP64_AddSat
#define FP_SubSat SST_FP64_SubSat
#define FP_Mul SST_FP64_Mul
#define FP_Div SST_FP64_DivNoAssert
#define FP_Min SST_FP64_Min
#define FP_Max SST_FP64_Max
#define FP_Abs SST_FP64_Abs
#define FP_Negate SST_FP64_Negate
#define FP_Floor SST_FP64_Floor
#define FP_Ceil SST_FP64_Ceil
#define FP_Round SST_FP64_Round
#define FP_Sqrt SST_FP64_Sqrt
#define FP_Sin SST_FP64_Sin
#define FP_Cos SST_FP64_Cos
#define FP_Log SST_FP64_Log
#define FP_Exp SST_FP64_Exp
#define FP_Pow SST_FP64_Pow
#define FP_LessThan SST_FP64_LessThan
#define FP_GreaterThan SST_FP64_GreaterThan
#define FP_Equal SST_FP64_Equal
#define FP_Lerp SST_FP64_Lerp
#define FP_LABEL "FP64 (Q64.64)"
#elif FP_TEST_BITS == 128
#include "FixedPoint64.h"
#include "FixedPoint128.h"
#define FP_T SST_FP128
#define FP_FromDouble SST_FP128_FromDouble
#define FP_ToDouble SST_FP128_ToDouble
#define FP_AddSat SST_FP128_AddSat
#define FP_SubSat SST_FP128_SubSat
#define FP_Mul SST_FP128_Mul
#define FP_Div SST_FP128_DivNoAssert
#define FP_Min SST_FP128_Min
#define FP_Max SST_FP128_Max
#define FP_Abs SST_FP128_Abs
#define FP_Negate SST_FP128_Negate
#define FP_Floor SST_FP128_Floor
#define FP_Ceil SST_FP128_Ceil
#define FP_Round SST_FP128_Round
#define FP_Sqrt SST_FP128_Sqrt
#define FP_Sin SST_FP128_Sin
#define FP_Cos SST_FP128_Cos
#define FP_Log SST_FP128_Log
#define FP_Exp SST_FP128_Exp
#define FP_Pow SST_FP128_Pow
#define FP_LessThan SST_FP128_LessThan
#define FP_GreaterThan SST_FP128_GreaterThan
#define FP_Equal SST_FP128_Equal
#define FP_Lerp SST_FP128_Lerp
#define FP_LABEL "FP128 (Q128.128)"
#elif FP_TEST_BITS >= 64 && (FP_TEST_BITS & (FP_TEST_BITS - 1)) == 0
// generic N-bit lib for any power-of-2 >= 64 (256, 512, 1024, etc)
#include "FixedPointN.h"
using FP_T = SST_FPN<FP_TEST_BITS>;
static inline FP_T FP_FromDouble(double d) {
    return SST_FPN_FromDouble<FP_TEST_BITS>(d);
}
static inline double FP_ToDouble(FP_T v) {
    return SST_FPN_ToDouble(v);
}
static inline FP_T FP_AddSat(FP_T a, FP_T b) {
    return SST_FPN_AddSat(a, b);
}
static inline FP_T FP_SubSat(FP_T a, FP_T b) {
    return SST_FPN_SubSat(a, b);
}
static inline FP_T FP_Mul(FP_T a, FP_T b) {
    return SST_FPN_Mul(a, b);
}
static inline FP_T FP_Div(FP_T a, FP_T b) {
    return SST_FPN_DivNoAssert(a, b);
}
static inline FP_T FP_Min(FP_T a, FP_T b) {
    return SST_FPN_Min(a, b);
}
static inline FP_T FP_Max(FP_T a, FP_T b) {
    return SST_FPN_Max(a, b);
}
static inline FP_T FP_Abs(FP_T a) {
    return SST_FPN_Abs(a);
}
static inline FP_T FP_Negate(FP_T a) {
    return SST_FPN_Negate(a);
}
static inline FP_T FP_Floor(FP_T a) {
    return SST_FPN_Floor(a);
}
static inline FP_T FP_Ceil(FP_T a) {
    return SST_FPN_Ceil(a);
}
static inline FP_T FP_Round(FP_T a) {
    return SST_FPN_Round(a);
}
static inline FP_T FP_Sqrt(FP_T a) {
    return SST_FPN_Sqrt(a);
}
static inline FP_T FP_Sin(FP_T a) {
    return SST_FPN_Sin(a);
}
static inline FP_T FP_Cos(FP_T a) {
    return SST_FPN_Cos(a);
}
static inline FP_T FP_Log(FP_T a) {
    return SST_FPN_Log(a);
}
static inline FP_T FP_Exp(FP_T a) {
    return SST_FPN_Exp(a);
}
static inline FP_T FP_Pow(FP_T a, FP_T b) {
    return SST_FPN_Pow(a, b);
}
static inline int FP_LessThan(FP_T a, FP_T b) {
    return SST_FPN_LessThan(a, b);
}
static inline int FP_GreaterThan(FP_T a, FP_T b) {
    return SST_FPN_GreaterThan(a, b);
}
static inline int FP_Equal(FP_T a, FP_T b) {
    return SST_FPN_Equal(a, b);
}
static inline FP_T FP_Lerp(FP_T a, FP_T b, FP_T t) {
    return SST_FPN_Lerp(a, b, t);
}
#define FP_LABEL "FPN (generic)"
#else
#error "FP_TEST_BITS must be 16, 32, or a power of 2 >= 64"
#endif

using namespace std;

//======================================================================================================
// [TEST HELPERS]
//======================================================================================================
static int tests_passed = 0;
static int tests_failed = 0;

static void check(const char *name, double got, double expected, double tolerance) {
    double diff = got - expected;
    if (diff < 0)
        diff = -diff;
    if (diff <= tolerance) {
        tests_passed++;
    } else {
        printf("  FAIL: %s = %.10f (expected %.10f, off by %.10f)\n", name, got, expected, diff);
        tests_failed++;
    }
}

//======================================================================================================
// [TESTS]
//======================================================================================================
int main() {
    printf("=== Testing %s ===\n\n", FP_LABEL);

    double tol = (FP_TEST_BITS == 16) ? 0.001 : 0.0000001;

    //--- conversion roundtrip ---
    printf("[conversion]\n");
    double test_vals[] = {0.0, 1.0, -1.0, 3.14159, -42.5, 100.125, 0.001};
    for (int i = 0; i < 7; i++) {
        // skip large values for FP16 since it overflows past 32K
        if (FP_TEST_BITS == 16 && (test_vals[i] > 30000 || test_vals[i] < -30000))
            continue;
        FP_T v = FP_FromDouble(test_vals[i]);
        check("roundtrip", FP_ToDouble(v), test_vals[i], tol);
    }

    //--- arithmetic ---
    printf("[arithmetic]\n");
    FP_T a = FP_FromDouble(10.5);
    FP_T b = FP_FromDouble(3.25);

    check("add", FP_ToDouble(FP_AddSat(a, b)), 13.75, tol);
    check("sub", FP_ToDouble(FP_SubSat(a, b)), 7.25, tol);
    check("mul", FP_ToDouble(FP_Mul(a, b)), 34.125, tol);
    check("div", FP_ToDouble(FP_Div(a, b)), 10.5 / 3.25, tol);

    //--- negatives ---
    printf("[negatives]\n");
    FP_T neg_a = FP_FromDouble(-7.5);
    FP_T neg_b = FP_FromDouble(2.0);
    check("neg + pos", FP_ToDouble(FP_AddSat(neg_a, neg_b)), -5.5, tol);
    check("neg * pos", FP_ToDouble(FP_Mul(neg_a, neg_b)), -15.0, tol);
    check("neg * neg", FP_ToDouble(FP_Mul(neg_a, neg_a)), 56.25, tol);

    //--- utility ---
    printf("[utility]\n");
    check("abs(-7.5)", FP_ToDouble(FP_Abs(neg_a)), 7.5, tol);
    check("negate(10.5)", FP_ToDouble(FP_Negate(a)), -10.5, tol);
    check("min(10.5, 3.25)", FP_ToDouble(FP_Min(a, b)), 3.25, tol);
    check("max(10.5, 3.25)", FP_ToDouble(FP_Max(a, b)), 10.5, tol);

    //--- comparisons ---
    printf("[comparisons]\n");
    check("10.5 < 3.25", FP_LessThan(a, b), 0.0, 0);
    check("3.25 < 10.5", FP_LessThan(b, a), 1.0, 0);
    check("10.5 > 3.25", FP_GreaterThan(a, b), 1.0, 0);
    check("10.5 == 10.5", FP_Equal(a, a), 1.0, 0);

    //--- rounding ---
    printf("[rounding]\n");
    FP_T frac = FP_FromDouble(3.7);
    check("floor(3.7)", FP_ToDouble(FP_Floor(frac)), 3.0, tol);
    check("ceil(3.7)", FP_ToDouble(FP_Ceil(frac)), 4.0, tol);
    check("round(3.7)", FP_ToDouble(FP_Round(frac)), 4.0, tol);
    FP_T frac2 = FP_FromDouble(3.2);
    check("round(3.2)", FP_ToDouble(FP_Round(frac2)), 3.0, tol);

    //--- math functions ---
    printf("[math]\n");
    double math_tol = (FP_TEST_BITS == 16) ? 0.01 : 0.001;
    check("sqrt(9)", FP_ToDouble(FP_Sqrt(FP_FromDouble(9.0))), 3.0, math_tol);
    check("sin(0)", FP_ToDouble(FP_Sin(FP_FromDouble(0.0))), 0.0, math_tol);
    check("cos(0)", FP_ToDouble(FP_Cos(FP_FromDouble(0.0))), 1.0, math_tol);
    check("exp(1)", FP_ToDouble(FP_Exp(FP_FromDouble(1.0))), 2.71828, math_tol);
    check("log(e)", FP_ToDouble(FP_Log(FP_FromDouble(2.71828))), 1.0, math_tol);
    check("pow(2,10)", FP_ToDouble(FP_Pow(FP_FromDouble(2.0), FP_FromDouble(10.0))), 1024.0, math_tol);

    //--- lerp ---
    printf("[lerp]\n");
    FP_T zero = FP_FromDouble(0.0);
    FP_T ten  = FP_FromDouble(10.0);
    FP_T half = FP_FromDouble(0.5);
    check("lerp(0,10,0.5)", FP_ToDouble(FP_Lerp(zero, ten, half)), 5.0, tol);

    //--- financial values (skip for FP16, range too small) ---
    if (FP_TEST_BITS >= 32) {
        printf("[financial]\n");
        FP_T btc = FP_FromDouble(97432.12345678);
        check("btc price", FP_ToDouble(btc), 97432.12345678, (FP_TEST_BITS == 32) ? 0.01 : 0.0000001);

        FP_T volume = FP_FromDouble(0.00000001);
        check("1 satoshi", FP_ToDouble(volume), 0.00000001, (FP_TEST_BITS == 32) ? 0.0000001 : 0.000000000001);

        FP_T total = FP_Mul(btc, FP_FromDouble(1.5));
        check("1.5 BTC value", FP_ToDouble(total), 97432.12345678 * 1.5, (FP_TEST_BITS == 32) ? 0.01 : 0.001);
    }

    //--- FP128 specific: accumulation test ---
#if FP_TEST_BITS == 128
    printf("[accumulation]\n");
    FP_T acc  = FP_FromDouble(0.0);
    FP_T tiny = FP_FromDouble(0.001);
    for (int i = 0; i < 1000000; i++) {
        acc = FP_AddSat(acc, tiny);
    }
    check("1M * 0.001", FP_ToDouble(acc), 1000.0, 0.01);
#endif

    //--- results ---
    printf("\n=== %s: %d passed, %d failed ===\n", FP_LABEL, tests_passed, tests_failed);
    return tests_failed;
}
