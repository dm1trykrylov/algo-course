#include <algorithm>
#include <iostream>
#include <queue>
#include <vector>

const size_t kInf = 1e9;
const int64_t kMaxFlow = 1 << 30;
const int64_t kMaxUtility = 1001;

struct Edge {
  int64_t from;       // source vertex
  int64_t to_vertex;  // destinaton
  int64_t capacity;   // edge max capacity
  int64_t flow;       // flow over edge
  size_t id;          // id to separate initial edges

  int64_t CurrentCapacity() const { return capacity - flow; }

  Edge() : Edge(0, 0, 0) {}

  Edge(int64_t from, int64_t to_vertex, int64_t capacity, int64_t flow = 0,
       size_t edge_id = kInf)
      : from(from),
        to_vertex(to_vertex),
        capacity(capacity),
        flow(flow),
        id(edge_id) {}
};

class Network {
 private:
  std::vector<std::vector<int64_t>> graph_;
  std::vector<Edge> edges_;
  size_t source_;
  size_t target_;

 public:
  Network(size_t vertices_count)
      : graph_(vertices_count + 2, std::vector<int64_t>()) {
    source_ = 0;
    target_ = vertices_count + 1;
    utility.resize(vertices_count + 2);
  }

  void AddEdge(int64_t from, int64_t to_vertex, int64_t capacity,
               size_t edge_id) {
    edges_.emplace_back(from, to_vertex, capacity, 0, edge_id);
    graph_[from].emplace_back(edges_.size() - 1);
    edges_.emplace_back(to_vertex, from, 0);  // back edge
    graph_[to_vertex].emplace_back(edges_.size() - 1);
  }

  size_t VerticesCount() const { return graph_.size(); }

  const std::vector<int64_t>& Neighbours(size_t vertex) const {
    return graph_[vertex];
  }
  Edge& GetEdge(size_t edge_id) { return edges_[edge_id]; }
  constexpr size_t Source() const { return source_; }
  constexpr size_t Target() const { return target_; }

  std::vector<int64_t> utility;
};

struct Dinic {
 private:
  Network& net_;
  std::vector<int64_t> layer_;      // vertex layer in BFS traverse
  std::vector<size_t> edge_index_;  // index of first available edge from vertex

 public:
  Dinic(Network& net) : net_(net) {
    layer_.resize(net.VerticesCount());
    edge_index_.resize(net.VerticesCount());
  }

  bool BFS(int64_t flow) {
    std::fill(layer_.begin(), layer_.end(), kInf);
    layer_[net_.Source()] = 0;
    std::queue<size_t> bfs_queue;
    bfs_queue.push(net_.Source());
    while (!bfs_queue.empty() && layer_[net_.Target()] == kInf) {
      size_t current = bfs_queue.front();
      bfs_queue.pop();
      for (auto edge_id : net_.Neighbours(current)) {
        auto edge = net_.GetEdge(edge_id);
        size_t to_v = edge.to_vertex;
        if (layer_[to_v] == kInf && edge.CurrentCapacity() >= flow) {
          layer_[to_v] = layer_[current] + 1;
          bfs_queue.push(to_v);
        }
      }
    }
    return (layer_[net_.Target()] != kInf);
  }

  bool DFS(size_t vertex, int64_t flow) {  // Check if it is possible to push
                                           // 'flow' in current network
    if (flow == 0) {
      return false;
    }
    if (vertex == net_.Target()) {
      return true;
    }
    for (; edge_index_[vertex] < net_.Neighbours(vertex).size();
         edge_index_[vertex]++) {
      int64_t edge_id = net_.Neighbours(vertex)[edge_index_[vertex]];
      auto& edge = net_.GetEdge(edge_id);
      int64_t to_v = edge.to_vertex;

      if (layer_[to_v] == layer_[vertex] + 1 &&
          edge.CurrentCapacity() >= flow) {
        bool pushed = DFS(to_v, flow);
        if (pushed) {
          edge.flow += flow;
          net_.GetEdge((edge_id ^ 1)).flow -= flow;
          return true;
        }
      }
    }
    return false;
  }

  int64_t FindMaxFlow() {
    size_t source = net_.Source();
    int64_t flow = 0;
    for (int64_t new_flow = kMaxFlow; new_flow >= 1;) {
      if (!BFS(new_flow)) {  // try to push 'new_flow' from s to t
        new_flow >>= 1;
        continue;
      }
      std::fill(edge_index_.begin(), edge_index_.end(), 0);
      while (DFS(source, new_flow)) {  // push 'new_flow'
        flow = flow + new_flow;
      }
    }
    return flow;
  }
};

void EnableFastIO() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
}

void BuildNetwork(size_t vertices_count, Network& net) {
  int64_t v_utility;
  for (size_t vertex = 1; vertex <= vertices_count; ++vertex) {
    std::cin >> v_utility;
    net.AddEdge(net.Source(), vertex, kMaxUtility + v_utility, 1);
    net.AddEdge(vertex, net.Target(), kMaxUtility, 1);
    net.utility[vertex] = v_utility;
  }

  int64_t to_v;
  size_t connections_count;
  for (size_t from = 1; from <= vertices_count; ++from) {
    std::cin >> connections_count;
    for (size_t i = 0; i < connections_count; ++i) {
      std::cin >> to_v;
      net.AddEdge(from, to_v, kMaxFlow, 1);
    }
  }
}

int64_t UtilityInResidualNetwork(Network& net, size_t vertex,
                                 std::vector<bool>& visited) {
  visited[vertex] = true;
  int64_t current_utility = net.utility[vertex];
  for (auto edge_id : net.Neighbours(vertex)) {
    auto edge_to = net.GetEdge(edge_id);
    if (!visited[edge_to.to_vertex] && edge_to.CurrentCapacity() > 0) {
      current_utility +=
          UtilityInResidualNetwork(net, edge_to.to_vertex, visited);
    }
  }
  return current_utility;
}

void Solve(Network& net) {
  Dinic dinic(net);
  dinic.FindMaxFlow();
  std::vector<bool> used(net.VerticesCount(), false);
  int64_t total_utility = UtilityInResidualNetwork(net, net.Source(), used);
  std::cout << total_utility;
}

int main() {
  EnableFastIO();
  size_t vertices_count;
  std::cin >> vertices_count;
  Network net(vertices_count);
  BuildNetwork(vertices_count, net);
  Solve(net);
  return 0;
}