#include <iostream>
#include <vector>

using namespace std;

vector<int> findDisappearedNumbers(vector<int> &nums) {
  int n = nums.size();
  vector<int> missing(n, 0);
  int m = missing.size();
  vector<int> ans;

  for (int i = 0; i < n; i++) {
    missing[nums[i] - 1]++; // updates a count at index nums[i] - 1
  }
  int m = missing.size();
  vector<int> ans;

  for (int i = 0; i < m; i++) {
    if (missing[i] == 0) {
      ans.push_back(i + 1); // adds one back because - 1 logic, and it was off
                            // by 1 in the answer
    }
  }
  return ans;
}
