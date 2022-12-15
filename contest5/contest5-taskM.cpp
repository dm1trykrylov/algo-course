#include <algorithm>
#include <iostream>

namespace constants {
const size_t kMaxCnt = 14;
const size_t kMaxMask = (1 << kMaxCnt) - 1;
const size_t kInf = 1 << 24;
}  // namespace constants

size_t n;
uint64_t dist[constants::kMaxCnt][constants::kMaxCnt];
uint64_t dp[constants::kMaxMask][constants::kMaxCnt];
uint64_t parent[constants::kMaxMask][constants::kMaxCnt];

template <typename T>
void ReadArray(T dest[], size_t len) {
  for (size_t i = 0; i < len; ++i) {
    std::cin >> dest[i];
  }
}

int main() {
  std::cin >> n;

  for (size_t i = 0; i < n; ++i) {
    ReadArray(dist[i], n);
  }
  for (size_t t = 0; t < constants::kMaxMask; ++t) {
    std::fill(dp[t], dp[t] + constants::kMaxCnt, constants::kInf);
  }
  for (size_t i = 0; i < n; ++i) {
    dp[0][i] = 0;
    dp[(1 << i)][i] = 0;
    parent[(1 << i)][i] = i;
  }

  size_t current_mask = (1 << n) - 1;

  for (size_t mask = 0; mask < current_mask; ++mask) {
    for (size_t v = 0; v < n; ++v) {
      if ((mask & (1 << v)) == 0) {
        for (size_t u = 0; u < n; ++u) {
          if ((mask & (1 << u)) > 0) {
            size_t new_mask = mask | (1 << v);
            if (dp[new_mask][v] > dp[mask][u] + dist[u][v]) {
              dp[new_mask][v] = dp[mask][u] + dist[u][v];
              parent[new_mask][v] = u;
            }
          }
        }
      }
    }
  }

  size_t ans = constants::kInf;
  size_t last_v = 0;
  for (size_t i = 0; i < n; ++i) {
    if (dp[current_mask][i] < ans) {
      ans = dp[current_mask][i];
      last_v = i;
    }
  }
  std::cout << ans << '\n';

  size_t tmp;
  while (n > 0) {
    std::cout << last_v + 1 << ' ';
    tmp = last_v;
    last_v = parent[current_mask][last_v];
    current_mask -= (1 << tmp);
    --n;
  }

  return 0;
}