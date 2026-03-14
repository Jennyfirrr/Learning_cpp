#include "fixed_point.h"
#include <stdio.h>
#include <iostream>

int main() {
    double x, y;

    std::cout << "Enter two floating-point numbers!\n";

    std::cout << "x: ";
    std::cin >> x;

    std::cout << "y: ";
    std::cin >> y;

    SST_FP a = SST_FP_FromDouble(x);
    SST_FP b = SST_FP_FromDouble(y);

    printf("Testing fixed-point arithmetic:\n");
    printf("%lld, double: %f\n", a.raw_value, SST_FP_ToDouble(a));
    printf("%lld, double: %f\n", b.raw_value, SST_FP_ToDouble(b));

    SST_FP sum        = SST_FP_Add(a, b);
    SST_FP difference = SST_FP_Sub(a, b);
    SST_FP product    = SST_FP_Mul(a, b);
    SST_FP quotient   = SST_FP_Div(a, b);

    printf("Math results:\n");
    printf("a: %f\n", SST_FP_ToDouble(a));
    printf("b: %f\n", SST_FP_ToDouble(b));
    printf("a + b: %f\n", SST_FP_ToDouble(sum));
    printf("a - b: %f\n", SST_FP_ToDouble(difference));
    printf("a * b: %f\n", SST_FP_ToDouble(product));
    printf("a / b: %f\n", SST_FP_ToDouble(quotient));
    printf("==============================\n");

    printf("Comparisons:\n");
    printf("a == b: %d\n", SST_FP_Equal(a, b));
    printf("a != b: %d\n", SST_FP_NotEqual(a, b));
    printf("a < b: %d\n", SST_FP_LessThan(a, b));
    printf("a <= b: %d\n", SST_FP_LessThanOrEqual(a, b));
    printf("a > b: %d\n", SST_FP_GreaterThan(a, b));
    printf("a >= b: %d\n", SST_FP_GreaterThanOrEqual(a, b));
    printf("==============================\n");

    printf("Testing edge cases:\n");
    SST_FP zero    = SST_FP_FromDouble(0.0);
    SST_FP one     = SST_FP_FromDouble(1.0);
    SST_FP neg_one = SST_FP_FromDouble(-1.0);

    printf("zero: %f\n", SST_FP_ToDouble(zero));
    printf("one: %f\n", SST_FP_ToDouble(one));
    printf("neg_one: %f\n", SST_FP_ToDouble(neg_one));

    printf("zero + one: %f\n", SST_FP_ToDouble(SST_FP_Add(zero, one)));
    printf("one - one: %f\n", SST_FP_ToDouble(SST_FP_Sub(one, one)));
    printf("one * one: %f\n", SST_FP_ToDouble(SST_FP_Mul(one, one)));
    printf("one / one: %f\n", SST_FP_ToDouble(SST_FP_Div(one, one)));

    printf("Positive/Negative comparisons:\n");
    printf("one > zero: %d\n", SST_FP_GreaterThan(one, zero));
    printf("neg_one < zero: %d\n", SST_FP_LessThan(neg_one, zero));
    printf("neg_one < one: %d\n", SST_FP_LessThan(neg_one, one));
    printf("Absolute value of neg_one: %f\n", SST_FP_ToDouble(SST_FP_Abs(neg_one)));
    printf("Absolute value of neg_one: %f\n", SST_FP_ToDouble(SST_FP_Abs(one)));
    printf("Absolute value of neg_one: %f\n", SST_FP_ToDouble(SST_FP_Abs(zero)));
    printf("Sign of neg_one: %d\n", SST_FP_Sign(neg_one));
    printf("Sign of zero: %d\n", SST_FP_Sign(zero));
    printf("Sign of one: %d\n", SST_FP_Sign(one));
    printf("==============================\n");

    return 0;
}
