#!/bin/bash
#======================================================================================================
# [FIXED-POINT LIBRARY TEST RUNNER]
#======================================================================================================
# runs FP_header_test.cpp at every power-of-2 bit width from 64 up to the specified max
#
# usage:
#   ./run_tests.sh          # test 64 through 4096 (default)
#   ./run_tests.sh 8192     # test 64 through 8192
#   ./run_tests.sh 512      # test 64 through 512 (quick)
#
# compile flags: -std=c++17 -O2 -lm
# each width compiles separately since the template instantiation is per-width
#======================================================================================================

MAX_BITS=${1:-4096}
PASS_TOTAL=0
FAIL_TOTAL=0
COMPILER="g++"
FLAGS="-std=c++17 -O2"

echo "=== FixedPointN.h Test Suite ==="
echo "Testing bit widths: 64 to $MAX_BITS"
echo ""

BITS=64
while [ $BITS -le $MAX_BITS ]; do
    echo -n "FP$BITS: compiling... "

    if ! $COMPILER $FLAGS -DFP_TEST_BITS=$BITS -o "test_fp$BITS" FP_header_test.cpp -lm 2>/dev/null; then
        echo "COMPILE FAILED"
        FAIL_TOTAL=$((FAIL_TOTAL + 1))
        BITS=$((BITS * 2))
        continue
    fi

    echo -n "running... "
    OUTPUT=$(./"test_fp$BITS" 2>&1)
    EXIT_CODE=$?

    # extract results line
    RESULT=$(echo "$OUTPUT" | grep -E '=== FP[0-9]+:')
    PASSED=$(echo "$RESULT" | grep -oP '\d+ passed' | grep -oP '\d+')
    FAILED=$(echo "$RESULT" | grep -oP '\d+ failed' | grep -oP '\d+')

    # extract pi digits
    PI_LINE=$(echo "$OUTPUT" | grep 'computed.*digits of pi')
    PI_DIGITS=$(echo "$PI_LINE" | grep -oP 'computed \K\d+')

    if [ "$EXIT_CODE" -eq 0 ]; then
        echo "$PASSED passed, $FAILED failed | pi: $PI_DIGITS digits"
    else
        echo "$PASSED passed, $FAILED FAILED | pi: $PI_DIGITS digits"
        # show failures
        echo "$OUTPUT" | grep "FAIL:"
    fi

    PASS_TOTAL=$((PASS_TOTAL + PASSED))
    FAIL_TOTAL=$((FAIL_TOTAL + FAILED))

    rm -f "test_fp$BITS"
    BITS=$((BITS * 2))
done

echo ""
echo "=== Total: $PASS_TOTAL passed, $FAIL_TOTAL failed ==="
