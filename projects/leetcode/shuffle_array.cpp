#include <iostream>
#include <vector>

using namespace std;

vector<int> shuffle(const vector<int> &nums, int n) {
  vector<int> ans(
      2 * n); // if you know the size of the first array, precomputing the size
              // for the end array is faster than using push_back

  for (int i = 0; i < n; i++) {
    ans[i * 2] =
        nums[i]; // i* 2 for the index of the first values 0 = =, 1=2 2=4, etc
    ans[i * 2 + 1] =
        nums[n +
             i]; // i * 2 + 1 for the second half, so 0 = 1, 1 = 3, 2 = 5, n + i
                 // to grab the last half, becuase the length is always 2n
  }

  return ans;
}

int main() {
  vector<int> nums = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  int n = nums.size() / 2;

  vector<int> ans = shuffle(nums, n);

  for (int i = 0; i < (int)ans.size(); i++) {
    if (i == 0) {
      cout << "[ " << ans[0] << ", ";
    } else if (i == (int)ans.size() - 1) {
      cout << ans[ans.size() - 1] << " ]" << endl;
    } else {
      cout << ans[i] << ", ";
    }
  }

  return 0;
}
