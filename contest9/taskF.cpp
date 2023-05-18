// 87093283

#include <algorithm>
#include <functional>
#include <iostream>
#include <limits>
#include <numeric>
#include <optional>
#include <queue>
#include <stack>
#include <unordered_map>
#include <vector>

template <class T>
using FilterFunction = std::function<bool(const T)>;

template <class VType = size_t, class EType = std::pair<VType, VType>>
class IteratorImpl {
 public:
  using Iterator = IteratorImpl;

  IteratorImpl(VType vertex, typename std::vector<EType>::iterator begin,
               typename std::vector<EType>::iterator end,
               const FilterFunction<EType>& filter)
      : v_(vertex), now_(begin), end_(end), kFilter(filter) {
    if (now_ != end_) {
      now_edge_ = *now_;
      if (!kFilter(now_edge_)) {
        ++*this;
      }
    } else {
      now_edge_ = EType();
    }
  }

  IteratorImpl& operator++() {
    do {
      ++now_;
      if (now_ == end_) {
        return *this;
      }
      now_edge_ = *now_;
    } while (!kFilter(now_edge_) && now_ != end_);
    return *this;
  }

  bool operator==(const IteratorImpl& other) const {
    return now_ == other.now_;
  }

  bool operator!=(const IteratorImpl& other) const {
    return now_ != other.now_;
  }

  // NOLINTNEXTLINE
  IteratorImpl begin() const { return *this; }

  // NOLINTNEXTLINE
  IteratorImpl end() const { return {v_, end_, end_, kFilter}; }

  const EType& operator*() { return now_edge_; }

 private:
  EType now_edge_;
  VType v_;
  typename std::vector<EType>::iterator now_;
  typename std::vector<EType>::iterator end_;
  const FilterFunction<EType> kFilter;
};

template <class VType = size_t, class EType = std::pair<VType, VType>>
class Graph {
 public:
  using VertexType = VType;
  using EdgeType = EType;

  virtual size_t EdgeCount() const = 0;
  virtual size_t VertexCount() const = 0;
  virtual std::vector<VType> Vertices() const = 0;

  virtual typename std::vector<EType>::iterator NeighboursBegin(
      VType vertex) = 0;
  virtual typename std::vector<EType>::iterator NeighboursEnd(VType vertex) = 0;
  virtual IteratorImpl<VType, EType> NeighboursIt(
      VType vertex, const FilterFunction<EType>& filter) = 0;

  virtual ~Graph() = default;
};

struct Edge {
  int64_t first;     // source vertex
  int64_t second;    // destinaton
  int64_t capacity;  // edge max capacity
  int64_t flow;      // flow over edge
  Edge* back = nullptr;

  int64_t CurrentCapacity() const { return capacity - flow; }

  Edge() : Edge(0, 0, 0, 0) {}

  Edge(int64_t from, int64_t to_vertex, int64_t capacity, int64_t flow,
       Edge* back = nullptr)
      : first(from),
        second(to_vertex),
        capacity(capacity),
        flow(flow),
        back(back) {}
};

template <class VType = size_t>
class ListGraph : public Graph<VType, Edge*> {
 public:
  using EType = Edge*;
  ListGraph(const std::vector<VType>& vertices, const std::vector<EType>& edges)
      : vertices_(vertices),
        edges_(edges),
        num_vertex_(vertices.size()),
        num_edges_(edges.size()) {
    for (const auto& edge : edges) {
      adjacency_lists_[edge->first].push_back(edge);
    }
  }
  void AddEdge(int32_t from, int32_t to_v, int32_t capacity,
               bool add_inv = true) {
    edges_.push_back(new Edge(from, to_v, capacity, 0));
    size_t edge_id = edges_.size() - 1;
    auto* edge = edges_.back();
    if (add_inv) {
      edges_.push_back(new Edge(to_v, from, 0, 0));
      size_t back_edge_id = edges_.size() - 1;
      auto* back_edge = edges_.back();
      adjacency_lists_[to_v].push_back(back_edge);
      back_edge->back = edge;
      // adjacency_lists_[back_edge_id]->back = edges_[edge_id];
      // edges_[edge_id]->back = edges_[back_edge_id];
      edge->back = back_edge;
    }

    adjacency_lists_[from].push_back(edge);
  } /*
   void AddEdge(int64_t from, int64_t to_vertex, int64_t capacity) {
     edges_.emplace_back(new Edge(from, to_vertex, capacity));
     auto* edge = edges_.back();
     edges_.emplace_back(new Edge(to_vertex, from, 0));  // back edge
     auto* back_edge = edges_.back();
     edge->back = back_edge;
     back_edge->back = edge;
     adjacency_lists_[from].push_back(edge);
     adjacency_lists_[to_vertex].push_back(back_edge);
   }*/

