#include <algorithm>
#include <iostream>
#include <limits>
#include <vector>

const int32_t kInf = (std::numeric_limits<int32_t>::max() >> 7);
const int32_t kNoEdge = 100000;

struct Edge {
  int32_t first;
  int32_t second;
  int32_t weight;
};

int32_t BellmanFord(size_t vertex_count, std::vector<Edge>& edges,
                    std::vector<int32_t>& parents) {
  std::vector<int32_t> distance(vertex_count, kInf);
  distance[0] = 0;         // starting vertex
  int32_t indicator = -1;  // negative cycle indicator
  for (size_t i = 0; i < vertex_count; ++i) {
    indicator = -1;
    for (auto & [ from, to, weight ] : edges) {
      if (distance[to] > distance[from] + weight) {
        distance[to] = std::max(-kInf, distance[from] + weight);
        parents[to] = from;
        indicator = to;
      }
    }
  }
  return indicator;
}

bool FindNegativeCycle(size_t vertex_count, std::vector<Edge>& edges,
                       std::vector<int32_t>& cycle) {
  std::vector<int32_t> parents(vertex_count, -1);
  int32_t indicator = BellmanFord(vertex_count, edges, parents);
  if (indicator == -1) {
    return false;
  }
  int32_t start = indicator;
  for (size_t i = 0; i < vertex_count; ++i) {
    start = parents[start];  // to avoid endless loop where parents[u] = u
  }
  int32_t current_vertex = start;
  while (true) {
    cycle.push_back(current_vertex);
    if (current_vertex == start && cycle.size() > 1) {
      break;
    }
    current_vertex = parents[current_vertex];
  }
  std::reverse(cycle.begin(), cycle.end());
  return true;
}

int main() {
  int32_t vertex_count;
  std::cin >> vertex_count;
  std::vector<Edge> edges;
  for (int i = 0; i < vertex_count; ++i) {
    for (int j = 0; j < vertex_count; ++j) {
      int32_t weight;
      std::cin >> weight;
      if (weight != kNoEdge) {
        edges.push_back({i, j, weight});
      }
    }
  }
  std::vector<int32_t> cycle;
  if (FindNegativeCycle(vertex_count, edges, cycle)) {
    std::cout << "YES\n" << cycle.size() << '\n';
    for (auto vertex : cycle) {
      std::cout << vertex + 1 << ' ';
    }
  } else {
    std::cout << "NO";
  }

  return 0;
}