#include <chrono>
#include <iostream>
#include <thread>

using namespace std;

int main() {
  int x = 0;
  int max_len = 50;
  int line_count = 0;

  while (true) {
    cout << "\r"; // return to start of line
    for (int i = 0; i < x; i++) {
      cout << " ";
    }

    cout << "poop";
    cout.flush();

    x++;
    if (x > max_len) {
      x = 0;
      max_len -= 5;
    }

    if (max_len == 0) {
      cout << "\n";
      line_count++;

      max_len = 50;
      x = 0;
    }

    this_thread::sleep_for(chrono::milliseconds(25));
  }

  return 0;
}
