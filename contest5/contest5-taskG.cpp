#include <algorithm>
#include <iostream>
#include <vector>

const size_t kMaxCnt = 1 << 17;

template <typename T>
class SegmentTree {
 public:
  SegmentTree(size_t size, const T array[]) : len_(size) {
    tree_.resize(len_ * 2 + 2, 0);
    for (size_t i = len_; i < len_ + size; ++i) {
      tree_[i] = array[i - len_];
    }
    for (size_t i = len_ - 1; i > 0; --i) {
      tree_[i] = std::max(tree_[i * 2], tree_[i * 2 + 1]);
    }
  }

  void Update(size_t index, T value) {
    index += len_;
    tree_[index] = value;
    index >>= 1;
    while (index > 0u) {
      tree_[index] = std::max(tree_[index * 2], tree_[index * 2 + 1]);
      index >>= 1;
    }
  }

  T Max(size_t l, size_t r) {
    l += len_;
    r += len_ + 1;
    T max = 0;
    while (l < r) {
      if ((l & 1) == 1) {
        max = std::max(max, tree_[l++]);
      }
      if ((r & 1) == 1) {
        max = std::max(max, tree_[--r]);
      }
      l >>= 1;
      r >>= 1;
    }
    return max;
  }

  T Top() { return tree_[1]; }

 private:
  std::vector<T> tree_;
  size_t len_;
};

struct Pair {
  int64_t value;
  size_t index;
};

bool operator<(Pair a, Pair b) {
  return (a.value < b.value) || ((a.value == b.value) && (a.index > b.index));
}

size_t n, m, ptr, max_id, max_k, max_prev;
int64_t array[kMaxCnt];
Pair pokemons[kMaxCnt];
int64_t ans[kMaxCnt];
uint64_t dp[kMaxCnt];

template <typename T>
void ReadArray(T dest[], size_t len) {
  for (size_t i = 1; i <= len; ++i) {
    std::cin >> dest[i];
  }
}

void ReadInput(Pair data[], size_t& n) {
  std::cin >> n;
  ReadArray(array, n);
  for (size_t i = 1; i <= n; ++i) {
    data[i] = {array[i], i};
  }
  std::sort(data + 1, data + n + 1);
}

void FastIO() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  std::cout.tie(nullptr);
}

int main() {
  FastIO();

  ReadInput(pokemons, n);
  auto dptree = new SegmentTree<uint64_t>(n + 1, dp);

  for (size_t i = 1; i <= n; ++i) {
    ptr = pokemons[i].index;
    dp[ptr] = 1;
    max_prev = std::max(0ul, dptree->Max(0, ptr - 1));

    dp[ptr] += max_prev;
    dptree->Update(ptr, dp[ptr]);
  }

  std::cout << dptree->Top();
  delete dptree;
  return 0;
}