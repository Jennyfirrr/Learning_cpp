#include <iostream>
#include <vector>

using namespace std;

// fastest solution on leetcode, learning why its faster and how it works

vector<int> smallerNumbersThanCurrent(vector<int> &nums) {
  int n = nums.size();
  vector<int> count(101, 0);

  for (int x :
       nums) { // update the count +1 at index x, for each value within nums
    count[x]++;
  }

  for (int i = 1; i < 101;
       i++) { // for each item in count, updates the one at i with the total
              // from the value at i - 1, because it will only have 1 per time a
              // number is seen, so like [1,4,3,2,1] would be 1 0, 4:1, 3:2 2:3,
              // 1:4, and by coutning up per the value there you get a total sum
              // for everything smaller
    count[i] += count[i - 1];
  }

  vector<int> ans;
  for (int x : nums) {
    if (x == 0) // nothing is smaller than 0 when i > -1
      ans.push_back(0);
    else
      ans.push_back(count[x - 1]);
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
