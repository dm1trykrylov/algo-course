#include <algorithm>
#include <iostream>
#include <limits>
#include <numeric>
#include <unordered_map>
#include <vector>

static constexpr size_t kInfty = std::numeric_limits<size_t>::max();

struct Time {
  std::vector<size_t>& in;
  std::vector<size_t>& up;
  size_t& time;
  size_t& color;
};

struct Vertex {
  size_t id;       // vertex now
  size_t edge_id;  // edge to vertex
};

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

struct Graph {
  const std::vector<std::vector<size_t>>& graph;
  const std::vector<std::pair<size_t, size_t>>& edges;
};

void PaintDfs(Graph& g, Time& time, Vertex vertex, std::vector<size_t>& colors,
              size_t color) {
  colors[vertex.id] = color;
  for (auto from_edge_id : g.graph[vertex.id]) {
    if (from_edge_id == vertex.edge_id) {
      continue;
    }
    const auto& edge = g.edges[from_edge_id];
    size_t to = edge.first == vertex.id ? edge.second : edge.first;
    if (to == vertex.id) {  // loop
      continue;
    }
    if (colors[to] == kInfty) {
      if (time.up[to] > time.in[vertex.id]) {
        time.color++;
        PaintDfs(g, time, {to, from_edge_id}, colors, time.color);
      } else {
        PaintDfs(g, time, {to, from_edge_id}, colors, color);
      }
    }
  }
}

std::vector<size_t> GetBridges(
    const std::vector<std::vector<size_t>>& graph,
    const std::vector<std::pair<size_t, size_t>>& edges,
    std::vector<size_t>& colors) {
  size_t num_vertex = graph.size();
  std::vector<size_t> time_in(num_vertex, kInfty);  // time in
  // std::vector<size_t> time_out(num_vertex, kInfty);
  std::vector<size_t> f_up(num_vertex,
                           kInfty);  // aka RET(from lecture) magic function :)
  size_t time = 0;
  size_t color = 0;  // time now
  Time timer{time_in, f_up, time, color};
  std::vector<size_t> bridges;  // result - all bridges

  for (size_t i = 0; i < num_vertex; ++i) {
    if (time_in[i] == kInfty) {
      BridgesDfs(graph, edges, timer, {i, kInfty}, bridges);
    }
  }

  Graph g{graph, edges};
  for (size_t i = 0; i < num_vertex; ++i) {
    if (colors[i] == kInfty) {
      PaintDfs(g, timer, {i, kInfty}, colors, color);
      color++;
    }
  }

  return bridges;
}

void ReadGraph(std::vector<std::pair<size_t, size_t>>& edges_list,
               std::vector<std::vector<size_t>>& graph) {
  size_t from;
  size_t to;
  size_t edges = edges_list.size();
  for (size_t id = 0; id < edges; ++id) {
    std::cin >> from >> to;
    --from;
    --to;
    edges_list[id] = {std::min(from, to),
                      std::max(from, to)};  // make first <= second
    // unoriented edge
    graph[from].push_back(id);
    graph[to].push_back(id);
  }
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
  std::vector<std::vector<size_t>> graph(vertexes, std::vector<size_t>());
  ReadGraph(edges_list, graph);

  // return all bridges id
  std::vector<size_t> colors(vertexes, kInfty);
  auto bridges = GetBridges(graph, edges_list, colors);
  std::vector<size_t> bridge_components;
  for (auto b : bridges) {
    bridge_components.push_back(colors[edges_list[b].first]);
    bridge_components.push_back(colors[edges_list[b].second]);
  }
  std::sort(bridge_components.begin(), bridge_components.end());
  // Print(bridge_components);
  // std::cout << "\ngg\n";
  size_t leaves = 0;
  std::vector<size_t> frequency(vertexes, 0);
  for (auto bc : bridge_components) {
    ++frequency[bc];
  }

  for (size_t i = 0; i < vertexes; ++i) {
    if (frequency[i] == 1) {
      ++leaves;
    }
  }

  std::cout << (leaves / 2) + (leaves % 2) << '\n';
  return 0;
}
