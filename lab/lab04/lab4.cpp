#include <iostream>
#include <list>
#include <string>
#include <vector>
using namespace std;

bool reverse(string s) {
  int i = s.size();
  string my_string = "";
  while (i > 0) {
    my_string.push_back(s[i - 1]);
    --i;
  }

  if (my_string == s) {
    return true;
  } else {
    return false;
  }
}

int count(string s) {
  int vowel = 0;
  int i = s.size();
  while (i > 0) {
    char c = s[i - 1];
    if (c == 'a' || c == 'e' || c == 'o' || c == 'i' || c == 'u') {
      ++vowel;
    }
    --i;
  }
  return vowel;
}

int max(int a, int b, int c) {
  if (a > b && a > c) {
    return a;
  } else if (b > a && b > c) {
    return b;
  } else {
    return c;
  }
}

int last(const list<int> &nums) {
  if (nums.size() == 0) {
    return 0;
  } else {
    return static_cast<int>(nums.size()) - 1;
  }
}

list<int> testing = {10, 20, 30, 40};

int main() {
  cout << last(testing) << "\n";
  cout << max(10, 20, 30) << "\n";
  cout << count("HereComesJhonny") << "\n";
  cout << reverse("daddy") << "\n";
}
