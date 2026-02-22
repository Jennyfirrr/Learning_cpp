#include <vector>
using namespace std;

// i literally cannot think after doing the dives into bit masking lol, wtf,
// like my brain is like BUT NO BINARY WTF :(, i was procrastinating because i
// hit a medium leetcode and got frustrated lol, atleast i learned about
// branchless programming and stuff ig, idk, it was hella fun

vector<int> finalPrices(vector<int> &prices) {
  int n = prices.size();
  vector<int> answer(n);

  for (int i = 0; i < (int)prices.size(); i++) {
    answer[i] = prices[i];
    // sets the index of answer at i to the value of prices at index i
    for (int j = i + 1; j < n; j++) {
      if (prices[j] <= prices[i]) {
        // if the price of j is less than or = to the price of i, sets the index
        // of anser[i] to the result of price[i] - price[j]
        answer[i] = prices[i] - prices[j];
        break;
      }
    }
  }

  return answer;
}
