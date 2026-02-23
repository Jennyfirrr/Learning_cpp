#include <array>
#include <iostream>
#include <vector>

// probably gonna look at how this converts to asm when i havent been up for
// like 24 hours, toodles

int32_t kill_switch(int32_t order_book_state, int32_t kill_mask) {
  return (order_book_state & kill_mask) == kill_mask;
}

int32_t build_kill_mask(const std::array<int8_t, 32> &kill_mask_bits) {
  int32_t kill_mask_built = 0;
  for (int i = 0; i < 32; i++) {
    if (kill_mask_bits[i]) {
      kill_mask_built |= (1 << i);
    }
  }
  return kill_mask_built;
}

int main() {
  int order_book_cycle;

  std::cout << "Cycles: ";
  std::cin >> order_book_cycle;

  std::array<int8_t, 32> kill_mask_bits = {1, 0, 1, 1, 0, 0, 1, 0, 0, 0};

  int32_t kill_mask = build_kill_mask(kill_mask_bits);

  int32_t killed_trades = 0;

  for (int i = 0; i < order_book_cycle; i++) {
    if (kill_switch(i, kill_mask)) {
      killed_trades++;
    }
  }

  std::cout << "Killed trades: " << killed_trades << "\n";
  std::cout << "% killed: "
            << (static_cast<float>(killed_trades) / order_book_cycle) * 100
            << "%\n";

  return 0;
}
