// 87301300	
#include <algorithm>
#include <functional>
#include <iostream>
#include <limits>
#include <map>
#include <numeric>
#include <queue>
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
  virtual std::vector<VType> Vertexes() const = 0;

  virtual typename std::vector<EType>::iterator NeighboursBegin(
      VType vertex) = 0;
  virtual typename std::vector<EType>::iterator NeighboursEnd(VType vertex) = 0;
  virtual IteratorImpl<VType, EType> NeighboursIt(
      VType vertex, const FilterFunction<EType>& filter) = 0;

  virtual ~Graph() = default;
};

template <class VType = size_t, class EType = std::pair<VType, VType>>
class MatrixGraph : public Graph<VType, EType> {
  MatrixGraph(const std::vector<VType>& vertexes,
              const std::vector<EType>& edges)
      : vertexes_(vertexes),
        num_vertex_(vertexes.size()),
        num_edges_(edges.size()) {
    adjacency_matrix_.resize(num_vertex_);
    for (auto row : adjacency_matrix_) {
      row.resize(num_vertex_);
    }
    for (auto& edge : edges) {
      adjacency_matrix_[edge.first][edge.second] = edge;
    }
  }

  size_t EdgeCount() const final { return num_edges_; }

  size_t VertexCount() const final { return num_vertex_; }

  typename std::vector<EType>::iterator NeighboursBegin(VType vertex) final {
    return adjacency_matrix_[vertex].begin();
  }

  typename std::vector<EType>::iterator NeighboursEnd(VType vertex) final {
    return adjacency_matrix_[vertex].end();
  }

  std::vector<VType> Vertexes() const final { return vertexes_; }

  IteratorImpl<VType, EType> NeighboursIt(
      VType vertex, const FilterFunction<EType>& filter) override {
    return {vertex, NeighboursBegin(vertex), NeighboursEnd(vertex), filter};
  }

 protected:
  std::vector<std::vector<EType>> adjacency_matrix_;
  std::vector<VType> vertexes_;
  size_t num_vertex_;
  size_t num_edges_;
};

template <class VType = size_t, class EType = std::pair<VType, VType>>
class UndirectedMatrixGraph : public MatrixGraph<VType, EType> {
 public:
  using MatrixGraph<VType, EType>::adjacency_lists_;
  UndirectedMatrixGraph(const std::vector<VType>& vertexes,
                        const std::vector<EType>& edges)
      : MatrixGraph<VType, EType>(vertexes, edges) {
    for (auto& edge : edges) {
      adjacency_lists_[edge.first].push_back(edge);
      adjacency_lists_[edge.second].push_back(edge);
      auto& back_edge = *adjacency_lists_[edge.second].rbegin();
      std::swap(back_edge.first, back_edge.second);
    }
  }
};

template <class VType = size_t, class EType = std::pair<VType, VType>>
class ListGraph : public Graph<VType, EType> {
 public:
  ListGraph(const std::vector<VType>& vertexes, const std::vector<EType>& edges)
      : vertexes_(vertexes),
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

  std::vector<VType> Vertexes() const final { return vertexes_; }

  IteratorImpl<VType, EType> NeighboursIt(
      VType vertex, const FilterFunction<EType>& filter) override {
    return {vertex, NeighboursBegin(vertex), NeighboursEnd(vertex), filter};
  }

 protected:
  std::unordered_map<VType, std::vector<EType>> adjacency_lists_;
  std::vector<VType> vertexes_;
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

template <>
struct std::hash<std::pair<size_t, size_t>> {
  std::size_t operator()(std::pair<size_t, size_t> const& pair) const noexcept {
    auto hasher = std::hash<size_t>{};
    size_t hash = hasher(pair.first) + hasher(pair.second);
    return hash;
  }
};

// abstract metric
template <typename T>
class AbstractMetric {
 public:
  virtual T operator()(size_t from, size_t dest) noexcept = 0;
  virtual void SetDist(size_t from, size_t dest, T distance) = 0;
};

// distance-based metric
template <typename T>
class DistMetric : public AbstractMetric<T> {
 private:
  std::unordered_map<std::pair<size_t, size_t>, T> dist_;

 public:
  DistMetric() {}

  void SetDist(size_t from, size_t dest, T distance) override {
    dist_[{std::min(from, dest), std::max(from, dest)}] = distance;
  }

