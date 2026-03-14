#include "fixed_point.h"
#include <stdio.h>

int main() {
    SST_FP a = SST_FP_FromDouble(1.5);
    SST_FP b = SST_FP_FromDouble(2.5);

    printf("Testing fixed-point arithmetic:\n");
    printf("%lld, double: %f\n", a.raw_value, SST_FP_ToDouble(a));
    printf("%lld, double: %f\n", b.raw_value, SST_FP_ToDouble(b));

    SST_FP sum        = SST_FP_Add(a, b);
    SST_FP difference = SST_FP_Sub(a, b);
    SST_FP product    = SST_FP_Mul(a, b);
    SST_FP quotient   = SST_FP_Div(a, b);

    printf("a: %f\n", SST_FP_ToDouble(a));
    printf("b: %f\n", SST_FP_ToDouble(b));
    printf("a + b: %f\n", SST_FP_ToDouble(sum));
    printf("a - b: %f\n", SST_FP_ToDouble(difference));
    printf("a * b: %f\n", SST_FP_ToDouble(product));
    printf("a / b: %f\n", SST_FP_ToDouble(quotient));

    return 0;
}
