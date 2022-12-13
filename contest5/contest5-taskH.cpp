#include <algorithm>
#include <iostream>

const size_t kMaxCnt = 1005;

size_t n, m, ptr;
int64_t array1[kMaxCnt];
int64_t array2[kMaxCnt];
int64_t ans[kMaxCnt];
uint64_t dp[kMaxCnt][kMaxCnt];
std::pair<size_t, size_t> parent[kMaxCnt][kMaxCnt];

template <typename T>
void ReadArray(T dest[], size_t len) {
  for (size_t i = 1; i <= len; ++i) {
    std::cin >> dest[i];
  }
}

int main() {
  std::cin >> n;
  ReadArray(array1, n);
  std::cin >> m;
  ReadArray(array2, m);

  for (size_t i = 1; i <= n; ++i) {
    for (size_t j = 1; j <= m; ++j) {
      if (array1[i] == array2[j]) {
        dp[i][j] = dp[i - 1][j - 1] + 1;
        parent[i][j] = {i - 1, j - 1};
      } else {
        if (dp[i][j - 1] > dp[i - 1][j]) {
          dp[i][j] = dp[i][j - 1];
          parent[i][j] = {i, j - 1};
        } else {
          dp[i][j] = dp[i - 1][j];
          parent[i][j] = {i - 1, j};
        }
      }
    }
  }

  ptr = 0;
  while (true) {
    if (n == 0 || m == 0) {
      break;
    }
    if (parent[n][m] == std::make_pair(n - 1, m - 1)) {
      ans[ptr++] = array1[n];
      --n;
      --m;
    } else if (parent[n][m] == std::make_pair(n - 1, m)) {
      --n;
    } else {
      --m;
    }
  }

  while (ptr > 0u) {
    std::cout << ans[--ptr] << ' ';
  }

  return 0;
}