#include <iostream>
#include <vector>
using namespace std;

vector<double> ratioFinder(int a) {
  vector<double> ratios{};
  int b = a;
  int z = 1;
  int y = 0;

  while (z <= b) {
    vector<int> sums{};

    a = z;
    for (int x = a; x > 0; x--) {
      for (int j = x; j > 0; j--) {
        y += j;
        if (j == 1) {
          sums.push_back(y);
          y -= y;
        }
      }
    }
    double total = 0;

    for (int x : sums) {
      total += x;
    }

    double ratio = total / a;
    ratios.push_back(ratio);
    z++;
  }
  return ratios;
}

vector<double> deltaRatios(vector<double> ratios, int x) {
  vector<double> ratioDiff{};
  for (size_t i = 0; i + x < ratios.size(); i += x) {
    double delta_calc = ratios[i + x] - ratios[i];
    ratioDiff.push_back(delta_calc);
  }
  return ratioDiff;
}

int main() {
  int a;
  int x;

  cout << "Please enter how many ratios for sums to layers you want to see: ";
  cin >> a;
  cout << "\nEnter the step count for the deltas: ";
  cin >> x;

  auto ratios = ratioFinder(a);
  auto diffs = deltaRatios(ratios, x);
  for (size_t i = 0; i < diffs.size(); ++i) {
    cout << diffs[i] << "\n";
  }
  return 0;
}
