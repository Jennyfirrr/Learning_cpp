#include <array>
#include <iostream>
#include <random>
#include <vector>

// LOL HERE WE GO NOW IM COOKING LOL
//
// gonna annotate this later, and probably do an inline asm analysis as well
// this is just playing around trying to see how packing different order bits
// into a single 32 bit int works

std::vector<int32_t>
build_order_book(const std::vector<int8_t> &potential_trades,
                 int32_t queued_orders) {
  int32_t order_bit_packed = 0;
  std::vector<int32_t> order_book_packed((queued_orders + 3) / 4);

  for (int j = 0; j < (int)order_book_packed.size(); j++) {
    order_bit_packed = 0;

    for (int i = 0; i < 4; i++) {
      order_bit_packed |= potential_trades[j * 4 + i] << (i * 8);
    }
    order_book_packed[j] = order_bit_packed;
  }

  return order_book_packed;
}

int32_t build_kill_mask(int8_t kill_mask_bits) {
  int32_t kill_mask_built = 0;

  // this should probably be renamed the risk gate
  // its not a true kill mask or kill switch
  // a kill mask would probably only be engaged if something went
  // catastrophically wrong, and would probably be engaged by a self monitoring
  // system for system health or something

  for (int i = 0; i < 4; i++) {
    kill_mask_built |= kill_mask_bits << (i * 8);
  }
  // use packed_orders & kill_mask == kill_mask

  return kill_mask_built;
}

int32_t kill_switch(int32_t packed_order_int, const int32_t &kill_mask_built) {
  return (packed_order_int & kill_mask_built) == kill_mask_built;
}

int main() {
  int32_t generate_x_orders;
  int8_t kill_mask_bits_id;

  std::cout << "Generate orders amount: ";
  std::cin >> generate_x_orders;
  generate_x_orders = generate_x_orders * 4;

  std::cout << "Select a kill mask id #[0-255]: ";
  std::cin >> kill_mask_bits_id;

  int32_t kill_mask_built = build_kill_mask(kill_mask_bits_id);

  std::mt19937 rng(21); // deterministic fixed seed
  std::uniform_int_distribution<int> dist(0, 255);

  std::vector<int8_t> potential_trades(generate_x_orders);

  for (int i = 0; i < generate_x_orders; i++) {
    potential_trades[i] = static_cast<int8_t>(dist(rng));
  }

  auto packed = build_order_book(potential_trades, generate_x_orders);

  int32_t valid_trades = 0;
  int32_t killed_trades = 0;

  for (auto &order : packed) {
    int32_t result = kill_switch(order, kill_mask_built);
    killed_trades += result;
    valid_trades += (1 - result);
  }

  std::cout << "Successful trades: " << valid_trades << "\n";
  std::cout << "Failed trades: " << killed_trades << "\n";
  std::cout << "Success rate: "
            << 100 - ((static_cast<float>(killed_trades) / valid_trades) * 100)
            << "%" << "\n";

  return 0;
}
