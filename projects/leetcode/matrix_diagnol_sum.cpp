#include <iostream>
#include <vector>

using namespace std;

int diagnoal(const vector<vector<int>> &matrix) {
  int count = 0;
  int length = matrix[0].size() - 1; // grab last index value

  for (int i = 0; i < (int)matrix.size(); i++) {
    count += matrix[i][i];          // add the diagnoal slanted like this[ \ ]
    count += matrix[i][length - i]; // add the diagnoal slanted this way [ / ]
  }

  if (matrix.size() % 2 != 0) { // if it is an odd matrix(3x3) then remove the
                                // center ebcause it adds twice otherwise
    count -= matrix[matrix.size() / 2][matrix.size() / 2];
  }

  return count;
}

int main() {
  vector<vector<int>> matrix = {{1, 2, 3, 4, 5, 6}, {1, 2, 3, 4, 5, 6},
                                {1, 2, 3, 4, 5, 6}, {6, 4, 2, 1, 3, 5},
                                {5, 4, 2, 1, 5, 7}, {7, 6, 8, 9, 0, 1}};

  cout << "Total of diagnols is: " << diagnoal(matrix) << endl;

  return 0;
}
