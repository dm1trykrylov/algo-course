#include <algorithm>
#include <iostream>
#include <queue>
#include <vector>

const int64_t kMaxFlow = 1 << 12;
const int64_t kInf = 1 << 30;
const size_t kScaleFactor = 5;
// const int64_t kMaxCoordinate = 1001;

struct Edge {
  int64_t from;       // source vertex
  int64_t to_vertex;  // destinaton
  int64_t capacity;   // edge max capacity
  int64_t flow;       // flow over edge
  bool has_inv;
  Edge* inverse = nullptr;

  int64_t CurrentCapacity() const { return capacity - flow; }

  Edge() : Edge(0, 0, 0, 0, true) {}

  Edge(int64_t from, int64_t to_vertex, int64_t capacity, int64_t flow,
       bool has_inv)
      : from(from),
        to_vertex(to_vertex),
        capacity(capacity),
        flow(flow),
        has_inv(has_inv) {}
};

// const size_t kMaxLength = 300;
// int32_t edge_matrix[kMaxLength][kMaxLength];

class Network {
 private:
  std::vector<std::vector<int64_t>> graph_;
  std::vector<Edge*> edges_;
  size_t source_;
  size_t target_;

 public:
  Network(size_t vertices_count)
      : graph_(vertices_count + 2, std::vector<int64_t>()) {
    source_ = 0;
    target_ = vertices_count + 1;
    utility.resize(vertices_count + 2);
    /*for (size_t i = 0; i < vertices_count + 2; ++i) {
      for (size_t j = 0; j < vertices_count + 2; ++j) {
        edge_matrix[i][j] = -1;
      }
    }*/
  }

  void SetSource(int64_t source) { source_ = source; }
  void SetTarget(int64_t target) { target_ = target; }

  void AddEdge(int32_t from, int32_t to_v, int32_t capacity,
               bool add_inv = true) {
    edges_.push_back(new Edge(from, to_v, capacity, 0, add_inv));
    size_t edge_id = edges_.size() - 1;
    if (add_inv) {
      edges_.push_back(new Edge(to_v, from, 0, 0, add_inv));
      size_t back_edge_id = edges_.size() - 1;
      graph_[to_v].push_back(back_edge_id);
      edges_[back_edge_id]->inverse = edges_[edge_id];
      edges_[edge_id]->inverse = edges_[back_edge_id];
      // edge_matrix[to_v][from] = back_edge_id;
    }

    graph_[from].push_back(edge_id);
    // edge_matrix[from][to_v] = edge_id;
  }

  size_t VerticesCount() const { return graph_.size(); }

  std::vector<Edge*>& Edges() { return edges_; }

  const std::vector<int64_t>& Neighbours(size_t vertex) const {
    return graph_[vertex];
  }
  Edge* GetEdge(size_t edge_id) { return edges_[edge_id]; }
  constexpr size_t Source() const { return source_; }
  constexpr size_t Target() const { return target_; }

