#include <chrono>
#include <fstream>
#include <ncurses.h>
#include <vector>

using namespace std;

int main() {

  ofstream log("Math_app.log", ios::app);
  log << "Started\n";

  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  nodelay(stdscr, TRUE);
  curs_set(0);

  int last_key = 0;
  int counter = 0;

  bool running = true;
  while (running) {
    int ch = getch();
    if (ch != ERR) {
      last_key = ch;
      if (ch == 'q' | ch == 'Q') {
        log << "Quit requested.\n";
        running = false;
      } else if (ch == ' ') {
        counter = 0;
        log << "Counter reset\n";
      } else if (ch == 'j') {
        counter -= 1;
        log << "Counter [-1]\n";
      } else if (ch == 'k') {
        counter += 1;
        log << "Counter [+1]\n";
      }
    }
    clear();

    mvprintw(0, 0, "TUI volume math - press q to quit");
    mvprintw(2, 0, "Showing results for index position: %d", counter);

    mvprintw(5, 0, "Keys: q=quit, space=reset, j/k=[-1/+1 index]");
  }

  endwin();
  log << "Exited cleanly\n";
  return 0;
}