  size_t EdgeCount() const final { return num_edges_; }

  size_t VertexCount() const final { return num_vertex_; }

  typename std::vector<EType>::iterator NeighboursBegin(VType vertex) final {
    return adjacency_lists_[vertex].begin();
  }

  typename std::vector<EType>::iterator NeighboursEnd(VType vertex) final {
    return adjacency_lists_[vertex].end();
  }

  std::vector<VType> Vertices() const final { return vertices_; }

  std::vector<EType> Edges() const { return edges_; }

  IteratorImpl<VType, EType> NeighboursIt(
      VType vertex, const FilterFunction<EType>& filter) override {
    return {vertex, NeighboursBegin(vertex), NeighboursEnd(vertex), filter};
  }

  ~ListGraph() {
    for (auto* edge : edges_) {
      delete edge;
    }
  }

 protected:
  std::unordered_map<VType, std::vector<EType>> adjacency_lists_;
  std::vector<VType> vertices_;
  std::vector<EType> edges_;
  size_t num_vertex_;
  size_t num_edges_;
};

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

class Network {
 private:
  ListGraph<int64_t> graph_;
  std::vector<std::vector<int64_t>> adjacency_list_;
  std::vector<Edge*> edges_;
  size_t source_;
  size_t target_;

 public:
  // Initialize network
  Network(size_t vertices_count, const std::vector<int64_t>& vertices_tmp)
      : adjacency_list_(vertices_count + 2, std::vector<int64_t>()),
        edges_(std::vector<Edge*>()),
        graph_(vertices_tmp, edges_) {
    source_ = 0;
    target_ = vertices_count + 1;
    // std::vector<int64_t> vertices_tmp(vertices_count + 2);
    // graph_ = ListGraph<int64_t>(vertices_tmp, edges_);
    utility.resize(vertices_count + 2);
  }

  void SetSource(int64_t source) { source_ = source; }
  void SetTarget(int64_t target) { target_ = target; }

  void AddNeighbours(std::pair<size_t, size_t> point, Encoder& enc) {
    size_t pt_x = point.first;
    size_t pt_y = point.second;
    size_t idx = enc.InIdx(point);
    size_t lower = enc.InIdx({pt_x + 1, pt_y});
    size_t right = enc.InIdx({pt_x, pt_y + 1});
    graph_.AddEdge(enc.OutIdx(idx), lower, kMaxFlow);
    graph_.AddEdge(enc.OutIdx(lower), idx, kMaxFlow);
    graph_.AddEdge(enc.OutIdx(idx), right, kMaxFlow);
    graph_.AddEdge(enc.OutIdx(right), idx, kMaxFlow); /*
     AddEdge(enc.OutIdx(idx), lower, kMaxFlow);
     AddEdge(enc.OutIdx(lower), idx, kMaxFlow);
     AddEdge(enc.OutIdx(idx), right, kMaxFlow);
     AddEdge(enc.OutIdx(right), idx, kMaxFlow);*/
  }

  void ConnectNeighbours(size_t height, size_t width, Encoder& enc) {
    for (size_t i = 0; i < height - 1; ++i) {
      for (size_t j = 0; j < width - 1; ++j) {
        AddNeighbours({i, j}, enc);
      }
    }

    for (size_t i = 0; i < height - 1; ++i) {
      graph_.AddEdge(enc.OutIdx({i, width - 1}), enc.InIdx({i + 1, width - 1}),
                     kMaxFlow);
      graph_.AddEdge(enc.OutIdx({i + 1, width - 1}), enc.InIdx({i, width - 1}),
                     kMaxFlow);
      /*AddEdge(enc.OutIdx({i, width - 1}), enc.InIdx({i + 1, width - 1}),
              kMaxFlow);
      AddEdge(enc.OutIdx({i + 1, width - 1}), enc.InIdx({i, width - 1}),
              kMaxFlow);*/
    }

    for (size_t j = 0; j < width - 1; ++j) {
      graph_.AddEdge(enc.OutIdx({height - 1, j}),
                     enc.InIdx({height - 1, j + 1}), kMaxFlow);
      graph_.AddEdge(enc.OutIdx({height - 1, j + 1}),
                     enc.InIdx({height - 1, j}), kMaxFlow);
      /*AddEdge(enc.OutIdx({height - 1, j}), enc.InIdx({height - 1, j + 1}),
              kMaxFlow);
      AddEdge(enc.OutIdx({height - 1, j + 1}), enc.InIdx({height - 1, j}),
              kMaxFlow);*/
    }
  }
  /*
    void AddEdge(int32_t from, int32_t to_v, int32_t capacity,
                 bool add_inv = true) {
      edges_.push_back(new Edge(from, to_v, capacity, 0));
      size_t edge_id = edges_.size() - 1;
      if (add_inv) {
        edges_.push_back(new Edge(to_v, from, 0, 0));
        size_t back_edge_id = edges_.size() - 1;
        adjacency_list_[to_v].push_back(back_edge_id);
        edges_[back_edge_id]->back = edges_[edge_id];
        edges_[edge_id]->back = edges_[back_edge_id];
      }

      adjacency_list_[from].push_back(edge_id);
    }
  */
  size_t VerticesCount() const { /*return adjacency_list_.size();*/
    return graph_.VertexCount();
  }

