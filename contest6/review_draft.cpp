#include <algorithm>
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

  IteratorImpl(VType v, typename std::vector<EType>::iterator begin,
               typename std::vector<EType>::iterator end,
               const FilterFunction<EType>& filter)
      : v_(v), now_(begin), end_(end), kFilter(filter) {
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

  virtual typename std::vector<EType>::iterator NeighboursBegin(VType v) = 0;
  virtual typename std::vector<EType>::iterator NeighboursEnd(VType v) = 0;
  virtual IteratorImpl<VType, EType> NeighboursIt(
      VType v, const FilterFunction<EType>& filter) = 0;

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

  typename std::vector<EType>::iterator NeighboursBegin(VType v) final {
    return adjacency_matrix_[v].begin();
  }

  typename std::vector<EType>::iterator NeighboursEnd(VType v) final {
    return adjacency_matrix_[v].end();
  }

  std::vector<VType> Vertexes() const final { return vertexes_; }

  IteratorImpl<VType, EType> NeighboursIt(
      VType v, const FilterFunction<EType>& filter) override {
    return {v, NeighboursBegin(v), NeighboursEnd(v), filter};
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

  typename std::vector<EType>::iterator NeighboursBegin(VType v) final {
    return adjacency_lists_[v].begin();
  }

  typename std::vector<EType>::iterator NeighboursEnd(VType v) final {
    return adjacency_lists_[v].end();
  }

  std::vector<VType> Vertexes() const final { return vertexes_; }

  IteratorImpl<VType, EType> NeighboursIt(
      VType v, const FilterFunction<EType>& filter) override {
    return {v, NeighboursBegin(v), NeighboursEnd(v), filter};
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

static constexpr size_t kInfty = std::numeric_limits<size_t>::max();

template <typename Graph>
class DFSImpl {
 private:
  struct Time {
    std::vector<size_t>& in;
    std::vector<size_t>& up;
    size_t& time;
  };

 public:
  void DFS(typename Graph::EdgeType parent, typename Graph::VertexType from,
           Graph& graph, Time& time,
           std::vector<typename Graph::EdgeType>& bridges) {
    // using VType = typename Graph::VertexType;
    using EType = typename Graph::EdgeType;
    time.in[from] = time.time++;
    time.up[from] = time.in[from];
    auto neighbours = graph.NeighboursIt(from, [&](const EType& edge) {
      return (edge.second != from) && (edge.id != parent.id);
    });
    for (auto edge : neighbours) {
      auto to = edge.second;
      if (time.in[to] != kInfty) {
        time.up[from] = std::min(time.up[from], time.in[to]);
      } else {
        DFS(edge, edge.second, graph, time, bridges);
        time.up[from] = std::min(time.up[to], time.up[from]);
      }
      if (time.up[to] > time.in[from]) {
        bridges.push_back(edge);
      }
    };
  }

  void GetBridges(Graph& graph,
                  std::vector<typename Graph::EdgeType>& bridges) {
    bridges.clear();
    auto vertexes = graph.Vertexes();

    std::vector<size_t> time_in(vertexes.size() + 1, kInfty);  // time in
    std::vector<size_t> f_up(
        vertexes.size() + 1,
        kInfty);      // aka RET(from lecture) magic function :)
    size_t time = 0;  // time now
    Time timer{time_in, f_up, time};
    //      BridgesDfs(graph, edges, timer, {i, kInfty}, bridges);

    for (auto v : vertexes) {
      if (timer.in[v] == kInfty) {
        DFS({v, v, 0}, v, graph, timer, bridges);
      }
    }
  }
};

template <class VType = size_t>
struct Edge {
  VType first;
  VType second;
  uint32_t id;
};

template <class VType = size_t>
bool operator<(const Edge<VType>& a, const Edge<VType>& b) {
  return a.id < b.id;
}

template <class VType = size_t>
bool operator==(const Edge<VType>& a, const Edge<VType>& b) {
  return a.id == b.id;
}

void FastIO() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  std::cout.tie(nullptr);
}

template <class VType = size_t, class EType = std::pair<VType, VType>>
void ReadGraph(std::vector<EType>& edges_list, std::vector<VType>& g) {
  size_t id = 0;
  for (auto& edge : edges_list) {
    std::cin >> edge.first >> edge.second;
    edge.id = ++id;
  }
  size_t cnt = 1;
  for (auto& v : g) {
    v = cnt++;
  };
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
  UndirectedListGraph<uint32_t, EType> graph(vertex_list, edges_list);

  std::vector<EType> bridges;
  DFSImpl<UndirectedListGraph<uint32_t, EType>> dfs;
  dfs.GetBridges(graph, bridges);

  std::sort(bridges.begin(), bridges.end());
  std::cout << bridges.size() << '\n';
  for (auto bridge : bridges) {
    std::cout << bridge.id << '\n';
  }

  return 0;
}