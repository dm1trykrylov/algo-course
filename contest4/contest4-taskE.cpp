#include <algorithm>
#include <iostream>
#include <vector>

namespace utility {
const int32_t kMax = 1e9 + 1;
size_t global_counter = 0;
}  // namespace utility

template <typename T, typename OperationFunctor>
class SparseTable {
 public:
  SparseTable(const std::vector<T>& data)
      : n_(data.size()), logs_(data.size() + 3, 0) {
    PrecalcLogs(data.size() + 2);
    log_n_ = logs_[n_];
    sparse_.assign(log_n_ + 1, std::vector<T>(n_ + 1, T()));
    BuildSparse(data);
  }

  T Query(size_t l, size_t r) {  // interval [l, r)
    size_t lg = logs_[r - l];
    if (l == r - (1 << lg)) {
      return sparse_[lg][l];
    }
    return op_(sparse_[lg][l], sparse_[lg][r - (1 << lg)]);
  }

 private:
  void BuildSparse(const std::vector<T>& data) {
    sparse_[0] = data;
    for (size_t k = 1; k <= log_n_; ++k) {
      for (size_t i = 0; i + (1 << k) <= n_; ++i) {
        sparse_[k][i] =
            op_(sparse_[k - 1][i], sparse_[k - 1][i + (1 << (k - 1))]);
      }
    }
  }

  void PrecalcLogs(size_t n) {
    logs_[0] = 0;
    logs_[1] = 0;
    for (size_t i = 2; i <= n; ++i) {
      logs_[i] = logs_[i / 2] + 1;
    }
  }

  size_t n_;
  std::vector<size_t> logs_;
  size_t log_n_;
  std::vector<std::vector<T>> sparse_;
  OperationFunctor op_;
};

template <typename T>
struct Cell {
  T min1;
  T min2;
};

template <typename T>
struct Number {
  T val;
  size_t id;
  Number() : Number(0) {}
  Number(T val) : val(val), id(utility::global_counter++) {}
  Number& operator=(Number num) {
    this->val = num.val;
    this->id = num.id;
    return *this;
  }
};

template <typename T>
bool operator<(Number<T> a, Number<T> b) {
  return a.val < b.val;
}

template <typename T>
void ReadInput(size_t& n, size_t& m, std::vector<T>& data) {
  std::cin >> n >> m;
  data.resize(n);
  int32_t tmp;
  for (size_t i = 0; i < n; ++i) {
    std::cin >> tmp;
    data[i].min1 = Number(tmp);
    data[i].min2 = Number(utility::kMax);
  }
}

template <typename T, typename OperationFunctor>
void ProcessRequests(size_t m, SparseTable<T, OperationFunctor>* table) {
  size_t left;
  size_t right;
  for (size_t i = 0; i < m; ++i) {
    std::cin >> left >> right;
    std::cout << table->Query(left - 1, right).min2.val << '\n';
  }
}

void FastIO() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  std::cout.tie(nullptr);
}

template <typename T>
struct Min {
  T operator()(T a, T b) {
    Number<int32_t> res[4] = {a.min1, b.min1, a.min2, b.min2};
    std::sort(res, res + 4);
    T min = {res[0], Number(utility::kMax)};
    for (size_t i = 1; i < 4; ++i) {
      if (res[i].id != res[i - 1].id) {
        min.min2 = res[i];
        break;
      }
    }
    return min;
  }
};

int main() {
  // FastIO();
  size_t n, m;
  std::vector<Cell<Number<int32_t>>> data;
  ReadInput(n, m, data);
  auto table =
      new SparseTable<Cell<Number<int32_t>>, Min<Cell<Number<int32_t>>>>(data);
  ProcessRequests(m, table);
  delete table;

  return 0;
}