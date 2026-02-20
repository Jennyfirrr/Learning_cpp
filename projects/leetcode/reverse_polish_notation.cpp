// this one was frustrating and hard, i didnt even try to find the optimal and i
// think i spent like 2-3 hours trying to figure it out, and just looked up an
// answer, so im gonna go back and try to figure out why it works, this was also
// like one of the first ones i attempted, coming back to it after doing like
// ~20 more problems, its nowhere near as scary as before

int evalRPN(vector<string> &tokens) {

  stack<int> nums; // initialize stack here
  int i = 0;

  while (i < tokens.size()) {
    if (tokens[i] == "*" || tokens[i] == "+" || tokens[i] == "-" ||
        tokens[i] ==
            "/") { // set a condition where if the item in the tokens array
                   // matches an operation, it sets the int x to the top of the
                   // stack, then pops it and does the same for y, then
                   // depending on the operator that is found, it pushes the
                   // result of that to the top of the stack, then continues
      int x = nums.top();
      nums.pop();

      int y = nums.top();
      nums.pop();

      if (tokens[i] == "*") {
        nums.push(y * x);
      } else if (tokens[i] == "+") {
        nums.push(y + x);
      } else if (tokens[i] == "-") {
        nums.push(y - x);
      } else if (tokens[i] == "/") {
        nums.push(y / x);
      }

    } else {
      nums.push(stoi(
          tokens[i])); // this just converts everything from a string to int, i
                       // cant remember exactly why, but its important, right
                       // its because if the value of tokens[i] isnt an
                       // operator, then you need to convert the string that
                       // contains the number to an integer, because the
                       // problems wants you to return an integer, and its
                       // faster to return nums.top(), instead of storing
                       // everything in an uneeded intialized int
    }

    i++; // this just updates the int i at the top, to make the while loop
         // operate like a for loop, becausea for loop isnt the time optimal
         // solution here, i think if you use that you need like nested loops,
         // while this just becomes like O(n) or something instead of O(n^2), i
         // cant remember if nested loops even work for this one anyways
  }
  return nums.top();
}