  std::vector<Edge*> Edges() const { /*return edges_;*/
    return graph_.Edges();
  }
  IteratorImpl<int64_t, Edge*> NeighboursIt(
      int64_t vertex, const FilterFunction<Edge*>& filter) {
    return graph_.NeighboursIt(vertex, filter);
  }
  /*
    const std::vector<int64_t>& Neighbours(size_t vertex) const {
      return adjacency_list_[vertex];
    }
    Edge* GetEdge(size_t edge_id) { return edges_[edge_id]; }
  */
  constexpr size_t Source() const { return source_; }
  constexpr size_t Target() const { return target_; }

  ~Network() {
    for (auto* edge : edges_) {
      delete edge;
    }
  }
  static const int64_t kMaxFlow = 1 << 12;
  std::vector<int64_t> utility;
};

struct Dinic {
 private:
  Network& net_;
  std::vector<int64_t> layer_;      // vertex layer in BFS traverse
  std::vector<size_t> edge_index_;  // index of first available edge from vertex
  const int64_t kInf = 1 << 30;

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
      auto neighbours = net_.NeighboursIt(current, [&](const Edge* edge) {
        return layer_[edge->second] == kInf && edge->CurrentCapacity() >= 0;
      });
      for (auto edge_id : neighbours) {
        auto* edge = edge_id /*net_.GetEdge(edge_id)*/;
        size_t to_v = edge->second;
        // if (layer_[to_v] == kInf && edge->CurrentCapacity() >= flow) {
        layer_[to_v] = layer_[current] + 1;
        bfs_queue.push(to_v);
        //}
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
    auto neighbours =
        net_.NeighboursIt(vertex, [](const Edge*) { return true; });
    for (; edge_index_[vertex] < net_.Neighbours(vertex).size();
         edge_index_[vertex]++) {
      int64_t edge_id = net_.Neighbours(vertex)[edge_index_[vertex]];
      auto* edge = net_.GetEdge(edge_id);
      int64_t to_v = edge->second;

      if (layer_[to_v] == layer_[vertex] + 1 &&
          edge->CurrentCapacity() >= flow) {
        bool pushed = DFS(to_v, flow);
        if (pushed) {
          edge->flow += flow;
          if (edge->back != nullptr) {
            edge->back->flow -= flow;
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

// Build network anr return start-target pair
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
        net.AddEdge(idx, enc.OutIdx(idx), Network::kMaxFlow, false);
      }
    }
  }
  net.ConnectNeighbours(height, width, enc);
  return {real_source, real_target};
}

void WalkInResidualNetwork(Network& net, size_t vertex,
                           std::vector<bool>& visited) {
  visited[vertex] = true;
  for (auto edge_id : net.Neighbours(vertex)) {
    auto* edge_to = net.GetEdge(edge_id);
    if (!visited[edge_to->second] && edge_to->CurrentCapacity() > 0) {
      WalkInResidualNetwork(net, edge_to->second, visited);
    }
  }
}

void Solve(Network& net, std::pair<size_t, size_t> st_pair, Encoder& enc) {
  Dinic dinic(net);
  auto flow = dinic.FindMaxFlow();
  if (flow >= Network::kMaxFlow) {
    std::cout << -1;
    return;
  }
  std::cout << flow << '\n';
  std::vector<bool> used(net.VerticesCount(), false);
  size_t source = st_pair.first;
  WalkInResidualNetwork(net, source, used);
  for (auto* edge : net.Edges()) {
    if (used[edge->first] && !used[edge->second] && edge->flow == 1) {
      std::cout << enc.IdxToCoordinates(edge->second) << '\n';
    }
  }
}

void EnableFastIO() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
}

int main() {
  EnableFastIO();
  size_t height;
  size_t width;
  std::cin >> height >> width;
  Encoder enc{height, width};
  const size_t kScaleFactor = 5;  // to have additional space for vertices
  std::vector<int64_t> vertices_tmp(height * width * kScaleFactor + 2);
  std::iota(vertices_tmp.begin(), vertices_tmp.end(), 0);
  Network net(height * width * kScaleFactor, vertices_tmp);
  auto st_pair = BuildNetwork(height, width, enc, net);
  Solve(net, st_pair, enc);
  return 0;
}