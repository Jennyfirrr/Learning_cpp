#include <chrono>
#include <iostream>
#include <string>
#include <thread>

using namespace std;

int main() {
  int x = 0;
  int max_len = 100;
  bool moving_right = true;
  int line_count = 0;

  while (true) {
    cout << "\r";

    if (moving_right) {
      for (int i = 0; i < x; i++) {
        if (i % 2 == 0) {
          cout << "#";
        } else {
          cout << "|";
        }
      }

      cout.flush();
      x++;

      if (x > max_len) {
        moving_right = false;
        cout << "\n";
        line_count++;
      }
    } else {
      int count = 0;
      for (int i = 0; i < x - 2; i++) {
        cout << " ";
        count++;
      }

      if (count % 2 == 0) {
        cout << "|";
      } else {
        cout << "#";
      }

      cout.flush();

      x--;
      if (x < 0) {
        count = 0;
        moving_right = true;
        line_count++;
        cout << "\n";
      }
    }

    if (line_count == 19) {
      cout << "\033[2J\033[H";
      line_count = 0;
    }

    this_thread::sleep_for(chrono::milliseconds(25));
  }
  return 0;
}
