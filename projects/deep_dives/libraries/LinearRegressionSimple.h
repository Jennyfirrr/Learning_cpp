//======================================================================================================
// [LINEAR REGRESSION]
//======================================================================================================
// oh yeah its gonna be fixed point boiiiiiii, this is just a simple implementation, with only a single feature, im learning, for the more advanced one it will probably use a struct to just pack all the features in idk yet, im learning and trying to imagine ways to use this, so w/e, if you see me IRL it would make my day if you told me you love me, and remember class, java is bad
//======================================================================================================
#ifndef LINEAR_REGRESSION_H
#define LINEAR_REGRESSION_H

#include "FixedPoint16.h"
//======================================================================================================
// [LINEAR REGRESSION STRUCTURES]
//======================================================================================================
typedef struct {
    SST_FP slope;
    SST_FP intercept;
} LinearRegressionModel;
//======================================================================================================
// [LINEAR REGRESSION FUNCTION PROTOTYPES]
//======================================================================================================
LinearRegressionModel LinearRegression_Fit(SST_FP *x_values, SST_FP *y_values, int count) {
    LinearRegressionModel model;

    SST_FP sum_x = {0}, sum_y = {0};
    for (int i = 0; i < count; i++) {
        sum_x = SST_FP_Add(sum_x, x_values[i]);
        sum_y = SST_FP_Add(sum_y, y_values[i]);
    }
    SST_FP mean_x = SST_FP_Div(sum_x, (SST_FP){count << SST_FP_FRAC_BITS});
    SST_FP mean_y = SST_FP_Div(sum_y, (SST_FP){count << SST_FP_FRAC_BITS});

    SST_FP numerator = {0}, denominator = {0};
    for (int i = 0; i < count; i++) {
        SST_FP x_diff = SST_FP_Sub(x_values[i], mean_x);
        SST_FP y_diff = SST_FP_Sub(y_values[i], mean_y);
        numerator     = SST_FP_Add(numerator, SST_FP_Mul(x_diff, y_diff));
        denominator   = SST_FP_Add(denominator, SST_FP_Mul(x_diff, x_diff));
    }

    if (denominator.raw_value == 0) {
        model.slope = (SST_FP){0};
    } else {
        model.slope = SST_FP_Div(numerator, denominator);
    }

    model.intercept = SST_FP_Sub(mean_y, SST_FP_Mul(model.slope, mean_x));

    return model;
}

SST_FP LinearRegression_Predict(LinearRegressionModel model, SST_FP x) {
    return SST_FP_Add(SST_FP_Mul(model.slope, x), model.intercept);
}

//======================================================================================================
//======================================================================================================
//======================================================================================================
//======================================================================================================
//======================================================================================================
//======================================================================================================
//======================================================================================================
//======================================================================================================
//======================================================================================================
//======================================================================================================
#endif
