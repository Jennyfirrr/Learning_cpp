//======================================================================================================
// [LINEAR REGRESSION]
//======================================================================================================
// oh yeah its gonna be fixed point boiiiiiii, this is just a simple implementation, with only a single feature, im learning, for the more advanced one it will probably use a struct to just pack all the features in idk yet, im learning and trying to imagine ways to use this, so w/e, if you see me IRL it would make my day if you told me you love me, and remember class, java is bad
//======================================================================================================
#ifndef LINEAR_REGRESSION_H
#define LINEAR_REGRESSION_H

#include "FixedPointN.h"
//======================================================================================================
// [LINEAR REGRESSION STRUCTURES]
//======================================================================================================
template<unsigned F>
struct LinearRegressionModel {
    SST_FPN<F> slope;
    SST_FPN<F> intercept;
};
//======================================================================================================
// [LINEAR REGRESSION FUNCTION PROTOTYPES]
//======================================================================================================
template<unsigned F>
inline LinearRegressionModel<F> LinearRegression_Fit(SST_FPN<F> *x_values, SST_FPN<F> *y_values, int count) {
    using FP = SST_FPN<F>;
    LinearRegressionModel<F> model;

    FP sum_x = SST_FPN_Zero<F>(), sum_y = SST_FPN_Zero<F>();
    for (int i = 0; i < count; i++) {
        sum_x = SST_FPN_Add(sum_x, x_values[i]);
        sum_y = SST_FPN_Add(sum_y, y_values[i]);
    }
    FP n_fp  = SST_FPN_FromDouble<F>((double)count);
    FP mean_x = SST_FPN_DivNoAssert(sum_x, n_fp);
    FP mean_y = SST_FPN_DivNoAssert(sum_y, n_fp);

    FP numerator = SST_FPN_Zero<F>(), denominator = SST_FPN_Zero<F>();
    for (int i = 0; i < count; i++) {
        FP x_diff   = SST_FPN_Sub(x_values[i], mean_x);
        FP y_diff   = SST_FPN_Sub(y_values[i], mean_y);
        numerator   = SST_FPN_Add(numerator, SST_FPN_Mul(x_diff, y_diff));
        denominator = SST_FPN_Add(denominator, SST_FPN_Mul(x_diff, x_diff));
    }

    int denom_nonzero = !SST_FPN_IsZero(denominator);
    FP safe_denom     = denom_nonzero ? denominator : SST_FPN_FromDouble<F>(1.0);
    FP raw_slope      = SST_FPN_DivNoAssert(numerator, safe_denom);
    model.slope       = denom_nonzero ? raw_slope : SST_FPN_Zero<F>();

    model.intercept = SST_FPN_Sub(mean_y, SST_FPN_Mul(model.slope, mean_x));

    return model;
}

template<unsigned F>
inline SST_FPN<F> LinearRegression_Predict(LinearRegressionModel<F> model, SST_FPN<F> x) {
    return SST_FPN_Add(SST_FPN_Mul(model.slope, x), model.intercept);
}

//======================================================================================================
//======================================================================================================
#endif
