#include <algorithm>
#include <iomanip>
#include <iostream>

namespace constants {
const size_t kMaxCnt = 200;
const long double kTwo = 2.0L;
}  // namespace constants

size_t n;
long double dp[constants::kMaxCnt][constants::kMaxCnt];
long double numbers[constants::kMaxCnt];

template <typename T>
void ReadArray(T dest[], size_t len) {
  for (size_t i = 0; i < len; ++i) {
    std::cin >> dest[i];
  }
}

long double Avg(long double a, long double b) {
  return (a + b) / constants::kTwo;
}

int main() {
  std::cin >> n;

  ReadArray(numbers, n);

  for (size_t i = 0; i < n; ++i) {
    dp[i][1] = numbers[i];
    dp[i][0] = numbers[i];
    dp[i][2] = Avg(numbers[i], numbers[i + 1]);
  }
  dp[n - 1][2] = numbers[n - 1];

  for (size_t len = 3; len <= n; ++len) {
    for (size_t i = 0; i < n; ++i) {
      long double current_avg;
      dp[i][len] = 0.0L;
      for (size_t mid = i; mid < i + len - 1; ++mid) {
        current_avg = dp[i][(mid - i) + 1];
        dp[i][len] = std::max(
            dp[i][len], Avg(current_avg, dp[mid + 1][len - (mid - i) - 1]));
      }

      for (size_t mid = i + len - 2; mid > i; --mid) {
        current_avg = dp[mid][len - mid + i];
        dp[i][len] = std::max(dp[i][len], Avg(current_avg, dp[i][mid - i]));
      }
      dp[i][len] = std::max(
          {dp[i][len], Avg(numbers[i], dp[i + 1][len - 1]),
           Avg(numbers[i + len - 1], dp[i][len - 1]),
           Avg(numbers[i], Avg(numbers[i + len - 1], dp[i + 1][len - 2])),
           Avg(numbers[i + len - 1], Avg(numbers[i], dp[i + 1][len - 2]))});
      /*
            std::cout << "i: " << i << " len: " << len << " dp: " << dp[i][len]
                      << '\n';*/
    }
  }

  std::cout << std::fixed << std::setprecision(40) << dp[0][n];

  return 0;
}