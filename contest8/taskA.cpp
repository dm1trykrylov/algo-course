// 83533486
#include <algorithm>
#include <iostream>
#include <numeric>
#include <unordered_map>
#include <vector>

template <typename T, typename P>
class DSU {
 public:
  DSU(size_t n) {
    for (size_t i = 0; i < n; ++i) {
      parent_[i] = i;
      rank_[i] = 0;
      power_[i] = 0;
    }
  }
  DSU(std::vector<T>& nodes) {
    for (size_t i = 0; i < nodes.size(); ++i) {
      parent_[nodes[i]] = nodes[i];
      rank_[nodes[i]] = 0;
    }
  }

  bool IsSame(T lhs, T rhs) { return Find(lhs) == Find(rhs); }

  T Find(T vertex) {  // find root
    if (parent_[vertex] == vertex) {
      return vertex;
    }
    parent_[vertex] = Find(parent_[vertex]);  // path compression heuristic
    power_[vertex] = power_[parent_[vertex]];
    return parent_[vertex];
  }

  void AddPower(T vertex, P power) {
    vertex = Find(vertex);
    power_[vertex] += power;
  }

  P Power(T vertex) {
    vertex = Find(vertex);
    return power_[vertex];
  }

  void Union(T lhs, T rhs) {
    T root_u = Find(lhs);
    T root_v = Find(rhs);
    if (root_u != root_v) {  // rank heuristic
      if (rank_[root_u] < rank_[root_v]) {
        std::swap(root_v, root_u);
      }
      parent_[root_v] = root_u;
      power_[root_u] += power_[root_v];
      if (rank_[root_u] == rank_[root_v]) {
        rank_[root_u] += 1;
      }
    }
  }

 private:
  std::unordered_map<T, T> parent_;
  std::unordered_map<T, P> power_;
  std::unordered_map<T, size_t> rank_;
};

template <>
class DSU<size_t, size_t> {
 public:
  DSU(size_t n) : parent_(n, 0), rank_(n, 0), power_(n, 0) {
    for (size_t i = 0; i < n; ++i) {
      parent_[i] = i;
    }
  }

  bool IsSame(size_t lhs, size_t rhs) { return Find(lhs) == Find(rhs); }

  size_t Find(size_t vertex) {  // find root
    if (parent_[vertex] == vertex) {
      return vertex;
    }
    parent_[vertex] = Find(parent_[vertex]);  // path compression heuristic
    power_[vertex] = power_[parent_[vertex]];
    return parent_[vertex];
  }

  void AddPower(size_t vertex, size_t power) {
    vertex = Find(vertex);
    power_[vertex] += power;
  }

  size_t Power(size_t vertex) {
    vertex = Find(vertex);
    return power_[vertex];
  }

  void Union(size_t lhs, size_t rhs) {
    size_t root_u = Find(lhs);
    size_t root_v = Find(rhs);
    if (root_u != root_v) {  // rank heuristic
      if (rank_[root_u] < rank_[root_v]) {
        std::swap(root_v, root_u);
      }
      parent_[root_v] = root_u;
      power_[root_u] += power_[root_v];
      if (rank_[root_u] == rank_[root_v]) {
        rank_[root_u] += 1;
      }
    }
  }

 private:
  std::vector<size_t> parent_;
  std::vector<size_t> rank_;
  std::vector<size_t> power_;
};

void AddFriends(DSU<size_t, size_t>& dsu) {
  size_t lhs;
  size_t rhs;
  size_t pair_power;
  std::cin >> lhs >> rhs >> pair_power;
  dsu.Union(lhs, rhs);
  dsu.AddPower(lhs, pair_power);
}

void inline FastIO() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  std::cout.tie(nullptr);
}

int main() {
  FastIO();
  size_t students;
  size_t queries;
  std::cin >> students >> queries;
  DSU<size_t, size_t> dsu(students + 1);

  char operation_type;
  size_t student;
  for (size_t i = 0; i < queries; ++i) {
    std::cin >> operation_type;
    if (operation_type == '1') {
      AddFriends(dsu);
    } else {
      std::cin >> student;
      std::cout << dsu.Power(student) << '\n';
    }
  }
  return 0;
}