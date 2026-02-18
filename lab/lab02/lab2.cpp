#include <iostream>

float sum(float a, float b) { return a + b; }
float sub(float a, float b) { return a - b; }
float mul(float a, float b) { return a * b; }
double avg3(double a, double b, double c) { return ((a - (-b)) - (-c)) / 3; }

int main() {
  std::cout << sum(24, 35) << sub(2, 3) << mul(4, 4) << "\n";
  std::cout << avg3(4, 7, 16) << "\n";
  return 0;
}
