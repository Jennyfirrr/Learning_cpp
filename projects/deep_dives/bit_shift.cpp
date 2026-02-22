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
// possibilities lol, this is SO COOL WTF. You could combine this with case
// switching to make super fast interpretations by the way the compiler converts
// to a jump table, like if case 8, you know bit 3 is a true condition, and
// 2,1,0 are all false, leading to extremely fast, low memory solutions for
// tracking multivariable states that need up to 32 conditions in unsigned 32
// bit integers, and 64 conditions in unsigned 64 bit ints, unisnged is
// prefereed bceause i read that signed ints have some weird interactions in
// some cases, and for low latency systems, mitigating any chance of risk is
// highly desireable. This could probably be further expanded to work by using
// integer addition or something or the & operator to set matching conditions
// for what action happens when using whole integers in a cas switch format,
// maybe just cutting out the entire case switching thing, and just adding the &
// operator to compare numbers, im not sure. I read more about this and
// apparently ive arrived at bitmasking dispatchers, so I guess there is a name
// for this lol, i love this shit. Ill probably do a deep dive on that at some
// point, im just kinda adding stuff here that I find interesting as I go and
// learn, and right now its bit manipulation and its applications. THeres not
// rubric or syllabus im following, this just makes my brain happy going down
// these rabbit holes. You could probably even encode numbers as blocks of sell
// orders, like 8 (00001000) could indicate sell x shares, or sell y block, or
// buy n number of something, you could literally do so much with this lol, wtf,
// this is way more interesting than java(doo doo), sorry not sorry. The
// encoding mechanism would probably be far more complex than just using simple
// case switching, or if else statements, but this would be magnitudes faster,
// and use far less memory for the actual execution side of things. its like
// using Fixed point math for ensuring floating point operations are actually
// what the number is by converting them to ints first, or only using ints for
// math, but for representing operations and function calls by using a single
// bit.
//
// You could probably even do like huge chains of if statements up to 64 long,
// by just using the INT_MAX, because as long as its an odd number, which im
// pretty sure it is, everything would have to be 1, so you could say, using 255
// as an example, only execute function x, if 255 is returned, bceause it
// requires all 7 prior conditions to be met to be true, like 11111111, so for
// the 7th bit to be true, all the other bits need to be true as well, which
// eleminates the need for using if statements to check true values for whether
// or not to execute a function. LOL a single byte to represent 8 conditional
// statements nested within eachother. THIS SHIT IS W I L D
//
// I guess this is why the L1 cache is so important. uint64_t is a size of 8
// bytes, and the cache line is 64 bytes long, so you can essentially fit 8
// complete trading states into a single cache line fetch, for a single
// nanosecond of operations lol. W I L D, and using a struct, which is what i
// thought was how it was done at first, is 32 bytes M I N I M U M, just to
// store 32 seperate bools, AND its stored scattered around memory, whereas
// this, you can control exactly where it is, for a fraction of the space, and
// magnitudes faster access.
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
    // going to use n = 16, shift = 4, 1 << 4 = 10000, which
    // is 16(the mask), ~(1 << 4) = 11101111, because every
    // bit is flipped, so 00010000 & 11101111 = 00000000(16
    // AND the mask clears that bit), so n ^ 0 = 00010000,
    // for n = 12, shift = 4, 1 << 4 is 00010000, ~(1 << 4)
    // is 11101111, 12 & 11101111 = 00001100 & 11101111 =
    // 00001100(12, unchanged because bit 4 wasnt set), so
    // 12 ^ 12 = 0,
    //
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

// so apparently, result5 = n ^ (n & ~(1 << shift)); , is a way to check to see
// if an individual bit is set, at the shift value, the mask is created by using
// the ~(1 << shift), so like, if you want to check the 3rd bit, you would set
// the shift to 3, and 1 << 3 gives you 00001000, the not (~) operator flips
// these so that it becomes 11110111, this is the inverse mask, which
// essentially says, Protect everything EXCEPT the third bit, the isolator is
// the (n & mask), where mask is defined earlier in this explanation, so if n is
// 00001100, and you apply the mask to that number, the result of (n & ~(1 <<
// shift)) is going to have a guarenteed 0 at the 3rd spot, because 0 & 1 == 0,
// due to the way the AND operator only returns a 1, if both bits are 1,
// everything else will return 0, the comparison, n ^ (result), the XOR operator
// returns a 1 only if the bits are DIFFERENT, and based on the operations done
// before, the only place that the bits will be different is the 3rd bit,
// because we manually forced it to 0 in an earlier step, so if the 3rd bit was
// 1 in the original n, then the XOR sees 1 ^ 0 and returns a 1, where as if the
// original 3rd bit was a 0, it sees 0 ^ 0, and by the rules of how XOR works,
// any value that is XOR that same value will return a 0. This leaves result 5
// as either 0, or an exact power of 2, which can be checked using the n & (n -
// 1) operator as outlined above how the relationship with those 2 binary values
// works, since you dont have to convert to a bool, you can just pass this value
// onto another bitwise function without any conversion, saving time, speed, and
// memory, especially when dealing with L1 cache hits vs reading a line from
// RAM(Y U C K Y), there is apparently a built in function within the standard
// library, that can do this faster, using the call std::has_single_bit(C++ 20),
// or n & -n can apparently find it even faster than that, but im kinda relaxing
// at the time of writing this, so ill probably make notes about that in another
// file at some point,

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