  ~Network() {
    for (auto* edge : edges_) {
      delete edge;
    }
  }

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
        auto* edge = net_.GetEdge(edge_id);
        size_t to_v = edge->to_vertex;
        if (layer_[to_v] == kInf && edge->CurrentCapacity() >= flow) {
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
      auto* edge = net_.GetEdge(edge_id);
      int64_t to_v = edge->to_vertex;

      if (layer_[to_v] == layer_[vertex] + 1 &&
          edge->CurrentCapacity() >= flow) {
        bool pushed = DFS(to_v, flow);
        if (pushed) {
          edge->flow += flow;
          if (edge->has_inv) {
            edge->inverse->flow -= flow;
          }
          return true;
        }
      }
    }
    return false;
  }

  int64_t FindMaxFlow() {
    size_t source = net_.Source();
    int64_t flow = 0;
    for (int64_t new_flow = 2; new_flow >= 1;) {
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

struct FordFulkerson {
 private:
  Network& net_;

 public:
  FordFulkerson(Network& net) : net_(net) {}

  int64_t FindIncreasingPath(std::vector<uint32_t>& used, uint32_t phase,
                             int64_t vertex, int64_t destination, int64_t min) {
    if (vertex == destination) {
      return min;
    }
    used[vertex] = phase;
    for (int64_t edge_id : net_.Neighbours(vertex)) {
      if (net_.GetEdge(edge_id)->CurrentCapacity() == 0) {
        continue;
      }
      if (used[net_.GetEdge(edge_id)->to_vertex] == phase) {
        continue;
      }
      int64_t delta = FindIncreasingPath(
          used, phase, net_.GetEdge(edge_id)->to_vertex, destination,
          std::min(min, net_.GetEdge(edge_id)->CurrentCapacity()));
      if (delta > 0) {
        net_.GetEdge(edge_id)->flow += delta;
        if (net_.GetEdge(edge_id)->has_inv) {
          net_.GetEdge(edge_id)->inverse->flow -= delta;  // back edge
        }

        return delta;
      }
    }
    return 0;
  }

  int64_t FindMaxFlow() {
    int64_t source = net_.Source();
    int64_t target = net_.Target();
    std::vector<uint32_t> used(net_.VerticesCount(), 0);
    uint32_t phase = 1;
    while (FindIncreasingPath(used, phase, source, target, kInf) > 0) {
      ++phase;
    }

    int64_t result = 0;
    for (uint64_t i = 0; i < net_.Neighbours(source).size(); ++i) {
      result += net_.GetEdge(net_.Neighbours(source)[i])->flow;
    }
    return result;
  }
};

void EnableFastIO() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
}

class Encoder {
 public:
  size_t height;
  size_t width;

  size_t CoordinatesToIdx(std::pair<size_t, size_t> point) const {
    return width * point.first + point.second;
  }
  size_t OutIdx(std::pair<size_t, size_t> point) const {
    return OutIdx(CoordinatesToIdx(point));
  }
  size_t InIdx(std::pair<size_t, size_t> point) const {
    return CoordinatesToIdx(point);
  }
  size_t OutIdx(size_t in_idx) const { return in_idx + width * height; }
  size_t InIdx(size_t out_idx) const { return out_idx - width * height; }

  std::pair<size_t, size_t> IdxToCoordinates(size_t idx) {
    if (idx >= width * height) {
      return IdxToCoordinates(idx - width * height);
    }
    return {idx / width, idx % width};
  }
};

void AddNeighbours(std::pair<size_t, size_t> point, Network& net,
                   Encoder& enc) {
  size_t pt_x = point.first;
  size_t pt_y = point.second;
  size_t idx = enc.InIdx(point);
  size_t lower = enc.InIdx({pt_x + 1, pt_y});
  size_t right = enc.InIdx({pt_x, pt_y + 1});
  net.AddEdge(enc.OutIdx(idx), lower, kMaxFlow);
  net.AddEdge(enc.OutIdx(lower), idx, kMaxFlow);
  net.AddEdge(enc.OutIdx(idx), right, kMaxFlow);
  net.AddEdge(enc.OutIdx(right), idx, kMaxFlow);
}

std::istream& operator>>(std::istream& stream,
                         std::pair<size_t, size_t>& point) {
  stream >> point.first >> point.second;
  --point.first;
  --point.second;
  return stream;
}

std::ostream& operator<<(std::ostream& stream,
                         std::pair<size_t, size_t> point) {
  stream << point.first + 1 << ' ' << point.second + 1;
  return stream;
}

void SetMountains(Encoder& enc, Network& net, std::vector<bool>& used,
                  size_t mountains) {
  std::pair<size_t, size_t> cell;
  size_t idx;
  for (size_t i = 0; i < mountains; ++i) {  // can't go through mountains
    std::cin >> cell;
    idx = enc.InIdx(cell);
    used[idx] = true;
    net.AddEdge(idx, enc.OutIdx(idx), 0, false);
  }
}

void SetAvailableCells(Encoder& enc, Network& net, std::vector<bool>& used,
                       size_t wall_cells) {
  std::pair<size_t, size_t> cell;
  size_t idx;
  for (size_t i = 0; i < wall_cells; ++i) {
    std::cin >> cell;
    idx = enc.InIdx(cell);
    used[idx] = true;
    net.AddEdge(idx, enc.OutIdx(idx), 1, false);
  }
}

void ConnectNeighbours(size_t height, size_t width, Encoder& enc,
                       Network& net) {
  for (size_t i = 0; i < height - 1; ++i) {
    for (size_t j = 0; j < width - 1; ++j) {
      AddNeighbours({i, j}, net, enc);
    }
  }

  for (size_t i = 0; i < height - 1; ++i) {
    net.AddEdge(enc.OutIdx({i, width - 1}), enc.InIdx({i + 1, width - 1}),
                kMaxFlow);
    net.AddEdge(enc.OutIdx({i + 1, width - 1}), enc.InIdx({i, width - 1}),
                kMaxFlow);
  }

  for (size_t j = 0; j < width - 1; ++j) {
    net.AddEdge(enc.OutIdx({height - 1, j}), enc.InIdx({height - 1, j + 1}),
                kMaxFlow);
    net.AddEdge(enc.OutIdx({height - 1, j + 1}), enc.InIdx({height - 1, j}),
                kMaxFlow);
  }
}

std::pair<size_t, size_t> BuildNetwork(size_t height, size_t width,
                                       Encoder& enc, Network& net) {
  size_t mountains;
  size_t wall_cells;
  std::cin >> mountains >> wall_cells;
  std::vector<bool> used((height + 1) * (width + 1) + 2, false);
  SetMountains(enc, net, used, mountains);
  SetAvailableCells(enc, net, used, wall_cells);
  std::pair<size_t, size_t> cell;
  size_t idx;
  std::cin >> cell;  // E
  size_t real_source = enc.OutIdx(cell);
  used[enc.InIdx(cell)] = true;
  net.SetSource(real_source);
  std::cin >> cell;  // C
  size_t real_target = enc.InIdx(cell);
  used[real_target] = true;
  net.SetTarget(real_target);

  for (size_t i = 0; i < height; ++i) {
    for (size_t j = 0; j < width; ++j) {
      idx = enc.InIdx({i, j});
      if (!used[idx]) {
        net.AddEdge(idx, enc.OutIdx(idx), kMaxFlow, false);
      }
    }
  }
  ConnectNeighbours(height, width, enc, net);
  return {real_source, real_target};
}

void WalkInResidualNetwork(Network& net, size_t vertex,
                           std::vector<bool>& visited) {
  visited[vertex] = true;
  for (auto edge_id : net.Neighbours(vertex)) {
    auto* edge_to = net.GetEdge(edge_id);
    if (!visited[edge_to->to_vertex] && edge_to->CurrentCapacity() > 0) {
      WalkInResidualNetwork(net, edge_to->to_vertex, visited);
    }
  }
}

void Solve(Network& net, std::pair<size_t, size_t> st_pair, Encoder& enc) {
  Dinic dinic(net);
  // FordFulkerson ff(net);
  // auto f = ff.FindMaxFlow();
  auto flow = dinic.FindMaxFlow();
  if (flow >= kMaxFlow) {
    std::cout << -1;
    return;
  }
  std::cout << flow << '\n';
  std::vector<bool> used(net.VerticesCount(), false);
  size_t source = st_pair.first;
  WalkInResidualNetwork(net, source, used);
  for (auto* edge : net.Edges()) {
    if (used[edge->from] && !used[edge->to_vertex] && edge->flow == 1) {
      std::cout << enc.IdxToCoordinates(edge->to_vertex) << '\n';
    }
  }
}

int main() {
  EnableFastIO();
  size_t height;
  size_t width;
  std::cin >> height >> width;
  Encoder enc{height, width};
  Network net(height * width * kScaleFactor);
  auto st_pair = BuildNetwork(height, width, enc, net);
  Solve(net, st_pair, enc);
  return 0;
}