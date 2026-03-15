#include "ROR_regressor.h"
#include <stdio.h>

static void print_result(const char *label, LinearRegression3XResult r) {
    printf("%-24s slope: %8.4f  intercept: %8.4f  r^2: %6.4f\n",
           label,
           SST_FP32_ToDouble(r.model.slope),
           SST_FP32_ToDouble(r.model.intercept),
           SST_FP32_ToDouble(r.r_squared));
}

int main() {
    // ==========================================
    // TEST 1: constant slope (linear prices)
    // ==========================================
    // prices go up by 1.0 each tick, so inner slope should be ~1.0 every time
    // outer slope-of-slopes should be ~0 (trend is steady, not accelerating)
    // ==========================================
    printf("=== TEST 1: constant slope (linear prices) ===\n");
    {
        RegressionFeederX feeder = RegressionFeederX_Init();
        RORRegressor ror         = RORRegressor_Init();

        for (int i = 0; i < 16; i++) {
            RegressionFeederX_Push(&feeder, SST_FP32_FromDouble(100.0 + i * 1.0));
            LinearRegression3XResult inner = RegressionFeederX_Compute(&feeder);

            printf("  tick %2d | ", i);
            print_result("inner", inner);

            RORRegressor_Push(&ror, inner);
        }

        LinearRegression3XResult outer = RORRegressor_Compute(&ror);
        printf("  ");
        print_result(">>> OUTER (slope-of-slopes)", outer);
        printf("  expected: slope ~0 (steady trend)\n\n");
    }

    // ==========================================
    // TEST 2: accelerating trend
    // ==========================================
    // prices increase faster over time (quadratic-ish)
    // inner slopes should grow, outer slope should be positive
    // ==========================================
    printf("=== TEST 2: accelerating trend ===\n");
    {
        RegressionFeederX feeder = RegressionFeederX_Init();
        RORRegressor ror         = RORRegressor_Init();

        for (int i = 0; i < 16; i++) {
            double price = 100.0 + i * i * 0.5;
            RegressionFeederX_Push(&feeder, SST_FP32_FromDouble(price));
            LinearRegression3XResult inner = RegressionFeederX_Compute(&feeder);

            printf("  tick %2d (price=%6.1f) | ", i, price);
            print_result("inner", inner);

            RORRegressor_Push(&ror, inner);
        }

        LinearRegression3XResult outer = RORRegressor_Compute(&ror);
        printf("  ");
        print_result(">>> OUTER (slope-of-slopes)", outer);
        printf("  expected: slope > 0 (accelerating)\n\n");
    }

    // ==========================================
    // TEST 3: decelerating / reversing trend
    // ==========================================
    // prices go up then come back down
    // inner slopes should go from positive to negative
    // outer slope should be negative
    // ==========================================
    printf("=== TEST 3: decelerating / reversing ===\n");
    {
        RegressionFeederX feeder = RegressionFeederX_Init();
        RORRegressor ror         = RORRegressor_Init();

        double prices[] = {100, 103, 105, 106, 106.5, 106.5, 106, 105,
                           103, 100, 96,  91,  85,    78,    70,  61};

        for (int i = 0; i < 16; i++) {
            RegressionFeederX_Push(&feeder, SST_FP32_FromDouble(prices[i]));
            LinearRegression3XResult inner = RegressionFeederX_Compute(&feeder);

            printf("  tick %2d (price=%5.1f) | ", i, prices[i]);
            print_result("inner", inner);

            RORRegressor_Push(&ror, inner);
        }

        LinearRegression3XResult outer = RORRegressor_Compute(&ror);
        printf("  ");
        print_result(">>> OUTER (slope-of-slopes)", outer);
        printf("  expected: slope < 0 (decelerating/reversing)\n\n");
    }

    // ==========================================
    // TEST 4: flat prices
    // ==========================================
    // all prices the same, both slopes should be ~0
    // ==========================================
    printf("=== TEST 4: flat prices ===\n");
    {
        RegressionFeederX feeder = RegressionFeederX_Init();
        RORRegressor ror         = RORRegressor_Init();

        for (int i = 0; i < 16; i++) {
            RegressionFeederX_Push(&feeder, SST_FP32_FromDouble(100.0));
            LinearRegression3XResult inner = RegressionFeederX_Compute(&feeder);

            printf("  tick %2d | ", i);
            print_result("inner", inner);

            RORRegressor_Push(&ror, inner);
        }

        LinearRegression3XResult outer = RORRegressor_Compute(&ror);
        printf("  ");
        print_result(">>> OUTER (slope-of-slopes)", outer);
        printf("  expected: slope ~0, r^2 ~0 (no trend at all)\n\n");
    }

    // ==========================================
    // TEST 5: high prices (1000+)
    // ==========================================
    // this is the case that broke Q16.16
    // ==========================================
    printf("=== TEST 5: high prices (1000+) ===\n");
    {
        RegressionFeederX feeder = RegressionFeederX_Init();
        RORRegressor ror         = RORRegressor_Init();

        for (int i = 0; i < 16; i++) {
            double price = 1500.0 + i * i * 2.0;
            RegressionFeederX_Push(&feeder, SST_FP32_FromDouble(price));
            LinearRegression3XResult inner = RegressionFeederX_Compute(&feeder);

            printf("  tick %2d (price=%7.1f) | ", i, price);
            print_result("inner", inner);

            RORRegressor_Push(&ror, inner);
        }

        LinearRegression3XResult outer = RORRegressor_Compute(&ror);
        printf("  ");
        print_result(">>> OUTER (slope-of-slopes)", outer);
        printf("  expected: slope > 0 (accelerating), all r^2 values 0-1\n\n");
    }

    return 0;
}
