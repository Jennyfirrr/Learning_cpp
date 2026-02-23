#include <iostream>
#include <vector>

// WIP FOR ACTUALLY BUILDING ORDERBOOK LOGIC, i understand that its better to
// use a single reusable function, but im doing it this way to actually see how
// things operate across the entire process for better understanding
// *sparkle emoji*T R U S T  T H E  P R O C E S S*sparkle emoji* <3

int32_t check_0bit(int32_t num) {
  int result0 = num & (1 << 0);
  return result0;
}

int32_t check_1bit(int32_t num) {
  int result1 = num & (1 << 1);
  return result1;
}

int32_t check_2bit(int32_t num) {
  int result2 = num & (1 << 2);
  return result2;
}

int32_t check_3bit(int32_t num) {
  int result3 = num & (1 << 3);
  return result3;
}

int32_t check_4bit(int32_t num) {
  int result4 = num & (1 << 4);
  return result4;
}

int32_t check_5bit(int32_t num) {
  int result5 = num & (1 << 5);
  return result5;
}

int32_t check_6bit(int32_t num) {
  int result6 = num & (1 << 6);
  return result6;
}

int32_t check_7bit(int32_t num) {
  int result7 = num & (1 << 7);
  return result7;
}

/*
 * single function to check all states at once, but AS EXPLAINED IN MY LAST
 * EMAIL, im learning the process by actually writing out the functions bit by
 * bit, for a better M E N T A L  M O D E L, kthxbaiiiiii int32_t
 * extract_bit_state(int32_t packed_state, uint8_t bit_index) { return
 * packed_state & (1 << bit_index);
 * }
 */

std::vector<int> build_vector_states(int32_t order_book0) {
  std::vector<int> states(8);

  states[0] = check_0bit(order_book0);
  states[1] = check_1bit(order_book0);
  states[2] = check_2bit(order_book0);
  states[3] = check_3bit(order_book0);
  states[4] = check_4bit(order_book0);
  states[5] = check_5bit(order_book0);
  states[6] = check_6bit(order_book0);
  states[7] = check_7bit(order_book0);

  // basically this function, uses the 8 other functions, to build a vector that
  // shows which states are true or false, and im probably gonna try to build a
  // fail switch function next, that uses the vector as an input and checks for
  // the nth bit which i wanna encode as a kill switch

  return states;
}

int main() {
  int32_t order_book_sim;
  std::cout << "Please choose a number between 0 and 255: ";
  std::cin >> order_book_sim;

  const std::vector<int32_t> states_of_order_book =
      build_vector_states(order_book_sim);

  for (auto order : states_of_order_book) {
    std::cout << order << ", ";
  }

  std::cout << "\n";

  return 0;
}
