#include <algorithm>
#include <iostream>
#include <limits>
#include <queue>
#include <string>
#include <vector>

// static constexpr int kInfinity = std::numeric_limits<int>::max();

enum { White, Grey, Black };
const size_t kAddSpace = 9;      // Additional space for safety
const size_t kMaxLength = 2100;  // Maximal string length
const int32_t kMaxFlow = (1 << 13);
const size_t kInf = 1e6;

struct Edge {
  int32_t from;
  int32_t to;
  int32_t flow;
  int32_t capacity;

  int32_t CurrentCapacity() const { return capacity - flow; }

  Edge(int32_t from, int32_t to_v, int32_t capacity)
      : from(from), to(to_v), flow(0), capacity(capacity) {}

  Edge(int32_t from, int32_t to_v, int32_t flow, int32_t capacity)
      : from(from), to(to_v), flow(flow), capacity(capacity) {}
};

int32_t edge_matrix[kMaxLength][kMaxLength];

class Graph {
 public:
  Graph(size_t vertices)
      : graph_(vertices, std::vector<size_t>()) /*,
         edge_matrix_(vertices, std::vector<int32_t>())*/
  {
    for (size_t i = 0; i < vertices; ++i) {
      for (size_t j = 0; j < vertices; ++j) {
        edge_matrix[i][j] = -1;
      }
    }
  }

  std::vector<std::vector<size_t>>& List() { return graph_; }

  std::vector<Edge>& Edges() { return edges_; }

  void AddEdge(int32_t from, int32_t to_v, int32_t capacity) {
    if (edge_matrix[from][to_v] == -1) {
      edges_.emplace_back(from, to_v, capacity);
      size_t edge_id = edges_.size() - 1;
      edges_.emplace_back(to_v, from, capacity);
      size_t back_edge_id = edges_.size() - 1;
      graph_[from].push_back(edge_id);
      edge_matrix[from][to_v] = edge_id;
      graph_[to_v].push_back(back_edge_id);
      edge_matrix[to_v][from] = back_edge_id;

    } else {
      ++edges_[edge_matrix[from][to_v]].capacity;
      ++edges_[edge_matrix[to_v][from]].capacity;
    }
  }
  /*
    bool BFS(size_t start, size_t end) {
      layer_.assign(graph_.size(), kInfinity);
      std::queue<size_t> queue;
      layer_[start] = 0;
      queue.push(start);
      while (!queue.empty() && layer_[end] == kInfinity) {
        size_t vertex = queue.front();
        queue.pop();
        for (size_t i = 0; i < graph_[vertex].size(); ++i) {
          size_t edge_id = graph_[vertex][i];
          if (layer_[edges_[edge_id].to] == kInfinity &&
              edges_[edge_id].flow < edges_[edge_id].capacity) {
            queue.push(edges_[edge_id].to);
            layer_[edges_[edge_id].to] = layer_[edges_[edge_id].from] + 1;
          }
        }
      }
      return layer_[end] != kInfinity;  // увы ниче не нашли!
    }

    int DFS(std::vector<char>& colors, size_t vertex, size_t end,
            int current_flow) {
      if (current_flow <= 0) {
        return 0;
      }
      if (vertex == end) {
        return current_flow;
      }
      colors[vertex] = Grey;
      for (size_t to_id : graph_[vertex]) {
        size_t to_v = edges_[to_id].to;
        if (colors[to_v] == White && layer_[to_v] == layer_[vertex] + 1) {
          int flow = DFS(colors, to_v, end,
                         std::min(current_flow,
                                  edges_[to_id].capacity - edges_[to_id].flow));
          if (flow > 0) {
            edges_[to_id].flow += flow;
            edges_[to_id ^ 1].flow -= flow;
            colors[vertex] = Black;
            return flow;
          }
        }
      }

      colors[vertex] = Black;
      return 0;
    }

    int DinicAlgo(size_t start, size_t end) {
      int flow = 0;
      while (BFS(start, end)) {
        int delta = 0;
        do {
          std::vector<char> colors(graph_.size(), White);
          delta = DFS(colors, start, end, kInfinity);
          flow += delta;
        } while (delta > 0);
      }
      return flow;
    }
  */
  bool BFS(int64_t flow, size_t source, size_t target) {
    std::fill(layer_.begin(), layer_.end(), kInf);
    layer_[source] = 0;
    std::queue<size_t> bfs_queue;
    bfs_queue.push(source);
    while (!bfs_queue.empty() && layer_[target] == kInf) {
      size_t current = bfs_queue.front();
      bfs_queue.pop();
      for (auto edge_id : graph_[current]) {
        size_t to_v = edges_[edge_id].to;
        if (layer_[to_v] == kInf && edges_[edge_id].CurrentCapacity() >= flow) {
          layer_[to_v] = layer_[current] + 1;
          bfs_queue.push(to_v);
        }
      }
    }
    return (layer_[target] != kInf);
  }