  T operator()(size_t from, size_t dest) noexcept override {
    return dist_[{std::min(from, dest), std::max(from, dest)}];
  }
};

template <class T, class VType = size_t, class EType = std::pair<VType, VType>>
class DijkstraVisitor {
 public:
  DijkstraVisitor(AbstractMetric<T>& metric, T max_value)
      : metric_(metric), kInfty(max_value) {}
  void InitializeVertex(const VType& vertex) { distances_[vertex] = kInfty; }
  void SetDistance(const VType& vertex, T value) { distances_[vertex] = value; }
  bool EdgeRelaxed(const EType& edge) {
    auto from = edge.first;
    auto dest = edge.second;
    if (distances_[dest] > distances_[from] + metric_(from, dest)) {
      distances_[dest] = distances_[from] + metric_(from, dest);
      parents_[dest] = from;
      return true;
    }
    return false;
  }
  VType Parent(const VType& vertex) const { return parents_[vertex]; }
  T Distance(const VType& vertex) { return distances_[vertex]; }

 private:
  std::unordered_map<VType, VType> parents_;
  std::unordered_map<VType, T> distances_;
  AbstractMetric<T>& metric_;
  const T kInfty;
};

// Dijkstra's algorithm
template <class Graph, typename T>
class Dijkstra {
 private:
  Graph& graph_;
  using VType = typename Graph::VertexType;
  using EType = typename Graph::EdgeType;
  using Visitor = DijkstraVisitor<T, VType, EType>;
  Visitor& visitor_;

 public:
  Dijkstra(Graph& graph, Visitor& visitor) : graph_(graph), visitor_(visitor) {}

  void operator()(VType from) {
    auto vertexes = graph_.Vertexes();
    for (auto vertex : vertexes) {
      visitor_.InitializeVertex(vertex);
    }
    visitor_.SetDistance(from, 0);
    using VertexAndDist = std::pair<VType, T>;
    std::priority_queue<VertexAndDist, std::vector<VertexAndDist>,
                        std::greater<VertexAndDist>>
        queue;
    queue.emplace(visitor_.Distance(from), from);
    while (!queue.empty()) {
      // get nearest vertex
      auto [dist, current] = queue.top();
      queue.pop();
      // update distance for neighbours
      if (dist <= visitor_.Distance(current)) {
        auto neighbours =
            graph_.NeighboursIt(current, [&](const EType&) { return true; });
        for (auto& edge : neighbours) {
          auto neighbour = edge.second;
          if (visitor_.EdgeRelaxed(edge)) {
            queue.emplace(visitor_.Distance(neighbour), neighbour);
          }
        }
      }
    }
  }

  T Distance(VType vertex) { return visitor_.Distance(vertex); }
};

template <class VType = size_t, class EType = std::pair<VType, VType>,
          typename T>
void ReadGraph(std::vector<EType>& edges_list, std::vector<VType>& vertex_list,
               AbstractMetric<T>& metric) {
  T weight;
  for (auto& [from, to] : edges_list) {
    std::cin >> from >> to >> weight;
    metric.SetDist(from, to, weight);
  }
  size_t cnt = 0;
  for (auto& vertex : vertex_list) {
    vertex = cnt++;
  };
}

template <typename T>
void Print(std::vector<T>& array, char delim = '\n') {
  for (auto element : array) {
    std::cout << element << delim;
  }
}

void FastIO() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  std::cout.tie(nullptr);
}

void ReadGraphAndFindPath() {
  size_t vertexes;
  size_t edges;
  std::cin >> vertexes >> edges;

  DistMetric<size_t> dist;

  std::vector<std::pair<size_t, size_t>> edges_list(edges);
  std::vector<size_t> vertex_list(vertexes);
  ReadGraph(edges_list, vertex_list, dist);
  size_t source;
  std::cin >> source;
  const size_t kMaxDistance = 2009000999;
  DijkstraVisitor<size_t, size_t> visitor(dist, kMaxDistance);
  UndirectedListGraph<size_t> graph(vertex_list, edges_list);
  Dijkstra<UndirectedListGraph<size_t>, size_t> dijkstra(graph, visitor);

  dijkstra(source);
  for (auto vertex : graph.Vertexes()) {
    std::cout << dijkstra.Distance(vertex) << ' ';
  }
}

int main() {
  FastIO();
  size_t maps_count;
  std::cin >> maps_count;
  for (size_t i = 0; i < maps_count; ++i) {
    ReadGraphAndFindPath();
    std::cout << '\n';
  }
  return 0;
}
