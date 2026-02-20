// THe optimal solution

bool isOp(const string &s) {
  return s == "+" || s == "-" || s == "*" || s == "/";
}

int evalRPN(vector<string> &tokens) {
  vector<int> stack;

  for (int i = 0; i < tokens.size();
       i++) { // i guess i was wrong about the for loop being bad, this one
              // apparently uses it with a seperate function to check if the
              // string is an operator, basic seperation of concerns
    if (isOp(tokens[i])) { // uses the other function to check instead of the
                           // way i did
      int n2 = stack.back();
      stack.pop_back();
      int n1 = stack.back();
      stack.pop_back();

      char op = tokens[i][0];
      switch (
          op) { // im guessing switch case is faster than using long if/else
                // if loops, im not sure, i looked it up and switch case is
                // converted to a jump table by the compiler which is
                // apparently faster than if/else chains, similar to how using a
                // struct to wrap something like a double price, is better than
                // just slapping double price into what a function needs,
                // because the compiler just sees it as double 'value', and with
                // the struct, the compiler sees it as Price 'value'
      case '*':
        stack.push_back(n1 * n2);
        break;
      case '/':
        stack.push_back(n1 / n2);
        break;
      case '+':
        stack.push_back(n1 + n2);
        break;
      case '-':
        stack.push_back(n1 - n2);
        break;
      }
    } else {
      stack.push_back(stoi(tokens[i])); // same as what i did
    }
  }

  return stack.back();
}
