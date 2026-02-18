#include <iostream>
#include <list>
#include <string>

using namespace std;

auto even(const list<int> &nums) {
  list<int> my_list;
  for (int num : nums) {
    if (num % 2 == 0) {
      my_list.push_back(num);
    } else {
      continue;
    }
  }
  return my_list;
}

long factorial(int a) {
  if (a == 1 || a == 0) {
    return 1;
  }

  for (int i = 2; i <= a; i++) {
    int j =
  }
}

list<int> numbers = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

int main() {
  for (int i : even(numbers)) {
    cout << i << "\n";
  }
  cout << factorial(6) << "\n";
}
