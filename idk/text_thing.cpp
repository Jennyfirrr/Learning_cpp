#include <iostream>

using namespace std;

int main() {
  int a;
  int b;

  cout << "Pick how long the first list is: ";
  cin >> a;
  cout << "Pick how long the second list is: ";
  cin >> b;
  for (int i = 0; i < a; ++i) {
    for (int j = 0; j < b; ++j) {
      if (j == b - 1) {
        cout << j + 1 << endl;
        break;
      }
      cout << j + 1 << " ";
    }
  }
}
