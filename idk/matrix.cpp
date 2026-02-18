#include <cmath>
#include <iostream>

unsigned long long matrix_multiply(int a) {
  int j = 1;
  for (int i = 1; i <= a; ++i) {
    j *= i;
  }
  return j;
}

int main() {
  int a = 0;
  std::cout << "Please enter how many numbers you want to print: ";
  std::cin >> a;
  std::cout << matrix_multiply(a) << std::endl;
}
