#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

vector<int> convert(int number) {
  int n = number, remainder = 0;
  vector<int> binary;

  while (n > 0) {
    remainder =
        n %
        2; // grabs the remainder and pushes it back to the vector for storage
    binary.push_back(remainder);
    n = n / 2; // divides n by 2 to find the next number to find the remainer
               // for
  }

  return binary;
}

int main() {
  int number;

  cout << "Please choose a number to convert to binary: ";
  cin >> number;

  vector<int> conversion = convert(number);
  reverse(conversion.begin(),
          conversion
              .end()); // reverses it becuase its top down, instead of bottom up

  for (int i = 0; i < (int)conversion.size(); i++) {
    if (i % 7 == 0) { // mod 7 because 0 is actually 1, and every mod 7 r 0 is
                      // the 8th place
      cout << " " << conversion[i];
    }
    cout << conversion[i];
  }

  cout << "\n";

  return 0;
}
