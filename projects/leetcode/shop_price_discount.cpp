#include <stack>
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

// second solution using the stack to actually answer this, on leetcode the
// above solution is faster than the one below this, but thats apparently
// because of the overhead that the pop and push operations add, and also on the
// website, the size is bounded to 1 -> 500 or something, in cases of like 10^6
// this should be faster in theory

vector<int> finalPrices(vector<int> &prices) {
  int n = prices.size();
  stack<int> price;
  vector<int> answer(n);

  for (int i = n - 1; i >= 0; i--) {
    while (!price.empty() && price.top() > prices[i]) {
      price.pop();
    }
    if (!price.empty()) {
      answer[i] = prices[i] - price.top();
    } else {
      answer[i] = prices[i];
    }
    price.push(prices[i]);
  }

  return answer;
}
