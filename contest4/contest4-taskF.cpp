#include <algorithm>
#include <bitset>
#include <iostream>
#include <stack>
#include <vector>

namespace constants {
const size_t kMax = 1e5 + 1;
}  // namespace constants

class FenwickTree {
 public:
  FenwickTree(size_t x_size, size_t y_size) : x_size_(x_size), y_size_(y_size) {
    values_.resize(x_size);
  }

  size_t GetSum(int32_t x, int32_t y) {
    size_t sum = 0;
    for (int32_t i = static_cast<int32_t>(x); i >= 0; i = (i & (i + 1)) - 1) {
      for (int32_t j = static_cast<int32_t>(y); j >= 0; j = (j & (j + 1)) - 1) {
        sum += values_[i].test(j);
      }
    }
    return sum;
  }

  size_t GetSum(int32_t left_x, int32_t left_y, int32_t right_x,
                int32_t right_y) {
    return GetSum(right_x, right_y) - GetSum(right_x, left_y - 1) -
           GetSum(left_x - 1, right_y) + GetSum(left_x - 1, left_y - 1);
  }

  void Add(int32_t x, int32_t y) {
    for (; x < x_size_; x = x | (x + 1)) {
      for (; y < y_size_; y = y | (y + 1)) {
        values_[x].flip(y);
      }
    }
  }

 private:
  size_t x_size_;
  size_t y_size_;
  std::vector<std::bitset<constants::kMax>> values_;
};

template <typename T, typename V>
void CompressValues(std::vector<T>& values) {
  std::vector<V> tmp;
  for (auto seg : values) {
    tmp.emplace_back(seg.l);
    tmp.emplace_back(seg.r);
  }

  std::sort(tmp.begin(), tmp.end());
  auto last = std::unique(tmp.begin(), tmp.end());
  tmp.erase(last, tmp.end());

  for (auto& v : values) {
    v.l = std::lower_bound(tmp.begin(), tmp.end(), v.l) - tmp.begin() + 1;
    v.r = std::lower_bound(tmp.begin(), tmp.end(), v.r) - tmp.begin() + 1;
  }
}

template <typename T, typename V>
void ReadValues(size_t& n, std::vector<T>& values) {
  std::cin >> n;
  values.resize(n);
  for (size_t i = 0; i < n; ++i) {
    std::cin >> values[i].l >> values[i].r;
  }
  std::sort(values.begin(), values.end());
  CompressValues<T, V>(values);
}
/*
template <typename V, typename T>
void InsertValues(const size_t& n, std::vector<V>& values,
                  FenwickTree<T>* tree) {
  for (size_t i = 0; i < n; ++i) {
    tree->Add(values[i].r, 1);
  }
}

template <typename V, typename T>
T CalcSegments(std::vector<V>& values, FenwickTree<T>* tree) {
  return res;
}
*/
void FastIO() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  std::cout.tie(nullptr);
}

int main() {
  // FastIO();
  /*
    std::vector<Segment<int64_t>> segments;
    size_t n;
    ReadValues<Segment<int64_t>, int64_t>(n, segments);
    auto fw_tree = new FenwickTree<int64_t>((n << 1) + 2);
    InsertValues(n, segments, fw_tree);
    int64_t interwinded_segments = CalcSegments(segments, fw_tree);
    std::cout << interwinded_segments;

    delete fw_tree;*/
  return 0;
}