#include <iostream>
#include <limits>
#include <vector>

static constexpr int64_t kInfinity = std::numeric_limits<int64_t>::max();
static constexpr size_t kVerticesFactor = 5;

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

struct Table {
  std::vector<std::vector<size_t>> field;
  std::vector<size_t> games_in_div;
  static const size_t kMaxSize = 256;
  size_t exp_flow2 = 0;
};

void GetTable(Table& table, size_t size) {
  table.field.resize(size + 1);
  table.games_in_div.resize(size + 1, 0);
  for (size_t i = 0; i < size; ++i) {
    table.field[i].resize(size + 1);
    for (size_t j = 0; j < size; ++j) {
      size_t games_count;
      std::cin >> games_count;
      table.field[i][j] = games_count;
      table.games_in_div[i] += games_count;
      // table.games_in_div[j] += games_count;
    }
  }
}

class Reader {
 private:
  std::vector<std::vector<int64_t>>& graph_;
  std::vector<Edge>& edges_;
  std::vector<size_t>& wins_;
  std::vector<size_t>& games_left_;
  int64_t source_ = 0;
  int64_t target_ = 0;

 public:
  Reader(std::vector<std::vector<int64_t>>& graph, std::vector<Edge>& edges,
         std::vector<size_t>& wins, std::vector<size_t>& g_left)
      : graph_(graph), edges_(edges), wins_(wins), games_left_(g_left) {}
  void ReadGraph(size_t teams, size_t max_out, Table field) {
    target_ = teams * teams * kVerticesFactor + 1;
    size_t games;
    size_t hub_idx = teams * 2;
    for (size_t i = 1; i < teams; ++i) {
      AddEdge(graph_, edges_, source_, i, games_left_[i]);
      AddEdge(graph_, edges_, OutIdx(i, teams), target_, max_out - wins_[i]);
      for (size_t j = i + 1; j < teams; ++j) {
        games = field.field[i][j];
        if (games > 0) {
          AddEdge(graph_, edges_, i, hub_idx, games);
          AddEdge(graph_, edges_, j, hub_idx, games);
          ++hub_idx;
          AddEdge(graph_, edges_, hub_idx - 1, hub_idx, games);
          AddEdge(graph_, edges_, hub_idx, OutIdx(i, teams), games);
          AddEdge(graph_, edges_, hub_idx, OutIdx(j, teams), games);
          ++hub_idx;
        }
      }
    }
  }

  static size_t OutIdx(size_t idx, size_t count) { return idx + count; }

  static size_t ToIdx(size_t h_i, size_t w_i, size_t width) {
    size_t idx = 0;
    idx += width * (h_i - 1);
    idx += w_i;
    return idx;
  }
};

size_t GetMaxFlow(std::vector<std::vector<int64_t>>& graph,
                  std::vector<Edge>& edges, size_t teams) {
  int64_t source = 0;
  int64_t target = teams * teams * kVerticesFactor + 1;
  auto algo = FordFulkerson(graph, edges);
  return algo.FindMaxFlow(source, target);
}

bool Solve(size_t teams, std::vector<size_t>& wins, std::vector<size_t>& left,
           Table& table) {
  size_t start = 0;
  wins[start] += left[start];
  left[start] = 0;
  size_t expected = 0;  // expercted number of games played
  size_t curr_max = 0;
  for (size_t i = 1; i < teams; ++i) {
    left[i] = table.games_in_div[i];
    left[i] -= table.field[start][i];
    expected += left[i];
    curr_max = std::max(wins[i], curr_max);
  }
  if (curr_max > wins[start]) {
    return false;
  }
  std::vector<std::vector<int64_t>> graph(teams * teams * kVerticesFactor + 2,
                                          std::vector<int64_t>());
  std::vector<Edge> edges;
  auto reader = Reader(graph, edges, wins, left);
  reader.ReadGraph(teams, wins[start], table);
  size_t max_flow = GetMaxFlow(graph, edges, teams);
  return (max_flow * 2 == expected);
}

template <typename T>
void Read(std::vector<T>& destination, size_t size) {
  for (size_t i = 0; i < size; ++i) {
    std::cin >> destination[i];
  }
}

int main() {
  size_t teams;  // number of teams in division
  std::cin >> teams;
  std::vector<size_t> wins(teams, 0);  // number of wins
  Read(wins, teams);
  std::vector<size_t> games_left(teams, 0);  // games left for each team
  Read(games_left, teams);
  Table table;  // games table for division
  GetTable(table, teams);
  std::cout << (Solve(teams, wins, games_left, table) ? "Yes" : "No");
  return 0;
}