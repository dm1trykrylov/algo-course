// 87460551
#include <algorithm>
#include <functional>
#include <iostream>
#include <limits>
#include <numeric>
#include <optional>
#include <queue>
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
  virtual size_t VerticesCount() const = 0;
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

  size_t VerticesCount() const final { return num_vertex_; }

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
  void Reset() final {
    min_flow_ = 0;
    while (!flow_stack_.empty()) {
      flow_stack_.pop();
    }
    flow_stack_.push(kInfinity);
    for (auto& node : colors_) {
      node.second = White;
    }
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

template <>
class IncreasingPathVisitor<int64_t, int64_t>
    : public Visitor<int64_t, Edge*, int64_t> {
 public:
  using EType = Edge*;
  using VType = int64_t;
  using T = int64_t;
  IncreasingPathVisitor(VType source, VType target)
      : source_(source), target_(target), min_flow_(0) {
    flow_stack_.push(kInfinity);
  }
  void Initialize(const std::vector<VType>& vertices) final {
    used_.resize(vertices.size(), 0);
    min_flow_ = 0;
    std::ignore = source_;
  }
  std::optional<T> DiscoverVertex(VType& vertex) final {
    if (vertex == target_) {
      min_flow_ = flow_stack_.top();
      return min_flow_;
    }
    used_[vertex] = phase_;
    return std::nullopt;
  }
  bool CheckEdge(const EType& edge) final {
    return used_[edge->second] != phase_ && edge->CurrentCapacity() != 0;
  }
  T FinishVertex(const VType& vertex) final {
    std::ignore = vertex;
    return 0;
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
  void Reset() final {
    min_flow_ = 0;
    while (!flow_stack_.empty()) {
      flow_stack_.pop();
    }
    flow_stack_.push(kInfinity);
    ++phase_;
  }

 private:
  VType source_;
  VType target_;
  T min_flow_;
  std::stack<T> flow_stack_;
  static constexpr T kInfinity = 1 << 30;
  std::vector<size_t> used_;
  size_t phase_;
};

class MinCutVisitor : public Visitor<int64_t, Edge*, bool> {
 public:
  using EType = Edge*;
  using VType = int64_t;
  using T = bool;
  MinCutVisitor(VType source, VType target) : source_(source), target_(target) {
    std::ignore = source_;
    std::ignore = target_;
  }
  void Initialize(const std::vector<VType>& vertices) final {
    visited_.resize(vertices.size(), false);
  }
  std::optional<T> DiscoverVertex(VType& vertex) final {
    visited_[vertex] = true;
    return std::nullopt;
  }
  bool CheckEdge(const EType& edge) final {
    return !visited_[edge->second] && edge->CurrentCapacity() > 0;
  }
  T FinishVertex(const VType& vertex) final {
    std::ignore = vertex;
    return false;
  }
  void TreeEdge(EType& edge) final { std::ignore = edge; }
  void BackEdge(EType& edge) final { std::ignore = edge; }
  bool FinishEdge(EType& edge) final {
    std::ignore = edge;
    return false;
  }
  bool Visited(VType vertex) const { return visited_[vertex]; }
  void Reset() final {}

 private:
  VType source_;
  VType target_;
  std::vector<bool> visited_;
};

// DFS with return value
template <class Graph, class Visitor>
class DFSImpl {
 private:
  Graph& graph_;
  using VType = typename Graph::VertexType;
  using EType = typename Graph::EdgeType;
  Visitor& visitor_;

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
  void Reset() { visitor_.Reset(); }
};

struct FordFulkerson {
 private:
  ListGraph<int64_t>& graph_;

 public:
  FordFulkerson(ListGraph<int64_t>& graph) : graph_(graph) {}

  int64_t FindMaxFlow(int64_t source, int64_t target) {
    std::vector<uint32_t> used(graph_.VerticesCount(), 0);
    IncreasingPathVisitor<int64_t, int64_t> visitor(source, target);
    DFSImpl<ListGraph<int64_t>, IncreasingPathVisitor<int64_t, int64_t>> dfs(
        graph_, visitor);
    while (true) {
      dfs.Reset();
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

class Encoder {
 public:
  size_t height;
  size_t width;

  size_t CoordinatesToIdx(std::pair<size_t, size_t> point) const {
    return width * point.first + point.second;
  }
  size_t OutIdx(std::pair<size_t, size_t> point) const {
    return OutIdx(CoordinatesToIdx(point));
  }
  size_t InIdx(std::pair<size_t, size_t> point) const {
    return CoordinatesToIdx(point);
  }
  size_t OutIdx(size_t in_idx) const { return in_idx + width * height; }
  size_t InIdx(size_t out_idx) const { return out_idx - width * height; }

  std::pair<size_t, size_t> IdxToCoordinates(size_t idx) {
    if (idx >= width * height) {
      return IdxToCoordinates(idx - width * height);
    }
    return {idx / width, idx % width};
  }
};

std::istream& operator>>(std::istream& stream,
                         std::pair<size_t, size_t>& point) {
  stream >> point.first >> point.second;
  --point.first;
  --point.second;
  return stream;
}

std::ostream& operator<<(std::ostream& stream,
                         std::pair<size_t, size_t> point) {
  stream << point.first + 1 << ' ' << point.second + 1;
  return stream;
}

struct NetworkBuilder {
 private:
  ListGraph<int64_t>& net_;

 public:
  static const int64_t kMaxFlow = 1 << 12;
  NetworkBuilder(ListGraph<int64_t>& net) : net_(net) {}
  void AddNeighbours(std::pair<size_t, size_t> point, Encoder& enc) {
    size_t pt_x = point.first;
    size_t pt_y = point.second;
    size_t idx = enc.InIdx(point);
    size_t lower = enc.InIdx({pt_x + 1, pt_y});
    size_t right = enc.InIdx({pt_x, pt_y + 1});
    net_.AddEdge(enc.OutIdx(idx), lower, kMaxFlow);
    net_.AddEdge(enc.OutIdx(lower), idx, kMaxFlow);
    net_.AddEdge(enc.OutIdx(idx), right, kMaxFlow);
    net_.AddEdge(enc.OutIdx(right), idx, kMaxFlow);
  }

  void SetMountains(Encoder& enc, std::vector<bool>& used, size_t mountains) {
    std::pair<size_t, size_t> cell;
    size_t idx;
    for (size_t i = 0; i < mountains; ++i) {  // can't go through mountains
      std::cin >> cell;
      idx = enc.InIdx(cell);
      used[idx] = true;
      net_.AddEdge(idx, enc.OutIdx(idx), 0);
    }
  }

  void SetAvailableCells(Encoder& enc, std::vector<bool>& used,
                         size_t wall_cells) {
    std::pair<size_t, size_t> cell;
    size_t idx;
    for (size_t i = 0; i < wall_cells; ++i) {
      std::cin >> cell;
      idx = enc.InIdx(cell);
      used[idx] = true;
      net_.AddEdge(idx, enc.OutIdx(idx), 1);
    }
  }

  void ConnectNeighbours(size_t height, size_t width, Encoder& enc) {
    for (size_t i = 0; i < height - 1; ++i) {
      for (size_t j = 0; j < width - 1; ++j) {
        AddNeighbours({i, j}, enc);
      }
    }

    for (size_t i = 0; i < height - 1; ++i) {
      net_.AddEdge(enc.OutIdx({i, width - 1}), enc.InIdx({i + 1, width - 1}),
                   kMaxFlow);
      net_.AddEdge(enc.OutIdx({i + 1, width - 1}), enc.InIdx({i, width - 1}),
                   kMaxFlow);
    }

    for (size_t j = 0; j < width - 1; ++j) {
      net_.AddEdge(enc.OutIdx({height - 1, j}), enc.InIdx({height - 1, j + 1}),
                   kMaxFlow);
      net_.AddEdge(enc.OutIdx({height - 1, j + 1}), enc.InIdx({height - 1, j}),
                   kMaxFlow);
    }
  }

  std::pair<size_t, size_t> BuildNetwork(size_t height, size_t width,
                                         Encoder& enc) {
    size_t mountains;
    size_t wall_cells;
    std::cin >> mountains >> wall_cells;
    std::vector<bool> used((height + 1) * (width + 1) + 2, false);
    SetMountains(enc, used, mountains);
    SetAvailableCells(enc, used, wall_cells);
    std::pair<size_t, size_t> cell;
    size_t idx;
    std::cin >> cell;  // E
    size_t real_source = enc.OutIdx(cell);
    used[enc.InIdx(cell)] = true;
    // net.SetSource(real_source);
    std::cin >> cell;  // C
    size_t real_target = enc.InIdx(cell);
    used[real_target] = true;
    // net.SetTarget(real_target);

    for (size_t i = 0; i < height; ++i) {
      for (size_t j = 0; j < width; ++j) {
        idx = enc.InIdx({i, j});
        if (!used[idx]) {
          net_.AddEdge(idx, enc.OutIdx(idx), kMaxFlow);
        }
      }
    }
    ConnectNeighbours(height, width, enc);
    return {real_source, real_target};
  }
};

void WalkInResidualNetwork(ListGraph<int64_t>& net, size_t vertex,
                           std::vector<bool>& visited) {
  visited[vertex] = true;
  auto neighbours = net.NeighboursIt(vertex, [&](const Edge* edge) {
    return !visited[edge->second] && edge->CurrentCapacity() > 0;
  });
  for (auto* edge : neighbours) {
    WalkInResidualNetwork(net, edge->second, visited);
  }
}

void Solve(ListGraph<int64_t>& net, std::pair<size_t, size_t> st_pair,
           Encoder& enc) {
  FordFulkerson ff_algo(net);
  auto flow = ff_algo.FindMaxFlow(st_pair.first, st_pair.second);
  if (flow >= NetworkBuilder::kMaxFlow / 2) {
    std::cout << -1;
    return;
  }
  std::cout << flow << '\n';
  std::vector<bool> used(net.VerticesCount(), false);
  size_t source = st_pair.first;
  MinCutVisitor visitor(source, st_pair.second);
  visitor.Initialize(net.Vertices());
  DFSImpl<ListGraph<int64_t>, MinCutVisitor> dfs(net, visitor);
  dfs.DFS(source);
  for (auto* edge : net.Edges()) {
    bool used_first = visitor.Visited(edge->first);
    bool not_used_second = !visitor.Visited(edge->second);
    if (used_first && not_used_second && edge->flow == 1) {
      std::cout << enc.IdxToCoordinates(edge->second) << '\n';
    }
  }
}

void EnableFastIO() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
}

int main() {
  EnableFastIO();
  size_t height;
  size_t width;
  std::cin >> height >> width;
  Encoder enc{height, width};
  const size_t kScaleFactor = 5;  // to get enough space for extra vertices
  std::vector<int64_t> vertices_tmp(height * width * kScaleFactor + 2);
  std::iota(vertices_tmp.begin(), vertices_tmp.end(), 0);
  std::vector<Edge*> edges_tmp;
  ListGraph<int64_t> net(vertices_tmp, edges_tmp);
  NetworkBuilder builder(net);
  auto st_pair = builder.BuildNetwork(height, width, enc);
  Solve(net, st_pair, enc);
  return 0;
}