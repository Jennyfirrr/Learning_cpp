#include <cmath>
#include <iostream>
using namespace std;

double height(double h) { return h * h; }

double bmi(double a, double b) { return 703 * (a / b); }

int multi(int a) {
  int term = 0;
  int sum = 0;

  for (int i = 0; i < 3; ++i) {
    term = term * 10 + a;
    sum += term;
  }
  return sum;
}

bool grader(float a) {
  if (a < 70) {
    return false;
  } else {
    return true;
  }
}

double volcone(double r, double h) {
  double r2 = r * r;
  double x = M_PI;
  double y = h / 3;
  return r2 * x * y;
}

int main() {
  cout << volcone(2, 2) << "\n";
  cout << grader(69) << "\n";
  cout << multi(5) << "\n";
  cout << bmi(150, height(72)) << "\n";
}
