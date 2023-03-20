#include <algorithm>
#include <bitset>
#include <functional>
#include <iostream>
#include <limits>
#include <map>
#include <numeric>
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
  virtual void DiscoverVertex(const VType&, const VType&){};
  virtual void DiscoverEdge(const EType&){};
  virtual void TreeEdge(const EType&){};
  virtual void BackEdge(const EType&){};
  virtual void FinishEdge(const EType&){};
  virtual void FinishVertex(const VType&){};
  ~Visitor() = default;
};

template <class VType = size_t, class EType = std::pair<VType, VType>>
class BridgesVisitor : public Visitor<VType, EType> {
 public:
  void InitializeTimer(size_t vertex_count) {
    // counter_ = vertex_count + 1;
    timer_.in.resize(vertex_count + 1, kInfty);
    timer_.up.resize(vertex_count + 1, kInfty);
    timer_.time = 0;
  }
  void DiscoverVertex(const VType& vertex, const VType& parent) final {
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
    }
  }
  std::vector<EType> GetBridges() { return bridges_; }

 private:
  static constexpr size_t kInfty = std::numeric_limits<uint32_t>::max();
  struct Time {
    std::vector<uint32_t> in;
    std::vector<uint32_t> up;
    size_t time;
  };
  Time timer_;
  std::vector<EType> bridges_;
};

template <class VType = size_t, class EType = std::pair<VType, VType>>
class CompVisitor : public Visitor<VType, EType> {
 public:
  void InitializeTimer(size_t vertex_count) {
    counter_ = 1;
    comp_mask_.set(1);
    components_.resize(vertex_count + 2);
  }
  void DiscoverVertex(const VType& vertex, const VType& parent) final {
    components_[vertex] = counter_;
    comp_mask_.set(counter_);
  }
  void DiscoverEdge(const EType& edge) final {
    if (bridges_mask_.test(edge.id)) {
      do {
        ++counter_;
      } while (comp_mask_.test(counter_));
    }
  }
  void FinishEdge(const EType& edge) final {
    if (bridges_mask_.test(edge.id)) {
      counter_ = components_[edge.first];
    }
  }
  void SetBridge(const size_t id) { bridges_mask_.set(id); }
  size_t GetComponent(const VType& vertex) { return components_[vertex]; }

 private:
  uint32_t counter_;
  std::vector<uint32_t> components_;
  uint32_t prev_comp_;
  static constexpr size_t kMaxEdges = 1 << 19;
  std::bitset<kMaxEdges> bridges_mask_;
  std::bitset<kMaxEdges> comp_mask_;
};

static constexpr size_t length = 5e5 + 5, logn = 19;
size_t binup_[length][logn + 1], depth_[length], parents_[length];
template <class VType = size_t, class EType = std::pair<VType, VType>>
class LCAVisitor : public Visitor<VType, EType> {
 public:
  void InitializeTimer(size_t vertex_count) {
    // counter_ = vertex_count + 1;
    timer_.in.resize(vertex_count + 1, kInfty);
    timer_.out.resize(vertex_count + 1, kInfty);
    timer_.time = 0;
    std::fill(depth_, depth_ + vertex_count + 1, 0);
    std::fill(parents_, parents_ + vertex_count + 1, 0);
  }
  void DiscoverVertex(const VType& vertex, const VType& parent) final {
    timer_.in[vertex] = timer_.time++;
    binup_[vertex][0] = parent;
    parents_[vertex] = parent;
    depth_[vertex] = depth_[parents_[vertex]] + 1;
    for (size_t i = 1; i <= logn; ++i) {
      binup_[vertex][i] = binup_[binup_[vertex][i - 1]][i - 1];
    }
  }

  void FinishVertex(const VType& vertex) final {
    timer_.out[vertex] = timer_.time++;
  }

  bool CheckParent(VType parent, VType child) {
    return (timer_.in[parent] <= timer_.in[child] &&
            timer_.out[parent] >= timer_.out[child]);
  }
  size_t GetDepth(const VType& vertex) { return depth_[vertex]; }
  //
  VType LCA(VType lhs, VType rhs) {
    VType ans;
    if (depth_[lhs] > depth_[rhs]) {
      std::swap(rhs, lhs);
    }
    if (CheckParent(rhs, lhs)) {
      ans = rhs;
    } else if (CheckParent(lhs, rhs)) {
      ans = lhs;
    } else {
      for (int i = logn; i >= 0; --i)
        if (!CheckParent(binup_[lhs][i], rhs)) {
          lhs = binup_[lhs][i];
        } else {
          ans = binup_[lhs][i];
        }
    }
    return ans;
  }

