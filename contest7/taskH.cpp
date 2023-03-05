#include <iostream>
#include <queue>
#include <unordered_map>
#include <vector>

class State {
 private:
  uint64_t data_;
  size_t zero_pos_;
  const uint64_t mask = 0xF;
  static constexpr uint64_t AntiMasks[] = {
      0xFFFFFFFF0, 0xFFFFFFF0F, 0xFFFFFF0FF, 0xFFFFF0FFF, 0xFFFF0FFFF,
      0xFFF0FFFFF, 0xFF0FFFFFF, 0xF0FFFFFFF, 0x0FFFFFFFF};

  static constexpr uint64_t Masks[] = {0x00000000F, 0x0000000F0, 0x000000F00,
                                       0x00000F000, 0x0000F0000, 0x000F00000,
                                       0x00F000000, 0x0F0000000, 0xF00000000};

 public:
  State() : State(0){};
  State(uint64_t data) : data_(data) {}
  State(const State& rhs) {
    data_ = rhs.data_;
    zero_pos_ = rhs.zero_pos_;
  }
  State(std::vector<size_t>& tiles);
  State& operator=(const State& rhs) {
    data_ = rhs.data_;
    zero_pos_ = rhs.zero_pos_;
    return *this;
  }
  uint64_t Data() const { return data_; }
  size_t Zero() const { return zero_pos_; }
  uint64_t GetTile(size_t position) const;
  void SetTile(size_t position, uint64_t value);
  void Swap(size_t lhs, size_t rhs);
  State SwapC(size_t lhs, size_t rhs) {
    State new_state(*this);
    new_state.Swap(lhs, rhs);
    return new_state;
  }
  void Print();
};

bool operator==(const State& lhs, const State& rhs) {
  return lhs.Data() == rhs.Data();
}

bool operator<(const State& lhs, const State& rhs) {
  return lhs.Data() < rhs.Data();
}

bool Relax(State& from_state, State& to_state, State& source,
           std::unordered_map<uint64_t, size_t>& distance,
           std::priority_queue<std::pair<size_t, State>,
                               std::vector<std::pair<size_t, State>>,
                               std::greater<std::pair<size_t, State>>>& queue) {
  uint64_t to = to_state.Data();
  uint64_t from = from_state.Data();
  if ((distance[to] == 0 && to != source.Data()) ||
      distance[to] > distance[from] + 1) {
    distance[to] = distance[from] + 1;
    queue.push({distance[to], to_state});
    return true;
  }
  return false;
}

void Dijkstra(State& source, State& target,
              std::unordered_map<uint64_t, size_t>& distance) {
  std::priority_queue<std::pair<size_t, State>,
                      std::vector<std::pair<size_t, State>>,
                      std::greater<std::pair<size_t, State>>>
      queue;
  auto td = std::make_pair(distance[source.Data()], source);
  queue.emplace(td);
  size_t zero_pos;
  while (!queue.empty()) {
    auto from = queue.top().second;
    queue.pop();
    // from.Print();
    // std::cout << distance[from.Data()] << '\n';
    if (from == target) {
      break;
    }
    zero_pos = from.Zero();
    if (zero_pos % 3 > 0) {  // move zero left
      auto left = from;
      left.Swap(zero_pos, zero_pos - 1);
      Relax(from, left, source, distance, queue);
    }
    if (zero_pos % 3 < 2) {  // move zero right
      auto right = from;
      right.Swap(zero_pos, zero_pos + 1);
      Relax(from, right, source, distance, queue);
    }
    if (zero_pos > 2) {  // move zero up
      auto up = from;
      up.Swap(zero_pos, zero_pos - 3);
      Relax(from, up, source, distance, queue);
    }
    if (zero_pos < 6) {  // move zero down
      auto down = from;
      down.Swap(zero_pos, zero_pos + 3);
      Relax(from, down, source, distance, queue);
    }
  }
}

bool CheckZeroAccessibility(std::vector<uint64_t>& board, State& state) {
  size_t counter = 0;
  size_t zero_row = state.Zero() / 3 + 1;
  for (size_t i = 0; i < board.size(); ++i) {
    for (size_t j = 0; j < i; ++j) {
      if (board[i] != 0 && board[i] < board[j]) {
        ++counter;
      }
    }
  }
  return (counter + zero_row) % 2 == 0;
}

int main() {
  std::vector<uint64_t> source_board(9, 0);
  std::vector<uint64_t> target_board = {1, 2, 3, 4, 5, 6, 7, 8, 0};
  for (size_t i = 0; i < 3; ++i) {
    for (size_t j = 0; j < 3; ++j) {
      std::cin >> source_board[i * 3 + j];
    }
  }
  State source(source_board);
  State target(target_board);
  if (CheckZeroAccessibility(source_board, source)) {
    std::unordered_map<uint64_t, size_t> distance;
    Dijkstra(source, target, distance);
    std::cout << distance[target.Data()];
  } else {
    std::cout << -1;
  }

  return 0;
}

State::State(std::vector<size_t>& tiles) {
  for (size_t i = 0; i < tiles.size(); ++i) {
    SetTile(i, tiles[i]);
  }
}

uint64_t State::GetTile(size_t position) const {
  return (data_ & Masks[position]) >> (position * 4);
}

void State::SetTile(size_t position, uint64_t value) {
  value &= Masks[0];
  if (value == 0) {
    zero_pos_ = position;
  }
  data_ &= AntiMasks[position];
  value <<= (position * 4);
  data_ |= value;
}

void State::Swap(size_t lhs, size_t rhs) {
  uint64_t src = (data_ & Masks[lhs]) >> (lhs * 4);
  uint64_t dest = (data_ & Masks[rhs]) >> (rhs * 4);
  SetTile(lhs, dest);
  SetTile(rhs, src);
}

void State::Print() {
  for (size_t i = 0; i < 3; ++i) {
    for (size_t j = 0; j < 3; ++j) {
      std::cout << GetTile(3 * i + j) << ' ';
    }
    std::cout << '\n';
  }
}

/*

template <>
struct std::hash<State> {
  std::uint64_t operator()(State const& s) const noexcept {
    auto hasher = std::hash<uint64_t>{};
    uint64_t hash = hasher(s.Data());
    return hash;
  }
};

struct TileData {
  size_t dist;
  State state;
  TileData(size_t new_dist, State new_state)
      : dist(new_dist), state(new_state) {}
  TileData& operator=(const TileData& rhs) {
    dist = rhs.dist;
    state = rhs.state;
    return *this;
  }
};

bool operator==(const TileData& lhs, const TileData& rhs) {
  return lhs.dist == rhs.dist && lhs.state == rhs.state;
}

bool operator<(const TileData& lhs, const TileData& rhs) {
  return lhs.dist < rhs.dist;
}
*/