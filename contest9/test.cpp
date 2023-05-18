// 87305639
#include <functional>
#include <iostream>
#include <limits>
#include <numeric>
#include <optional>
#include <stack>
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
    } else {
      now_edge_ = EType();
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

struct Edge {
  int64_t first;     // source
  int64_t second;    // destinaton
  int64_t capacity;  // edge max capacity
  int64_t flow;      // flow over edge
  Edge* back;

  int64_t CurrentCapacity() const { return capacity - flow; }
  Edge() : Edge(-1, -1, 0) {}
  Edge(int64_t from, int64_t to_vertex, int64_t capacity, Edge* back = nullptr)
      : first(from),
        second(to_vertex),
        capacity(capacity),
        flow(0),
        back(back) {}
};

template <class VType = size_t>
class ListGraph : public Graph<VType, Edge*> {
 public:
  using EType = Edge*;
  ListGraph(const std::vector<VType>& vertexes, const std::vector<EType>& edges)
      : vertices_(vertexes),
        edges_(edges),
        num_vertex_(vertexes.size()),
        num_edges_(edges.size()) {
    for (const auto& edge : edges) {
      adjacency_lists_[edge->first].push_back(edge);
    }
  }

  void AddEdge(int64_t from, int64_t to_vertex, int64_t capacity) {
    edges_.emplace_back(new Edge(from, to_vertex, capacity));
    auto* edge = edges_.back();
    edges_.emplace_back(new Edge(to_vertex, from, 0));  // back edge
    auto* back_edge = edges_.back();
    edge->back = back_edge;
    back_edge->back = edge;
    adjacency_lists_[from].push_back(edge);
    adjacency_lists_[to_vertex].push_back(back_edge);
  }

  size_t EdgeCount() const final { return num_edges_; }

  size_t VertexCount() const final { return num_vertex_; }

  typename std::vector<EType>::iterator NeighboursBegin(VType vertex) final {
    return adjacency_lists_[vertex].begin();
  }

  typename std::vector<EType>::iterator NeighboursEnd(VType vertex) final {
    return adjacency_lists_[vertex].end();
  }

  std::vector<VType> Vertices() const final { return vertices_; }

  std::vector<EType> Edges() const { return edges_; }

  IteratorImpl<VType, EType> NeighboursIt(
      VType vertex, const FilterFunction<EType>& filter) override {
    return {vertex, NeighboursBegin(vertex), NeighboursEnd(vertex), filter};
  }

  ~ListGraph() {
    for (auto* edge : edges_) {
      delete edge;
    }
  }

 protected:
  std::unordered_map<VType, std::vector<EType>> adjacency_lists_;
  std::vector<VType> vertices_;
  std::vector<EType> edges_;
  size_t num_vertex_;
  size_t num_edges_;
};

template <class VType = size_t, class EType = std::pair<VType, VType>,
          class T = int64_t>
class Visitor {
 public:
  using ValueType = T;
  virtual void Initialize(const std::vector<VType>& vertices) = 0;
  virtual std::optional<T> DiscoverVertex(VType& vertex) = 0;
  virtual bool CheckEdge(const EType& edge) = 0;
  virtual void TreeEdge(EType& edge) = 0;
  virtual void BackEdge(EType& edge) = 0;
  virtual bool FinishEdge(EType& edge) = 0;
  virtual T FinishVertex(const VType& vertex) = 0;
  virtual void Reset() = 0;
  ~Visitor() = default;
};

template <class VType = int64_t, class T = int64_t>
class IncreasingPathVisitor : public Visitor<VType, Edge*, T> {
 public:
  using EType = Edge*;
  IncreasingPathVisitor(VType source, VType target)
      : source_(source), target_(target), min_flow_(0) {
    flow_stack_.push(kInfinity);
  }
  void Initialize(const std::vector<VType>& vertices) final {
    for (auto vertex : vertices) {
      colors_[vertex] = White;
    }
    min_flow_ = 0;
  }
  std::optional<T> DiscoverVertex(VType& vertex) final {
    if (vertex == target_) {
      min_flow_ = flow_stack_.top();
      return min_flow_;
    }
    colors_[vertex] = Grey;
    return std::nullopt;
  }
  bool CheckEdge(const EType& edge) final {
    return colors_[edge->second] == White && edge->CurrentCapacity() != 0;
  }
  T FinishVertex(const VType& vertex) final {
    colors_[vertex] = Black;
    return T(0);
  }
  void TreeEdge(EType& edge) final {
    flow_stack_.push(std::min(flow_stack_.top(), edge->CurrentCapacity()));
  }
  void BackEdge(EType& edge) final { std::ignore = edge; }
  bool FinishEdge(EType& edge) final {
    T delta = min_flow_;
    flow_stack_.pop();
    if (delta > 0) {
      edge->flow += delta;
      edge->back->flow -= delta;  // back edge
      return true;
    }
    return false;
  }
  void Reset() /*final*/ {
    min_flow_ = 0;
    while (!flow_stack_.empty()) {
      flow_stack_.pop();
    }
    flow_stack_.push(kInfinity);
  }
  bool FoundIncreasingPath() { return min_flow_ > 0; }

 private:
  VType source_;
  VType target_;
  T min_flow_;
  std::stack<T> flow_stack_;
  static constexpr T kInfinity = std::numeric_limits<T>::max();
  enum Colors { White, Grey, Black };
  using Color = char;
  std::unordered_map<VType, Color> colors_;
};

// DFS with return value
template <class Graph, class Visitor>
class DFSImpl {
 private:
  Graph& graph_;

  enum Colors { White, Grey, Black };
  using VType = typename Graph::VertexType;
  using EType = typename Graph::EdgeType;
  using Color = char;
  std::unordered_map<VType, Color> colors_;
  Visitor visitor_;

 public:
  DFSImpl(Graph& graph, Visitor& visitor) : graph_(graph), visitor_(visitor) {
    visitor_.Initialize(graph_.Vertices());
  }

  typename Visitor::ValueType DFS(typename Graph::VertexType from) {
    auto result = visitor_.DiscoverVertex(from);
    if (result.has_value()) {
      return result.value();
    }
    auto neighbours = graph_.NeighboursIt(
        from, [&](const EType& edge) { return visitor_.CheckEdge(edge); });
    for (auto edge : neighbours) {
      auto to_v = edge->second;
      visitor_.TreeEdge(edge);
      auto dfs_value = DFS(to_v);
      // check result
      if (visitor_.FinishEdge(edge)) {
        return dfs_value;
      }
    };
    return visitor_.FinishVertex(from);
  }
};

struct FordFulkerson {
 private:
  ListGraph<int64_t>& graph_;

 public:
  FordFulkerson(ListGraph<int64_t>& graph) : graph_(graph) {}

  int64_t FindMaxFlow(int64_t source, int64_t target) {
    std::vector<uint32_t> used(graph_.VertexCount(), 0);
    while (true) {
      IncreasingPathVisitor<int64_t, int64_t> visitor(source, target);
      DFSImpl<ListGraph<int64_t>, IncreasingPathVisitor<int64_t, int64_t>> dfs(
          graph_, visitor);
      auto delta = dfs.DFS(source);
      if (delta == 0) {
        break;
      }
    }
    int64_t result = 0;
    auto neighbours =
        graph_.NeighboursIt(source, [](const Edge*) { return true; });
    for (auto* edge : neighbours) {
      result += edge->flow;
    }
    return result;
  }
};

void ReadGraph(size_t edges_count, ListGraph<int64_t>& graph) {
  for (size_t i = 0; i < edges_count; ++i) {
    int64_t from;
    int64_t to_v;
    int64_t capacity;
    std::cin >> from >> to_v >> capacity;
    --from;
    --to_v;
    graph.AddEdge(from, to_v, capacity);  // add edge and back edge
  }
}

class Network : public ListGraph<int64_t> {
 public:
  Network(const std::vector<int64_t>& vertexes, const std::vector<Edge*>& edges)
      : ListGraph<int64_t>(vertexes, edges) {}
};

int main() {
  size_t vertices_count;
  size_t edges_count;
  std::cin >> vertices_count >> edges_count;
  std::vector<int64_t> vertices(vertices_count, 0);
  std::vector<Edge*> edges;
  std::iota(vertices.begin(), vertices.end(), 1);
  ListGraph<int64_t> graph(vertices, edges);
  
  ReadGraph(edges_count, graph);
  Network net(vertices, edges);
  auto nbr = net.NeighboursIt(0, [](const Edge* edge){return true;});
  for (auto e : nbr) {
    std::cout << e->second;
  }
  std::cout << net.Vertices()[0];
  auto algo = FordFulkerson(graph);
  std::cout << algo.FindMaxFlow(0, vertices_count - 1) << std::endl;
  for (auto* edge : edges) {
    delete edge;
  }
  
  return 0;
}