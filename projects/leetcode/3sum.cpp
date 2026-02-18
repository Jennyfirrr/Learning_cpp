#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

vector<vector<int>>
three_sum(const vector<int> &digits, int target, int clamp1,
          int clamp2) { // only works for sorted list because of the invarient
                        // specifying right always decreases values, and left
                        // always increases, if that doesnt hold true it breaks
  vector<vector<int>> indexes;

  if (clamp1 > clamp2) {
    swap(clamp1, clamp2);
    cout << "Swapping min and max clamps!" << endl;
  }

  for (int i = 0; i < (int)digits.size() - 2; i++) {
    if (i > 0 && digits[i] == digits[i - 1])
      continue;
    if (digits[i] < clamp1 ||
        digits[i] > clamp2) { // W I N S O R I Z E | i just wanted to add faux
                              // winsorization lol
      continue;
    }
    int left = i + 1;
    int right =
        (int)digits.size() - 1; // reintializes the pointers to search the rest
                                // of the values once i increases

    while (left < right) {
      while (left < right && (digits[left] > clamp2 ||
                              digits[left] < clamp1)) { // W I N S O R I Z E
        left++;
      }
      while (left < right && (digits[right] > clamp2 ||
                              digits[right] < clamp1)) { // W I N S O R I Z E
        right--;
      }
      if (left >= right) { // breaks the loop once the pointers cross eachother
                           // and returns to the outer loop
        break;
      }
      int result = digits[i] + digits[left] +
                   digits[right]; // standard 2 sum + i for a 3sum solution

      if (result <
          target) { // if the result is less than the target, moves the left
                    // pointer because the left pointer increases values
        left++;
      } else if (result > target) { // if result is greater than the target,
                                    // moves the right pointer because the right
                                    // pointer controls decreasing values
        right--;
      } else {
        indexes.push_back(
            {digits[i], digits[left],
             digits[right]}); // adds to the index list once a mathc is found
        left++;
        right--;

        while (left < right &&
               digits[left] ==
                   digits[left - 1]) { // checks if the last index for the left
                                       // pointer is the same as the current
                                       // index, and if so skips
          left++;
        }

        while (left < right &&
               digits[right] ==
                   digits[right + 1]) { // does the same as above, but for the
                                        // right pointer
          right--;
        }
      }
    }
  }
  return indexes; // returns a vector<vector<int>> so you can see every possible
                  // combination
}

int main() {
  vector<int> digits = {1, 2, 3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 2, 4,
                        6, 7, 8,  2,  3,  5,  1,  9,  0,  3,  2,  5,  6,  7, 8,
                        9, 9, 10, 20, 30, 20, 10, 6,  5,  4,  3,  7,  2,  1, 23,
                        4, 6, 7,  13, 2,  5,  7,  10, 29, 30, 25, 13};

  sort(digits.begin(), digits.end());

  int target = 0;

  int clamp1 = 0;
  int clamp2 = 0;

  size_t times_C = 50;

  cout << "Please select a target to find pairs of 3 for: ";
  cin >> target;

  cout << "Please select a minimum clamp: ";
  cin >> clamp1;

  cout << "Please select a maximum clamp: ";
  cin >> clamp2;

  auto result = three_sum(digits, target, clamp1, clamp2);

  cout << string(times_C, '=') << "\n";

  for (const auto &pair : result) {
    cout << "[" << pair[0] << "," << pair[1] << "," << pair[2] << "]" << "\n";
  }

  cout << string(times_C, '=') << "\n";

  cout << "Triplets found: ";
  cout << (int)result.size() << endl;

  return 0;
}
