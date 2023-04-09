#include <algorithm>
#include <iostream>
#include <queue>
#include <vector>

const size_t kInf = 1e9;
const int64_t kMaxFlow = 1 << 30;

struct Edge {
  int64_t from;       // source vertex
  int64_t to_vertex;  // destinaton
  int64_t capacity;   // edge max capacity
  int64_t flow;       // flow over edge
  size_t id;          // id to separate initial edges

  int64_t CurrentCapacity() const { return capacity - flow; }

  Edge() : Edge(0, 0, 0) {}

  Edge(int64_t from, int64_t to_vertex, int64_t capacity, int64_t flow = 0,
       size_t id = kInf)
      : from(from),
        to_vertex(to_vertex),
        capacity(capacity),
        flow(flow),
        id(id) {}
};

struct Dinic {
 private:
  std::vector<std::vector<int64_t>>& graph_;
  std::vector<Edge>& edges_;
  std::vector<int64_t> layer_;      // vertex layer in BFS traverse
  std::vector<size_t> edge_index_;  // index of first available edge from vertex
  size_t source_;
  size_t target_;

 public:
  void AddEdge(int64_t from, int64_t to_vertex, int64_t capacity, size_t id) {
    edges_.emplace_back(from, to_vertex, capacity, 0, id);
    graph_[from].emplace_back(edges_.size() - 1);
    edges_.emplace_back(to_vertex, from, 0);  // back edge
    graph_[to_vertex].emplace_back(edges_.size() - 1);
  }

  Dinic(std::vector<std::vector<int64_t>>& graph, std::vector<Edge>& edges)
      : graph_(graph), edges_(edges) {
    layer_.resize(graph.size() + 2);
    edge_index_.resize(graph.size() + 2);
  }
  bool BFS(int64_t flow) {
    std::fill(layer_.begin() + source_, layer_.begin() + target_ + 1, kInf);
    layer_[source_] = 0;
    std::queue<size_t> bfs_queue;
    bfs_queue.push(source_);
    while (!bfs_queue.empty() && layer_[target_] == kInf) {
      size_t current = bfs_queue.front();
      bfs_queue.pop();
      for (auto edge_id : graph_[current]) {
        size_t to_v = edges_[edge_id].to_vertex;
        if (layer_[to_v] == kInf && edges_[edge_id].CurrentCapacity() >= flow) {
          layer_[to_v] = layer_[current] + 1;
          bfs_queue.push(to_v);
        }
      }
    }
    return (layer_[target_] != kInf);
  }

  bool DFS(size_t vertex, int64_t flow) {  // Check if it is possible to push
                                           // 'flow' in current network
    if (flow == 0) {
      return false;
    }
    if (vertex == target_) {
      return true;
    }
    for (; edge_index_[vertex] < graph_[vertex].size(); edge_index_[vertex]++) {
      int64_t edge_id = graph_[vertex][edge_index_[vertex]];
      int64_t to_v = edges_[edge_id].to_vertex;

      if (layer_[to_v] == layer_[vertex] + 1 &&
          edges_[edge_id].CurrentCapacity() >= flow) {
        bool pushed = DFS(to_v, flow);
        if (pushed) {
          edges_[edge_id].flow += flow;
          edges_[edge_id ^ 1].flow -= flow;
          return true;
        }
      }
    }
    return false;
  }

  int64_t FindMaxFlow(size_t source, size_t target) {
    source_ = source;
    target_ = target;
    int64_t flow = 0;
    for (int64_t new_flow = kMaxFlow; new_flow >= 1;) {
      if (!BFS(new_flow)) {  // try to push 'new_flow' from s to t
        new_flow >>= 1;
        continue;
      }
      std::fill(edge_index_.begin() + source_,
                edge_index_.begin() + target_ + 1, 0);
      while (DFS(source_, new_flow)) {  // push 'new_flow'
        flow = flow + new_flow;
      }
    }
    return flow;
  }
};

void ReadGraph(size_t edges_count, Dinic& dinic) {
  for (size_t i = 0; i < edges_count; ++i) {
    int64_t from;
    int64_t to_v;
    int64_t capacity;
    std::cin >> from >> to_v >> capacity;
    dinic.AddEdge(from, to_v, capacity, i);  // add edge and back edge
  }
}

void PrintFlows(std::vector<Edge>& edges) {
  for (size_t i = 0; i < edges.size(); ++i) {
    if (i % 2 == 0) {
      std::cout << edges[i].flow << '\n';
    }
  }
}

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  size_t vertices_count;
  size_t edges_count;
  std::cin >> vertices_count >> edges_count;
  std::vector<std::vector<int64_t>> graph(vertices_count + 2,
                                          std::vector<int64_t>());
  std::vector<Edge> edges;
  auto algo = Dinic(graph, edges);
  ReadGraph(edges_count, algo);

  std::cout << algo.FindMaxFlow(1, vertices_count) << '\n';
  PrintFlows(edges);
  return 0;
}