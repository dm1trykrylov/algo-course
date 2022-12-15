#include <algorithm>
#include <iostream>

namespace constants {
const size_t kMaxCnt = 1 << 11;
const size_t kInf = 1 << 24;
const int64_t kMod = 1e9;
}  // namespace constants

struct NMod {
  int64_t val;
  NMod(uint64_t v = 0) { val = v; }
  NMod operator++() {
    ++val;
    if (val > constants::kMod) {
      val -= constants::kMod;
    }
    return *this;
  }
};

NMod operator+(NMod a, NMod b) {
  a.val += b.val;
  if (a.val > constants::kMod) {
    a.val -= constants::kMod;
  }
  return a;
}

NMod operator-(NMod a, NMod b) {
  a.val -= b.val;
  if (a.val < 0) {
    a.val += constants::kMod;
  }
  return a;
}

size_t n;
NMod dp[constants::kMaxCnt][constants::kMaxCnt];
uint64_t numbers[constants::kMaxCnt];

template <typename T>
void ReadArray(T dest[], size_t len) {
  for (size_t i = 0; i < len; ++i) {
    std::cin >> dest[i];
  }
}

int main() {
  std::cin >> n;

  ReadArray(numbers, n);
  NMod total_cnt;

  for (size_t i = 0; i < n; ++i) {
    dp[i][1] = 1;  // dp[i][len] -- count of palindromic subsequences in [i, i +
    dp[i][0] = 0;  // len - 1]
  }

  for (size_t len = 2; len <= n; ++len) {
    for (size_t i = 0; i + len <= n; ++i) {
      if (numbers[i] == numbers[i + len - 1]) {
        dp[i][len] = dp[i + 1][len - 1] + dp[i][len - 1] + 1;
      } else {
        dp[i][len] = dp[i + 1][len - 1] + dp[i][len - 1] - dp[i + 1][len - 2];
      }

      // std::cout << "i: " << i << " len: " << len << " dp: " << dp[i][len].val
      // << '\n';
    }
  }
  std::cout << dp[0][n].val;

  return 0;
}