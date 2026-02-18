#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

int binary_search(const vector<int> &nums, int target) {
  int left = 0;
  int right = nums.size() - 1;

  while (left <= right) {
    int mid = left +
              (right - left) /
                  2; // finds the middle by calculating the left pointer, with
                     // right - left, to find bounds depending on side, IE, if
                     // left 0 and right is the vector length, 0 + size / 2, etc

    if (nums[mid] == target) { // checks middle value
      return mid;
    } else if (nums[mid] <
               target) { // if middle value is less than the target, shifts left
                         // pointer to middle index + 1, to indicate right half
      left = mid + 1;
    } else { // if middle index is greater than target, shifts the search space
             // to left by making right pointer middle index - 1
      right = mid - 1;
    }
  }
  return left;
}
