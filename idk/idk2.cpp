#include <iostream>
#include <vector>

using namespace std;

int value(int a) {
  int total = 0;

  for (int i = 0; i < a; ++i) {
    if (i / 2 == 0) {
      total += i + 1;
    } else {
      total += i * 2;
    }
  }
  return total;
}

int main() {
  int a;

  cout << "Pick a number: ";
  cin >> a;
  cout << value(a) << endl;
}
