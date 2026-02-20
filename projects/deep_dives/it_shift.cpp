#include <iostream>
#include <vector>

using namespace std;

int bitShift(vector<int> &nums, int shift) {

  cout << "Bits shifted right: " << shift << endl;

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
