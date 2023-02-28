#include <algorithm>
#include <functional>
#include <iostream>
#include <limits>
#include <map>
#include <numeric>
#include <unordered_map>
#include <vector>

namespace Graph {
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
};  // namespace Graph

namespace Visitor {
template <class VType = size_t, class EType = std::pair<VType, VType>>
class Visitor {
 public:
  virtual void DiscoverVertex(const VType& vertex) = 0;
  virtual void TreeEdge(const EType& edge) = 0;
  virtual void BackEdge(const EType& edge) = 0;
  virtual void FinishEdge(const EType& edge) = 0;
  ~Visitor() = default;
};

template <class VType = size_t, class EType = std::pair<VType, VType>>
class DFSVisitor : public Visitor<VType, EType> {
 public:
  void InitializeTimer(size_t vertex_count) {
    timer_.in.resize(vertex_count + 1, kInfty);
    timer_.up.resize(vertex_count + 1, kInfty);
    timer_.time = 0;
  }
  void DiscoverVertex(const VType& vertex) final {
    timer_.in[vertex] = timer_.time++;
    timer_.up[vertex] = timer_.in[vertex];
  }
  void TreeEdge(const EType& edge) final {
    const VType& from = edge.first;
    const VType& to_v = edge.second;
    timer_.up[from] = std::min(timer_.up[from], timer_.up[to_v]);
  }
  void BackEdge(const EType& edge) final {
    const VType& from = edge.first;
    const VType& to_v = edge.second;
    timer_.up[from] = std::min(timer_.up[from], timer_.in[to_v]);
  }
  void FinishEdge(const EType& edge) final {
    const VType& from = edge.first;
    const VType& to_v = edge.second;
    if (timer_.up[to_v] > timer_.in[from]) {
      bridges_.push_back(edge);
      is_bridge_[edge] = true;
    }
  }

  bool IsBridge(const EType& edge) { return is_bridge_[edge]; }

  std::vector<EType> GetBridges() { return bridges_; }

 private:
  static constexpr size_t kInfty = std::numeric_limits<size_t>::max();
  struct Time {
    std::vector<size_t> in;
    std::vector<size_t> up;
    size_t time;
  };
  Time timer_;
  std::map<EType, bool> is_bridge_;
  std::vector<EType> bridges_;
};

}  // namespace Visitor

template <class Graph, class Visitor>
class DFSImpl {
 private:
  Graph& graph_;
  Visitor& visitor_;
  enum Colors { White, Grey, Black };
  using VType = typename Graph::VertexType;
  using EType = typename Graph::EdgeType;
  using Color = char;
  std::unordered_map<VType, Color> colors_;

 public:
  DFSImpl(Graph& graph, Visitor& visitor) : graph_(graph), visitor_(visitor) {}

  void DFS(size_t from_edge_id, typename Graph::VertexType from) {
    visitor_.DiscoverVertex(from);
    colors_[from] = Grey;
    auto neighbours = graph_.NeighboursIt(from, [&](const EType& edge) {
      return (colors_[edge.second] != Black) && (edge.second != from) &&
             (edge.id != from_edge_id);
    });
    for (auto edge : neighbours) {
      auto to_v = edge.second;
      if (colors_[to_v] == Grey) {
        visitor_.BackEdge(edge);
      } else {
        DFS(edge.id, edge.second);
        visitor_.TreeEdge(edge);
      }
      visitor_.FinishEdge(edge);
    };
    colors_[from] = Black;
  }

  void FindBridges() {
    auto vertexes = graph_.Vertexes();
    for (auto& vertex : vertexes) {
      colors_[vertex] = White;
    };
    visitor_.InitializeTimer(vertexes.size());
    for (auto vertex : vertexes) {
      if (colors_[vertex] == White) {
        DFS(0, vertex);
      }
    }
  }
};

template <class VType = size_t>
struct Edge {
  VType first = 0;
  VType second = 0;
  uint32_t id = 0;
};

template <class VType = size_t>
bool operator<(const Edge<VType>& lhs, const Edge<VType>& rhs) {
  return lhs.id < rhs.id;
}

template <class VType = size_t>
bool operator==(const Edge<VType>& lhs, const Edge<VType>& rhs) {
  return lhs.id == rhs.id;
}

void FastIO() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  std::cout.tie(nullptr);
}

template <class VType = size_t, class EType = std::pair<VType, VType>>
void ReadGraph(std::vector<EType>& edges_list, std::vector<VType>& vertexes) {
  size_t edge_id = 0;
  for (auto& edge : edges_list) {
    std::cin >> edge.first >> edge.second;
    edge.id = ++edge_id;
  }
  size_t cnt = 1;
  for (auto& vertex : vertexes) {
    vertex = cnt++;
  };
}

template <typename EType>
void PrintBridges(std::vector<EType>& bridges) {
  std::sort(bridges.begin(), bridges.end());
  std::cout << bridges.size() << '\n';
  for (auto bridge : bridges) {
    std::cout << bridge.id << '\n';
  }
}

int main() {
  FastIO();
  size_t vertexes;
  size_t edges;
  std::cin >> vertexes >> edges;
  using EType = Edge<uint32_t>;
  std::vector<EType> edges_list(edges);
  std::vector<uint32_t> vertex_list(vertexes);
  ReadGraph(edges_list, vertex_list);
  Graph::UndirectedListGraph<uint32_t, EType> graph(vertex_list, edges_list);

  Visitor::DFSVisitor<uint32_t, EType> visitor;

  DFSImpl<Graph::UndirectedListGraph<uint32_t, EType>,
          Visitor::DFSVisitor<uint32_t, EType>>
      dfs(graph, visitor);
  dfs.FindBridges();
  std::vector<EType> bridges = visitor.GetBridges();
  PrintBridges<EType>(bridges);

  return 0;
}