#include <iostream>
#include <vector>

using namespace std;

// I got bored and started looking into bitshifting, and found about shifting to
// the right is essnetially 2^n per shift, essentially being divison by 2 per
// shift right and multiplication when shifted left, due to the way binary
// works. Then I came across the xor operation, where you have an array like
// {4,1,2,1,2}, and when you compare everything to its self, like [ 4^1^2^1^2]
// the double 1's and 2's essentially cancel eachother out, leaving the 4,
// leading to an even faster way to find a single number, in an array with pairs

int findSingle(vector<int> &nums) {
  int result = 0;
  for (int n : nums) {
    result ^= n; // xor bit-shifting
  }
  return result;
}
