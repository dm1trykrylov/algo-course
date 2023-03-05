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
class ListGraph : public Graph<VType, EType> {
 public:
  ListGraph(const std::vector<VType>& vertexes, const std::vector<EType>& edges)
      : vertexes_(vertexes),
        num_vertex_(vertexes.size()),
        num_edges_(edges.size()) {
    adjacency_lists.resize(num_vertex_ + 1);
    for (auto& edge : edges) {
      adjacency_lists[edge.first].push_back(edge);
    }
  }

  size_t EdgeCount() const final { return num_edges_; }

  size_t VertexCount() const final { return num_vertex_; }

  typename std::vector<EType>::iterator NeighboursBegin(VType vertex) final {
    return adjacency_lists[vertex].begin();
  }

  typename std::vector<EType>::iterator NeighboursEnd(VType vertex) final {
    return adjacency_lists[vertex].end();
  }

  std::vector<VType> Vertexes() const final { return vertexes_; }

  IteratorImpl<VType, EType> NeighboursIt(
      VType vertex, const FilterFunction<EType>& filter) override {
    return {vertex, NeighboursBegin(vertex), NeighboursEnd(vertex), filter};
  }
  std::vector<std::vector<EType>> adjacency_lists;

 protected:
  std::vector<VType> vertexes_;
  size_t num_vertex_;
  size_t num_edges_;
};

template <class VType = size_t, class EType = std::pair<VType, VType>>
class UndirectedListGraph : public ListGraph<VType, EType> {
 public:
  using ListGraph<VType, EType>::adjacency_lists;
  UndirectedListGraph(const std::vector<VType>& vertexes,
                      const std::vector<EType>& edges)
      : ListGraph<VType, EType>(vertexes, edges) {
    for (auto& edge : edges) {
      adjacency_lists[edge.second].push_back(edge);
      // by default all edges from v should have edge.first = v
      auto& back_edge = *adjacency_lists[edge.second].rbegin();
      std::swap(back_edge.first,
                back_edge.second);  // that's why we reverse this edge
    }
  }
};

template <class Graph, typename T>
class Dijkstra {
 private:
  Graph& graph_;
  using VType = typename Graph::VertexType;
  using EType = typename Graph::EdgeType;
  std::vector<T> distances_;

 public:
  Dijkstra(Graph& graph) : graph_(graph) {}

  void operator()(VType from, T max_distance) {
    auto vertexes = graph_.Vertexes();
    distances_.resize(vertexes.size() + 1, max_distance);
    distances_[from] = 0;

    std::priority_queue<std::pair<T, VType>> queue;

    queue.emplace(distances_[from], from);

    while (!queue.empty()) {
      auto[dist, current] = queue.top();
      queue.pop();
      if (dist > distances_[current]) {
        continue;
      }

      for (auto& edge : graph_.adjacency_lists[current]) {
        auto neighbour = edge.second;
        T distance = edge.time;
        if (distances_[current] + distance < distances_[neighbour]) {
          distances_[neighbour] = distances_[current] + distance;
          queue.emplace(distances_[neighbour], neighbour);
        }
      }
    }
  }

  T& Distance(VType vertex) { return distances_[vertex]; }
  std::vector<T>& Distances() { return distances_; }
};

template <class Graph, typename T, typename C>
class PathCounter {
 private:
  Graph& graph_;
  using VType = typename Graph::VertexType;
  using EType = typename Graph::EdgeType;
  size_t max_time_;
  std::vector<T>& distances_;
  std::vector<bool> visited_;
  std::vector<size_t> current_path_;
  C min_cost_;
  // std::vector<VType> parents_;

 public:
  PathCounter(Graph& graph, size_t max_time, std::vector<T>& distances)
      : graph_(graph), max_time_(max_time), distances_(distances) {}

  size_t CountPaths(size_t src, size_t dst, size_t vertices_count,
                    std::vector<VType>& min_path) {
    min_cost_ = std::numeric_limits<C>::max();
    visited_.resize(vertices_count + 1, false);
    // parents_[src] = src;
    // std::vector<size_t> cur_path;
    current_path_.reserve(graph_.VertexCount());
    min_path.reserve(graph_.VertexCount());
    Dfs(src, dst, 0, 0, min_path);

    return min_cost_;
  }

  void Dfs(size_t source, size_t target, size_t path_time, size_t path_cost,
           std::vector<size_t>& best_path) {
    if (((path_time + distances_[source] > max_time_) ||
         (path_cost > min_cost_))) {
      return;
    }
    visited_[source] = true;
    current_path_.push_back(source);
    if (source == target) {
      if (min_cost_ > path_cost) {
        best_path = current_path_;
        min_cost_ = path_cost;
      }
    } else {
      for (auto& edge : graph_.adjacency_lists[source]) {
        auto neighbour = edge.second;
        if (!visited_[neighbour] &&
            (path_time + distances_[source] <= max_time_) &&
            (path_cost + edge.cost < min_cost_)) {
          // parents_[neighbour] = source;
          Dfs(neighbour, target, path_time + edge.time, path_cost + edge.cost,
              best_path);
        }
      }
    }
    current_path_.pop_back();
    visited_[source] = false;
  }
};
template <class VType, typename T>
struct Edge {
  VType first;
  VType second;
  T cost;
  T time;
};

template <class VType = size_t, class EType = std::pair<VType, VType>>
void ReadGraph(std::vector<EType>& edges_list,
               std::vector<VType>& vertex_list) {
  for (auto & [ from, to, cost, time ] : edges_list) {
    std::cin >> from >> to >> cost >> time;
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
  size_t vertices_count;
  size_t edges_count;
  int64_t max_time;
  std::cin >> vertices_count >> edges_count >> max_time;
  using EType = Edge<size_t, int64_t>;
  std::vector<EType> edges(edges_count);
  std::vector<size_t> vertices(vertices_count);
  ReadGraph(edges, vertices);
  size_t source = 1;
  size_t target = vertices_count;

  UndirectedListGraph<size_t, EType> graph(vertices, edges);
  Dijkstra<UndirectedListGraph<size_t, EType>, int64_t> dijkstra(graph);
  int64_t max_dist = (std::numeric_limits<int64_t>::max() >> 4);
  dijkstra(target, max_dist);
  if (dijkstra.Distance(source) > max_time) {
    std::cout << -1;
  } else {
    std::vector<size_t> min_path;
    PathCounter<UndirectedListGraph<size_t, EType>, int64_t, size_t> counter(
        graph, max_time, dijkstra.Distances());
    std::cout << counter.CountPaths(source, target, vertices_count, min_path)
              << '\n';
    // std::reverse(min_path.begin(), min_path.end());
    std::cout << min_path.size() << '\n';
    Print(min_path, ' ');
  }

  return 0;
}