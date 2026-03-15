// ============================================================
// Bitwise Operators
// (combined from: or_operator.cpp, xor.cpp, bit_or+shift.cpp)
// ============================================================

#include <iostream>
#include <vector>

using namespace std;

// -----------------------------------------------------------
// OR Operator (or_operator.cpp)
// -----------------------------------------------------------

// exploring the | operator, which is the OR gate, this can be used to directly
// manipulate a single bit without touching the others using x |= (1 << 3),
// which means for x, update the 3rd bit from the right to be a 1, OR sets bits,
// AND clears bits, and XOR toggles bits

int or_op(int x) {
    x |= (1 << 3);
    return x;
}

// this specific example basically functions as a +8 to whatever number is
// input, because you are directly manipulating the 3rd bit from the right, so
// if you have say, 16, which is 00010000 |= 00000100 = 00010100, which is 24,
// note if you try to use 8, nothing happens, if the bit is already 1 that you
// are trying to set to 1, nothing actually changes in that specific case

#if 0 // main from or_operator.cpp
int main() {
  int num;
  std::cout << "Please select a number to try: ";
  std::cin >> num;

  std::cout << or_op(num) << "\n";

  return 0;
}
#endif

// -----------------------------------------------------------
// XOR — find single in paired array (xor.cpp)
// -----------------------------------------------------------

// I got bored and started looking into bitshifting, and found about shifting to
// the right is essnetially 2^n per shift, essentially being divison by 2 per
// shift right and multiplication when shifted left, due to the way binary
// works. Then I came across the xor operation, where you have an array like
// {4,1,2,1,2}, and when you compare everything to its self, like [ 4^1^2^1^2]
// the double 1's and 2's essentially cancel eachother out, leaving the 4,
// leading to an even faster way to find a single number, in an array with pairs

int findSingle(vector<int> &nums) {
    int result = 0;
    for (int n : nums) {
        result ^= n; // xor bit-shifting
    }
    return result;
}

// -----------------------------------------------------------
// OR + XOR + Shift Combos (bit_or+shift.cpp)
// -----------------------------------------------------------

// just playing with bitwise operators for funsies

void or_shift_combo(int cycles) {
    int result = 0;

    for (int i = 0; i < cycles; i++) {
        result = result << 1;
        // shift the bit to the left once, so at 0, you get 0
        // becuase there is nothing to shift
        result = result ^ (result << 1);
        // use the xor operator with the value from the above shifted
        // right once again to nullify the prior
        result |= 1;
        // use the or operator to add a 1 to the end, so that your
        // always adding one every cycle
        std::cout << "Cycle #" << i + 1 << ": " << result << "\n";
    }
}

// easier visualization of the xor operator for the last cycle referenced
//
//  0 0 0 0 1 1 1 0 = 14
//  0 0 0 1 1 1 0 0 = 28
//  ---------------
//  0 0 0 1 0 0 1 0 = 18

// what this does, you start with 00000000_00000000, xor does nothing because
// there is nothing to shift, so your left with 00000000 | 1, which becomes
// 00000001, or 1, on the first cycle, then the next cycle, you shift the 1 left
// by 1 bit, so you get 2, which is 00000010, then 00000010 ^ 00000100, which
// becomes 6, because xor only nullifys if the bits match, so the result is
// 00000110 then you 00000110 | 00000001 which becomes 00000111, then on 3rd
// cycle, you shift 00000111 to the right 1 bit, so you get 00001110, which is
// 14, then you xor 00001110 ^ 00011100, which becomes 00010010, which is 18,
// and then 00010010 | 00000001 which becomes 19 or 00010011, and so on
//
// you could probably do some wacky shit with this regarding ANSI or ASCII codes
// lol, since each character has an associated number assigned to it
//
// java doesnt let you do cool shit like this, it assumes your stupid and need
// hand holding

void or_exp(int cycles) {
    int result = 0;

    for (int i = 0; i < cycles; i++) {
        result = result << 1;
        // shifts the bit left 1
        result |= 3;
        // adds a 1 to the 0 and 1 bit, assuming they dont already have one
        result = result ^ (result >> 1);
        // xor the result value against result shifted back right 1
        std::cout << "Cycle #" << i + 1 << ": " << result << "\n";
    }
}

// this has a really weird growth pattern lol, this shit is wacky lol, i love
// it, instead of having a massive exponential growth rate always, every 2^n
// power cycles, it just does +2 instead of a huge jump, overflows at cycle > 31

void werid(int cycles) {

    for (int i = 0; i < cycles; i++) {
        int x = i >> (i & (i << 1));
        if (x != 0) {
            std::cout << "Cycle #" << i + 1 << ": " << x << "\n";
        }
    }
}

// these are so weird lol, this shit is SO FREAKING COOL

int main() {
    int cycles;
    int selection = 0;

    while (selection == 0) {
        std::cout << "Please select a function: ";
        std::cin >> selection;
    }

    std::cout << "Please select a cycle count: ";
    std::cin >> cycles;
    if (selection == 1) {
        or_shift_combo(cycles);
    } else if (selection == 2) {
        or_exp(cycles);
    } else {
        werid(cycles);
    }

    return 0;
}
