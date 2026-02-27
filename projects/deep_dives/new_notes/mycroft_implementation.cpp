#include <iostream>
#include <random>
#include <vector>

std::vector<uint64_t>
build_order_book(const std::vector<uint8_t> &potential_trades,
                 uint32_t queued_orders) {
  std::vector<uint64_t> order_book_packed(queued_orders / 8);

  uint64_t order_bit_packed = 0;

  for (int i = 0; i < (int)order_book_packed.size(); i++) {
    order_bit_packed = 0;

    for (int j = 0; j < 8; j++) {
      order_bit_packed |= potential_trades[i * 8 + j] << (j * 8);
    }
    order_book_packed[i] = order_bit_packed;
  }

  return order_book_packed;
}

uint64_t risk_gate(uint8_t risk_gate_sell, uint8_t risk_gate_buy) {
  uint64_t risk_gate_built = 0;

  // i wanted to try packing 4 buy and 4 sell ID's here
  // gonna write more on this later, E X P E C T  A N  E S S A Y  L O L

  for (int i = 0; i < 4; i++) {
    risk_gate_built |= risk_gate_sell << (i * 8);
  }
  // 0 -> 31 are sell bytes

  for (int i = 0; i < 4; i++) {
    risk_gate_built |= risk_gate_buy << ((i * 8) + 32);
  }

  // 32 -> 63 are buy bytes

  return risk_gate_built;
}

/*
uint32_t calc_laneMatchCount(uint64_t packed_order, uint64_t risk_gate) {
  uint64_t diff = packed_order ^ risk_gate;

  uint64_t matches = ((diff - 0x01010101) & ~diff & 0x80808080);

  return __builtin_popcount(matches);
}
*/

uint32_t calc_laneMatchCount64(uint64_t packed_order, uint64_t risk_gate) {
  uint64_t xor_result = packed_order ^ risk_gate;

  uint32_t count = 0;
  for (int i = 0; i < 8; i++) {
    count += ((xor_result >> (i * 8)) & 0xFF) == 0;
  }
  return count;
}

int main() {
  uint32_t generate_x_orders;
  uint32_t temp_kill_mask_buy;
  uint32_t temp_kill_mask_sell;

  std::cout << "Generate orders amount: ";
  std::cin >> generate_x_orders;

  generate_x_orders = generate_x_orders * 8;

  std::cout << "Select a kill mask ID, buy side[0-255]: ";
  std::cin >> temp_kill_mask_buy;
  std::cout << "Select a kill mask ID, sell side[0-255]: ";
  std::cin >> temp_kill_mask_sell;

  uint8_t kill_mask_buy = static_cast<uint8_t>(temp_kill_mask_buy);
  uint8_t kill_mask_sell = static_cast<uint8_t>(temp_kill_mask_sell);

  uint64_t risk_gate_built = risk_gate(kill_mask_sell, kill_mask_buy);

  std::mt19937 rng(42);
  std::uniform_int_distribution<int> dist(0, 255);

  std::vector<uint8_t> potential_trades(generate_x_orders);

  for (int i = 0; i < (int)generate_x_orders; i++) {
    potential_trades[i] = static_cast<uint8_t>(dist(rng));
  }

  auto packed = build_order_book(potential_trades, generate_x_orders);

  uint64_t valid_trades = 0;

  for (auto &order : packed) {
    valid_trades += calc_laneMatchCount64(order, risk_gate_built);
  }

  std::cout << "Total trades: " << generate_x_orders << "\n";

  std::cout << "Successful trades: " << valid_trades << "\n";
  std::cout << "Success rate: "
            << (static_cast<float_t>(valid_trades) / generate_x_orders) * 100
            << "%\n";
  std::cout << "Failed trades: " << generate_x_orders - valid_trades << "\n";
  std::cout << "Fail rate: "
            << ((static_cast<float_t>(generate_x_orders) - valid_trades) /
                generate_x_orders) *
                   100
            << "%\n";

  return 0;
}
