#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

int binarySearch(const vector<int> &nums, int target) {
  int left = 0;
  int right = nums.size() - 1;

  while (left <= right) {
    int mid = left + (right - left) / 2;

    if (nums[mid] == target) {
      return mid;
    } else if (nums[mid] < target) {
      left = mid + 1;
    } else {
      right = mid - 1;
    }
  }

  return left;
}

int main() {
  vector<int> nums = {1,  2,  3,  4,   5, 6, 7,   8, 9,   10, 20,
                      30, 40, 41, 130, 1, 3, 204, 6, 302, 4};

  sort(nums.begin(), nums.end());

  cout << "Select a target: ";
  int target;
  cin >> target;

  cout << binarySearch(nums, target) << endl;

  return 0;
}
