#include <iostream>
using namespace std;

int fib(int n) {
  if (n == 0) {
    return 0;
  }
  if (n == 1) {
    return 1;
  }

  int prev = 0;
  int curr = 1;
  for (int i = 2; i <= n; ++i) {
    int next = prev + curr;
    prev = curr;
    curr = next;
  }
  return curr;
}

int main() {
  int n = 0;

  cout << "Select how many iterations of the fibonacci sequences you want: ";
  cin >> n;

  cout << fib(n) << endl;
  return 0;
}
