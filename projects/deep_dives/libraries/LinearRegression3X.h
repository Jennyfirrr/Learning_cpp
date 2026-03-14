//======================================================================================================
// [LINEAR REGRESSION 3X]
//======================================================================================================
// this is just a basic time and price regression model, im gonna keep iterating on expadning these as i learn more about them
//======================================================================================================
#ifndef LINEAR_REGRESSION_3X_H
#define LINEAR_REGRESSION_3X_H

#include "FixedPoint16.h"

#define MAX_WINDOW 8
//======================================================================================================
// [LINEAR REGRESSION STRUCTURES]
//======================================================================================================
// [LinearRegression3XModel]
//======================================================================================================
// this is basically the equation y = mx + b, slope is m, intercept is b, and these numbers give you the slope, we track them in a struct for better organization, and the slope means on average that the price changes by this much per interval
//======================================================================================================
typedef struct {
    SST_FP slope;
    SST_FP intercept;
} LinearRegression3XModel;
static_assert(sizeof(LinearRegression3XModel) == 8, "struct must be 8 bytes");
//======================================================================================================
// [FEEDER STRUCT]
//======================================================================================================
// this struct just has where there data lives before the regression takes place, apparently this is called a ring buffer, it holds the last 8 price sampels which is from the [MAX_WINDOW], head is where the next write goes, and count tracks which sample your on within the sample window, you can extend this for more intervals, but then the assert size changes, ill probably look into making that confirgurable outside of the file and based on dynamic observations but for now this works as i learn it, this also means that once the buffer gets full, it will start overwriting the oldest data point, which creates like a moving average for the slope, so basically it gives you the most recent output for the 8 prices
//======================================================================================================
typedef struct {
    SST_FP price_samples[MAX_WINDOW];
    int head;
    int count;
} RegressionFeederX;
static_assert(sizeof(RegressionFeederX) == 40, "struct must be 40 bytes");
//======================================================================================================
// [RESULT STRUCT]
//======================================================================================================
// this is the fitted line, (slope + intercept), plus the r^2 value, which is just a measure of how well the line fits the data, where 1.0 is perfect, and 0, is no relationship at all, ill probably go more into stuff like overfitting and stuff later, but generally you cant trust extreme values, like most things with stocks will probably fall within like .3 -> .7, and even then, .7 is still unusually high
//======================================================================================================
typedef struct {
    LinearRegression3XModel model;
    SST_FP r_squared;
} LinearRegression3XResult;
static_assert(sizeof(LinearRegression3XResult) == 12, "struct must be 12 bytes");
//======================================================================================================
// [LINEAR REGRESSION FUNCTION PROTOTYPES]
//======================================================================================================
// [FEEDER FUNCTION]
//======================================================================================================
// just creates a clean feeder with everything zerod out, you call this once at startup and basically only then
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
//======================================================================================================
// [PUSH FUNCTION]
//======================================================================================================
// this writes the new price at the current head position, and it advances with a wrap,so that after it hits the max window length, it starts wrapping around, the count just increments until youve filled the window, and at 8 it just starts overwriting the older value, in short the data ingestion path
//======================================================================================================
static inline void RegressionFeederX_Push(RegressionFeederX *feeder, SST_FP price) {
    feeder->price_samples[feeder->head] = price;
    feeder->head                        = (feeder->head + 1) % MAX_WINDOW;

    if (feeder->count < MAX_WINDOW) {
        feeder->count++;
    }
}
//======================================================================================================
// [COMPUTE FUNCTION]
//======================================================================================================
// this just finds the best fit line throughout the data points, its basically what would be the SMA or a moving average, but for a linear regression, it implements ordinary least squares, which are the the operators in the for loop, it makes one pass through the data, accumulats 5 sums, and the sum_x and sum_y are just totals, and sum_xy is the sum of each x times its corresponding y, it basically caoutes the relationship between x and y, which in this case are like price and time
//
// then we get to the part with n_fp, and this part is just the count converted to a fixed point, numerator is just the covariance between x and y(how much they move together), and the denominator is the variance of x(how spread out the values are), and then these 2 values are shared between the slope and intercept formulas
//
// then the slope part is literally just how much does y change per unit of x, and the magnintude tells you how fast
//
// the result.model_intercept is just the value where y crosses zero, combined with the slope, you can now predict y for any x
//
// ss_total is the total variance in y, which is just how much the prices vary overall, ss_reg is how much of that the variance the regression line explains, and r^2 is just ss_reg / ss_total, which basically means "what fraction of the price movement is explained by the trend", oh thats cool, its literally a ratio of the relationship converted to a percentage, this stuff is neat to finally dig into, its such a shame that college classes feel like funeral processions, this sshit is actually cool when its applied to stuff outside of school, but college is just like "dot your t's and cross your i's and make sure you answer these problems which have no actual meaning", but thats a different tangent, and you arnt reading this for my opinions on how lacking alot of CS education actually is
//======================================================================================================
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
        SST_FP ss_total = SST_FP_Sub(SST_FP_Mul(n_fp, sum_y2), SST_FP_Mul(sum_y, sum_y));
        if (ss_total.raw_value != 0) {
            result.r_squared = SST_FP_Div(SST_FP_Mul(numerator, numerator), SST_FP_Mul(denominator, ss_total));
        }
    }

    return result;
}
//======================================================================================================
// [PREDICTION FUNCTION]
//======================================================================================================
// this is just run the regression on the ring buffer, it doesnt store it in chronological order, so you have to walk from the oldest to the newest and copy them into the linearized in the correct time order, then it just hands the arrays to the _Fit and computes, theres probably a better and faster way to do this but idk, im figuring this out as a go, because FUCK java(I C K Y)
//======================================================================================================
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
//======================================================================================================
// [PREDICTION FUNCTION]
//======================================================================================================
// this is just given the line what is the y at the assocaited x, literally just y = mx + b, an oldie but a goodie
//======================================================================================================
static inline SST_FP LinearRegression3X_Predict(LinearRegression3XModel model, SST_FP x) {
    return SST_FP_Add(SST_FP_Mul(model.slope, x), model.intercept);
}
//======================================================================================================
// [DATA FLOW]
//======================================================================================================
// [market tick] -> [_Push] -> [ring buffer fills] -> [_Compute] -> [linearize + _Fit] -> [slope/intercept/r^2] -> [_Predict for future price] -> [compare to actual] -> [trading signal]
//======================================================================================================
// damn this stuff is actualyl really cool digging into the lower level stuff, like the overall architecture in FoxML was interesting to build with ai, but actually learning how this stuff works is actually really interesting tbh, like idk, it makes my brain happy, even if it is relatively simple to whats actually out there, and it feels more satisfying than just calling scikit_learn.fit() or whatever the lib call is
//======================================================================================================
#endif // LINEAR_REGRESSION_3X_H
