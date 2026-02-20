#include <iostream>
#include <vector>

using namespace std;

// i first thought i needed a nested for loop, but then realized i just needed a
// counter instead before i submitted

vector<string> buildArray(vector<int> &target, int n) {
  vector<string> operations;
  int j = 0; // counter to match the size of target, so like, when a match is
             // found at i + 1, it updates, and when i thits the size of the
             // vector target, the loops breaks

  for (int i = 0; i < n; i++) {
    if (i + 1 == target[j]) {
      operations.push_back("Push");
      j++;
    } else {
      operations.push_back("Push"); // if i isnt a match with the associated
                                    // index in target, then it pushes and pops,
                                    // because thats what the problem said
      operations.push_back("Pop");
    }
    if (j ==
        target.size()) { // once j reaches the size of the target array, the
                         // loop ends because it will have the ideal stack
                         // operations to build the stack the problem wanted
      break;
    }
  }
  return operations;
}
