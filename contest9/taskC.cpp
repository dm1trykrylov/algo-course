#include <algorithm>
#include <iostream>
#include <limits>
#include <queue>
#include <vector>

static constexpr int64_t kInfinity = std::numeric_limits<int64_t>::max();
const size_t kInf = 1e9;

struct Edge {
  int64_t from;       // source vertex
  int64_t to_vertex;  // destinaton
  int64_t capacity;   // edge max capacity
  int64_t flow;       // flow over edge
  size_t id;          // id to separate initial edges

  int64_t CurrentCapacity() const { return capacity - flow; }

  Edge() : Edge(0, 0, 0) {}

  Edge(int64_t from, int64_t to_vertex, int64_t capacity, size_t idx = kInf)
      : from(from),
        to_vertex(to_vertex),
        capacity(capacity),
        flow(0),
        id(idx) {}
};

struct FordFulkerson {
 private:
  std::vector<std::vector<int64_t>>& graph_;
  std::vector<Edge>& edges_;

 public:
  FordFulkerson(std::vector<std::vector<int64_t>>& graph,
                std::vector<Edge>& edges)
      : graph_(graph), edges_(edges) {}

  void AddEdge(int64_t from, int64_t to_vertex, int64_t capacity, size_t idx) {
    edges_.emplace_back(from, to_vertex, capacity, idx);
    graph_[from].emplace_back(edges_.size() - 1);
    edges_.emplace_back(to_vertex, from, 0);  // back edge capacity = 0
    graph_[to_vertex].emplace_back(edges_.size() - 1);
  }

  int64_t FindIncreasingPath(std::vector<uint32_t>& used, uint32_t phase,
                             int64_t vertex, int64_t destination, int64_t min) {
    if (vertex == destination) {
      return min;
    }
    used[vertex] = phase;
    for (int64_t edge_id : graph_[vertex]) {
      if (edges_[edge_id].CurrentCapacity() == 0) {
        continue;
      }
      if (used[edges_[edge_id].to_vertex] == phase) {
        continue;
      }
      int64_t delta = FindIncreasingPath(
          used, phase, edges_[edge_id].to_vertex, destination,
          std::min(min, edges_[edge_id].CurrentCapacity()));
      if (delta > 0) {
        edges_[edge_id].flow += delta;
        edges_[edge_id ^ 1].flow -= delta;  // back edge
        return delta;
      }
    }
    return 0;
  }

  int64_t FindMaxFlow(int64_t source, int64_t target) {
    std::vector<uint32_t> used(graph_.size(), 0);
    uint32_t phase = 1;
    while (FindIncreasingPath(used, phase, source, target, kInfinity) > 0) {
      ++phase;
    }

    int64_t result = 0;
    for (uint64_t i = 0; i < graph_[source].size(); ++i) {
      result += edges_[graph_[source][i]].flow;
    }
    return result;
  }

  // Find all vertices available by not filled paths
  void DFSnet(int64_t vertex, std::vector<bool>& used) {
    used[vertex] = true;
    for (auto edge_id : graph_[vertex]) {
      if (!used[edges_[edge_id].to_vertex] &&
          edges_[edge_id].CurrentCapacity() > 0) {
        DFSnet(edges_[edge_id].to_vertex, used);
      }
    }
  }

  std::vector<size_t> FindMinCut(int64_t source) {
    std::vector<bool> used(graph_.size() * 2 + 1, false);
    DFSnet(source, used);
    std::vector<size_t> cut_edges;
    for (auto edge : edges_) {
      if (used[edge.from] != used[edge.to_vertex] && edge.id > 0 &&
          edge.id < kInf) {
        cut_edges.push_back(edge.id);
      }
    }
    std::sort(cut_edges.begin(), cut_edges.end());
    auto last = std::unique(cut_edges.begin(), cut_edges.end());
    cut_edges.erase(last, cut_edges.end());
    return cut_edges;
  }
};

void ReadGraph(/*size_t vertices_count,*/ size_t edges_count,
               FordFulkerson& algo) {
  // size_t counter = vertices_count + 2;
  for (size_t i = 0; i < edges_count; ++i) {
    int64_t dest;
    int64_t vertex;
    int64_t capacity;
    std::cin >> vertex >> dest >> capacity;

    // add edge and back edge
    algo.AddEdge(vertex - 1, dest - 1, capacity, i + 1);
    --vertex;
    --dest;
    algo.AddEdge(dest, vertex, capacity, i + 1);
  }
}

int main() {
  std::vector<Edge> edges;
  size_t vertices;
  size_t edges_count;
  std::cin >> vertices >> edges_count;
  std::vector<std::vector<int64_t>> graph(vertices * vertices * 2 + 1);
  auto f_algo = FordFulkerson(graph, edges);
  ReadGraph(edges_count, f_algo);
  auto flow = f_algo.FindMaxFlow(0, vertices - 1);
  auto mincut = f_algo.FindMinCut(0);
  std::cout << mincut.size() << ' ' << flow << '\n';  // find max flow
  for (auto edge_id : mincut) {
    std::cout << edge_id << ' ';
  }
  return 0;
}