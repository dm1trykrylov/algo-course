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

/* Own hash! */
template <>
struct std::hash<std::pair<size_t, size_t>> {
  std::size_t operator()(std::pair<size_t, size_t> const& s) const noexcept {
    auto hasher = std::hash<size_t>{};
    size_t hash = hasher(s.first) + hasher(s.second);
    return hash;
  }
};

// класс абстрактной метрики
template <typename T>
class AbstractMetric {
 public:
  virtual T operator()(size_t from, size_t to) noexcept = 0;
  virtual void SetDist(size_t from, size_t to, T dist) = 0;
};

template <typename T>
class DistMetric : public AbstractMetric<T> {
 private:
  std::unordered_map<std::pair<size_t, size_t>, T> dist_;

 public:
  DistMetric() {}

  void SetDist(size_t from, size_t to, T dist) override {
    dist_[{std::min(from, to), std::max(from, to)}] = dist;
  }

  T operator()(size_t from, size_t to) noexcept override {
    return dist_[{std::min(from, to), std::max(from, to)}];
  }
};

template <class VType = size_t, class EType = std::pair<VType, VType>>
class DijkstraVisitor {
 public:
  void EdgeRelaxed(const EType& edge) { parents_[edge.second] = edge.first; }
  VType Parent(const VType& vertex) { return parents_[vertex]; }

 private:
  static constexpr size_t kInfty = std::numeric_limits<size_t>::max();
  std::unordered_map<VType, VType> parents_;
};

template <class Graph, class Visitor, typename T>
class Dijkstra {
 private:
  Graph& graph_;
  using VType = typename Graph::VertexType;
  using EType = typename Graph::EdgeType;
  DijkstraVisitor<VType, EType>& visitor_;
  std::unordered_map<VType, T> distances_;

 public:
  Dijkstra(Graph& graph, Visitor& visitor) : graph_(graph), visitor_(visitor) {}

  void operator()(VType from, T max_distance, AbstractMetric<T>& metric) {
    auto vertexes = graph_.Vertexes();
    for (auto vertex : vertexes) {
      distances_[vertex] = max_distance;
    }
    distances_[from] = 0;

    std::priority_queue<std::pair<VType, T>, std::vector<std::pair<VType, T>>,
                        std::greater<std::pair<VType, T>>>
        queue;

    queue.emplace(distances_[from], from);

    while (!queue.empty()) {
      auto[dist, current] = queue.top();
      queue.pop();
      if (dist > distances_[current]) {
        continue;
      }
      auto neighbours =
          graph_.NeighboursIt(current, [&](const EType&) { return true; });
      for (auto& edge : neighbours) {
        auto neighbour = edge.second;
        T distance = metric(current, neighbour);
        if (distances_[current] + distance < distances_[neighbour]) {
          distances_[neighbour] = distances_[current] + distance;
          queue.emplace(distances_[neighbour], neighbour);
          visitor_.EdgeRelaxed(edge);
        }
      }
    }
  }

  T Distance(VType vertex) { return distances_[vertex]; }
};

template <class VType = size_t, class EType = std::pair<VType, VType>,
          typename T>
void ReadGraph(std::vector<EType>& edges_list, std::vector<VType>& vertex_list,
               AbstractMetric<T>& metric) {
  T weight;
  for (auto & [ from, to ] : edges_list) {
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

  DijkstraVisitor<size_t> visitor;
  UndirectedListGraph<size_t> graph(vertex_list, edges_list);
  Dijkstra<UndirectedListGraph<size_t>, DijkstraVisitor<size_t>, size_t>
      dijkstra(graph, visitor);
  dijkstra(source, 2009000999, dist);
  for (auto v : graph.Vertexes()) {
    std::cout << dijkstra.Distance(v) << ' ';
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