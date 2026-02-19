#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

vector<int> convert(int number) {
  int n = number, remainder = 0;
  vector<int> binary;

  while (n > 0) {
    remainder = n % 2;
    binary.push_back(remainder);
    n = n / 2;
  }

  return binary;
}

int main() {
  int number;

  cout << "Please choose a number to convert to binary: ";
  cin >> number;

  vector<int> conversion = convert(number);
  reverse(conversion.begin(), conversion.end());

  for (int i = 0; i < (int)conversion.size(); i++) {
    cout << conversion[i];
  }

  cout << "\n";

  return 0;
}
