#include <algorithm>
#include <functional>
#include <iostream>
#include <limits>
#include <numeric>
#include <stdexcept>
#include <unordered_map>
#include <vector>

template <class T>
using FilterFunction = std::function<bool(const T)>;

template <class VType = size_t, class EType = std::pair<VType, VType>>
class IteratorImpl {
 public:
  using Iterator = IteratorImpl;

  IteratorImpl(VType v, typename std::vector<VType>::iterator begin,
               typename std::vector<VType>::iterator end,
               const FilterFunction<EType>& filter)
      : v_(v), now_(begin), end_(end), kFilter(filter) {
    if (now_ != end_) {
      now_edge_ = {v_, *now_};
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
      now_edge_ = {v_, *now_};
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
  typename std::vector<VType>::iterator now_;
  typename std::vector<VType>::iterator end_;
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

  virtual typename std::vector<VType>::iterator NeighboursBegin(VType v) = 0;
  virtual typename std::vector<VType>::iterator NeighboursEnd(VType v) = 0;
  virtual IteratorImpl<VType, EType> NeighboursIt(
      VType v, const FilterFunction<EType>& filter) = 0;

  virtual ~Graph() = default;
};

template <class VType = size_t, class EType = std::pair<VType, VType>>
class ListGraph : public Graph<VType, EType> {
 public:
  ListGraph(const std::vector<VType>& vertexes, const std::vector<EType>& edges)
      : vertexes_(vertexes),
        num_vertex_(vertexes.size()),
        num_edges_(edges.size()) {
    for (auto& vertex : vertexes) {
      adjacency_lists_[vertex] = std::vector<VType>();
    }
    for (auto& edge : edges) {
      adjacency_lists_[edge.first].push_back(edge.second);
    }
  }

  size_t EdgeCount() const final { return num_edges_; }

  size_t VertexCount() const final { return num_edges_; }

  typename std::vector<VType>::iterator NeighboursBegin(VType v) override {
    return adjacency_lists_[v].begin();
  }

  typename std::vector<VType>::iterator NeighboursEnd(VType v) override {
    return adjacency_lists_[v].end();
  }

  std::vector<VType> Vertexes() const final { return vertexes_; }

  IteratorImpl<VType, EType> NeighboursIt(
      VType v, const FilterFunction<EType>& filter) override {
    return {v, NeighboursBegin(v), NeighboursEnd(v), filter};
  }

 protected:
  std::unordered_map<VType, std::vector<VType>> adjacency_lists_;
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
      adjacency_lists_[edge.second].push_back(edge.first);
      adjacency_lists_[edge.first].push_back(edge.second);
    }
  }
};

enum Colors { White, Grey, Black };

static constexpr size_t kInfty = std::numeric_limits<size_t>::max();

struct Time {
  std::vector<size_t>& in;
  std::vector<size_t>& up;
  size_t& time;
};

struct Vertex {
  size_t id;       // vertex now
  size_t edge_id;  // edge to vertex
};

// ----------------------------------------

void BridgesDfs(const std::vector<std::vector<size_t>>& graph,
                const std::vector<std::pair<size_t, size_t>>& edges, Time& time,
                Vertex vertex, std::vector<size_t>& bridges) {
  time.in[vertex.id] = time.time++;
  time.up[vertex.id] = time.in[vertex.id];
  for (auto from_edge_id : graph[vertex.id]) {
    if (from_edge_id == vertex.edge_id) {
      continue;
    }
    const auto& edge = edges[from_edge_id];
    size_t to = edge.first == vertex.id ? edge.second : edge.first;
    if (to == vertex.id) {  // loop
      continue;
    }
    if (time.in[to] != kInfty) {
      time.up[vertex.id] = std::min(time.up[vertex.id], time.in[to]);
    } else {
      BridgesDfs(graph, edges, time, {to, from_edge_id}, bridges);
      time.up[vertex.id] = std::min(time.up[to], time.up[vertex.id]);
    }
    if (time.up[to] > time.in[vertex.id]) {
      bridges.push_back(from_edge_id);
    }
  }
}

// ----------------------------------------

template <typename Graph>
void DFSCutpoints(typename Graph::VertexType from, Graph& graph, Time& time,
                  std::vector<typename Graph::VertexType>& cutpoints,
                  bool root = false) {
  // using VType = typename Graph::VertexType;
  using EType = typename Graph::EdgeType;
  time.in[from] = time.time++;
  time.up[from] = time.in[from];
  auto neighbours = graph.NeighboursIt(
      from, [&](const EType& edge) { return edge.second != from; });
  size_t neighbours_count = 0;
  std::for_each(neighbours.begin(), neighbours.end(), [&](const EType& edge) {
    auto to = edge.second;
    if (time.in[to] != kInfty) {
      time.up[from] = std::min(time.up[from], time.in[to]);
    } else {
      ++neighbours_count;
      DFSCutpoints(edge.second, graph, time, cutpoints);
      time.up[from] = std::min(time.up[to], time.up[from]);
      if (time.up[to] >= time.in[from] && !root) {
        cutpoints.push_back(from);
      }
    }
  });
  if (root && neighbours_count > 1) {
    cutpoints.push_back(from);
  }
}

template <typename Graph>
std::unordered_map<typename Graph::VertexType, char> colors;

template <typename Graph>
void GetCutpoints(Graph& graph,
                  std::vector<typename Graph::VertexType>& cutpoints) {
  cutpoints.clear();
  auto vertexes = graph.Vertexes();

  std::vector<size_t> time_in(vertexes.size() + 1, kInfty);  // time in
  std::vector<size_t> f_up(vertexes.size() + 1,
                           kInfty);  // aka RET(from lecture) magic function :)
  size_t time = 0;                   // time now
  Time timer{time_in, f_up, time};
  //      BridgesDfs(graph, edges, timer, {i, kInfty}, bridges);

  for (auto v : vertexes) {
    if (timer.in[v] == kInfty) {
      DFSCutpoints(v, graph, timer, cutpoints, true);
    }
  }
}

template <class VType = size_t, class EType = std::pair<VType, VType>>
void ReadGraph(std::vector<EType>& edges_list, std::vector<VType>& g) {
  for (auto& edge : edges_list) {
    std::cin >> edge.first >> edge.second;
  }
  size_t cnt = 1;
  for (auto& v : g) {
    v = cnt++;
  };
}

template <typename T>
void Print(std::vector<T>& array, char delim = '\n') {
  for (auto a : array) {
    std::cout << a << delim;
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

  std::vector<std::pair<size_t, size_t>> edges_list(edges);
  std::vector<size_t> g(vertexes);
  ReadGraph(edges_list, g);
  UndirectedListGraph<size_t> graph(g, edges_list);
  std::vector<size_t> cutpoints;
  GetCutpoints(graph, cutpoints);
  std::sort(cutpoints.begin(), cutpoints.end());
  cutpoints.erase(std::unique(cutpoints.begin(), cutpoints.end()),
                  cutpoints.end());
  std::cout << cutpoints.size() << '\n';
  Print(cutpoints);

  return 0;
}
