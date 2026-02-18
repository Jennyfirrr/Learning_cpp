#include <iostream>

using namespace std;

int main() {
  int a = 0;
  int b = 0;
  int *x = &a;
  int *y = &b;

  cout << "Please enter how long both lists will be: ";
  cin >> a;

  for (int i = 0; i < *x; ++i) {
    *y = 2 * i;
    cout << "____________________________________" << endl;
    cout << "Value: " << *y << endl;
    cout << "Memory address: " << y << endl;
    cout << "____________________________________" << endl;
  }
  return 0;
}
