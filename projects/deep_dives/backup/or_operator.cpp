#include <iostream>
#include <vector>

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

int main() {
  int num;
  std::cout << "Please select a number to try: ";
  std::cin >> num;

  std::cout << or_op(num) << "\n";

  return 0;
}
