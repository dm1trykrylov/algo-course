#include <iostream>
#include <set>

const long long kMax = 1 << 30;

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  std::cout.tie(nullptr);
  size_t n;
  size_t k;
  long long a_0, a_i;
  long long x, y;
  std::cin >> n >> k;
  std::cin >> a_0 >> x >> y;

  std::multiset<long long> first_k_elements;

  for (size_t i = 0; i < n; ++i) {
    a_i = (x * a_0 + y) % kMax;
    a_0 = a_i;
    if (first_k_elements.size() < k) {
      first_k_elements.insert(a_i);
    } else {
      if (a_i < *first_k_elements.rbegin()) {
        first_k_elements.erase(--first_k_elements.end());
        first_k_elements.insert(a_i);
      }
    }
  }

  for (auto element : first_k_elements) {
    std::cout << element << ' ';
  }
  return 0;
}