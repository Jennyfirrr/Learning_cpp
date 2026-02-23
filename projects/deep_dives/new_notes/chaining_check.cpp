#include <iostream>
#include <vector>

// WIP FOR ACTUALLY BUILDING ORDERBOOK LOGIC, i understand that its better to
// use a single reusable function, but im doing it this way to actually see how
// things operate across the entire process for better understanding
// *sparkle emoji*T R U S T  T H E  P R O C E S S*sparkle emoji* <3

int32_t check_0bit(int32_t num) { return (num & (1 << 0)) >> 0; }

int32_t check_1bit(int32_t num) { return (num & (1 << 1)) >> 1; }

int32_t check_2bit(int32_t num) { return (num & (1 << 2)) >> 2; }

int32_t check_3bit(int32_t num) { return (num & (1 << 3)) >> 3; }

int32_t check_4bit(int32_t num) { return (num & (1 << 4)) >> 4; }

int32_t check_5bit(int32_t num) { return (num & (1 << 5)) >> 5; }

int32_t check_6bit(int32_t num) { return (num & (1 << 6)) >> 6; }

int32_t check_7bit(int32_t num) { return (num & (1 << 7)) >> 7; }

// i was wrong about this, this way normalizes to one, because it first checks
// if the associated bit is active, then it shifts the value returend back to
// the 0 bit
//
// originially i was just using something like result = num & (1 << bit_index),
// which just returned the actual value of the bit, like for 7, returns 128,
// which isnt ideal, so i went back and used logic to normalize it to a 1 or 0
// using a right shift for a written out example see below
//
// so assuming you start with bit 7 in a number like 255, bit 7 represents 128
// so
//
// 1 1 1 1 1 1 1 1 = 255
// 1 0 0 0 0 0 0 0 = 1 << 7, then apply the & operator
// ---------------
// 1 0 0 0 0 0 0 0 = 128, so then the reason you wanna use the result >> 7, is
// because this normalizes it to either 1 or 0
//
// 1 0 0 0 0 0 0 0 >> 7 = 0 0 0 0 0 0 0 1 which equals 1, and allows for better
// and easier parsing, because it simply cuts it down to an on or off state, god
// playing with my bits is so much fun

/*
 * single function to check all states at once, but AS EXPLAINED IN MY LAST
 * EMAIL, im learning the process by actually writing out the functions bit by
 * bit, for a better M E N T A L  M O D E L, kthxbaiiiiii int32_t
 * extract_bit_state(int32_t packed_state, uint8_t bit_index) { return
 * packed_state & (1 << bit_index);
 * }
 */

std::vector<int32_t> build_vector_states(int32_t order_book0) {
  std::vector<int32_t> states(8);

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
  // the nth bit which i wanna encode as a kill switch, im also probably gonna
  // start looking into how to actually convert these to buy/sell statements but
  // based off of what i know about jump tables, it may be oky(?) to use a jump
  // table here because its basically a single clock cycle instead of the branch
  // prediction mess that if else would cause, i could be wrong, but im learning
  // as i go, so BEAR WITH ME PLEASE

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
