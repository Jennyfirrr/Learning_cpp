#include <iostream>
using namespace std;

int main() {
  double a = 0;
  double b = 0;
  char op;
  double result = 0;

  cout << "Please enter the first value: ";
  cin >> a;
  cout << "Enter operator(+ - / *): ";
  cin >> op;
  cout << "Please enter the second value: ";
  cin >> b;

  switch (op) {
  case '+':
    result = a + b;
    break;
  case '*':
    result = a * b;
    break;
  case '/':
    result = a / b;
    break;
  case '-':
    result = a - b;
    break;
  }

  cout << result << endl;
  return 0;
}
