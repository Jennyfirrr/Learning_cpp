#include "FixedPointN.h"

using FP = SST_FPN<64>;

__attribute__((noinline)) FP do_add(FP a, FP b) { return SST_FPN_AddSat(a, b); }
__attribute__((noinline)) FP do_sub(FP a, FP b) { return SST_FPN_SubSat(a, b); }
__attribute__((noinline)) FP do_mul(FP a, FP b) { return SST_FPN_Mul(a, b); }
__attribute__((noinline)) FP do_div(FP a, FP b) { return SST_FPN_DivNoAssert(a, b); }
__attribute__((noinline)) FP do_abs(FP a) { return SST_FPN_Abs(a); }
__attribute__((noinline)) FP do_negate(FP a) { return SST_FPN_Negate(a); }
__attribute__((noinline)) FP do_min(FP a, FP b) { return SST_FPN_Min(a, b); }
__attribute__((noinline)) FP do_max(FP a, FP b) { return SST_FPN_Max(a, b); }
__attribute__((noinline)) int do_lt(FP a, FP b) { return SST_FPN_LessThan(a, b); }
__attribute__((noinline)) int do_gt(FP a, FP b) { return SST_FPN_GreaterThan(a, b); }
__attribute__((noinline)) int do_eq(FP a, FP b) { return SST_FPN_Equal(a, b); }
__attribute__((noinline)) FP do_floor(FP a) { return SST_FPN_Floor(a); }
__attribute__((noinline)) FP do_ceil(FP a) { return SST_FPN_Ceil(a); }
__attribute__((noinline)) FP do_round(FP a) { return SST_FPN_Round(a); }

FP sink_fp;
int sink_int;

int main() {
    FP a = SST_FPN_FromDouble<64>(10.5);
    FP b = SST_FPN_FromDouble<64>(3.25);
    sink_fp = do_add(a, b);
    sink_fp = do_sub(a, b);
    sink_fp = do_mul(a, b);
    sink_fp = do_div(a, b);
    sink_fp = do_abs(a);
    sink_fp = do_negate(a);
    sink_fp = do_min(a, b);
    sink_fp = do_max(a, b);
    sink_int = do_lt(a, b);
    sink_int = do_gt(a, b);
    sink_int = do_eq(a, b);
    sink_fp = do_floor(a);
    sink_fp = do_ceil(a);
    sink_fp = do_round(a);
    return 0;
}
