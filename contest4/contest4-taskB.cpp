#include <algorithm>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

size_t GetLastBit(size_t number);

template <typename T>
class FenwickTree {
 public:
  FenwickTree(size_t size) : values_(new T[size]), size_(size - 1) {
    std::fill(values_, values_ + size, 0);
  }

  T GetSum(size_t right) {
    size_t sum = 0;
    for (; right > 0; right -= GetLastBit(right)) {
      sum += values_[right];
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
    for (; index < size_; index += GetLastBit(index)) {
      values_[index] += incr;
    }
  }

  void Set(size_t index, T value) {
    T old_value = GetSum(index, index);
    Add(index, value - old_value);
  }

  ~FenwickTree() { delete[] values_; }

 private:
  T* values_;
  size_t size_;
};

size_t GetLastBit(size_t number) {
  int64_t tmp = (int64_t)number;
  tmp = tmp & (-tmp);
  return (size_t)tmp;
}

template <typename T>
void CompressValues(std::vector<T>& values) {
  std::vector<T> tmp = values;
  std::sort(tmp.begin(), tmp.end());

  for (auto& v : values) {
    v = std::lower_bound(tmp.begin(), tmp.end(), v) - tmp.begin() + 1;
  }
}

template <typename T>
void ReadValues(size_t n, std::vector<T>& values) {
  values.resize(n);
  for (size_t i = 0; i < n; ++i) {
    std::cin >> values[i];
  }
  CompressValues<T>(values);
}

template <typename T>
void ReadArray(FenwickTree<T>* tree, T* values, size_t n) {
  T t;
  for (size_t i = 1; i <= n; ++i) {
    std::cin >> t;
    if (i % 2 == 0) {
      t *= -1;
    }
    tree->Add(i, t);
    values[i] = t;
  }
}

template <typename T>
void ProcessRequests(FenwickTree<T>* tree, T* values) {
  size_t m = 0;
  std::cin >> m;
  uint8_t command;
  for (size_t i = 0; i < m; ++i) {
    std::cin >> command;
    if (command == '0') {
      size_t index;
      T new_value;
      std::cin >> index >> new_value;
      if (index % 2 == 0) {
        new_value *= -1;
      }
      tree->Add(index, new_value - values[index]);
      values[index] = new_value;
    } else {
      size_t left, right;
      std::cin >> left >> right;
      auto sum = tree->GetSum(left, right);
      if (left % 2 == 0) {
        sum *= -1;
      }
      std::cout << sum << '\n';
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
  int64_t* values = new int64_t[n + 1];
  FenwickTree<int64_t>* fw_tree = new FenwickTree<int64_t>(2 * n + 1);
  ReadArray(fw_tree, values, n);
  ProcessRequests(fw_tree, values);

  delete[] values;
  delete fw_tree;
  return 0;
}