#include <iostream>
#include <vector>

using namespace std;

int count_len(vector<int> digits) {
  int slow = 0;
  int fast = slow * 2;

  for (auto i = 0; i < digits.size() + 1; i++) {
    slow = slow + i;
  }

  return digits[fast / 2];
}

int main() {
  vector<int> nums = {1, 2, 3, 4, 5, 6, 7, 8};

  cout << count_len(nums) << endl;
}
