#include <cmath>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

string myName() { return "Jennifer Lewis"; }

string myBlazerID() { return "jtlewis"; }

string sumDigitsPrime(int a) {
  int num = a;
  vector<int> digits;
  int total = 0;
  while (num > 0) {
    digits.push_back(num % 10);
    num /= 10;
  }
  for (int x : digits) {
    total += x;
  }
  int check = 2;
  double root1 = std::sqrt(total);
  int root = static_cast<int>(std::ceil(root1));
  while (check <= root) {
    if (total % check == 0) {
      return "Not prime.";
    }
    ++check;
  }
  return "Prime.";
}

int main() {
  int a = 0;
  cout << "my name is = ";
  cout << myName();
  cout << " any my BlazerID is = ";
  cout << myBlazerID() << "\n";
  cout << "Please enter an integer: ";
  cin >> a;
  cout << sumDigitsPrime(a) << endl;
}
