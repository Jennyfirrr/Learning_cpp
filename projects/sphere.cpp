#include <cmath>
#include <ncurses.h>
#include <unistd.h>

using namespace std;

double sphereVol(double r) { return (4.0 / 3.0) * M_PI * r * r * r; }

int main() {

  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  nodelay(stdscr, TRUE);
  double volume = 0.0;
  double rad = 0.0;

  bool running = true;
  bool dirty = false;

  WINDOW *boxwin = newwin(9, 50, 0, 2);
  box(boxwin, 0, 0);

  while (running) {

    int ch = getch();
    if (ch != ERR) {
      if (ch == 'q' || ch == 'Q') {
        running = false;
      } else if (ch == ' ') {
        rad = 0.00;
        volume = 0.00;
      } else if (ch == 'e') {
        nodelay(stdscr, FALSE);
        echo();
        curs_set(1);
        volume = 0;
        rad = 0;

        mvwprintw(boxwin, 5, 3, "Enter radius: ");
        box(boxwin, 0, 0);
        wrefresh(boxwin);

        wscanw(boxwin, "%lf", &rad);

        volume = sphereVol(rad);

        noecho();
        curs_set(0);
        nodelay(stdscr, TRUE);
      }
    }

    mvwprintw(boxwin, 1, 3, "Volume finder for spheres - press q to quit ");
    mvwprintw(boxwin, 3, 3, "Radius: %.2f", rad);
    mvwprintw(boxwin, 4, 3, "Volume: %.2f", volume);
    mvwprintw(boxwin, 7, 3, "Press q to quit or e to enter a radius value");

    wrefresh(boxwin);
    usleep(15000);
  }
  endwin();
  return 0;
}
