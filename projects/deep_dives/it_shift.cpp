#include <iostream>
#include <vector>

using namespace std;

int bitShift(vector<int> &nums, int shift) {

  cout << "Bits shifted right: " << shift << endl;
  // every shift right is dividing by 2^n, where n is the shift right, so 1
  // shift is x/2, 2 shift is x/4,etc

  for (int n : nums) {
    int result = n >> shift;
    cout << n << " : " << result << "\n";
  }

  return 0;
}

int main() {
  int shift;

  cout << "Select how many bits you want to shift right: ";
  cin >> shift;

  vector<int> nums = {2, 4, 6, 8, 10, 12, 14, 16};

  bitShift(nums, shift);

  return 0;
}
