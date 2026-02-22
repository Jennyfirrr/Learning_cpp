#include <iostream>

int32_t check_first(int32_t num) {
  // this function extracts the bit from the from the bit index[1], while
  // leaving the original number untouched
  int32_t result = num & (1 << 1);
  // the cout here is so you can see what is happening
  std::cout << result << "\n";
  return result;
}

int32_t check_second(int32_t num) {
  // this function extracts the bit from the bit index [2], while leaving the
  // original untouched just like the first
  int32_t result = num & (1 << 2);
  std::cout << result << "\n";
  return result;
}

int32_t smallest(int32_t num1, int32_t num2) {
  // this compares the two bits to find which is the higher bit, sort of a small
  // signal extractor that would be used to unpack data from a bit
  // Apparently the name for this is a branchless comparator chain for comparing
  // bits, there are two ways to kind of use the data from this i guess, the one
  // im showing here is using the positional encoding that s foundin the bit,
  // where result from the prior 2 functions returns the actual value that the
  // bit represents, like 2, from check first, and 4 from check second, you
  // could also return a 0 or 1 by masking it, but im just kind of toying around
  // with this atm, according to what im reading, this is essentially the same
  // pattern used like i stated above or w/e, like imagine that the number 255
  // is an orderbook state, and you have the binary code 0x000000FF, so
  // 11111111, you basically have both the 2, and the 4 position considered
  // active, where these could represent different things, like, "is this bid
  // aggressive" and "is this spread wide", the positional value of the bit that
  // is active could also preserve where the bit is, not just is it set or not,
  // and this is apparently called implicit encoding, where the data carries its
  // own metadata, which further allows for even more functionality in the L1
  // cache memory limitations, because the more data you can encode into a
  // single bit, the faster your system will be, because no ICKY ram hits, we
  // dont like those, ram is for training models, not executing trades lol
  // im probably gonna keep expaning on this file for a bit, but its like 3am
  // lol on a sunday, and im procrastinating doing my java homework(shocker) by
  // experimenting with this lol, using this method though, you basically build
  // an if / else branch, without using branches that essntially do whatever is
  // encoded in num1, assuming that it is bigger than num2, if num2 is bigger,
  // do num2, im sure that there are versions of these out there that use the
  // entire 32bit or even 64 bit register and have it all running in tandem
  // using SIMD or AVX instructions but im just a girl, and a baby HFT(hopeful)
  // engineer, so im L E A R N I N G
  //
  int32_t diff = -(num1 < num2);
  std::cout << diff << "\n";
  int32_t mask = diff >> 31;
  std::cout << mask << "\n";

  return num2 ^ ((num1 ^ num2) & mask);
}

int main() {
  int32_t num;
  int32_t num2;

  std::cout << "Please choose a num to see how this works: ";
  std::cin >> num;
  std::cout << "Please choose a num2: ";
  std::cin >> num2;

  std::cout << smallest(check_first(num), check_second(num2)) << "\n";

  return 0;
}
