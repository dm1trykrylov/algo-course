#include <algorithm>
#include <bitset>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <set>
#include <string>
#include <vector>

using namespace std;

const size_t kN = 1 << 11;
size_t n, t, step = 0;

size_t used[kN], p[kN];
vector<bitset<(kN)>> dp(kN);

vector<vector<int>> g;
vector<vector<int>> e;

int main() {
  ios::sync_with_stdio(false);
  cin.tie(0);
  cin >> n;
  for (size_t i = 0; i < n; ++i) {
    for (size_t j = 0; j < n; ++j) {
      cin >> t;
      if (t != 0u) {
        dp[i][j] = true;
      }
    }
  }

  for (size_t k = 0; k < n; ++k) {
    for (size_t i = 0; i < n; ++i) {
      if (dp[i][k]) {
        dp[i] = (dp[i] | dp[k]);
      }
    }
  }

  for (size_t i = 0; i < n; ++i) {
    for (size_t j = 0; j < n; ++j) {
      cout << ((dp[i][j]) ? 1 : 0) << ' ';
    }
    cout << '\n';
  }

  return 0;
}
