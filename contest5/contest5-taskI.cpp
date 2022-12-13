#include <algorithm>
#include <iostream>

const size_t kMaxCnt = 2005;

size_t n, m, ptr, max_id, max_k;
int64_t array[kMaxCnt];
int64_t ans[kMaxCnt];
uint64_t dp[kMaxCnt][2];
uint64_t parent[kMaxCnt][2];

template <typename T>
void ReadArray(T dest[], size_t len) {
  for (size_t i = 1; i <= len; ++i) {
    std::cin >> dest[i];
  }
}

void FastIO() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  std::cout.tie(nullptr);
}

int main() {
  FastIO();

  std::cin >> n;
  ReadArray(array, n);

  for (size_t i = 1; i <= n; ++i) {
    dp[i][0] = 1;
    dp[i][1] = 1;
    for (size_t j = 1; j < i; ++j) {
      if (array[j] > array[i]) {
        if (dp[i][0] < dp[j][1] + 1) {
          dp[i][0] = dp[j][1] + 1;
          parent[i][0] = j;
        }
      }
      if (array[j] < array[i]) {
        if (dp[i][1] < dp[j][0] + 1) {
          dp[i][1] = dp[j][0] + 1;
          parent[i][1] = j;
        }
      }
    }

    if (m < dp[i][0]) {
      m = dp[i][0];
      max_id = i;
      max_k = 0;
    }

    if (m < dp[i][1]) {
      m = dp[i][1];
      max_id = i;
      max_k = 1;
    }
  }

  std::cout << m << '\n';

  ptr = 0;
  while (max_id > 0u) {
    ans[ptr++] = array[max_id];
    max_id = parent[max_id][max_k];
    max_k = 1 - max_k;
  }

  while (ptr > 0u) {
    --ptr;
    std::cout << ans[ptr] << ' ';
  }

  return 0;
}