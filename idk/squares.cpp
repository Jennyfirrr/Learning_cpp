#include <iostream>
#include <vector>
using namespace std;

vector<double> squaresArea(double step, double end, double length) {
  vector<double> squareAreasVector;

  for (double i = 0; squareAreasVector.size() < end; i += step) {
    double area = (length + i) * (length + i);
    squareAreasVector.push_back(area);
  }
  return squareAreasVector;
}

vector<double> squareAreaDiff(vector<double> area) {
  vector<double> areaDiffs;

  for (size_t i = 0; i < area.size(); ++i) {
    if (area[i] == 0) {
      areaDiffs.push_back(0);
    } else {
      double areaDiff = area[i] - area[i - 1];
      areaDiffs.push_back(areaDiff);
    }
  }
  return areaDiffs;
}

vector<double> areaByDiff(vector<double> areaList, vector<double> areaDiffs) {
  vector<double> areasByDiffs;

  for (size_t i = 0; i < areaList.size(); ++i) {
    double interesting = (areaDiffs[i] / areaList[i]) * 100;
    areasByDiffs.push_back(interesting);
  }
  return areasByDiffs;
}

vector<double> diffGrowth(vector<double> areaDiffs) {
  vector<double> diffDiff;

  for (size_t i = 0; i < areaDiffs.size(); ++i) {
    if (i == 0) {
      diffDiff.push_back(0);
    }
    double diffy = areaDiffs[i] - areaDiffs[i - 1];
    diffDiff.push_back(diffy);
  }
  return diffDiff;
}

vector<double> diffDivDiff(vector<double> areaDiffs, vector<double> diffDiff) {
  vector<double> weird;

  for (size_t i = 0; i < areaDiffs.size(); ++i) {
    double crazy = areaDiffs[i] / diffDiff[i];
    weird.push_back(crazy);
  }
  return weird;
}

int main() {
  double step = 0;
  double end = 0;
  double length = 0;

  cout << "________________________________" << endl;
  cout << "Square math" << endl;
  cout << "________________________________" << endl;

  while (step == 0) {
    cout << "Please enter a step size: ";
    cin >> step;
    if (step == 0) {
      cout << "Step cannot be 0." << endl;
    }
  }
  while (end == 0) {
    cout << "Please enter how many iterations: ";
    cin >> end;
    if (end == 0) {
      cout << "Iterations cannot be 0." << endl;
    }
  }
  while (length == 0) {
    cout << "Please enter the length of the squares side: ";
    cin >> length;
    if (length == 0) {
      cout << "Length cannot be 0." << endl;
    }
  }
  cout << "________________________________" << endl;

  vector<double> areaList = squaresArea(step, end, length);
  vector<double> areaDiffs = squareAreaDiff(areaList);
  vector<double> areaDivDif = areaByDiff(areaList, areaDiffs);
  vector<double> diffDiff = diffGrowth(areaDiffs);
  vector<double> interesting = diffDivDiff(areaDiffs, diffDiff);
  double totalArea = 0;
  double totalDiff = 0;

  for (size_t i = 0; i < areaList.size(); ++i) {
    cout << "________________________________" << endl;
    cout << "Iteration: " << i + 1 << endl;
    cout << "________________________________" << endl;
    cout << "Area: ";
    cout << areaList[i] << endl;
    cout << "Area differences: ";
    cout << areaDiffs[i] << endl;
    cout << "Diff diffs: " << diffDiff[i] << endl;
    cout << "Area diff / diffDiff: " << interesting[i] << endl;
    cout << "Area / diff: ";
    cout << areaDivDif[i] << "%" << endl;
    totalArea += areaList[i];
    totalDiff += areaDiffs[i];
  }

  cout << "________________________________" << endl;
  cout << "Finals" << endl;
  cout << "________________________________" << endl;
  cout << "Step size: " << step << endl;
  cout << "Total iterations: " << end << endl;
  cout << "Side length: " << length << endl;
  cout << "________________________________" << endl;
  cout << "Total area: " << totalArea << endl;
  cout << "Total area diffs: " << totalDiff << endl;
  cout << "Total area diff / iterations: " << totalDiff / end << endl;
  cout << "Total area / total diff: " << totalArea / totalDiff << endl;
  cout << "________________________________" << endl;
  cout << "________________________________" << endl;
}
