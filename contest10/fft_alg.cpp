#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

template <typename T>
void Read(std::vector<T>& dest, size_t len) {
  dest.resize(len);
  for (size_t i = 0; i < len; ++i) {
    std::cin >> dest[i];
  }
}

template <typename T>
void Print(const std::vector<T>& dest, char delim = ' ') {
  for (auto x : dest) {
    std::cout << x << delim;
  }
}

const int MOD = 998244353, W = 805775211, IW = 46809892;
const int MAXN = (1 << 19), INV2 = 499122177;

// W - первообразный корень MAXN-ной степени из 1, IW - обратное по модулю MOD к
// W Первообразный корень (1 << 23)-й степени из 1 по модулю MOD равен 31; тогда
// первообразный корень (1 << X)-й степени для X от 1 до 23 равен (31 * (1 <<
// (23 - X))) % MOD INV2 - обратное к двум по модулю MOD Данная реализация FFT
// перемножает два целых числа длиной до 250000 цифр за ~0.13 секунд без проблем
// с точностью и занимает всего 30 строк кода

int pws[MAXN + 1], ipws[MAXN + 1];

void init() {
  pws[MAXN] = W;
  ipws[MAXN] = IW;
  for (int i = MAXN / 2; i >= 1; i /= 2) {
    pws[i] = (pws[i * 2] * 1ll * pws[i * 2]) % MOD;
    ipws[i] = (ipws[i * 2] * 1ll * ipws[i * 2]) % MOD;
  }
  for (size_t i = 0; i < 8; ++i) {
    std::cout << pws[i] << ' ';
  }
  std::cout << '\n';
  std::cout << '\n';
}

void fft(vector<int>& a, vector<int>& ans, int l, int cl, int step, int n,
         bool inv) {
  if (n == 1) {
    ans[l] = a[cl];
    return;
  }
  fft(a, ans, l, cl, step * 2, n / 2, inv);
  fft(a, ans, l + n / 2, cl + step, step * 2, n / 2, inv);
  int cw = 1, gw = (inv ? ipws[n] : pws[n]);
  for (int i = l; i < l + n / 2; i++) {
    int u = ans[i], v = (cw * 1ll * ans[i + n / 2]) % MOD;
    ans[i] = (u + v) % MOD;
    ans[i + n / 2] = (u - v) % MOD;
    if (ans[i + n / 2] < 0) ans[i + n / 2] += MOD;
    if (inv) {
      ans[i] = (ans[i] * 1ll * INV2) % MOD;
      ans[i + n / 2] = (ans[i + n / 2] * 1ll * INV2) % MOD;
    }
    cw = (cw * 1ll * gw) % MOD;
  }
  Print(ans);
  std::cout << '\n';
}

int main() {
  init();
  size_t len1;
  vector<int> poly1;
  std::cin >> len1;
  Read(poly1, len1 + 1);
  reverse(poly1.begin(), poly1.end());
  poly1.resize(8);
  vector<int> ans(poly1.size());
  vector<int> ans1(poly1.size());
  fft(poly1, ans, 0, 0, 1, poly1.size(), false);
  // Print(ans);
  fft(ans, ans1, 0, 0, 1, ans.size(), true);
  cout << '\n' << 'a' << '\n';
  Print(ans1);

  return 0;
}
