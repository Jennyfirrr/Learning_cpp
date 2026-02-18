#include <iostream>
#include <vector>

using namespace std;

// i need to add the skip if same logic but im kinda lazy, ill get to it, for
// reference to that and how this works, check 3 sum

vector<vector<int>> pointe(const vector<int> &digits, int n) {

  vector<vector<int>> indexes;

  int target = n;

  int left = 0;
  int right = digits.size() - 1;

  if (digits.size() < 2) {
    return indexes;
  }

  while (left < right) {

    if (digits[left] + digits[right] < target) {
      left++;
    } else if (digits[left] + digits[right] > target) {
      right--;
    } else {
      indexes.push_back({left, right});
      right--;
      left++;
    }
  }

  return indexes;
}

int main() {
  vector<int> digits = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  int target = 10;

  auto result = pointe(digits, target);

  for (const auto &pair : result) {
    cout << pair[0] << ", " << pair[1] << "\n";
  }

  return 0;
}
