#include "LinearRegression3X.h"
#include <stdio.h>

int main() {
    // Test data: y = 2x + 1
    SST_FP x_values[] = {(SST_FP){0}, (SST_FP){1 << SST_FP_FRAC_BITS}, (SST_FP){2 << SST_FP_FRAC_BITS}, (SST_FP){3 << SST_FP_FRAC_BITS}};
    SST_FP y_values[] = {(SST_FP){1 << SST_FP_FRAC_BITS}, (SST_FP){3 << SST_FP_FRAC_BITS}, (SST_FP){5 << SST_FP_FRAC_BITS},
                         (SST_FP){7 << SST_FP_FRAC_BITS}};
    int count         = sizeof(x_values) / sizeof(x_values[0]);

    LinearRegression3XResult result = LinearRegression3X_Fit(x_values, y_values, count);

    printf("Slope: %f\n", result.model.slope.raw_value / (float)(1 << SST_FP_FRAC_BITS));
    printf("Intercept: %f\n", result.model.intercept.raw_value / (float)(1 << SST_FP_FRAC_BITS));
    printf("R-squared: %f\n", result.r_squared.raw_value / (float)(1 << SST_FP_FRAC_BITS));

    return 0;
}
