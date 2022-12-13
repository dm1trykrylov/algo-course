#include <algorithm>
#include <iostream>

const size_t kMaxCnt = 105;
const size_t kMaxWeight = 10005;

size_t n;
uint64_t max_weight;
uint64_t weight[kMaxCnt];
uint64_t cost[kMaxCnt];
uint64_t dp[kMaxWeight][kMaxCnt];

template <typename T>
void ReadArray(T dest[], size_t len) {
  for (size_t i = 1; i <= len; ++i) {
    std::cin >> dest[i];
  }
}

int main() {
  std::cin >> n >> max_weight;
  ReadArray(weight, n);
  ReadArray(cost, n);

  for (size_t i = 0; i < n; ++i) {
    dp[i][0] = 0;
  }

  for (size_t i = 1; i <= n; ++i) {
    for (size_t w = 1; w <= max_weight; ++w) {
      dp[w][i] = dp[w][i - 1];
      if (weight[i] <= w) {
        dp[w][i] = std::max(dp[w][i], dp[w - weight[i]][i - 1] + cost[i]);
      }
    }
  }
  size_t current_weight = max_weight;
  while (current_weight > 0) {
    if (dp[current_weight][n] == 0) {
      break;
    }
    if (dp[current_weight][n] != dp[current_weight][n - 1]) {
      std::cout << n << '\n';
      current_weight -= weight[n];
    }
    --n;
  }

  return 0;
}