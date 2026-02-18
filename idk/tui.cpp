#include <chrono>
#include <fstream>
#include <ncurses.h>
#include <thread>

using namespace std;

int main() {

  ofstream log("app.log", ios::app);
  log << "Started\n" << endl;

  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  nodelay(stdscr, TRUE);
  curs_set(0);

  if (has_colors()) {
    start_color();
    use_default_colors();
    init_pair(1, COLOR_CYAN, -1);
  }

  int counter = 0;
  int last_key = -1;

  bool running = true;
  while (running) {

    int ch = getch();
    if (ch != ERR) {
      last_key = ch;
      if (ch == 'q' || ch == 'Q') {
        log << "Quit requested\n";
        running = false;
      } else if (ch == ' ') {
        counter = 0;
        log << "Counter reset (space)\n";
      } else if (ch == KEY_UP) {
        counter += 10;
        log << "Counter += 10 (up)\n";
      } else if (ch == KEY_DOWN) {
        counter -= 10;
        log << "Counter -= 10 (down)\n";
      }
    }
    counter++;

    clear();

    if (has_colors())
      attron(COLOR_PAIR(1));
    mvprintw(0, 0, "TUI demo (ncurses) - press q to quit");
    if (has_colors())
      attroff(COLOR_PAIR(1));

    mvprintw(2, 0, "Counter: %d", counter);
    mvprintw(3, 0, "Last key code: %d", last_key);
    mvprintw(5, 0, "Keys: q=quit, space=reset, up/down=+/-10");

    refresh();

    this_thread::sleep_for(chrono::milliseconds(16));
  }
  endwin();
  log << "Exited cleanly\n";
  return 0;
}
