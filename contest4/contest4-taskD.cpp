#include <algorithm>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

template <typename T>
class FenwickTree {
 public:
  FenwickTree(size_t x, size_t y, size_t z)
      : x_size_(x), y_size_(y), z_size_(z) {
    values_ = new T**[x_size_];

    for (size_t i = 0; i < x_size_; ++i) {
      values_[i] = new T*[y_size_];
      for (size_t j = 0; j < y_size_; ++j) {
        values_[i][j] = new T[z_size_];
        std::fill(values_[i][j], values_[i][j] + z_size_, 0);
      }
    }
  }

  T GetSum(size_t x, size_t y, size_t z) {
    T sum = 0;
    for (int32_t i = static_cast<int32_t>(x); i >= 0; i = (i & (i + 1)) - 1) {
      for (int32_t j = static_cast<int32_t>(y); j >= 0; j = (j & (j + 1)) - 1) {
        for (int32_t k = static_cast<int32_t>(z); k >= 0;
             k = (k & (k + 1)) - 1) {
          sum += values_[i][j][k];
        }
      }
    }
    return sum;
  }

  T GetSum(size_t left_x, size_t left_y, size_t left_z, size_t right_x,
           size_t right_y, size_t right_z) {
    return GetSum(right_x, right_y, right_z) -
           GetSum(right_x, left_y - 1, right_z) -
           GetSum(left_x - 1, right_y, right_z) -
           GetSum(right_x, right_y, left_z - 1) +
           GetSum(right_x, left_y - 1, left_z - 1) +
           GetSum(left_x - 1, left_y - 1, right_z) +
           GetSum(left_x - 1, right_y, left_z - 1) -
           GetSum(left_x - 1, left_y - 1, left_z - 1);
    ;
  }

  void Add(size_t x, size_t y, size_t z, T incr) {
    for (size_t i = x; i < x_size_; i = i | (i + 1)) {
      for (size_t j = y; j < y_size_; j = j | (j + 1)) {
        for (size_t k = z; k < z_size_; k = k | (k + 1)) {
          values_[i][j][k] += incr;
        }
      }
    }
  }

  ~FenwickTree() {
    for (size_t i = 0; i < x_size_; ++i) {
      for (size_t j = 0; j < y_size_; ++j) {
        delete[] values_[i][j];
      }
      delete[] values_[i];
    }
    delete[] values_;
  }

 private:
  T*** values_;
  size_t x_size_;
  size_t y_size_;
  size_t z_size_;
};

template <typename T>
void ProcessRequests(FenwickTree<T>* tree) {
  size_t command;
  size_t x1, y1, z1, x2, y2, z2;
  int64_t k;
  while (true) {
    std::cin >> command;
    if (command == 1) {
      std::cin >> x1 >> y1 >> z1 >> k;
      tree->Add(x1, y1, z1, k);
    }
    if (command == 2) {
      std::cin >> x1 >> y1 >> z1 >> x2 >> y2 >> z2;
      std::cout << tree->GetSum(x1, y1, z1, x2, y2, z2) << '\n';
    }
    if (command == 3) {
      break;
    }
  }
}

void FastIO() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  std::cout.tie(nullptr);
}

int main() {
  // FastIO();
  size_t n;
  std::cin >> n;
  auto fw_tree = new FenwickTree<int64_t>(n + 2, n + 2, n + 2);

  ProcessRequests(fw_tree);

  delete fw_tree;
  return 0;
}