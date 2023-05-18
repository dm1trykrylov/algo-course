// 87092867
#include <functional>
#include <iostream>
#include <limits>
#include <numeric>
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

 protected:
  std::unordered_map<VType, std::vector<EType>> adjacency_lists_;
  std::vector<VType> vertices_;
  std::vector<EType> edges_;
  size_t num_vertex_;
  size_t num_edges_;
};

void AddEdge(
    /*std::vector<std::vector<int64_t>>& graph, */ std::vector<Edge*>& edges,
    int64_t from, int64_t to_vertex, int64_t capacity) {
  edges.emplace_back(new Edge(from, to_vertex, capacity));
  auto* edge = edges.back();
  edges.emplace_back(new Edge(to_vertex, from, 0));  // back edge
  auto* back_edge = edges.back();
  edge->back = back_edge;
  back_edge->back = edge;
}

struct FordFulkerson {
 private:
  ListGraph<int64_t>& graph_;
  static constexpr int64_t kInfinity = std::numeric_limits<int64_t>::max();

 public:
  FordFulkerson(ListGraph<int64_t>& graph) : graph_(graph) {}

  int64_t FindIncreasingPath(std::vector<uint32_t>& used, uint32_t phase,
                             int64_t vertex, int64_t destination, int64_t min) {
    if (vertex == destination) {
      return min;
    }
    used[vertex] = phase;
    auto neighbours = graph_.NeighboursIt(vertex, [&](const Edge* edge) {
      return used[edge->second] != phase && edge->CurrentCapacity() != 0;
    });
    for (auto* edge : neighbours) {
      int64_t delta =
          FindIncreasingPath(used, phase, edge->second, destination,
                             std::min(min, edge->CurrentCapacity()));
      if (delta > 0) {
        edge->flow += delta;
        edge->back->flow -= delta;  // back edge
        return delta;
      }
    }
    return 0;
  }

  int64_t FindMaxFlow(int64_t source, int64_t target) {
    std::vector<uint32_t> used(graph_.VertexCount(), 0);
    uint32_t phase = 1;
    while (FindIncreasingPath(used, phase, source, target, kInfinity) > 0) {
      ++phase;
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

void ReadGraph(size_t edges_count, std::vector<Edge*>& edges) {
  for (size_t i = 0; i < edges_count; ++i) {
    int64_t from;
    int64_t to_v;
    int64_t capacity;
    std::cin >> from >> to_v >> capacity;
    --from;
    --to_v;
    AddEdge(edges, from, to_v, capacity);  // add edge and back edge
  }
}

int main() {
  std::vector<Edge*> edges;
  size_t vertices_count;
  size_t edges_count;
  std::cin >> vertices_count >> edges_count;
  ReadGraph(edges_count, edges);
  std::vector<int64_t> vertices(vertices_count, 0);
  std::iota(vertices.begin(), vertices.end(), 1);
  ListGraph<int64_t> graph(vertices, edges);
  auto algo = FordFulkerson(graph);
  std::cout << algo.FindMaxFlow(0, vertices_count - 1) << std::endl;
  for (auto* edge : edges) {
    delete edge;
  }
  return 0;
}