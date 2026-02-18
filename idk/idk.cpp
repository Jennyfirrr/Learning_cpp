#include <iostream>
#include <vector>
using namespace std;

vector<int> random(int a) {
  vector<int> nums{};
  for (int i = 0; i < a; ++i) {
    nums.push_back(i + 1);
  }
  return nums;
}

int main() {
  size_t a;

  cout << "Pick a number: ";
  cin >> a;
  vector<int> nums = random(a);
  for (size_t i = 0; i < nums.size(); ++i) {
    for (size_t j = 0; j <= i; ++j) {
      if (j == i) {
        cout << j + 1 << endl;
      } else {
        cout << j + 1 << " ";
      }
    }
  }
}
