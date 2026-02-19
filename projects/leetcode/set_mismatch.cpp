#include <iostream>
#include <vector>

using namespace std;

vector<int> findErrorNums(const vector<int> &nums) {
  int n = nums.size();
  vector<int> v(n + 1, 0); // fills a vector of size n + 1 with 0's
  int missing = 0, duplicate = 0;

  for (int i = 0; i < n; i++) {
    v[nums[i]]++; // uses the vector v to count each num that appears in nums,
                  // using the index of v as the value, so that if a number is
                  // seen once, it is 1, if it is missing, it is 0, if it is
                  // duplicated, it is 2
  }

  for (int i = 0; i < v.size(); i++) {
    if (v[i] == 2) { // using the logc as laid out above, only 1 index will have
                     // the value of 2 at index i, based on the problem
      duplicate = i;
    }
    if (v[i] == 0) { // based on the logic from above, only 1 index will have a
                     // value of 0 at index i
      missing = i;
    }
  }

  return {duplicate, missing};
}
