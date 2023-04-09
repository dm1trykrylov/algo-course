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

struct Field {
  std::vector<std::vector<char>> field;
  static const size_t kMaxSize = 256;
  size_t links[kMaxSize];
  size_t exp_flow2 = 0;
};

void GetField(Field& field, size_t height, size_t width) {
  field.links[static_cast<uint8_t>('H')] = 1;
  field.links[static_cast<uint8_t>('O')] = 2;
  field.links[static_cast<uint8_t>('N')] = 3;
  field.links[static_cast<uint8_t>('C')] = 4;
  field.links[static_cast<uint8_t>('.')] = 0;
  field.exp_flow2 = 0;
  field.field.resize(height + 1);
  for (size_t i = 1; i <= height; ++i) {
    field.field[i].resize(width + 1);
    for (size_t j = 1; j <= width; ++j) {
      char cell;
      std::cin >> cell;
      field.field[i][j] = cell;
      field.exp_flow2 += field.links[static_cast<uint8_t>(cell)];
    }
  }
}

class Reader {
 private:
  std::vector<std::vector<int64_t>>& graph_;
  std::vector<Edge>& edges_;
  int64_t source_ = 0;
  int64_t target_ = 0;

 public:
  Reader(std::vector<std::vector<int64_t>>& graph, std::vector<Edge>& edges)
      : graph_(graph), edges_(edges) {}
  void ReadGraph(size_t height, size_t width, Field field) {
    target_ = height * width + 1;
    uint8_t cell;
    size_t idx = 1;
    for (size_t i = 1; i <= height; ++i) {
      for (size_t j = 1; j <= width; ++j) {
        cell = static_cast<uint8_t>(field.field[i][j]);
        if (cell != '.') {         // cell is unbroken
          if ((i + j) % 2 == 1) {  // white cell
            LinkNeighbours(i, j, height, width, field);
            AddEdge(graph_, edges_, source_, idx, field.links[cell]);
          } else {  // black cell
            AddEdge(graph_, edges_, idx, target_, field.links[cell]);
          }
        }
        ++idx;
      }
    }
  }

  static size_t ToIdx(size_t h_i, size_t w_i, size_t width) {
    size_t idx = 0;
    idx += width * (h_i - 1);
    idx += w_i;
    return idx;
  }

  void LinkNeighbours(size_t h_i, size_t w_i, size_t height, size_t width,
                      Field& field) {
    size_t idx = ToIdx(h_i, w_i, width);
    if (h_i > 1 && field.field[h_i - 1][w_i] != '.') {
      AddEdge(graph_, edges_, idx, ToIdx(h_i - 1, w_i, width), 1);
    }
    if (h_i < height && field.field[h_i + 1][w_i] != '.') {
      AddEdge(graph_, edges_, idx, ToIdx(h_i + 1, w_i, width), 1);
    }
    if (w_i > 1 && field.field[h_i][w_i - 1] != '.') {
      AddEdge(graph_, edges_, idx, ToIdx(h_i, w_i - 1, width), 1);
    }
    if (w_i < width && field.field[h_i][w_i + 1] != '.') {
      AddEdge(graph_, edges_, idx, ToIdx(h_i, w_i + 1, width), 1);
    }
  }
};

size_t GetMaxFlow(std::vector<std::vector<int64_t>>& graph,
                  std::vector<Edge>& edges, size_t height, size_t width) {
  int64_t source = 0;
  int64_t target = height * width + 1;
  auto algo = FordFulkerson(graph, edges);
  return algo.FindMaxFlow(source, target);
}

bool Solve(size_t height, size_t width, Field& field) {
  std::vector<std::vector<int64_t>> graph(height * width + 2,
                                          std::vector<int64_t>());
  std::vector<Edge> edges;
  auto reader = Reader(graph, edges);
  reader.ReadGraph(height, width, field);
  if (field.exp_flow2 % 2 != 0 || field.exp_flow2 == 0) {
    return false;
  }
  size_t max_flow = GetMaxFlow(graph, edges, height, width);
  return (max_flow * 2 == field.exp_flow2);
}

int main() {
  size_t height;
  size_t width;
  std::cin >> height >> width;
  Field field;
  GetField(field, height, width);
  std::cout << (Solve(height, width, field) ? "Valid" : "Invalid");
  return 0;
}