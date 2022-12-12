#include <iostream>
#include <string>
#include <vector>

template <typename T, typename V>
void CompressValues(std::vector<T>& values) {
  std::vector<V> tmp;
  for (auto seg : values) {
    tmp.emplace_back(seg.l);
    tmp.emplace_back(seg.r);
  }

  std::sort(tmp.begin(), tmp.end());
  auto last = std::unique(tmp.begin(), tmp.end());
  tmp.erase(last, tmp.end());

  for (auto& v : values) {
    v.l = std::lower_bound(tmp.begin(), tmp.end(), v.l) - tmp.begin() + 1;
    v.r = std::lower_bound(tmp.begin(), tmp.end(), v.r) - tmp.begin() + 1;
  }
}

template <typename T, typename V>
void ReadValues(size_t& n, std::vector<T>& values) {
  std::cin >> n;
  values.resize(n);
  for (size_t i = 0; i < n; ++i) {
    std::cin >> values[i].l >> values[i].r;
  }
  std::sort(values.begin(), values.end());
  CompressValues<T, V>(values);
}