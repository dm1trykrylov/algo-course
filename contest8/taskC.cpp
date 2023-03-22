// 84431076
#include <algorithm>
#include <bitset>
#include <functional>
#include <iostream>
#include <limits>
#include <map>
#include <numeric>
#include <queue>
#include <set>
#include <unordered_map>
#include <vector>

template <class T>
using FilterFunction = std::function<bool(const T)>;

template <class VType = size_t, class EType = std::pair<VType, VType>>
class IteratorImpl {
 public:
  using Iterator = IteratorImpl;

  IteratorImpl(VType vertex, typename std::vector<EType>::iterator begin,
               typename std::vector<EType>::iterator end,
               const FilterFunction<EType>& filter)
      : v_(vertex), now_(begin), end_(end), kFilter(filter) {
    if (now_ != end_) {
      now_edge_ = *now_;
      if (!kFilter(now_edge_)) {
        ++*this;
      }
    }
  }

  IteratorImpl& operator++() {
    do {
      ++now_;
      if (now_ == end_) {
        return *this;
      }
      now_edge_ = *now_;
    } while (!kFilter(now_edge_) && now_ != end_);
    return *this;
  }

  bool operator==(const IteratorImpl& other) const {
    return now_ == other.now_;
  }

  bool operator!=(const IteratorImpl& other) const {
    return now_ != other.now_;
  }

  // NOLINTNEXTLINE
  IteratorImpl begin() const { return *this; }

  // NOLINTNEXTLINE
  IteratorImpl end() const { return {v_, end_, end_, kFilter}; }

  const EType& operator*() { return now_edge_; }

 private:
  EType now_edge_;
  VType v_;
  typename std::vector<EType>::iterator now_;
  typename std::vector<EType>::iterator end_;
  const FilterFunction<EType> kFilter;
};

template <class VType = size_t, class EType = std::pair<VType, VType>>
class Graph {
 public:
  using VertexType = VType;
  using EdgeType = EType;

  virtual size_t EdgeCount() const = 0;
  virtual size_t VertexCount() const = 0;
  virtual std::vector<VType> Vertices() const = 0;

  virtual typename std::vector<EType>::iterator NeighboursBegin(
      VType vertex) = 0;
  virtual typename std::vector<EType>::iterator NeighboursEnd(VType vertex) = 0;
  virtual IteratorImpl<VType, EType> NeighboursIt(
      VType vertex, const FilterFunction<EType>& filter) = 0;

  virtual ~Graph() = default;
};

template <class VType = size_t, class EType = std::pair<VType, VType>>
class ListGraph : public Graph<VType, EType> {
 public:
  ListGraph(const std::vector<VType>& vertexes, const std::vector<EType>& edges)
      : vertexes_(vertexes),
        edges_(edges),
        num_vertex_(vertexes.size()),
        num_edges_(edges.size()) {
    for (auto& edge : edges) {
      adjacency_lists_[edge.first].push_back(edge);
    }
  }

  size_t EdgeCount() const final { return num_edges_; }

  size_t VertexCount() const final { return num_vertex_; }

  typename std::vector<EType>::iterator NeighboursBegin(VType vertex) final {
    return adjacency_lists_[vertex].begin();
  }

  typename std::vector<EType>::iterator NeighboursEnd(VType vertex) final {
    return adjacency_lists_[vertex].end();
  }

  std::vector<VType> Vertices() const final { return vertexes_; }

  std::vector<EType> Edges() const { return edges_; }

  IteratorImpl<VType, EType> NeighboursIt(
      VType vertex, const FilterFunction<EType>& filter) override {
    return {vertex, NeighboursBegin(vertex), NeighboursEnd(vertex), filter};
  }

 protected:
  std::unordered_map<VType, std::vector<EType>> adjacency_lists_;
  std::vector<VType> vertexes_;
  std::vector<EType> edges_;
  size_t num_vertex_;
  size_t num_edges_;
};

