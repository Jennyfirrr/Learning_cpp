//======================================================================================================
// [FIXED-POINT HEADER TEST]
//======================================================================================================
// set which precision to test by defining FP_TEST_BITS before compiling:
//   g++ -std=c++17 -DFP_TEST_BITS=64  -o fp_test FP_header_test.cpp -lm
//   g++ -std=c++17 -DFP_TEST_BITS=128 -o fp_test FP_header_test.cpp -lm
//   g++ -std=c++17 -DFP_TEST_BITS=256 -o fp_test FP_header_test.cpp -lm
//
// must be a power of 2 >= 64, defaults to 64 if not specified
//======================================================================================================
#ifndef FP_TEST_BITS
#define FP_TEST_BITS 64
#endif

#include <stdio.h>
#include "FixedPointN.h"

//======================================================================================================
// [ALIAS WRAPPERS]
//======================================================================================================
// thin wrappers so the test code below doesnt need template syntax everywhere
//======================================================================================================
using FP_T = SST_FPN<FP_TEST_BITS>;
static inline FP_T FP_FromDouble(double d)             { return SST_FPN_FromDouble<FP_TEST_BITS>(d); }
static inline double FP_ToDouble(FP_T v)               { return SST_FPN_ToDouble(v); }
static inline FP_T FP_AddSat(FP_T a, FP_T b)          { return SST_FPN_AddSat(a, b); }
static inline FP_T FP_SubSat(FP_T a, FP_T b)          { return SST_FPN_SubSat(a, b); }
static inline FP_T FP_Mul(FP_T a, FP_T b)             { return SST_FPN_Mul(a, b); }
static inline FP_T FP_Div(FP_T a, FP_T b)             { return SST_FPN_DivNoAssert(a, b); }
static inline FP_T FP_Min(FP_T a, FP_T b)             { return SST_FPN_Min(a, b); }
static inline FP_T FP_Max(FP_T a, FP_T b)             { return SST_FPN_Max(a, b); }
static inline FP_T FP_Abs(FP_T a)                     { return SST_FPN_Abs(a); }
static inline FP_T FP_Negate(FP_T a)                   { return SST_FPN_Negate(a); }
static inline FP_T FP_Floor(FP_T a)                    { return SST_FPN_Floor(a); }
static inline FP_T FP_Ceil(FP_T a)                     { return SST_FPN_Ceil(a); }
static inline FP_T FP_Round(FP_T a)                    { return SST_FPN_Round(a); }
static inline FP_T FP_Sqrt(FP_T a)                     { return SST_FPN_Sqrt(a); }
static inline FP_T FP_Sin(FP_T a)                      { return SST_FPN_Sin(a); }
static inline FP_T FP_Cos(FP_T a)                      { return SST_FPN_Cos(a); }
static inline FP_T FP_Log(FP_T a)                      { return SST_FPN_Log(a); }
static inline FP_T FP_Exp(FP_T a)                      { return SST_FPN_Exp(a); }
static inline FP_T FP_Pow(FP_T a, FP_T b)             { return SST_FPN_Pow(a, b); }
static inline int FP_LessThan(FP_T a, FP_T b)          { return SST_FPN_LessThan(a, b); }
static inline int FP_GreaterThan(FP_T a, FP_T b)       { return SST_FPN_GreaterThan(a, b); }
static inline int FP_Equal(FP_T a, FP_T b)             { return SST_FPN_Equal(a, b); }
static inline FP_T FP_Lerp(FP_T a, FP_T b, FP_T t)    { return SST_FPN_Lerp(a, b, t); }

using namespace std;

//======================================================================================================
// [TEST HELPERS]
//======================================================================================================
static int tests_passed = 0;
static int tests_failed = 0;

static void check(const char *name, double got, double expected, double tolerance) {
    double diff = got - expected;
    if (diff < 0) diff = -diff;
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
    printf("=== Testing FP%d (Q%d.%d) ===\n\n", FP_TEST_BITS, FP_TEST_BITS, FP_TEST_BITS);

    double tol = 0.0000001;

    //--- conversion roundtrip ---
    printf("[conversion]\n");
    double test_vals[] = {0.0, 1.0, -1.0, 3.14159, -42.5, 100.125, 0.001};
    for (int i = 0; i < 7; i++) {
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
    double math_tol = 0.001;
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

    //--- financial values ---
    printf("[financial]\n");
    FP_T btc = FP_FromDouble(97432.12345678);
    check("btc price", FP_ToDouble(btc), 97432.12345678, 0.0000001);

    FP_T volume = FP_FromDouble(0.00000001);
    check("1 satoshi", FP_ToDouble(volume), 0.00000001, 0.000000000001);

    FP_T total = FP_Mul(btc, FP_FromDouble(1.5));
    check("1.5 BTC value", FP_ToDouble(total), 97432.12345678 * 1.5, 0.001);

    //--- accumulation test ---
    printf("[accumulation]\n");
    FP_T acc  = FP_FromDouble(0.0);
    FP_T tiny = FP_FromDouble(0.001);
    for (int i = 0; i < 1000000; i++) {
        acc = FP_AddSat(acc, tiny);
    }
    check("1M * 0.001", FP_ToDouble(acc), 1000.0, 0.01);

    //--- results ---
    printf("\n=== FP%d: %d passed, %d failed ===\n", FP_TEST_BITS, tests_passed, tests_failed);
    return tests_failed;
}
