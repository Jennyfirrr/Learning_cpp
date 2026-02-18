#include <chrono>
#include <cmath>
#include <iostream>
#include <sys/ioctl.h>
#include <thread>
#include <unistd.h>

using namespace std;

int main() {
  float t = 0.0f;

  while (true) {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    int width = w.ws_col;
    int height = w.ws_row;

    cout << "\033[H";

    int wave[width];

    for (int i = 0; i < width; i++) {
      float y = 0.4f * sin(t + i * 0.2f) + 0.3f * sin(t * 0.7f + i * 0.1f) +
                0.1f * sin(t * 0.4f + i * 0.05f);
      wave[i] = (y + 1.0f) * (height / 2);
    }

    for (int row = 0; row < height; row++) {
      for (int col = 0; col < width; col++) {
        float d2 = sqrt((row - wave[col]) * (row - wave[col]));

        if (d2 < 0.5) {
          cout << "@";
        } else if (d2 < 1.0) {
          cout << "0";
        } else if (d2 < 1.5) {
          cout << "o";
        } else if (d2 < 2.0) {
          cout << ".";
        } else if (d2 < 2.5) {
          cout << ",";
        } else {
          cout << " ";
        }
      }
      cout << "\n";
    }
    t += 0.1f;
    this_thread::sleep_for(chrono::milliseconds(25));
  }
  return 0;
}
