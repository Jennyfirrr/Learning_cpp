#include <chrono>
#include <cmath>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/ioctl.h>
#include <thread>
#include <unistd.h>

using namespace std;

int main() {
  string pink = "\033[38;5;213m";
  string purple = "\033[38;5;141m";
  string navy = "\033[38;5;17m";
  string reset = "\033[0m";
  string clear = "\033[H";

  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

  const int width = w.ws_col;
  const int height = w.ws_row - 1;

  int wave[width];

  unsigned int frame = 0;

  while (true) {
    stringstream buffer;
    buffer << clear;

    for (int i = 0; i < width; i++) {
      wave[i] = height / 2 + sin(i * 0.2 + frame * 0.1) * 3;
    }

    for (int row = 0; row < height; row++) {
      for (int col = 0; col < width; col++) {
        float dist = sqrt((row - wave[col]) * (row - wave[col]));

        if (dist < 2) {
          buffer << pink << "█";
        } else if (dist < 4) {
          buffer << purple << "█";
        } else {
          buffer << navy << "█";
        }
      }
    }

    cout << buffer.str() << reset << flush;

    this_thread::sleep_for(chrono::milliseconds(25));
    frame++;
  }
  return 0;
}
