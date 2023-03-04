#include <iostream>
#include <limits>
#include <unordered_map>
#include <vector>

static constexpr int32_t kInfty = 6000000;

void Floyd(size_t vertex_count, std::vector<std::vector<int32_t>>& distance) {
  for (size_t middle = 0; middle < vertex_count; ++middle) {
    for (size_t from = 0; from < vertex_count; ++from) {
      for (size_t to = 0; to < vertex_count; ++to) {
        if (distance[from][middle] < kInfty && distance[middle][to] < kInfty) {
          distance[from][to] =
              std::min(distance[from][to],
                       distance[from][middle] + distance[middle][to]);
        }
      }
    }
  }
}

void ReadGraph(size_t edges_count,
               std::vector<std::vector<int32_t>>& distance) {
  uint32_t from, to;
  int32_t weight;
  for (size_t i = 0; i < edges_count; ++i) {
    std::cin >> from >> to >> weight;
    --from;
    --to;
    distance[from][to] = weight;
  }
}

int main() {
  size_t vertex_count, edges_count;
  std::cin >> vertex_count >> edges_count;
  std::vector<std::vector<int32_t>> distance(vertex_count);
  for (size_t i = 0; i < vertex_count; ++i) {
    distance[i].resize(vertex_count, kInfty);
    distance[i][i] = 0;
  }
  ReadGraph(edges_count, distance);
  Floyd(vertex_count, distance);
  size_t source = 0;
  for (size_t dest = 0; dest < vertex_count; ++dest) {
    std::cout << (distance[source][dest] < 30000 ? distance[source][dest] : 30000) << ' ';
  }
  return 0;
}