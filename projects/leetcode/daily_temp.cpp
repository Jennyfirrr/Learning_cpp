#include <stack>
#include <vector>
using namespace std;

vector<int> dailyTemperatures(vector<int> &temperatures) {
  int l = temperatures.size();
  vector<int> answer(l);
  stack<int> temps;

  for (int i = 0; i < l; i++) {
    while (!temps.empty() && temperatures[temps.top()] < temperatures[i]) {
      answer[temps.top()] = i - temps.top();
      temps.pop();
    }
    temps.push(i);
  }
  return answer;
}
// this was my answer after having the 48 test case break the brute force
// solution
//
// optimal answer for studying later
vector<int> dailyTemperatures(vector<int> &temperatures) {
  vector<int> res(temperatures.size());
  for (int i = temperatures.size() - 1; i >= 0; --i) {
    int j = i + 1;
    while (j < temperatures.size() && temperatures[j] <= temperatures[i]) {
      if (res[j] > 0)
        j = res[j] + j;
      else
        j = temperatures.size();
    }
    if (j < temperatures.size())
      res[i] = j - i;
  }
  return res;
}

// i can already kinda tell hwy this is faster, i just hate doing these
