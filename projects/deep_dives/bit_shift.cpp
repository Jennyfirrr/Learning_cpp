#include <iostream>
#include <vector>

using namespace std;

// probably gonna look at n & (n - 1) == 0 for fast power of 2 checks, I read
// this is heavily used in quant/HFT systems, and instead of passing full bool
// values for T/F and stuff like that, just using a status bit, with a 1 or 0 is
// faster and lower latency. This would allow 32 status flags to be stored in a
// single 4 byte integer, following up with the n & ( n - 1), I was wrong, the ~
// operator,  flips all the bits, where 1000 would be 8, then it flips the bits
// to 0111, which is 7. the n & (n - 1) just clears the lowest bit, my bad, im
// learning. Im gonna read about and update another file with bitmasking, as
// well as L1 and L2 caching since the ram is usually the bottleneck when trying
// to access data in HFT systems, or exeuction engines. This is beneficial for
// the L1 cahcing though, because it created far smaller data structures, which
// allows more to be stored using less, in a faster section of memory.
//
// I learned about bitfields while doing this, where you use each index in a
// binary number to store a true or false value, and instead of sequentially
// reading them 1 by 1, you can just return the actual number it is, to figure
// out which are true and false in a single read cycle, because every number up
// to 255 has a unique binary ID. LOL you can store 32 t/f values in the space
// of a single integer. Like if you returned the integer 8 when reading a
// number, that could potnetially mean that only the condition assigned to the
// 4th, or the 3rd index of the bit if you wanna be pedantic, but whatever, bit
// is true, while every other condition is false, this leads to so many
// possibilities lol, this is SO COOL WTF.
//
// the reason that n & (n - 1) == 0 actually finds powers of 2, is that because
// of the way the & (AND) operator works, using 8 as an example, you have 8 & (8
// - 1) which becomes 1000 & 0111, so because & requires that the bits in the
// same positions need to be the same, you end up with 0000, which means 8 is a
// power of 2, because only powers of 2 have this relationship with (2^n)-1, my
// earlier explanation and notes were just surface level reading before a java
// exam, this was more indepth when i wasnt tired, and actually learning.

int bitShift(vector<int> &nums, int shift) {

  // shifts like for example, 1000 is 8, shifting right by 1 makes 0100, which
  // is 4, and again 0010, which is 2

  cout << "Bits shifted right: " << shift << endl;
  // every shift right is dividing by 2^n, where n is the shift right, so 1
  // shift is x/2, 2 shift is x/4,etc

  for (int n : nums) {
    int result = n >> shift;
    int result2 = n & (n - 1);
    int result3 = n & ~(1 << shift);
    int result4 = n & ~(2 << shift);
    int result5 = n ^ (n & ~(1 << shift));
    int result6 = n & (1 << shift);
    // this is basically find the number
    // that is 2 ^ shift power lol wtf, for example im
    // going to use n = 16, shift = 4, 1 << 4 = 1000, which
    // is 16(the mask), ~(1 << 4) = 11101111, because every
    // bit is flipped, so 0001000 & 11101111 = 00000000(16
    // AND the mask clears that bit), so n ^ 0 = 0001000,
    // for n = 12, shift = 4, 1 << 4 is 00010000, ~(1 << 4)
    // is 11101111, 12 & 11101111 = 0001100 & 11101111 =
    // 00001100(12, unchanged because bit 4 wasnt set), so
    // 12 ^ 12 = 0,

    // neat trick to find powers of 2, because
    // if it ends in a 0, its a power of 2
    // class even/odd trick is just n & 1 apparently, because if the last number
    // is odd, the last index is a 1, the & operator essentially acts as a true
    // false gate for the number in binary, where it only lets n through, if
    // both numbers have identical bits in the same index
    cout << "Right shift: [" << n << " -> " << result << "] , ";
    cout << "[" << n << " -> " << result2 << "] , ";
    cout << "[" << n << " -> " << result3 << "] , ";
    cout << "[" << n << " -> " << result4 << "] , ";
    cout << "[" << n << " -> " << result5 << "] , ";
    cout << "[" << n << " -> " << result6 << "]" << "\n";
  }

  return 0;
}

int bitShiftLeft(vector<int> &nums, int shift) {

  // opposite of the above, 1000 is 8, so left by 1 is 10000 which is 16

  cout << "Bits shifted left: " << shift << endl;
  // every shift left is dividing by 2^n, where n is the shift left, so 1
  // shift is x * 2, 2 shift is x * 4,etc

  for (int n : nums) {
    int result = n << shift;
    cout << "Left Shift: [" << n << " -> " << result << "]" << "\n";
  }

  return 0;
}

int main() {
  int shift;
  int selection = 0;

  while (selection != 1 && selection != 2) {
    cout << "Please select if you want to shift right or left[1 for left : 2 "
            "for right]: ";
    cin >> selection;
  }

  cout << "Please select how many indexes you want to shift: ";
  cin >> shift;

  vector<int> nums = {2,  3,  4,  5,  6,  7,  8,  9,   10,  11,
                      12, 13, 14, 15, 16, 32, 64, 128, 256, 512};

  if (selection == 1) {
    bitShiftLeft(nums, shift);

  } else {
    bitShift(nums, shift);
  }
  return 0;
}
