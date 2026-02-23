#include <array>
#include <iostream>
#include <random>
#include <vector>

// probably gonna look at how this converts to asm when i havent been up for
// like 24 hours, toodles

int32_t kill_switch(int32_t order_book_state, int32_t kill_mask) {
  return (order_book_state & kill_mask) == kill_mask;
  // this compiles to asm with no jumps, only cmovne
}

int32_t build_kill_mask(const std::array<int8_t, 32> &kill_mask_bits) {
  int32_t kill_mask_built = 0;

  for (int i = 0; i < 32; i++) {
    // this if statement compules to a cmovne
    if (kill_mask_bits[i]) {
      kill_mask_built |= (1 << i);
    }
  }

  return kill_mask_built;
}

std::array<int8_t, 32> build_kill_switch_bits(const int32_t &order_book_seed) {
  std::array<int8_t, 32> kill_switch_bits;

  for (int i = 0; i < 32; i++) {
    kill_switch_bits[i] = (order_book_seed >> i) & 1;
    // this basically checks each bit in the order_book_seed, and shifts it to
    // the right according to the index, so its always comparing the bit to the
    // 1 binary
    //
    // my bad, i keep defaulting to the positional storage instead of the
    // normalized 1/0, im learning :D
  }

  return kill_switch_bits;
}

int main() {
  int order_book_cycle;
  int kill_mask_seed;

  std::cout << "Cycles: ";
  std::cin >> order_book_cycle;

  std::cout << "Seed: ";
  std::cin >> kill_mask_seed;

  std::array<int8_t, 32> kill_mask_bits =
      build_kill_switch_bits(kill_mask_seed);

  int32_t kill_mask = build_kill_mask(kill_mask_bits);

  int32_t killed_trades = 0;

  for (int i = 0; i < 32; i++) {
    std::cout << static_cast<int>(kill_mask_bits[i]) << " ";
  }

  for (int i = 0; i < order_book_cycle; i++) {
    // the loop condition within this for loops compiles down to a jump using
    // cmpl %ecx, %eax+jne
    if (kill_switch(i, kill_mask)) {
      killed_trades++;
    }
  }

  std::cout << "\nCompleted trades: " << order_book_cycle - killed_trades
            << "\n";
  std::cout << "Killed trades: " << killed_trades << "\n";
  std::cout << "% killed: "
            << (static_cast<float>(killed_trades) / order_book_cycle) * 100
            << "%\n";

  return 0;
}
