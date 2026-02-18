#include <iostream>

using namespace std;

long fib(long seed) {

  long a = 0;
  long b = 1;

  for (long i = 0; i < seed; ++i) {
    cout << (i + 1) << " : " << a << endl;
    cout << "__________________" << endl;

    long next = a + b;
    a = b;
    b = next;
  }

  cout << "~~Done!~~" << endl;
  cout << "__________________" << endl;
  return 0;
}

int main() {
  long a = 0;

  cout << "__________________" << endl;
  cout << "Please choose a number: ";
  cin >> a;
  cout << "__________________" << endl;

  fib(a);
}
