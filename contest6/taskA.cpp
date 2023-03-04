#include <algorithm>
#include <functional>
#include <iostream>
#include <limits>
#include <numeric>
#include <queue>
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

template <typename Graph>
void BFS(typename Graph::VertexType from, typename Graph::VertexType to,
         Graph& graph,
         std::unordered_map<typename Graph::VertexType,
                            typename Graph::VertexType>& parents) {
  // using VType = typename Graph::VertexType;
  using EType = typename Graph::EdgeType;
  std::unordered_map<typename Graph::VertexType, size_t> distance(
      graph.VertexCount());
  for (auto vertex : graph.Vertexes()) {
    distance[vertex] = kInfty;
  }
  distance[from] = 0;

  std::queue<typename Graph::VertexType> bfs_queue;
  bfs_queue.push(from);
  typename Graph::VertexType vertex;
  while (!bfs_queue.empty()) {
    vertex = bfs_queue.front();
    bfs_queue.pop();
    auto neighbours =
        graph.NeighboursIt(vertex, [&](const EType&) { return true; });
    for (auto edge : neighbours) {
      if (distance[edge.second] > distance[vertex] + 1) {
        distance[edge.second] = distance[vertex] + 1;
        bfs_queue.push(edge.second);
        parents[edge.second] = vertex;
      }
      if (edge.second == to) {
        throw std::logic_error("Found path!");
      }
    }
  }
}

template <typename Graph>
bool FindPath(Graph& graph, typename Graph::VertexType from,
              typename Graph::VertexType to,
              std::vector<typename Graph::VertexType>& path) {
  std::unordered_map<typename Graph::VertexType, typename Graph::VertexType>
      parents;
  auto vertexes = graph.Vertexes();

  try {
    BFS(from, to, graph, parents);
  } catch (std::exception& e) {
    path.push_back(to);
    to = parents[to];
    while (to != from) {
      path.push_back(to);
      to = parents[to];
    }
    path.push_back(from);
    std::reverse(path.begin(), path.end());
    return true;
  }

  return false;
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
  size_t source;
  size_t target;
  std::cin >> source >> target;

  std::vector<std::pair<size_t, size_t>> edges_list(edges);
  std::vector<size_t> g(vertexes);
  ReadGraph(edges_list, g);

  UndirectedListGraph<size_t> graph(g, edges_list);
  std::vector<size_t> path;
  if (source == target) {
    std::cout << 0 << '\n' << source;
  } else if (FindPath(graph, source, target, path)) {
    std::cout << path.size() - 1 << '\n';
    Print(path, ' ');
  } else {
    std::cout << -1;
  }
  return 0;
}