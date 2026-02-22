#include <iostream>

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
    std::cout << "Cycle #: " << i + 1 << " : " << result << "\n";
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

int main() {
  int cycles;

  std::cout << "Please select a cycle count: ";
  std::cin >> cycles;

  or_shift_combo(cycles);

  return 0;
}
