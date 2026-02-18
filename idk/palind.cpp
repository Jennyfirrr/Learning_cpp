#include <iostream>

using namespace std;

int reverseal(int x) {
  if (x < 0) {
    return 0;
  }

  int result = 0;

  while (x > 0) {
    int digit = x % 10;
    result = result * 10 + digit;
    x = x / 10;
  }
  return result;
}

int main() {
  int integer = 0;

  cout << "Please select a number to reverse: " << endl;
  cin >> integer;

  cout << reverseal(integer) << endl;
}
