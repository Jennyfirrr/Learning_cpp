#include <iostream>
#include <unordered_set>
#include <vector>

using namespace std;

bool matrix_check(const vector<vector<int>> &matrix) {
  unordered_set<int>
      rows[100]; // easy to check for multiples using the count() functions
  unordered_set<int> cols[100];

  for (int i = 0; i < (int)matrix.size();
       i++) { // double for loop to check every j for i value
    for (int j = 0; j < (int)matrix[0].size(); j++) {
      int val = matrix[i][j];   // intializes the the integer to add to the
                                // appropraite unordered_set
      if (rows[i].count(val)) { // if the count is true, return false
        return false;
      }
      rows[i].insert(val); // assuming it hasnt been discovered yet, add it to
                           // the unordered set

      if (cols[j].count(val)) {
        return false;
      }
      cols[j].insert(val);
    }
  }
  return true;
}

int main() {
  vector<vector<int>> matrix = {{1, 2, 3}, {1, 2, 3}, {3, 2, 1}};
  vector<vector<int>> matrix1 = {{3, 2, 1}, {3, 2, 1}, {3, 2, 2}};
  vector<vector<int>> matrix2 = {{3, 1, 2}, {2, 3, 1}, {1, 2, 3}};

  cout << boolalpha << matrix_check(matrix) << endl;
  cout << boolalpha << matrix_check(matrix1) << endl;
  cout << boolalpha << matrix_check(matrix2) << endl;

  return 0;
}
