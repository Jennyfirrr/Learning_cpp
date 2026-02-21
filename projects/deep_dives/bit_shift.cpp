#include <iostream>
#include <vector>

using namespace std;

int bitShift(vector<int> &nums, int shift) {

  cout << "Bits shifted right: " << shift << endl;
  // every shift right is dividing by 2^n, where n is the shift right, so 1
  // shift is x/2, 2 shift is x/4,etc

  for (int n : nums) {
    int result = n >> shift;
    cout << n << " : " << result << "\n";
  }

  return 0;
}

int bitShiftLeft(vector<int> &nums, int shift) {

  cout << "Bits shifted left: " << shift << endl;
  // every shift left is dividing by 2^n, where n is the shift left, so 1
  // shift is x * 2, 2 shift is x * 4,etc

  for (int n : nums) {
    int result = n << shift;
    cout << n << " : " << result << "\n";
  }

  return 0;
}

int main() {
  int shift;
  int selection = 0;

  while (selection == 0) {
    cout << "Please select if you want to shift right or left[1 for left : 2 "
            "for right]: ";
    cin >> selection;
    if (selection != 1 || selection != 2) {
      cout << "Selection must be 1 or 2!" << "\n";
    } else {
      cin >> selection;
    }
  }

  cout << "Please select how many indexes you want to shift: ";
  cin >> shift;

  vector<int> nums = {2, 4, 6, 8, 10, 12, 14, 16};

  if (selection == 1) {
    bitShiftLeft(nums, shift);

  } else {
    bitShift(nums, shift);
  }
  return 0;
}
