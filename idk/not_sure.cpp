#include <iostream>
#include <vector>
using namespace std;

vector<size_t> squares(size_t step, size_t endPoint) {
  vector<size_t> squareList;

  for (size_t i = step; i < endPoint; i += step) {
    vector<size_t> temp;
    if (step > 0) {
      size_t squareArea = step * step;
      temp.push_back(squareArea);
    } else {
      cout << "You cannot have a square with 0 length." << endl;
    }
  }
  return squareList;
}

int main() {
  size_t step;
  size_t endPoint;

  cout << "Please enter a step size: ";
  cin >> step;
  cout << "Please enter an endpoint: ";
  cin >> endPoint;

  vector<size_t> squaresList = squares(step, endPoint);

  for (size_t i = 0; i < squaresList.size(); ++i) {
    cout << squaresList[i];
  }
  return 0;
}
