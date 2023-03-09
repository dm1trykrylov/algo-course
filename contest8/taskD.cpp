#include <algorithm>
#include <iostream>
#include <numeric>
#include <vector>

class DST {
 public:
  DST(size_t n) : parent_(n), rank_(n, 0) {
    for (size_t i = 0; i < n; ++i) {
      parent_[i] = i;
    }
  }

  bool IsSame(size_t u, size_t v) { return Find(u) == Find(v); }

  size_t Find(size_t v) {  // ищем корень
    if (parent_[v] == v) return v;
    parent_[v] = Find(parent_[v]);  // эвристика сжатия пути
    return parent_[v];
  }

  void Union(size_t u, size_t v) {
    size_t root_u = Find(u);
    size_t root_v = Find(v);
    if (root_u != root_v) {  // используем ранговую эвристику
      if (rank_[root_u] < rank_[root_v]) {
        std::swap(root_v, root_u);
      }
      parent_[root_v] = root_u;  // подвесили
      if (rank_[root_u] == rank_[root_v]) {
        rank_[root_u] += 1;  // старший брат имеет ранг на 1 больш!
      }
    }
  }

 private:
  std::vector<size_t> parent_;
  std::vector<size_t> rank_;
};

struct Edge {
  size_t from;
  size_t to;
  size_t weight;
};

std::vector<Edge> MSTKruscal(size_t n, std::vector<Edge>& edges) {
  std::sort(edges.begin(), edges.end(), [](const Edge& e1, const Edge& e2) {
    return e1.weight < e2.weight;
  });

  DST dst_vertexes(n + 1);
  std::vector<Edge> answer;
  answer.reserve(n - 1);
  size_t founded = 0;
  for (auto& edge : edges) {
    if (!dst_vertexes.IsSame(edge.from, edge.to)) {
      ++founded;
      dst_vertexes.Union(edge.from, edge.to);
      answer.push_back(edge);
    }
    if (founded == n - 1) {
      break;
    }
  }
  return answer;
}

int main() {
  size_t n;
  std::cin >> n;
  std::vector<Edge> edges;
  size_t weight;
  for (size_t from = 1; from <= n; ++from) {
    for (size_t to = 1; to <= n; ++to) {
      std::cin >> weight;
      if (from > to) {
        edges.push_back({from, to, weight});
      }
    }
  }
  for (size_t to = 1; to <= n; ++to) {
    std::cin >> weight;
    edges.push_back({0, to, weight});
  }

  auto mst = MSTKruscal(n + 1, edges);
  std::cout << std::accumulate(
      mst.begin(), mst.end(), 0ULL,
      [](size_t acc, const Edge& edge) { return acc + edge.weight; });
}