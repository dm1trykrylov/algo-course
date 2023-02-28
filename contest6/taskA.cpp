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

template <class VType = size_t, class EType = std::pair<VType, VType>>
class Visitor {
 public:
  virtual void DiscoverVertex(const VType& vertex) = 0;
  virtual void TreeEdge(const EType& edge) = 0;
  ~Visitor() = default;
};

template <class VType = size_t, class EType = std::pair<VType, VType>>
class BFSVisitor : public Visitor<VType, EType> {
 public:
  void DiscoverVertex(const VType& vertex) final { parents_[vertex] = vertex; }
  void TreeEdge(const EType& edge) final {
    const VType& from = edge.first;
    const VType& to_v = edge.second;
    parents_[to_v] = from;
  }

  VType Parent(const VType& vertex) { return parents_[vertex]; }

 private:
  static constexpr size_t kInfty = std::numeric_limits<size_t>::max();
  std::unordered_map<VType, VType> parents_;
};

template <class Graph, class Visitor>
class BFSImpl {
 private:
  Graph& graph_;
  Visitor& visitor_;
  enum Colors { White, Grey, Black };
  using VType = typename Graph::VertexType;
  using EType = typename Graph::EdgeType;
  using Color = char;
  std::unordered_map<VType, Color> colors_;

 public:
  BFSImpl(Graph& graph, Visitor& visitor) : graph_(graph), visitor_(visitor) {}

  void BFS(typename Graph::VertexType from) {
    auto vertexes = graph_.Vertexes();
    for (auto& vertex : vertexes) {
      colors_[vertex] = White;
    };
    std::queue<typename Graph::VertexType> bfs_queue;
    bfs_queue.push(from);
    colors_[from] = Grey;
    visitor_.DiscoverVertex(from);
    while (!bfs_queue.empty()) {
      from = bfs_queue.front();

      bfs_queue.pop();
      auto neighbours = graph_.NeighboursIt(from, [&](const EType& edge) {
        return colors_[edge.second] == White;
      });
      for (auto edge : neighbours) {
        colors_[edge.second] = Grey;
        bfs_queue.push(edge.second);
        visitor_.TreeEdge(edge);
      }
      colors_[from] = Black;
    }
  }
};

template <class Graph>
bool GetPath(typename Graph::VertexType source,
             typename Graph::VertexType target,
             BFSVisitor<typename Graph::VertexType>& visitor,
             std::vector<typename Graph::VertexType>& path) {
  if (visitor.Parent(target) == 0) {
    return false;  // to has not been accessed
  }
  path.push_back(target);
  auto old_to = target;
  target = visitor.Parent(target);
  while (target != source) {
    path.push_back(target);
    target = visitor.Parent(target);
  }
  if (old_to != source) {
    path.push_back(source);
  }
  std::reverse(path.begin(), path.end());
  return true;
}

template <class VType = size_t, class EType = std::pair<VType, VType>>
void ReadGraph(std::vector<EType>& edges_list,
               std::vector<VType>& vertex_list) {
  for (auto& edge : edges_list) {
    std::cin >> edge.first >> edge.second;
  }
  size_t cnt = 1;
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

int main() {
  FastIO();
  size_t vertexes;
  size_t edges;
  std::cin >> vertexes >> edges;
  size_t source;
  size_t target;
  std::cin >> source >> target;

  std::vector<std::pair<size_t, size_t>> edges_list(edges);
  std::vector<size_t> vertex_list(vertexes);
  ReadGraph(edges_list, vertex_list);

  BFSVisitor<size_t> visitor;
  UndirectedListGraph<size_t> graph(vertex_list, edges_list);
  BFSImpl<UndirectedListGraph<size_t>, Visitor<size_t>> bfs(graph, visitor);
  std::vector<size_t> path;
  bfs.BFS(source);
  if (GetPath<UndirectedListGraph<size_t>>(source, target, visitor, path)) {
    std::cout << path.size() - 1 << '\n';
    Print(path, ' ');
  } else {
    std::cout << -1;
  }
  return 0;
}