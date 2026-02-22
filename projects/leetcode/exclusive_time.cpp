#include <stack>
#include <string>
#include <vector>

using namespace std;

vector<int> exclusiveTime(int n, vector<string> &logs) {
  stack<int> st;
  vector<int> result(n, 0);
  int prev_time = 0;
  // initializes everything

  for (auto &log1 : logs) {

    int first_colon = log1.find(':');
    // finds the first colon, and grabs the index value of it
    int second_colon = log1.find(':', first_colon + 1);
    // finds the second colon and grabs the index value of it, but starts at the
    // index of the first colon + 1

    int func_id = stoi(log1.substr(0, first_colon));
    // this grabs which function is starting/ending because the first vlaue was
    // the function ID
    string type = log1.substr(first_colon + 1, second_colon - first_colon - 1);
    // this grabs if a function was starting or ending, based on the if/else
    // statements below
    int timestamp = stoi(log1.substr(second_colon + 1));
    // grabs the time stamp at the end after the second colon

    if (type == "start") {
      if (!st.empty()) {
        result[st.top()] += timestamp - prev_time;
        // if the stack isnt empty, this makes the result vector at the id of
        // the value in the stack adding the timestamp - the previous time, to
        // get how long it had been running
      }
      st.push(func_id);
      // if the stack is empty, it pushes the function id to the stack, the sets
      // the prev_time, to the timestamp of the function ID
      prev_time = timestamp;
    } else {
      result[st.top()] += timestamp - prev_time + 1;
      // if the string == end, then this sets result index = to the top of the
      // stack to the timestamp - prevtime + 1, because it adds + 1 even when
      // ending, because it runs the entire block
      st.pop();
      // pops to clear
      prev_time = timestamp + 1;
      // then sets the prev time to the timestamp + 1 so the next iteration is
      // ready
    }
  }

  return result;
}

// this uses the stack to store the function ID's as a way to control which
// vector index gets added to
