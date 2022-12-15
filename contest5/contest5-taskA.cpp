#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

const int64_t kMax = 2e9;

class Vector {
 public:
  int64_t x, y;
  Vector() {
    x = 0;
    y = 0;
  }
  Vector(int64_t x1, int64_t y1) {
    x = x1;
    y = y1;
  }
  Vector& operator-=(Vector b) {
    x -= b.x;
    y -= b.y;
    return *this;
  }
  friend int64_t operator%(Vector b, Vector a) {
    Vector c;

    c.x = b.x * a.y;
    c.y = b.y * a.x;
    return c.x - c.y;
  }
  friend Vector operator-(Vector a, Vector b) { return a -= b; }
};

int main() {
  size_t n;
  Vector pokemon;
  Vector scanline(1, kMax);
  std::cin >> n >> pokemon.x >> pokemon.y;
  std::vector<Vector> points(n);

  for (size_t i = 0; i < n; ++i) {
    std::cin >> points[i].x >> points[i].y;
    points[i] -= pokemon;
  }

  size_t intersection_cnt = 0;
  Vector o(0, 0);

  for (size_t i = 0; i < n; ++i) {
    Vector p1 = points[i];
    Vector p2 = points[(i + 1) % n];
    bool sign_i = p1 % scanline > 0;
    bool sign_i1 = p2 % scanline > 0;
    Vector pp = p2 - p1;
    bool sign_i_1 = pp % (scanline - p1) > 0;
    bool sign_i1_1 = pp % (o - p1) > 0;
    if ((sign_i1 != sign_i) && (sign_i_1 != sign_i1_1)) {
      ++intersection_cnt;
    }
  }

  std::cout << ((intersection_cnt & 1) == 0 ? "NO" : "YES");
  return 0;
}