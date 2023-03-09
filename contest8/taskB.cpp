#include <algorithm>
#include <iostream>
#include <numeric>
#include <set>
#include <vector>

class DSU {
 public:
  DSU(size_t n) : parent_(n, 0), rank_(n, 0) {
    for (size_t i = 0; i < n; ++i) {
      parent_[i] = i;
    }
  }

  bool IsSame(size_t lhs, size_t rhs) { return Find(lhs) == Find(rhs); }

  size_t Find(size_t vertex) {  // find root
    if (parent_[vertex] == vertex) {
      return vertex;
    }
    parent_[vertex] = Find(parent_[vertex]);
    return parent_[vertex];
  }

  void Union(size_t lhs, size_t rhs) {
    size_t root_u = Find(lhs);
    size_t root_v = Find(rhs);
    if (root_u != root_v) {  // rank heuristic
      if (rank_[root_u] < rank_[root_v]) {
        std::swap(root_v, root_u);
      }
      parent_[root_v] = root_u;
      if (rank_[root_u] == rank_[root_v]) {
        rank_[root_u] += 1;
      }
    }
  }

 private:
  std::vector<size_t> parent_;
  std::vector<size_t> rank_;
};

template <>
struct std::hash<std::pair<size_t, size_t>> {
  std::size_t operator()(std::pair<size_t, size_t> const& pair) const noexcept {
    auto hasher = std::hash<size_t>{};
    auto hash1 = hasher(pair.first);
    auto hash2 = hasher(pair.second);
    if (hash1 != hash2) {
      return hash1 ^ hash2;
    }
    return hash1;
  }
};

void inline FastIO() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  std::cout.tie(nullptr);
}

struct Query {
  char type;
  size_t from;
  size_t to;
};

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  std::cout.tie(nullptr);
  size_t neurons_count;
  size_t connections_count;
  size_t queries_count;
  std::cin >> neurons_count >> connections_count >> queries_count;
  using Connection = std::pair<size_t, size_t>;
  std::set<Connection> connections;
  size_t from, to;
  for (size_t i = 0; i < connections_count; ++i) {
    std::cin >> from >> to;
    connections.insert({std::min(from, to), std::max(from, to)});
  }
  std::vector<Query> queries(queries_count);
  std::string type;
  for (size_t i = 0; i < queries_count; ++i) {
    std::cin >> type >> from >> to;
    queries[i] = {type[0], from, to};
    if (type == "cut") {
      connections.erase({std::min(from, to), std::max(from, to)});
    }
  }

  DSU dsu(neurons_count + 1);
  for (const auto& connection : connections) {
    dsu.Union(connection.first, connection.second);
  }
  std::vector<char> answers;
  answers.reserve(queries_count);
  for (size_t i = queries_count; i > 0; --i) {
    from = queries[i - 1].from;
    to = queries[i - 1].to;
    if (queries[i - 1].type == 'c') {
      dsu.Union(from, to);
    } else {
      answers.push_back(dsu.Find(from) == dsu.Find(to) ? 'y' : 'n');
    }
  }
  for (size_t i = answers.size(); i > 0; --i) {
    std::cout << (answers[i - 1] == 'y' ? "YES" : "NO") << '\n';
  }
  return 0;
}