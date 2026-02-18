#include <chrono>
#include <cmath>
#include <iostream>
#include <sys/ioctl.h>
#include <thread>
#include <unistd.h>
#include <vector>

using namespace std;

const int WIDTH = 80;
const int HEIGHT = 40;
const float DISTANCE = 3.0f;
const float SCALE = 20.0f;

struct Vec3 {
  float x, y, z;
};

Vec3 rotateX(const Vec3 &p, float angle) {
  return {p.x, p.y * cos(angle) - p.z * sin(angle),
          p.y * sin(angle) - p.z * cos(angle)};
}

Vec3 rotateY(const Vec3 &p, float angle) {
  return {p.x * cos(angle) + p.z * sin(angle), p.y,
          -p.x * sin(angle) + p.z * cos(angle)};
}

pair<int, int> project(const Vec3 &p) {
  float z = p.z + DISTANCE;
  float factor = SCALE / z;
  int x = int(p.x * factor + WIDTH / 2);
  int y = int(p.y * factor + HEIGHT / 2);
  return {x, y};
}

int main() {
  vector<Vec3> vertices = {{-1, -1, -1}, {1, -1, -1}, {1, 1, -1}, {-1, 1, -1},
                           {-1, -1, 1},  {1, -1, 1},  {1, 1, 1},  {-1, 1, 1}};

  vector<pair<int, int>> edges = {{0, 1}, {1, 2}, {2, 3}, {3, 0},
                                  {4, 5}, {5, 6}, {6, 7}, {7, 4},
                                  {0, 4}, {1, 5}, {2, 6}, {3, 7}};

  float angleX = 0.0f;
  float angleY = 0.0f;

  while (true) {
    vector<string> buffer(HEIGHT, string(WIDTH, ' '));

    vector<Vec3> rotated;
    for (auto &v : vertices) {
      Vec3 r = rotateX(v, angleX);
      r = rotateY(r, angleY);
      rotated.push_back(r);
    }

    vector<pair<int, int>> points;
    for (auto &v : rotated) {
      points.push_back(project(v));
    }

    for (auto &e : edges) {
      auto [x1, y1] = points[e.first];
      auto [x2, y2] = points[e.second];

      int dx = abs(x2 - x1);
      int dy = abs(y2 - y1);
      int steps = max(dx, dy);

      for (int i = 0; i <= steps; i++) {
        float t = steps == 0 ? 0 : float(i) / steps;
        int x = int(x1 + t * (x2 - x1));
        int y = int(y1 + t * (y2 - y1));

        if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT)
          buffer[y][x] = '#';
      }
    }
    cout << "\x1b[H";
    for (auto &line : buffer)
      cout << line << "\n";

    angleX += 0.03f;
    angleY += 0.05f;

    this_thread::sleep_for(chrono::milliseconds(25));
  }
  return 0;
}
