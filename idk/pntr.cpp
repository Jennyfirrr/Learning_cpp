#include <iostream>

using namespace std;

int main() {
  int x = 5;
  int *p = &x;

  cout << "x = " << x << endl;
  cout << "p = " << p << endl;
  cout << "&x = " << &x << endl;
  cout << "*p = " << *p << endl;

  *p = 99;

  cout << "x after *p = " << x << endl;
}
