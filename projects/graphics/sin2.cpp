#include <chrono>
#include <cmath>
#include <cstring>
#include <iostream>
#include <sys/ioctl.h>
#include <thread>
#include <unistd.h>

using namespace std;

int main() {
  float t = 0.0f;

  cout << "\033[?25l";

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
        float wobble = 2.0f * sin(t * 0.6f + row * 0.15f);

        float d = fabs(float(row) - (float(wave[col] + wobble)));

        float value = sin(d * 0.3f - t * 2.0f) + sin(col * 0.2f + t) +
                      sin(row * 0.15f - t);
        float intensity = (value / 3.0f + 1.0f) * 0.5f;

        int level = (int)lround(intensity * 5.0f);
        if (level < 0)
          level = 0;
        if (level > 8)
          level = 8;

        int r = 0;
        int g = level / 3;
        int b = level / 2 + 2;

        int color = 16 + 36 * r + 6 * g + b;

        cout << "\033[38;5;" << color << "m";

        if (d < 0.5f) {
          cout << "@";
        } else if (d < 1.5f) {
          cout << "o";
        } else if (d < 2.5f) {
          cout << ".";
        } else {
          cout << " ";
        }
      }
      cout << "\n";
    }
    cout << "\033[0m";
    t += 0.1f;
    this_thread::sleep_for(chrono::milliseconds(25));
  }
  return 0;
}