 private:
  size_t k, t;
  static constexpr size_t kInfty = std::numeric_limits<uint32_t>::max();
  struct Time {
    std::vector<uint32_t> in;
    std::vector<uint32_t> out;
    size_t time;
  };
  Time timer_;
  uint32_t counter_;
};

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

  void DFS(size_t from_edge_id, typename Graph::VertexType from,
           typename Graph::VertexType parent) {
    visitor_.DiscoverVertex(from, parent);
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
        visitor_.DiscoverEdge(edge);
        DFS(edge.id, edge.second, from);
        visitor_.TreeEdge(edge);
      }
      visitor_.FinishEdge(edge);
    };
    colors_[from] = Black;
    visitor_.FinishVertex(from);
  }

  void FindBridges() {
    auto vertexes = graph_.Vertexes();
    for (auto& vertex : vertexes) {
      colors_[vertex] = White;
    };
    visitor_.InitializeTimer(vertexes.size());
    for (auto vertex : vertexes) {
      if (colors_[vertex] == White) {
        DFS(0, vertex, vertex);
      }
    }
  }
  void SetComponents(VType from) {
    auto vertexes = graph_.Vertexes();
    for (auto& vertex : vertexes) {
      colors_[vertex] = White;
    };
    visitor_.InitializeTimer(vertexes.size());
    DFS(0, from, from);
  }

  void Prepare(VType from) {
    auto vertexes = graph_.Vertexes();
    for (auto& vertex : vertexes) {
      colors_[vertex] = White;
    };
    visitor_.InitializeTimer(vertexes.size());
    DFS(0, from, from);
  }
};
/*
int LCA(size_t v, size_t u);
bool CheckParent(int a = 0, int b = 0);
*/
template <class VType = size_t>
struct Edge {
  VType first = 0;
  VType second = 0;
  uint16_t id = 0;
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
  uint32_t target;
  std::cin >> vertexes >> edges >> target;
  using EType = Edge<uint32_t>;
  std::vector<EType> edges_list(edges);
  std::vector<uint32_t> vertex_list(vertexes);
  ReadGraph(edges_list, vertex_list);
  UndirectedListGraph<uint32_t, EType> graph(vertex_list, edges_list);

  BridgesVisitor<uint32_t, EType> visitor;

  DFSImpl<UndirectedListGraph<uint32_t, EType>, BridgesVisitor<uint32_t, EType>>
      dfs(graph, visitor);
  dfs.FindBridges();
  std::vector<EType> bridges = visitor.GetBridges();  // Find bridges in graph

  CompVisitor<uint32_t, EType> cmp_visitor;
  for (auto& edge : bridges) {
    cmp_visitor.SetBridge(edge.id);
  }
  DFSImpl<UndirectedListGraph<uint32_t, EType>, CompVisitor<uint32_t, EType>>
      dfs_cmp(graph, cmp_visitor);
  dfs_cmp.SetComponents(target);

  std::vector<uint32_t> cd_vertices;
  for (auto& [from, to, _] : bridges) {
    from = cmp_visitor.GetComponent(from);
    to = cmp_visitor.GetComponent(to);
    cd_vertices.push_back(from);
    cd_vertices.push_back(to);
  }
  std::sort(cd_vertices.begin(), cd_vertices.end());
  cd_vertices.erase(std::unique(cd_vertices.begin(), cd_vertices.end()),
                    cd_vertices.end());
  UndirectedListGraph<uint32_t, EType> condensed(cd_vertices, bridges);
  LCAVisitor<uint32_t, EType> evisitor;
  DFSImpl<UndirectedListGraph<uint32_t, EType>, LCAVisitor<uint32_t, EType>>
      dfs_con(condensed, evisitor);
  dfs_con.Prepare(cmp_visitor.GetComponent(target));

  size_t queries_count;
  std::cin >> queries_count;
  size_t yellow_v, blue_v, common_bridges;
  for (size_t i = 0; i < queries_count; ++i) {
    std::cin >> yellow_v >> blue_v;
    yellow_v = cmp_visitor.GetComponent(yellow_v);
    blue_v = cmp_visitor.GetComponent(blue_v);
    std::cout << evisitor.GetDepth(evisitor.LCA(yellow_v, blue_v)) - 1 << '\n';
  }

  return 0;
}