  bool DFS(size_t vertex, int64_t flow,
           size_t target) {  // Check if it is possible to push
                             // 'flow' in current network
    if (flow == 0) {
      return false;
    }
    if (vertex == target) {
      return true;
    }
    for (; edge_index_[vertex] < graph_[vertex].size(); edge_index_[vertex]++) {
      int64_t edge_id = graph_[vertex][edge_index_[vertex]];
      int64_t to_v = edges_[edge_id].to;

      if (layer_[to_v] == layer_[vertex] + 1 &&
          edges_[edge_id].CurrentCapacity() >= flow) {
        bool pushed = DFS(to_v, flow, target);
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
    // source_ = source;
    // target_ = target;
    layer_.resize(graph_.size() + 2);
    edge_index_.resize(graph_.size() + 2);
    int64_t flow = 0;
    for (int64_t new_flow = kMaxFlow; new_flow >= 1;) {
      if (!BFS(new_flow, source,
               target)) {  // try to push 'new_flow' from s to t
        new_flow >>= 1;
        continue;
      }
      std::fill(edge_index_.begin(), edge_index_.end(), 0);
      while (DFS(source, new_flow, target)) {  // push 'new_flow'
        flow = flow + new_flow;
      }
    }
    return flow;
  }

 private:
  std::vector<std::vector<size_t>> graph_;
  std::vector<Edge> edges_;
  std::vector<int32_t> layer_;      // vertex layer in BFS traverse
  std::vector<size_t> edge_index_;  // index of first available edge from vertex
};

static bool IsQuestion(char character) {  // checks if c is '?'
  return character == '?';
}

class Network {
  Graph& graph_;
  std::string& str_;
  std::string& pattern_;
  size_t distance_;
  const size_t kPtrOffset = 5;

 public:
  size_t source = 0;
  size_t target = 1;
  size_t max_size;

  Network(Graph& graph, std::string& str, std::string& pattern)
      : graph_(graph), str_(str), pattern_(pattern) {
    distance_ = 0;
    max_size = str.length() + pattern.length() + kAddSpace;
  }
  size_t CharToIdx(size_t pos,
                   uint8_t type) {  // type 0 -- strind, 1 -- pattern
    return pos + kPtrOffset + str_.length() * type;
  }

  std::pair<size_t, uint8_t> IdxToPos(size_t idx) {
    uint8_t type = 0;
    size_t pos = 0;
    if (idx > str_.length() + kPtrOffset - 1) {
      type = 1;
      pos = idx - str_.length() - kPtrOffset;
    } else {
      pos = idx - kPtrOffset;
    }
    return {pos, type};
  }

  void BuildNetwork() {
    size_t offset = str_.length() - pattern_.length();
    size_t init_distance = 0;
    distance_ = 0;
    for (size_t str_pos = 0; str_pos <= offset; ++str_pos) {
      for (size_t patt_pos = 0; patt_pos < pattern_.length(); ++patt_pos) {
        char str_c = str_[str_pos + patt_pos];  // current character in strig
        char patt_c = pattern_[patt_pos];       // current character in pattern
        size_t str_id = CharToIdx(str_pos + patt_pos, 0);
        size_t patt_id = CharToIdx(patt_pos, 1);

        if (!IsQuestion(str_c) && !IsQuestion(patt_c)) {
          if (str_c != patt_c) {  // both characters are unequal and not '?'
            ++init_distance;
          }
        }

        if (IsQuestion(patt_c)) {
          switch (str_c) {
            case '0':
              graph_.AddEdge(source, patt_id, 1);
              break;
            case '1':
              graph_.AddEdge(patt_id, target, 1);
              break;
            case '?':
              graph_.AddEdge(patt_id, str_id, 1);
              break;
          }
        }

        if (IsQuestion(str_c)) {
          switch (patt_c) {
            case '0':
              graph_.AddEdge(source, str_id, 1);
              break;
            case '1':
              graph_.AddEdge(str_id, target, 1);
              // graph_.AddEdge(target, str_id, 1);
              break;
          }
        }
      }
    }
    distance_ += init_distance;
  }

  size_t FindDistance() {
    distance_ += graph_.FindMaxFlow(source, target);
    return distance_;
  }

  void DFS(size_t vertex, std::vector<bool>& visited) {
    visited[vertex] = true;
    if (vertex != source && vertex != target) {
      auto curr = IdxToPos(vertex);
      if (curr.second == 0) {
        str_[curr.first] = '0';
      } else {
        pattern_[curr.first] = '0';
      }
    }
    for (auto edge_id : graph_.List()[vertex]) {
      auto edge_to = graph_.Edges()[edge_id];
      if (!visited[edge_to.to] && edge_to.capacity > 0 &&
          edge_to.CurrentCapacity() > 0) {
        DFS(edge_to.to, visited);
      }
    }
  }

  void SetZeroes() {
    std::vector<bool> visited(max_size, false);
    DFS(source, visited);
  }

  void SetOnes() {
    for (size_t i = 0; i < str_.length(); ++i) {
      if (IsQuestion(str_[i])) {
        str_[i] = '1';
      }
    }
    for (size_t i = 0; i < pattern_.length(); ++i) {
      if (IsQuestion(pattern_[i])) {
        pattern_[i] = '1';
      }
    }
  }
};

int main() {
  std::string str;
  std::string pattern;
  std::cin >> str >> pattern;
  size_t vertices_count = str.length() + pattern.length() + kAddSpace;
  Graph graph(vertices_count);
  Network net(graph, str, pattern);
  net.BuildNetwork();  // distance without '?'
  std::cout << net.FindDistance() << '\n';
  net.SetZeroes();
  net.SetOnes();
  std::cout << str << '\n' << pattern;
  return 0;
}