#include <iostream>
#include <limits>
#include <vector>

static constexpr int64_t kInfinity = std::numeric_limits<int64_t>::max();

struct Edge {
  int64_t to_vertex;  // destinaton
  int64_t capacity;   // edge max capacity
  int64_t flow;       // flow over edge

  int64_t CurrentCapacity() const { return capacity - flow; }

  Edge(int64_t to_vertex, int64_t capacity)
      : to_vertex(to_vertex), capacity(capacity), flow(0) {}
};

void AddEdge(std::vector<std::vector<int64_t>>& graph, std::vector<Edge>& edges,
             int64_t from, int64_t to_vertex, int64_t capacity) {
  edges.emplace_back(to_vertex, capacity);
  graph[from].emplace_back(edges.size() - 1);
  edges.emplace_back(from, 0);  // back edge capacity = 0
  graph[to_vertex].emplace_back(edges.size() - 1);
}

struct FordFulkerson {
 private:
  std::vector<std::vector<int64_t>>& graph_;
  std::vector<Edge>& edges_;

 public:
  FordFulkerson(std::vector<std::vector<int64_t>>& graph,
                std::vector<Edge>& edges)
      : graph_(graph), edges_(edges) {}

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
};

void ReadGraph(size_t l_size, size_t r_size,
               std::vector<std::vector<int64_t>>& graph,
               std::vector<Edge>& edges) {
  for (size_t from = 1; from <= l_size; ++from) {
    int64_t to_v;
    AddEdge(graph, edges, 0, from, 1);
    while (true) {
      std::cin >> to_v;
      if (to_v == 0) {
        break;
      }
      to_v += l_size;
      AddEdge(graph, edges, from, to_v, 1);  // add edge and back edge
    }
  }
  for (size_t from = l_size + 1; from <= r_size + l_size; ++from) {
    AddEdge(graph, edges, from, l_size + r_size + 1, 1);
  }
}

void PrintPairs(size_t l_size, std::vector<std::vector<int64_t>>& graph,
                std::vector<Edge>& edges) {
  for (size_t from = 1; from <= l_size; ++from) {
    for (auto edge_id : graph[from]) {
      if (edges[edge_id].flow == 1) {
        std::cout << from << ' ' << edges[edge_id].to_vertex - l_size << '\n';
      }
    }
  }
}

int main() {
  size_t left_size;
  size_t right_size;
  std::cin >> left_size >> right_size;
  std::vector<std::vector<int64_t>> graph(left_size + right_size + 2,
                                          std::vector<int64_t>());
  std::vector<Edge> edges;
  ReadGraph(left_size, right_size, graph, edges);
  int64_t source = 0;
  int64_t target = left_size + right_size + 1;
  auto algo = FordFulkerson(graph, edges);

  std::cout << algo.FindMaxFlow(source, target) << '\n';
  PrintPairs(left_size, graph, edges);
  return 0;
}