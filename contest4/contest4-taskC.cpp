#include <algorithm>
#include <iostream>
#include <stack>
#include <vector>

namespace constants {
const size_t kMax = 1e8;
}  // namespace constants

size_t GetLastBit(size_t number);

template <typename T>
class FenwickTree {
 public:
  FenwickTree(size_t size) {
    values_ = new T[size];
    std::fill(values_, values_ + size, 0);
    size_ = size;
  }

  T GetSum(size_t right) {
    T sum = 0;
    for (int32_t i = static_cast<int32_t>(right); i >= 0;
         i = (i & (i + 1)) - 1) {
      sum += values_[i];
    }
    return sum;
  }

  T GetSum(size_t left, size_t right) {
    if (right < left) {
      return 0;
    }
    if (left == 0) {
      return GetSum(right);
    }
    return GetSum(right) - GetSum(left - 1);
  }

  void Add(size_t index, T incr) {
    for (; index < size_; index = index | (index + 1)) {
      values_[index] += incr;
    }
  }

  ~FenwickTree() { delete[] values_; }

 private:
  T* values_;
  size_t size_;
};

template <typename T>
struct Segment {
  T l;
  T r;
};

template <typename T>
bool operator==(Segment<T> a, Segment<T> b) {
  return (a.l == b.l) && (a.r == b.r);
}

template <typename T>
bool operator<(Segment<T> a, Segment<T> b) {
  return (a.l < b.l) || ((a.l == b.l) && a.r < b.r);
}

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

template <typename V, typename T>
void InsertValues(const size_t& n, std::vector<V>& values,
                  FenwickTree<T>* tree) {
  for (size_t i = 0; i < n; ++i) {
    tree->Add(values[i].r, 1);
  }
}

template <typename V, typename T>
T CalcSegments(std::vector<V>& values, FenwickTree<T>* tree) {
  T res = 0;
  std::stack<V> prev_segments;
  T duplicate_counter = 0;
  for (auto seg : values) {
    while (!prev_segments.empty() && seg.l > prev_segments.top().l) {
      tree->Add(prev_segments.top().r, -1);
      prev_segments.pop();
    }
    res += tree->GetSum(seg.l, seg.r) - 1;
    if (!prev_segments.empty() && (seg == prev_segments.top())) {
      res -= 2;
      ++duplicate_counter;
    } else {
      res -= duplicate_counter * (duplicate_counter - 1);
      duplicate_counter = 0;
    }
    prev_segments.push(seg);
  }
  res -= duplicate_counter * (duplicate_counter - 1);

  return res;
}

void FastIO() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  std::cout.tie(nullptr);
}

int main() {
  FastIO();

  std::vector<Segment<int64_t>> segments;
  size_t n;
  ReadValues<Segment<int64_t>, int64_t>(n, segments);
  auto fw_tree = new FenwickTree<int64_t>((n << 1) + 2);
  InsertValues(n, segments, fw_tree);
  int64_t interwinded_segments = CalcSegments(segments, fw_tree);
  std::cout << interwinded_segments;

  delete fw_tree;
  return 0;
}