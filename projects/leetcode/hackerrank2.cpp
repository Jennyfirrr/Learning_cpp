#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

// second attempt at the same problem as specified in hacckerrank.cpp, but using
// a more optimal time complex solution

int generations(vector<int> layers) {
  if (layers.empty()) // edge case for empty array
    return 0;

  sort(layers.begin(), layers.end()); // once again could be handled in main or
                                      // presorting the array

  int max_layers = layers.back(); // same as the other one
  int total_missing = 0;

  for (int i = 0; i < (int)layers.size() - 1; i++) {
    total_missing +=
        max_layers - layers[i]; // calculates the total missing needed to max
                                // all layers the same
  }

  if (total_missing == 0)
    return 0;

  int complete_cycles =
      total_missing /
      3; // math solution, first divides the total_missing by 3, because for
         // every odd + even generation (3:1 + 4:2) the outcome is 3, so you
         // first find how many generations fit with that since the edge cases
         // appear at the end always
  int remainder = total_missing % 3; // finding the reamainder assuming that the
                                     // integer division isnt perfect

  if (remainder ==
      0) { // this handles the edge cases for arrays like {4,3,3,1} where you
           // need 5 generations to fill it out, and skipping the 4th because it
           // would cause an overlfow like {4,4,4,5}
    return complete_cycles *
           2; // multiply complete cycles by 2, beacuse its 2 generations being
              // computed for the division by 3
  } else if (remainder == 1) {
    return complete_cycles * 2 +
           1; // a remainder of 1 indiciates that it needs an odd generation,
              // due to the above, it will always be an even gen, so you just
              // add one
  } else {
    return complete_cycles * 2 +
           3; // other wise it takes another 2 generations
              // because of the way the math works, like if a remainder of 2
              // happens, you have to skip the generation after the + 1, because
              // it would over flow, and if it ends on needing a + 2 at the end,
              // its handled by perfect integer division
  }
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
