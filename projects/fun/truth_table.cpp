#include <iostream>

using namespace std;

bool eval_and(bool p, bool q) { return p && q; }

bool eval_or(bool p, bool q) { return p || q; }

bool eval_not(bool p) { return !p; }

bool eval_implies(bool p, bool q) { return !p || q; }

bool eval_iff(bool p, bool q) { return p == q; }

void print_truth_table() {
  cout << "p\tq\tAND\tOR\tp=>q\tp<=>q\n";
  cout << "--------------------------------------------------------\n";

  bool values[] = {true, false};

  for (bool p : values) {
    for (bool q : values) {
      cout << p << "\t" << q << "\t" << eval_and(p, q) << "\t" << eval_or(p, q)
           << "\t" << eval_implies(p, q) << "\t" << eval_iff(p, q) << "\n";
    }
  }
}

int main() {
  print_truth_table();
  return 0;
}
