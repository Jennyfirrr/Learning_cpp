#include <iostream>

using namespace std;

float calc(float a, float b, char oper) {

  if (oper == '+') {
    return a + b;
  } else if (oper == '-') {
    return a - b;
  } else if (oper == '/') {
    return a / b;
  } else if (oper == '*') {
    return a * b;
  } else {
    return 0.0;
  }
}

int main() {
  float a = 0;
  float b = 0;
  char op;

  cout << "Please pick a first number: ";
  cin >> a;
  cout << "Please pick a second number: ";
  cin >> b;
  cout << "Please pick an oper : ";
  cin >> op;

  cout << calc(a, b, op) << endl;

  return 0;
}
