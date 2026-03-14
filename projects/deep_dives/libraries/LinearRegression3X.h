//======================================================================================================
// [LINEAR REGRESSION 3X]
//======================================================================================================
#ifndef LINEAR_REGRESSION_3X_H
#define LINEAR_REGRESSION_3X_H

#include "FixedPoint16.h"

#define MAX_WINDOW 8
//======================================================================================================
// [LINEAR REGRESSION STRUCTURES]
//======================================================================================================
typedef struct {
    SST_FP slope;
    SST_FP intercept;
} LinearRegression3XModel;
static_assert(sizeof(LinearRegression3XModel) == 8, "struct must be 8 bytes");

typedef struct {
    SST_FP price_samples[MAX_WINDOW];
    int head;
    int count;
} RegressionFeederX;
static_assert(sizeof(RegressionFeederX) == 64, "struct must be 64 bytes");

typedef struct {
    LinearRegression3XModel model;
    SST_FP r_squared;
} LinearRegression3XResult;
static_assert(sizeof(LinearRegression3XResult) == 12, "struct must be 12 bytes");
//======================================================================================================
// [LINEAR REGRESSION FUNCTION PROTOTYPES]
//======================================================================================================
static inline RegressionFeederX RegressionFeederX_Init() {
    RegressionFeederX feeder;

    for (int i = 0; i < MAX_WINDOW; i++) {
        feeder.price_samples[i] = (SST_FP){0};
    }

    feeder.head  = 0;
    feeder.count = 0;

    return feeder;
}

static inline LinearRegression3XResult RegressionFeederX_Compute(RegressionFeederX *feeder) {
    if (feeder->count < 2) {
        LinearRegression3XResult empty;
        empty.model.slope     = (SST_FP){0};
        empty.model.intercept = (SST_FP){0};
        empty.r_squared       = (SST_FP){0};
        return empty; // Not enough data points
    }

    SST_FP linearized[MAX_WINDOW];
    SST_FP time_index[MAX_WINDOW];

    for (int i = 0; i < feeder->count; i++) {
        int idx       = (feeder->head - feeder->count + i + MAX_WINDOW) % MAX_WINDOW;
        linearized[i] = feeder->price_samples[idx];
        time_index[i] = (SST_FP){i << SST_FP_FRAC_BITS};
    }

    return LinearRegression3X_Fit(time_index, linearized, feeder->count);
}

static inline void RegressionFeederX_Push(RegressionFeederX *feeder, SST_FP price) {
    feeder->price_samples[feeder->head] = price;
    feeder->head                        = (feeder->head + 1) % MAX_WINDOW;

    if (feeder->count < MAX_WINDOW) {
        feeder->count++;
    }
}

static inline LinearRegression3XResult LinearRegression3X_Fit(SST_FP *x_values, SST_FP *y_values, int count) {
    LinearRegression3XResult result;
    result.model.slope     = (SST_FP){0};
    result.model.intercept = (SST_FP){0};
    result.r_squared       = (SST_FP){0};

    if (count < 2) {
        return result; // Not enough data points
    }

    SST_FP sum_x = {0}, sum_y = {0}, sum_xy = {0}, sum_x2 = {0}, sum_y2 = {0};
    for (int i = 0; i < count; i++) {
        sum_x  = SST_FP_Add(sum_x, x_values[i]);
        sum_y  = SST_FP_Add(sum_y, y_values[i]);
        sum_xy = SST_FP_Add(sum_xy, SST_FP_Mul(x_values[i], y_values[i]));
        sum_x2 = SST_FP_Add(sum_x2, SST_FP_Mul(x_values[i], x_values[i]));
        sum_y2 = SST_FP_Add(sum_y2, SST_FP_Mul(y_values[i], y_values[i]));
    }

    SST_FP n_fp        = (SST_FP){count << SST_FP_FRAC_BITS};
    SST_FP numerator   = SST_FP_Sub(SST_FP_Mul(n_fp, sum_xy), SST_FP_Mul(sum_x, sum_y));
    SST_FP denominator = SST_FP_Sub(SST_FP_Mul(n_fp, sum_x2), SST_FP_Mul(sum_x, sum_x));

    if (denominator.raw_value != 0) {
        result.model.slope     = SST_FP_Div(numerator, denominator);
        result.model.intercept = SST_FP_Div(SST_FP_Sub(SST_FP_Mul(sum_y, sum_x2), SST_FP_Mul(sum_x, sum_xy)), denominator);

        // Calculate R-squared
        SST_FP ss_total = SST_FP_Sub(sum_y2, SST_FP_Div(SST_FP_Mul(sum_y, sum_y), n_fp));
        if (ss_total.raw_value != 0) {
            result.r_squared = SST_FP_Div(SST_FP_Sub(ss_total, numerator), ss_total);
        }
    }

    return result;
}

static inline SST_FP LinearRegression3X_Predict(LinearRegression3XModel model, SST_FP x) {
    return SST_FP_Add(SST_FP_Mul(model.slope, x), model.intercept);
}
//======================================================================================================
//======================================================================================================
//======================================================================================================
#endif // LINEAR_REGRESSION_3X_H
