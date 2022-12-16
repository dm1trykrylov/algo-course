#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <queue>
#include <vector>

const size_t kMaxLength = 1e5 + 1;
const size_t kMax = 1e9 + 2;

int64_t x, y, diff, t;
class Vector {
 public:
  int64_t x, y, len;
  Vector() {
    x = 0;
    y = 0;
    len = 0;
  }
  Vector(int64_t x_in, int64_t y_in) {
    x = x_in;
    y = y_in;
    len = x * x + y * y;
  }
  void SetCor(int64_t x_in, int64_t y_in) {
    x = x_in;
    y = y_in;
  }
  void SetLen() { len = x * x + y * y; }
  Vector& operator-=(Vector second) {
    x -= second.x;
    y -= second.y;
    len = x * x + y * y;
    return *this;
  }
  friend Vector operator-(Vector first, Vector second) {
    first -= second;
    return first;
  }
  friend int64_t operator%(Vector first, Vector second) {
    int64_t product;
    product = first.x * second.y - second.x * first.y;
    return product;
  }
  friend std::istream& operator>>(std::istream& in, Vector& v_in) {
    in >> v_in.x >> v_in.y;
    return in;
  }
  friend std::ostream& operator<<(std::ostream& out, const Vector& k_v_out) {
    out << k_v_out.x << ' ' << k_v_out.y;
    return out;
  }
};
Vector vecs[kMaxLength];
std::vector<Vector> q_vec;

bool CompareByAngle(Vector lhs, Vector rhs) {
  Vector t_lhs = lhs - vecs[0];
  Vector t_rhs = rhs - vecs[0];
  return t_lhs % t_rhs < 0 || ((t_lhs % t_rhs == 0) && (t_lhs.len < t_rhs.len));
}

bool CompareByPos(Vector lhs, Vector rhs) {
  return lhs.y < rhs.y || ((lhs.y == rhs.y) && (lhs.x < rhs.x));
}

long double GetArea(const std::vector<Vector>& points) {
  int64_t area = 0;
  // trapeze method
  for (size_t i = 0; i < points.size() - 1; ++i) {
    area += (points[i].y + points[i + 1].y) * (points[i + 1].x - points[i].x);
  }
  size_t n = points.size();
  area += (points[0].y + points[n - 1].y) * (points[0].x - points[n - 1].x);

  return std::abs(static_cast<long double>(area)) / 2.0L;
}

int main() {
  size_t n;
  std::cin >> n;
  for (size_t i = 0; i < n; ++i) {
    std::cin >> vecs[i];
  }
  // sorting to find lowest point
  std::sort(vecs, vecs + n, CompareByPos);
  // vecs[0] - lowest point
  // sorting points by angle in relation to vecs[0]
  std::sort(vecs + 1, vecs + n, CompareByAngle);
  // constructing convex hull
  // convex hull vertices are stored in q_vec
  for (size_t i = 0; i < n; ++i) {
    while (q_vec.size() >= 2) {
      Vector vec1 = *q_vec.rbegin() - vecs[i];
      Vector vec2 = *(++q_vec.rbegin()) - *q_vec.rbegin();
      if (vec1 % vec2 > 0) {
        break;
      }
      q_vec.pop_back();
    }
    q_vec.push_back(vecs[i]);
  }

  std::cout << q_vec.size() << '\n';
  for (auto it : q_vec) {
    std::cout << it << '\n';
  }

  std::cout << std::fixed << std::setprecision(1) << GetArea(q_vec);

  return 0;
}