#include <algorithm>
#include <iostream>

using namespace std;

bool isBadVersion(int n) { // short test
  if (n >= 10) {           // change this value to test different values
    return true;
  } else {
    return false;
  }
}

int isBad(int search_space) {
  int left = 1;
  int right = search_space; // cap search space to n

  while (left < right) {
    int mid = left + (right - left) / 2; // find middle
    if (isBadVersion(mid)) { // if mid is bad, set right to middle, then repeat
      right = mid;
    } else {
      left = mid + 1; // if mid is not bad, set left to mid + 1, then repeat
                      // O(log n) complexity
    }
  }
  return left;
}

int main() {
  cout << "The bad version is: " << isBad(10000) << endl;

  return 0;
}
