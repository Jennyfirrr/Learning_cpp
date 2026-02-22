#include <iostream>

// im gonna start experimenting with different applications of bitmasking at
// branchless programming here, expect actual use cases and examples for how
// things actually work

int32_t smallest_of_two(int32_t a, int32_t b) {
  int32_t diff = a - b;
  int32_t mask = diff >> 31;
  // so the way this works, is that mask becomes 0xFFFFFFFF if a < b, and
  // 0x00000000 is a >= b, in an example, lets use 8 and 16, where a is 8, and b
  // is 16, the diff value becomes -8, which in binary is 11111001, when you
  // shift everything right 31 places in the mask, you get 0xFFFFFFFF, then when
  // you compare the diff & mask statement it does the following below
  //
  // 1 1 1 1 1 0 0 1
  // 1 1 1 1 1 1 1 1
  // ---------------
  // 1 1 1 1 1 0 0 1
  //
  // then you take 16, which is b, and add -8 to it, because that is what the &
  // operator output after doing the bit matching operation, and your left with
  // 8, which is a
  //
  // suppose a is 16, and b becomes 8, you first apply the diff, which becomes
  // 8, which is 00001000 shift to the right 31 places, and your left with
  // 0x00000000
  //
  // and the diff & mask operator return this value
  //
  // 0 0 0 0 1 0 0 0
  // 0 0 0 0 0 0 0 0
  // ---------------
  // 0 0 0 0 0 0 0 0
  //
  // which is 0, and b + 0 is 8 + 0, so b is the lower value
  //
  // i freaking love this stuff, it is 10:52pm on a saturday, and im losing my
  // mind over bitshifting techniques and branchless programming, when i should
  // probably be doing my homework assignments or leetcode

  return b + (diff & mask);
  // if a < b, diff is negative so the mask is all 1's, you get b + (a - b), and
  // if a >= b, the diff is positive, so the mask becomes all 0's and your left
  // with b + 0, which is b
  //
  // this is FREAKING awsome, because its another way to stop using if/else
  // statements to decide what to do, when you can just use pure bit based
  // operations, while if/else statements may be faster in some cases, pure bit
  // shiting and masking approaches like this are generally fast enough, and
  // have a FAR smaller variance, like if/else can go from like 2ns to 200ns
  // spikes, which is so bad its not even funny lol, but the bit masking
  // approach may only go from like 3ns to like 10ns which is a far more
  // acceptable tolerance, part of the reason that approaches like this are
  // branchless is because the decision is build into the calculation, by virtue
  // of how bit manipulation works, which is still blowing my mind you can even
  // do this just by messing around with 1/0 states, there is an optimization
  // where if you HAVE to use an if statement, you can do something like
  // if(__builtin_expect(order_is_valid, 1)) and this basically tells the
  // compiler to make the code tell the cpu like "hey, this is true 99.9% of the
  // time", and to flag it as the hot path
}

int main() {
  int32_t a;
  int32_t b;
  std::cout << "Please select value a: ";
  std::cin >> a;
  std::cout << "Please select value b: ";
  std::cin >> b;

  std::cout << smallest_of_two(a, b) << "\n";

  return 0;
}
