#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

// First attempt using brute force and simulation for this problem

int generations(vector<int> layers) {
  int total_missing = 0;
  int gen_count =
      0; // count for how many 'generations' it has taken to subtract all values
         // from total_missing, including generations where nothing happens
  sort(layers.begin(), layers.end()); // could probably be removed to main to
                                      // make actual logic less time complex

  int max_layers = layers[layers.size() - 1]; // grab max value

  for (int i = 0; i < (int)layers.size() - 1;
       i++) { // calculate the total missing from all layers that are < max
    total_missing += max_layers - layers[i];
  }

  for (int i = 0; total_missing > 0;
       i++) { // calculate how many generations are needed to increase all
              // layers < max to the highest value
    if (i % 2 == 0) {
      if (total_missing - 1 <
          0) { // on odd generations same as explanation below
        gen_count++;
      } else {
        total_missing--;
        gen_count++;
      }
    } else { // on even generations, if subtracting the value from the total
             // missing results in a value < 0, skips and continues
      if (total_missing - 2 < 0) {
        gen_count++;
      } else {
        total_missing -= 2;
        gen_count++;
      }
    }
  }

  return gen_count;
}

int main() {
  vector<int> layers = {4, 3, 3, 1, 4, 3, 6, 7, 1, 2, 0, 10, 20, 3, 9};
  vector<int> layers2 = {4, 3, 3, 1};
  vector<int> layers3 = {10, 2, 4, 6, 8, 3, 1, 2, 11, 15};

  cout << "Layers1 : " << generations(layers) << endl;
  cout << "Layers2 : " << generations(layers2) << endl;
  cout << "Layers3 : " << generations(layers3) << endl;

  return 0;
}
