#include <iostream>
#include <list>
#include <string>
#include <vector>
using namespace std;

string oddOrEven(int a) {
  if (a % 2 == 0) {
    return "The number is even.";
  }

  return "The number is odd.";
}

struct paint {
  float length;
  float width;
  float cost;
  float discount;
};

float floorPaintingCost(paint &p, bool extraCoat) {
  float area = p.length * p.width;
  float areaprice = area * p.cost;
  float disc = 1 - (p.discount / 100);
  float total = areaprice * disc;
  if (extraCoat) {
    return total * 2;
  }
  return total;
}

vector<string> findPairsWithSum(vector<int> pairs, int sum) {
  vector<string> outputs;
  for (int i : pairs) {
    for (int j : pairs) {
      if (i + j == sum) {
        string str1 = to_string(i);
        string str2 = to_string(j);
        outputs.push_back("(" + str1 + "+" + str2 + ")");
      }
      if (i - j == sum) {
        string str1 = to_string(i);
        string str2 = to_string(j);
        outputs.push_back("(" + str1 + "-" + str2 + ")");
      }
    }
  }
  return outputs;
}

int main() {
  int sum = 0;
  vector<int> list1;
  int b = 0;
  cout << "Please enter a list length: ";
  cin >> b;
  for (int i = 1; i < b; ++i) {
    list1.push_back(i);
  }

  cout << "Please select a num to find pairs for: ";
  cin >> sum;
  vector<string> outputs = findPairsWithSum(list1, sum);
  for (size_t i = 0; i < outputs.size(); ++i) {
    cout << outputs[i] << endl;
  }
  cout << list1.size() << endl;
  cout << "Length of the list: " << b << endl;
}
