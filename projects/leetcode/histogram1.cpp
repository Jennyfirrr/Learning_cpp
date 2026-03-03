#include <stack>
#include <vector>

using namespace std;

int largestRectangleArea(vector<int> &heights) {
  heights.push_back(0);
  // push a 0 to the back, so that when it reaches the max, it calculates all
  // the areas
  int len = heights.size();
  stack<int> heights1;
  int ans = 0;
  heights1.push(-1);
  // push a -1 to initialize the stack

  for (int i = 0; i < len; i++) {
    while (heights1.top() != -1 && heights[heights1.top()] >= heights[i]) {
      int h = heights[heights1.top()];
      // sets the heights value to the index stored in the stack, so you can
      // just iterate through the vactor witout using a nested for loop
      heights1.pop();
      // pops the value from the last
      int w = i - heights1.top() - 1;
      // sets width = to i - heights - 1
      ans = max(ans, h * w);
      // calculates and stores the answers, and finds the largest based on the
      // parameters above
    }
    heights1.push(i);
  }
  return ans;
}
// not time optimal
