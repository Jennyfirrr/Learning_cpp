#include <fstream>

#include <iostream>
#include <ncurses.h>
#include <string>
#include <vector>
using namespace std;

vector<double> squareVol(double length, int iteration, double step) {
  vector<double> volList;
  double step_size = 0;

  for (int i = 0; i < iteration; ++i) {
    double length_step = length + step_size;
    step_size += step;
    double vol = (length_step * length_step) * length_step;
    volList.push_back(vol);
  }
  return volList;
}

vector<double> volDiff(const vector<double> &volumes) {
  vector<double> volDiffs;

  for (size_t i = 0; i < volumes.size(); ++i) {
    if (i == 0) {
      volDiffs.push_back(0);
    } else {
      double diff = volumes[i] - volumes[i - 1];
      volDiffs.push_back(diff);
    }
  }
  return volDiffs;
}

vector<double> diffByVol(const vector<double> diffs,
                         const vector<double> &volumes) {
  vector<double> diffDivideArea;

  for (size_t i = 0; i < diffs.size(); ++i) {
    if (diffs[i] == 0) {
      diffDivideArea.push_back(0);
    } else {
      diffDivideArea.push_back((diffs[i] / volumes[i]) * 100);
    }
  }
  return diffDivideArea;
}

double ui(string varName) {
  double variableValue = 0;

  while (variableValue == 0) {
    cout << "Please choose a number for " << varName << ": ";
    cin >> variableValue;
    if (variableValue == 0) {
      cout << "The value must be > 0." << endl;
    }
  }
  return variableValue;
}

void printLoop(const vector<double> &vols, const vector<double> &diffs,
               const vector<double> diffDivide, int counter) {
  const size_t size = vols.size();
  int i = counter;

  cout << "_____________________________________________" << endl;
  cout << "Length: " << i + 1 << endl;
  cout << "_____________________________________________" << endl;
  cout << "Volume: ";
  cout << vols[i] << endl;
  cout << "Volume diff[curr - prev]: ";
  cout << diffs[i] << endl;
  cout << "Diff / volume(%): " << diffDivide[i] << "%" << endl;
  cout << "_____________________________________________" << endl;
}

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
  // inputs
  //
  //

  int counter = 0;
  double length = 0;
  double stepsize = 0;
  double volumes = 0;

  bool running = true;
  while (running) {
    // vectors

    auto ch = getch();
    if (ch != ERR) {
      if (ch == 'q' | ch == 'Q') {
        log << "Quit requested\n";
        running = false;
      } else if (ch == ' ') {
        counter++;
        log << "Press space for the next value!\n";
      }
      while (volumes == 0) {
        cout << "Please enter a volume count: ";
        cin >> volumes;
        if (ch > 0) {
          volumes += ch;
        }
      }
      while (length == 0) {
        cout << "Please enter a length: " << length;
      }
      while (stepsize == 0) {
        cout << "Please enter a step size: " << stepsize;
      }
    }
    if (length != 0 && volumes > 0 && stepsize > 0) {
      const auto squarevols = squareVol(length, volumes, stepsize);
      const auto volumeDiffs = volDiff(squarevols);
      const auto diffDiv = diffByVol(volumeDiffs, squarevols);
    }

    clear();

    if (has_colors())
      attron(COLOR_PAIR(1));
    mvprintw(0, 0, "Volume math");
    if (has_colors())
      attroff(COLOR_PAIR(1));

    mvprintw(2, 0, "Volume of length: %f", length);

    refresh();
  }
  endwin();
  log << "Exited cleanly\n";
  // print loop

  return 0;
}
