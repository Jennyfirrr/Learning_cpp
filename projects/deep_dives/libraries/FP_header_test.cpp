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
#include <string.h>
#include "FixedPointN.h"

//======================================================================================================
// [ALIAS WRAPPERS]
//======================================================================================================
// thin wrappers so the test code below doesnt need template syntax everywhere
//======================================================================================================
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

    //--- string conversion ---
    printf("[string conversion]\n");
    char buf[4096];

    // roundtrip simple values through string
    FP_T str_a = SST_FPN_FromString<FP_TEST_BITS>("42.125");
    check("fromstring 42.125", FP_ToDouble(str_a), 42.125, tol);

    FP_T str_b = SST_FPN_FromString<FP_TEST_BITS>("-7.5");
    check("fromstring -7.5", FP_ToDouble(str_b), -7.5, tol);

    FP_T str_z = SST_FPN_FromString<FP_TEST_BITS>("0.0");
    check("fromstring 0.0", FP_ToDouble(str_z), 0.0, tol);

    // tostring -> fromstring roundtrip
    FP_T orig = FP_FromDouble(3.14159);
    SST_FPN_ToString(orig, buf, sizeof(buf), 10);
    FP_T back = SST_FPN_FromString<FP_TEST_BITS>(buf);
    check("tostring->fromstring", FP_ToDouble(back), 3.14159, 0.0001);

    // full precision string roundtrip (beyond double)
    // string -> FP -> string -> FP -> string: compare only meaningful digits
    // meaningful decimal digits ≈ FRAC_BITS * log10(2) ≈ FRAC_BITS * 301 / 1000
    const char *precise = "123.456789012345678901234567890123456789";
    unsigned meaningful = (unsigned)((uint64_t)FP_TEST_BITS * 301 / 1000);
    FP_T prec_val       = SST_FPN_FromString<FP_TEST_BITS>(precise);
    SST_FPN_ToString(prec_val, buf, sizeof(buf), meaningful);
    char buf2[4096];
    FP_T prec_back = SST_FPN_FromString<FP_TEST_BITS>(buf);
    SST_FPN_ToString(prec_back, buf2, sizeof(buf2), meaningful);
    int strings_match = (strncmp(buf, buf2, meaningful) == 0);
    check("full precision roundtrip", strings_match, 1.0, 0);

    // arithmetic on string-parsed values
    FP_T sa = SST_FPN_FromString<FP_TEST_BITS>("10.5");
    FP_T sb = SST_FPN_FromString<FP_TEST_BITS>("3.25");
    check("string add", FP_ToDouble(FP_AddSat(sa, sb)), 13.75, tol);
    check("string mul", FP_ToDouble(FP_Mul(sa, sb)), 34.125, tol);

    // big integer via string
    FP_T big = SST_FPN_FromString<FP_TEST_BITS>("1000000");
    check("fromstring 1000000", FP_ToDouble(big), 1000000.0, tol);

    //--- full precision verification (no double involved) ---
    printf("[full precision - no double]\n");
    char fbuf[4096], fbuf2[4096];
    unsigned max_digits = (unsigned)((uint64_t)FP_TEST_BITS * 301 / 1000);

    // 1. exact powers of 2: 0.5 + 0.5 == 1.0 (must be bit-perfect, no rounding)
    FP_T half_a  = SST_FPN_FromString<FP_TEST_BITS>("0.5");
    FP_T half_b  = SST_FPN_FromString<FP_TEST_BITS>("0.5");
    FP_T one_sum = FP_AddSat(half_a, half_b);
    SST_FPN_ToString(one_sum, fbuf, sizeof(fbuf), 10);
    check("0.5+0.5 exact", strcmp(fbuf, "1.0000000000") == 0, 1.0, 0);

    // 2. 0.25 * 4 == 1.0 (exact in binary)
    FP_T quarter = SST_FPN_FromString<FP_TEST_BITS>("0.25");
    FP_T four    = SST_FPN_FromString<FP_TEST_BITS>("4.0");
    FP_T qx4     = FP_Mul(quarter, four);
    SST_FPN_ToString(qx4, fbuf, sizeof(fbuf), 10);
    check("0.25*4 exact", strcmp(fbuf, "1.0000000000") == 0, 1.0, 0);

    // 3. 1/3 should produce repeating 3s - verify first N digits are all 3
    FP_T one_fp           = SST_FPN_FromString<FP_TEST_BITS>("1.0");
    FP_T three_fp         = SST_FPN_FromString<FP_TEST_BITS>("3.0");
    FP_T third            = SST_FPN_DivNoAssert(one_fp, three_fp);
    unsigned check_digits = (max_digits > 5) ? max_digits - 5 : max_digits; // last few digits may round
    SST_FPN_ToString(third, fbuf, sizeof(fbuf), check_digits);
    int all_threes = 1;
    for (unsigned d = 2; d < check_digits && fbuf[d]; d++) { // skip "0."
        if (fbuf[d] != '3') {
            all_threes = 0;
            break;
        }
    }
    check("1/3 repeating 3s", all_threes, 1.0, 0);

    // 4. 1/3 * 3 should be very close to 1 (off by at most 1 ULP)
    FP_T third_x3 = FP_Mul(third, three_fp);
    SST_FPN_ToString(third_x3, fbuf, sizeof(fbuf), check_digits);
    // should be 0.9999...9 or 1.0000...0 - check first chars
    int close_to_one = (fbuf[0] == '0' && fbuf[2] == '9') || (fbuf[0] == '1');
    check("1/3*3 close to 1", close_to_one, 1.0, 0);

    // 5. subtraction precision: parse two values that differ only in the last digits
    //    (1 + 10^-15) - 1 should give 10^-15, not zero
    //    10^-15 is within precision of all widths >= 64 (~19 decimal digits)
    FP_T big_one     = SST_FPN_FromString<FP_TEST_BITS>("1.000000000000001");
    FP_T exact_one   = SST_FPN_FromString<FP_TEST_BITS>("1.0");
    FP_T tiny_diff   = FP_SubSat(big_one, exact_one);
    int diff_nonzero = !SST_FPN_IsZero(tiny_diff);
    check("sub precision 10^-15", diff_nonzero, 1.0, 0);
    // verify the result has the right magnitude: 15 leading zeros after "0."
    // (10^-15 rounds to 0.000000000000000999... in binary, so 15 not 14)
    SST_FPN_ToString(tiny_diff, fbuf, sizeof(fbuf), 20);
    int leading_zeros = 0;
    for (unsigned d = 2; fbuf[d]; d++) {
        if (fbuf[d] == '0')
            leading_zeros++;
        else
            break;
    }
    check("sub precision magnitude", leading_zeros == 15, 1.0, 0);

    // 6. carry chain stress: add 1 ULP repeatedly and verify it accumulates
    //    parse "0.0" then add "0." + string of zeros + "1" many times
    //    this forces carries across word boundaries
    FP_T carry_acc = SST_FPN_FromString<FP_TEST_BITS>("0.0");
    // 1 ULP at the boundary: 2^-64 = 1/18446744073709551616
    // instead of computing that, just use a small exact power of 2
    FP_T small_pow2 = SST_FPN_FromString<FP_TEST_BITS>("0.0000000000000000001");
    for (int i = 0; i < 1000; i++) {
        carry_acc = FP_AddSat(carry_acc, small_pow2);
    }
    FP_T expected_carry = FP_Mul(small_pow2, SST_FPN_FromString<FP_TEST_BITS>("1000.0"));
    SST_FPN_ToString(carry_acc, fbuf, sizeof(fbuf), 25);
    SST_FPN_ToString(expected_carry, fbuf2, sizeof(fbuf2), 25);
    check("carry chain accum", strncmp(fbuf, fbuf2, 20) == 0, 1.0, 0);

    // 7. word boundary carry: 0xFFFF...FFFF + 1 in fractional part
    //    use the fact that (1 - 2^-64) + 2^-64 == 1.0
    //    2^-64 = 0.0000000000000000000542101086242752217003726400434...
    //    easier: just verify (1 - tiny) + tiny == 1 through string
    FP_T almost_one  = SST_FPN_FromString<FP_TEST_BITS>("0.999999999999999999999999999999999999999");
    FP_T complement  = FP_SubSat(one_fp, almost_one);
    FP_T reassembled = FP_AddSat(almost_one, complement);
    SST_FPN_ToString(reassembled, fbuf, sizeof(fbuf), 10);
    check("(1-x)+x == 1", strcmp(fbuf, "1.0000000000") == 0, 1.0, 0);

    // 8. multiplication commutativity at full precision: a*b == b*a
    FP_T comm_a = SST_FPN_FromString<FP_TEST_BITS>("3.141592653589793238462643383279");
    FP_T comm_b = SST_FPN_FromString<FP_TEST_BITS>("2.718281828459045235360287471352");
    FP_T ab     = FP_Mul(comm_a, comm_b);
    FP_T ba     = FP_Mul(comm_b, comm_a);
    check("a*b == b*a", SST_FPN_Equal(ab, ba), 1.0, 0);

    // 9. division identity: (a/b)*b should be very close to a
    //    7/11*11 can be 6.999...9 (off by 1 ULP) - verify via subtraction magnitude
    FP_T div_a       = SST_FPN_FromString<FP_TEST_BITS>("7.0");
    FP_T div_b       = SST_FPN_FromString<FP_TEST_BITS>("11.0");
    FP_T quotient    = SST_FPN_DivNoAssert(div_a, div_b);
    FP_T product     = FP_Mul(quotient, div_b);
    FP_T div_err     = FP_SubSat(div_a, product);
    FP_T div_err_abs = FP_Abs(div_err);
    // error should be tiny: less than 2^-(FRAC_BITS-10) or so
    // verify by checking that the error is less than 0.000001 (extremely conservative)
    FP_T div_threshold = SST_FPN_FromString<FP_TEST_BITS>("0.000001");
    check("(a/b)*b == a", SST_FPN_LessThan(div_err_abs, div_threshold), 1.0, 0);

    // 10. COMPUTE pi via Machin's formula using only fixed-point arithmetic
    //     pi = 16*arctan(1/5) - 4*arctan(1/239)
    //     arctan(x) = x - x^3/3 + x^5/5 - x^7/7 + ...
    //     this tests multiply, divide, add, subtract at full precision - no double anywhere
    const char *known_pi = "3.14159265358979323846264338327950288419716939937510"
                           "58209749445923078164062862089986280348253421170679"
                           "82148086513282306647093844609550582231725359408128"
                           "48111745028410270193852110555964462294895493038196"
                           "44288109756659334461284756482337867831652712019091"
                           "45648566923460348610454326648213393607260249141273"
                           "72458700660631558817488152092096282925409171536436"
                           "78925903600113305305488204665213841469519415116094"
                           "33057270365759591953092186117381932611793105118548"
                           "07446237996274956735188575272489122793818301194912"
                           "98336733624406566430860213949463952247371907021798"
                           "60943702770539217176293176752384674818467669405132"
                           "00056812714526356082778577134275778960917363717872"
                           "14684409012249534301465495853710507922796892589235"
                           "42019956112129021960864034418159813629774771309960"
                           "51870721134999999837297804995105973173281609631859"
                           "50244594553469083026425223082533446850352619311881"
                           "71010003137838752886587533208381420617177669147303"
                           "59825349042875546873115956286388235378759375195778"
                           "18577805321712268066130019278766111959092164201989"
                           "38095257201065485863278865936153381827968230301952"
                           "03530185296899577362259941389124972177528347913151"
                           "55748572424541506959508295331168617278558890750983"
                           "81754637464939319255060400927701671139009848824012"
                           "85836160356370766010471018194295559619894676783744";

    // arctan via Taylor series, entirely in fixed-point
    // arctan(1/n) = 1/n - 1/(3*n^3) + 1/(5*n^5) - ...
    // we compute arctan(1/5) and arctan(1/239)
    FP_T five          = SST_FPN_FromString<FP_TEST_BITS>("5.0");
    FP_T twothirtynine = SST_FPN_FromString<FP_TEST_BITS>("239.0");

    // arctan(1/x) = sum of (-1)^k / ((2k+1) * x^(2k+1))
    // compute by: term = 1/x, x_sq = x*x, then each iteration: term /= -x_sq, add term/(2k+1)
    auto compute_arctan_recip = [](FP_T x) -> FP_T {
        FP_T x_sq     = SST_FPN_Mul(x, x);
        FP_T neg_x_sq = SST_FPN_Negate(x_sq);
        FP_T power    = SST_FPN_DivNoAssert(SST_FPN_FromString<FP_TEST_BITS>("1.0"), x); // 1/x
        FP_T result   = SST_FPN_Zero<FP_TEST_BITS>();

        // need enough terms: each term contributes ~log10(x^2) digits
        // for x=5: ~1.4 digits/term, need ~max_digits/1.4 terms
        // for x=239: ~4.75 digits/term, need ~max_digits/4.75 terms
        // use max_digits terms to be safe (overkill for x=239 but correct)
        unsigned n_terms = (unsigned)((uint64_t)FP_TEST_BITS * 301 / 1000) + 10;
        // cap at a reasonable number so the test doesn't take forever
        if (n_terms > 1500)
            n_terms = 1500;

        for (unsigned k = 0; k < n_terms; k++) {
            // divisor = 2k+1
            char div_str[32];
            sprintf(div_str, "%u.0", 2 * k + 1);
            FP_T divisor = SST_FPN_FromString<FP_TEST_BITS>(div_str);
            FP_T term    = SST_FPN_DivNoAssert(power, divisor);
            result       = SST_FPN_AddSat(result, term);

            // check if term is negligible (all fractional words zero after division)
            if (SST_FPN_IsZero(term))
                break;

            // next power: multiply by -1/x^2
            power = SST_FPN_DivNoAssert(power, neg_x_sq);
        }
        return result;
    };

    FP_T atan_5   = compute_arctan_recip(five);
    FP_T atan_239 = compute_arctan_recip(twothirtynine);

    // pi = 16*arctan(1/5) - 4*arctan(1/239)
    FP_T sixteen     = SST_FPN_FromString<FP_TEST_BITS>("16.0");
    FP_T four_fp     = SST_FPN_FromString<FP_TEST_BITS>("4.0");
    FP_T computed_pi = SST_FPN_SubSat(SST_FPN_Mul(sixteen, atan_5), SST_FPN_Mul(four_fp, atan_239));

    unsigned pi_digits = max_digits < 1250 ? max_digits : 1250;
    SST_FPN_ToString(computed_pi, fbuf, sizeof(fbuf), pi_digits);

    // count matching digits against known pi (+-1 tolerance on last digit for rounding)
    int computed_match = 0;
    for (int d = 0; known_pi[d] && fbuf[d]; d++) {
        int diff = known_pi[d] - fbuf[d];
        if (diff < 0)
            diff = -diff;
        if (diff <= 1)
            computed_match++;
        else
            break;
    }

    // should match close to the theoretical max: FRAC_BITS * log10(2)
    int expected_min = (int)((uint64_t)FP_TEST_BITS * 301 / 1000) - 5;
    if (expected_min > 1250)
        expected_min = 1245; // capped by reference string
    check("computed pi digits", computed_match >= expected_min, 1.0, 0);
    printf("  (computed %d digits of pi, expected >= %d)\n", computed_match, expected_min);

    //--- results ---
    printf("\n=== FP%d: %d passed, %d failed ===\n", FP_TEST_BITS, tests_passed, tests_failed);
    return tests_failed;
}
