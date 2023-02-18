#include <algorithm>
#include <iostream>
#include <limits>
#include <unordered_map>
#include <vector>

static constexpr size_t kInfty = std::numeric_limits<size_t>::max();

struct Time {
  std::vector<size_t>& time_in;
  std::vector<size_t>& f_up;
  size_t& time;
};

struct Vertex {
  size_t id;       // vertex now
  size_t edge_id;  // edge to vertex
};

void BridgesDfs(const std::vector<std::vector<size_t>>& graph,
                const std::vector<std::pair<size_t, size_t>>& edges, Time& time,
                Vertex vertex, std::vector<size_t>& bridges) {
  time.time_in[vertex.id] = time.time++;
  time.f_up[vertex.id] = time.time_in[vertex.id];
  for (auto from_edge_id : graph[vertex.id]) {
    if (from_edge_id == vertex.edge_id) {
      continue;
    }
    const auto& edge = edges[from_edge_id];
    size_t to = edge.first == vertex.id ? edge.second : edge.first;
    if (to == vertex.id) {  // loop
      continue;
    }
    if (time.time_in[to] != kInfty) {
      time.f_up[vertex.id] = std::min(time.f_up[vertex.id], time.time_in[to]);
    } else {
      BridgesDfs(graph, edges, time, {to, from_edge_id}, bridges);
      time.f_up[vertex.id] = std::min(time.f_up[to], time.f_up[vertex.id]);
    }
    if (time.f_up[to] > time.time_in[vertex.id]) {
      bridges.push_back(from_edge_id);
    }
  }
}

std::vector<size_t> GetBridges(
    const std::vector<std::vector<size_t>>& graph,
    const std::vector<std::pair<size_t, size_t>>& edges) {
  size_t num_vertex = graph.size();
  std::vector<size_t> time_in(num_vertex, kInfty);  // time in
  // std::vector<size_t> time_out(num_vertex, kInfty);
  std::vector<size_t> f_up(num_vertex,
                           kInfty);  // aka RET(from lecture) magic function :)
  size_t time = 0;                   // time now
  Time timer{time_in, f_up, time};
  std::vector<size_t> bridges;  // result - all bridges

  for (size_t i = 0; i < num_vertex; ++i) {
    if (time_in[i] == kInfty) {
      BridgesDfs(graph, edges, timer, {i, kInfty}, bridges);
    }
  }
  return bridges;
}

void FastIO() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  std::cout.tie(nullptr);
}

int main() {
  FastIO();
  size_t n, m;
  std::cin >> n >> m;
  std::vector<std::vector<size_t>> graph(
      n, std::vector<size_t>());                 // edges from v (id)
  std::vector<std::pair<size_t, size_t>> edges;  // all edges

  edges.reserve(m);
  for (size_t id = 0; id < m; ++id) {
    size_t from;
    size_t to;
    std::cin >> from >> to;
    --from;
    --to;
    edges.emplace_back(std::min(from, to),
                       std::max(from, to));  // make first <= second
    // unoriented edge
    graph[from].push_back(id);
    graph[to].push_back(id);
  }
  // return all bridges id
  auto bridges = GetBridges(graph, edges);
  std::sort(bridges.begin(), bridges.end());
  std::cout << bridges.size() << '\n';
  for (auto id : bridges) {
    std::cout << id + 1 << ' ';
  }

  return 0;
}