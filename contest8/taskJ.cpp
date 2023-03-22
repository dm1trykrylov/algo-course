#define _CRT_SECURE_NO_WARNINGS
#include <algorithm>
#include <bitset>
#include <cmath>
#include <iostream>
#include <vector>

const size_t kLength = 2e5 + 5, kLogn = 18;
int m, n, k, t, ans, v1, v2;

size_t binup_[kLength][kLogn + 1];
class BinUp {
 private:
  size_t parents_[kLength];
  size_t depth_[kLength];
  bool is_deleted_[kLength];
  size_t real_[kLength];
  size_t counter;

 public:
  BinUp() {
    parents_[1] = 1;
    depth_[1] = 1;
    counter = 1;
  }
  size_t Get(size_t vertex, size_t power) { return binup_[vertex][power]; }
  void Set(size_t vertex, size_t power, size_t value) {
    binup_[vertex][power] = value;
  }
  void SetAll(size_t vertex, size_t value) {
    for (size_t i = 0; i <= kLogn; ++i) binup_[vertex][i] = value;
  }

  void AddVertex(size_t parent) {
    size_t new_vertex = ++counter;
    parents_[new_vertex] = parent;
    depth_[new_vertex] = depth_[parent] + 1;
    binup_[new_vertex][0] = parent;
    for (size_t power = 1; power <= kLogn; ++power) {
      binup_[new_vertex][power] =
          binup_[binup_[new_vertex][power - 1]][power - 1];
    }
  }
  void Delete(size_t vertex) {
    is_deleted_[vertex] = true;
    real_[vertex] = Find(vertex);
  }

  size_t Find(size_t vertex) {
    if (!is_deleted_[parents_[vertex]]) {
      return parents_[vertex];
    }
    return parents_[vertex] = Find(parents_[vertex]);
  }

  size_t LCA(size_t lhs, size_t rhs) {
    if (depth_[lhs] < depth_[rhs]) {
      std::swap(lhs, rhs);
    }
    for (int h = kLogn; h >= 0; --h) {
      if (depth_[rhs] + (1 << h) <= depth_[lhs]) {
        lhs = binup_[lhs][h];
      }
    }
    if (lhs == rhs) {
      return lhs;
    }

    for (int i = kLogn; i >= 0; --i) {
      if (binup_[lhs][i] != binup_[rhs][i]) {
        rhs = binup_[rhs][i];
        lhs = binup_[lhs][i];
      }
    }
    
    return Find(lhs);
  }
};

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(0);

  size_t queries_count;
  std::cin >> queries_count;
  // DFSPrecalc(0);
  BinUp binup;
  binup.SetAll(1, 1);

  char query;
  size_t vertex, u, v;
  for (size_t i = 0; i < queries_count; ++i) {
    std::cin >> query;
    if (query == '+') {
      std::cin >> vertex;
      binup.AddVertex(vertex);

    } else if (query == '-') {
      std::cin >> vertex;
      binup.Delete(vertex);
    } else {
      std::cin >> u >> v;
      std::cout << binup.LCA(u, v) << '\n';
    }
  }

  return 0;
}
