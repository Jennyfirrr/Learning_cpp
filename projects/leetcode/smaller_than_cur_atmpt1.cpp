#include <iostream>
#include <vector>

using namespace std;

// first leetcode attempt without looking at optimal solution, faster than 68%

vector<int> smallerNumbersThanCurrent(vector<int> &nums) {
  int size = (int)nums.size();
  vector<int> ans;

  for (int i = 0; i < size; i++) {
    int count = 0; // set a counter, counter resets to 0 for every i
    for (int j = 0; j < size; j++) {
      if (nums[i] > nums[j] &&
          nums[i] != nums[j]) { // update the count when the conditions are met,
                                // then push back to the vector after the loop
        count++;
      }
    }
    ans.push_back(count);
  }
  return ans;
}

int main() {
  vector<int> nums = {8, 0, 1, 2, 3, 5, 7, 42, 4, 6, 8, 4, 2, 46, 0};

  vector<int> answer = smallerNumbersThanCurrent(nums);

  for (int i = 0; i < (int)answer.size(); i++) {
    if (i == 0) {
      cout << "[" << answer[i] << ",";
    } else if (i == (int)answer.size() - 1) {
      cout << answer[i] << "]" << "\n";
    } else {
      cout << answer[i] << ",";
    }
  }
  return 0;
}
