#include <algorithm>
#include <cmath>
#include <iostream>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

const size_t kReal = 1e6 + 2, kExtra = 1e6 + 1e5 + 2;
const uint32_t kInf = 2e9;

struct Dist {
  uint32_t w;
  uint32_t v;
};

bool operator<(const Dist& lhs, const Dist& rhs) {
  return lhs.w < rhs.w || (lhs.w == rhs.w && lhs.v < rhs.v);
}

struct PricesData {
  uint32_t up;
  uint32_t down;
  uint32_t lift_count;
  uint32_t lift_in;
  uint32_t lift_out;
};

class Dijkstra {
 public:
  Dijkstra(PricesData& data) : data_(data) {
    graph_.resize(kExtra + 2);
    std::fill(dist_, dist_ + kExtra + 2, kInf);
    // dist_.resize(kExtra + 2, kInf);
    AddStopFloors();
  };

  void operator()() {
    dist_[1] = 0;
    queue_.insert({0, 1});
    while (!queue_.empty()) {
      uint32_t from = queue_.begin()->second;
      queue_.erase(queue_.begin());
      for (auto edge : graph_[from]) {
        uint32_t weight = edges_[edge].w;
        size_t to_floor = edges_[edge].v;
        if (dist_[from] + weight < dist_[to_floor]) {
          queue_.erase(std::make_pair(dist_[to_floor], to_floor));
          dist_[to_floor] = dist_[from] + weight;
          queue_.insert(std::make_pair(dist_[to_floor], to_floor));
        }
      }
      CheckNeighbours(from);
    }
  }

  uint32_t GetDistance(uint32_t vertex) { return dist_[vertex]; }

 private:
  std::vector<std::vector<uint32_t>> graph_;
  std::vector<Dist> edges_;
  uint32_t dist_[kExtra + 2];
  // std::unordered_map<uint32_t, uint32_t> dist_;
  PricesData& data_;
  std::set<std::pair<uint32_t, uint32_t>> queue_;

  void AddStopFloors() {
    uint32_t stop_floor;
    uint32_t stops;
    uint32_t cnt = kReal + 1;
    for (uint32_t i = 1; i <= data_.lift_count; ++i) {
      std::cin >> stops;
      ++cnt;
      for (uint32_t j = 0; j < stops; ++j) {
        std::cin >> stop_floor;
        graph_[stop_floor].push_back(edges_.size());
        edges_.push_back({data_.lift_in, cnt});
        graph_[cnt].push_back(edges_.size());
        edges_.push_back({data_.lift_out, stop_floor});
      }
    }
  }

  void CheckNeighbours(uint32_t from) {
    if (from > 0 && from < kReal &&
        dist_[from] + data_.down < dist_[from - 1]) {
      queue_.erase(std::make_pair(dist_[from - 1], from - 1));
      dist_[from - 1] = dist_[from] + data_.down;
      queue_.insert(std::make_pair(dist_[from - 1], from - 1));
    }
    if (from < kReal - 1 && dist_[from] + data_.up < dist_[from + 1]) {
      queue_.erase(std::make_pair(dist_[from + 1], from + 1));
      dist_[from + 1] = dist_[from] + data_.up;
      queue_.insert(std::make_pair(dist_[from + 1], from + 1));
    }
  }
};

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  PricesData data;
  uint32_t destination;
  std::cin >> destination >> data.up >> data.down >> data.lift_in >>
      data.lift_out >> data.lift_count;

  Dijkstra dijkstra(data);
  dijkstra();

  std::cout << dijkstra.GetDistance(destination);
  return 0;
}
