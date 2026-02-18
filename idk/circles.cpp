#include <cmath>
#include <iostream>
#include <vector>
using namespace std;

vector<double> areaCirc(double step, double limit) {
  vector<double> areas{};
  double pi = acos(-1.0);

  for (double radius = step; radius < limit; radius += step) {
    double area = pi * (radius * radius);
    areas.push_back(area);
  }
  return areas;
}

vector<double> areaDelta(vector<double> areaList, int indexStep) {
  const auto &list = areaList;
  vector<double> areaDelta{};

  for (size_t i = 0; i + indexStep < list.size(); ++i) {
    double delta = (list[i] - list[i - indexStep]);
    areaDelta.push_back(delta);
  }
  return areaDelta;
}

vector<double> delta_diff(vector<double> deltas, int indexStep) {
  const auto &deltaList = deltas;
  vector<double> deltaDeltas{};

  for (size_t i = 0; i + indexStep < deltaList.size(); ++i) {
    double x = (deltaList[i + indexStep] - deltaList[i]);
    deltaDeltas.push_back(x);
  }
  return deltaDeltas;
}

vector<double> curious(vector<double> deltaDiffs, double step, int indexStep) {
  vector<double> interesting{};
  const auto &difff = deltaDiffs;
  double h = indexStep * step;

  for (size_t i = 0; i + indexStep < difff.size(); ++i) {
    double x = difff[i] / (h * h);
    interesting.push_back(x);
  }
  return interesting;
}

vector<double> ui(vector<double> x, vector<double> y, vector<double> z,
                  vector<double> u, int indexStep) {
  const auto &deltas = x;
  const auto &areaList = y;
  const auto &deltaDiffs = z;
  const auto &twoPi = u;
  vector<double> both{};

  double totalArea = 0;
  for (size_t i = 0; i < twoPi.size(); ++i) {
    totalArea += areaList[i + indexStep];
  }
  double lz = 0;

  for (size_t i = 0; i + indexStep < twoPi.size(); ++i) {
    cout << "______________________________________" << endl;
    cout << "Circle Area: " << areaList[i] << endl;
    cout << "Area Deltas[curr - prev]: " << deltas[i] << endl;
    cout << "Delta diffs: " << deltaDiffs[i] << endl;
    cout << "Its always pi lol: " << twoPi[i] / 2 << endl;
    cout << "Delta diffs / always pi: " << (deltaDiffs[i] / deltas[i]) * 100
         << "%" << endl;
    ++lz;
  }

  double averageArea = totalArea / lz;
  both.push_back(averageArea);
  both.push_back(lz);

  return both;
}

int main() {
  double step;
  double limit;
  int indexStep;

  cout << "______________________________________" << endl;
  cout << "Please select a step size: ";
  cin >> step;
  cout << "Please select a limit to stop at: ";
  cin >> limit;
  cout << "Please select an index step count for deltas: ";
  cin >> indexStep;
  cout << "______________________________________" << endl;

  auto areaList = areaCirc(step, limit);
  auto deltas = areaDelta(areaList, indexStep);
  auto deltaDiffs = delta_diff(deltas, indexStep);
  auto twoPi = curious(deltaDiffs, step, indexStep);

  ui(deltas, areaList, deltaDiffs, twoPi, indexStep);

  auto bothList = ui(deltas, areaList, deltaDiffs, twoPi, indexStep);
  double zl = bothList[1];

  cout << "______________________________________" << endl;
  cout << "Average area: " << bothList[0] << endl;
  cout << "Smallest area: " << areaList[zl - (zl - 1)] << endl;
  cout << "Largest area: " << areaList[zl] << endl;
  cout << "______________________________________" << endl;

  return 0;
}
