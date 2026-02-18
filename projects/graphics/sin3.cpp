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

    for (int row = 0; row < height; row++) {
      for (int col = 0; col < width; col++) {

        float cx = width / 2.0f;
        float cy = height / 2.0f;

        float dx = col - cx;
        float dy = row - cy;

        float d = sqrt(dx * dx + dy * dy);

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

        if (intensity > 0.85f) {
          cout << "\033[38;5;231m";
        } else {
          cout << "\033[38;5;" << color << "m";
        }

        const char *ramp = " .,:;irsXA253hMHGS#9B&@";
        int rampN = strlen(ramp);
        int idx = (int)lround(intensity * (rampN - 1));
        if (idx < 0)
          idx = 0;
        if (idx > rampN - 1)
          idx = rampN - 1;
        cout << ramp[idx];
      }
      cout << "\n";
    }
    cout << "\033[0m";
    t += 0.1f;
    this_thread::sleep_for(chrono::milliseconds(25));
  }
  cout << "\033[?25h";
  return 0;
}