template <class VType = size_t, class EType = std::pair<VType, VType>>
class UndirectedListGraph : public ListGraph<VType, EType> {
 public:
  using ListGraph<VType, EType>::adjacency_lists_;
  UndirectedListGraph(const std::vector<VType>& vertexes,
                      const std::vector<EType>& edges)
      : ListGraph<VType, EType>(vertexes, edges) {
    for (auto& edge : edges) {
      adjacency_lists_[edge.second].push_back(edge);
      // by default all edges from v should have edge.first = v
      auto& back_edge = *adjacency_lists_[edge.second].rbegin();
      std::swap(back_edge.first,
                back_edge.second);  // that's why we reverse this edge
    }
  }
};

class DSU {
 public:
  DSU(size_t n) : parent_(n), rank_(n, 0) {
    for (size_t i = 0; i < n; ++i) {
      parent_[i] = i;
    }
  }

  bool IsSame(size_t lhs, size_t rhs) { return Find(lhs) == Find(rhs); }

  size_t Find(size_t vertex) {  // ищем корень
    if (parent_[vertex] == vertex) {
      return vertex;
    }
    parent_[vertex] = Find(parent_[vertex]);  // эвристика сжатия пути
    return parent_[vertex];
  }

  void Union(size_t lhs, size_t rhs) {
    size_t root_u = Find(lhs);
    size_t root_v = Find(rhs);
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
  size_t first;
  size_t second;
  size_t weight;
};

// Kruskal's algorithm
// Returns Minimal spanning tree (ListGraph object)
template <class VType = size_t, class EType>
ListGraph<VType, EType> MSTKruscal(ListGraph<VType, EType>& graph) {
  auto edges = graph.Edges();
  std::sort(edges.begin(), edges.end(), [](const Edge& lhs, const Edge& rhs) {
    return lhs.weight < rhs.weight;
  });

  DSU dst_vertexes(graph.VertexCount() + 1);
  std::vector<Edge> answer;
  answer.reserve(graph.VertexCount() - 1);
  size_t found = 0;
  for (auto& edge : edges) {
    if (!dst_vertexes.IsSame(edge.first, edge.second)) {
      ++found;
      dst_vertexes.Union(edge.first, edge.second);
      answer.push_back(edge);
    }
    if (found == graph.VertexCount() - 1) {
      break;
    }
  }
  auto spanning_tree = ListGraph<VType, EType>(graph.Vertices(), answer);
  return spanning_tree;
}

// Prim's algotithm
const size_t kInf = 1e6;
template <class VType = size_t, class EType>
ListGraph<VType, EType> MSTPrim(ListGraph<VType, EType>& graph) {
  std::multiset<std::pair<VType, size_t>> queue;
  std::unordered_map<VType, size_t> key;
  std::unordered_map<VType, bool> in_queue;

  for (auto vertex : graph.Vertices()) {
    key[vertex] = kInf;
    if (vertex == 1) {  // Assuming 1 is the starting vertex
      key[vertex] = 0;
    }
    in_queue[vertex] = true;
    queue.insert({key[vertex], vertex});
  }
  std::vector<EType> mst_edges;
  while (!queue.empty()) {
    auto top = *queue.rbegin();
    queue.erase(queue.rbegin());
    in_queue[top.second] = false;
    auto neighbours = graph.NeighboursIt(
        top.second, [&](const EType& edge) { return in_queue[edge.second]; });
    for (auto& edge : neighbours) {
      if (key[edge.second] > edge.weight) {
        queue.erase({key[edge.second], edge.second});
        key[edge.second] = edge.weight;
        queue.insert({key[edge.second], edge.second});
        mst_edges.push_back(edge);
      }
    }
  }
  return ListGraph(graph.Vertices(), mst_edges);
}

int main() {
  size_t vertex_count;
  size_t edges_count;
  std::cin >> vertex_count >> edges_count;
  std::vector<Edge> edges;
  for (size_t i = 0; i < edges_count; ++i) {
    size_t from;
    size_t dest;
    size_t weight;
    std::cin >> from >> dest >> weight;
    edges.push_back({from, dest, weight});
  }
  std::vector<size_t> vertex_list(vertex_count);
  for (size_t i = 0; i < vertex_count; ++i) {
    vertex_list.push_back(i + 1);
  }
  ListGraph<size_t, Edge> graph(vertex_list, edges);

  auto mstedges = MSTKruscal(graph).Edges();
  std::cout << std::accumulate(
      mstedges.begin(), mstedges.end(), 0ULL,
      [](size_t acc, const Edge& edge) { return acc + edge.weight; });
}