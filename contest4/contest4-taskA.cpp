#include <algorithm>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

namespace constants {
size_t k_max_time = 42200;
size_t k_max_users = 1e5 + 2;
}  // namespace constants

size_t GetLastBit(size_t number);

template <typename T>
class FenwickTree {
 public:
  FenwickTree(size_t size = kCapacity) {
    values_ = new T[size];
    std::fill(values_, values_ + size, 0);
    size_ = size;
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

  void Add(size_t index, T incr = 1) {
    for (; index <= size_ + 1; index += GetLastBit(index)) {
      values_[index] += incr;
    }
  }

  void Set(size_t index, T value = 1) {
    T old_value = GetSum(index, index);
    Add(index, value - old_value);
  }

  ~FenwickTree() { delete[] values_; }

 private:
  T* values_;
  size_t size_;
  static constexpr size_t kCapacity = 1e5 + 10;
};

size_t GetLastBit(size_t number) {
  long long tmp = (long long)number;
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
void ProcessRequests(FenwickTree<T>* tree, size_t* user_time) {
  size_t n = 0;
  std::cin >> n;

  std::string command;
  size_t user;
  long double cnt, total;
  long double rate = 0;
  while (n--) {
    std::cin >> command >> user;
    if (command == "RUN") {
      std::cin >> cnt;
      if (user_time[user] > 0) {
        tree->Add(user_time[user], -1);
      }
      user_time[user] = cnt;
      tree->Add(user_time[user], 1);
    } else {
      total = tree->GetSum(constants::k_max_time + 1);
      rate = 0;
      if (user_time[user] > 0 && total > 1) {
        cnt = tree->GetSum(user_time[user] - 1);
        total -= 1;
        rate = cnt / total;
      }
      if (user_time[user] > 0 && total == 1) {
        rate = 1;
      }
      std::cout << std::setprecision(6) << rate << '\n';
      // std::cout << rate << '\n';
    }
  }
}

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  std::cout.tie(nullptr);

  FenwickTree<long double>* fw_tree =
      new FenwickTree<long double>(constants::k_max_time * 2);
  size_t* user_time = new size_t[constants::k_max_users];
  std::fill(user_time, user_time + constants::k_max_users, 0);

  ProcessRequests(fw_tree, user_time);
  delete[] user_time;
  delete fw_tree;
  return 0;
}