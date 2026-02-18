#include <iostream>
using namespace std;

int fib(int a) {
  if (a == 1) {
    return 1;
  }
  if (a == 0) {
    return 0;
  }
  if (a > 1) {
    int n = 0;
    n += fib(a) - fib(a - 1);

    return fib(n);
  }
  return 0;
}

int main() {
  int a = 0;
  cout << "Please select a number: " << endl;
  cin >> a;
  cout << fib(a);
}
