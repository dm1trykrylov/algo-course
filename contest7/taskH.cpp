#include <algorithm>
#include <iostream>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>

const size_t kSize = 3;
const size_t kMaxPos = 9;
const size_t kOffset = 3;

class State {
 private:
  uint64_t data_;
  size_t zero_pos_;
  const uint64_t kMask = 0xF;

  const static constexpr uint64_t k_anti_masks[] = {
      0xFFFFFFFF0, 0xFFFFFFF0F, 0xFFFFFF0FF, 0xFFFFF0FFF, 0xFFFF0FFFF,
      0xFFF0FFFFF, 0xFF0FFFFFF, 0xF0FFFFFFF, 0x0FFFFFFFF};

  const static constexpr uint64_t k_masks[] = {
      0x00000000F, 0x0000000F0, 0x000000F00, 0x00000F000, 0x0000F0000,
      0x000F00000, 0x00F000000, 0x0F0000000, 0xF00000000};

 public:
  State() : State(0, 0){};
  State(uint64_t data) : data_(data) {
    zero_pos_ = 8;
    for (size_t pos = 0; pos < 9; ++pos) {
      if (GetTile(pos) == 0) {
        zero_pos_ = pos;
        break;
      }
    }
  }
  constexpr State(const uint64_t kData, const size_t kZeroPos)
      : data_(kData), zero_pos_(kZeroPos) {}
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
  void Print() const;
};

bool operator==(const State& lhs, const State& rhs) {
  return lhs.Data() == rhs.Data();
}

bool operator<(const State& lhs, const State& rhs) {
  return lhs.Data() < rhs.Data();
}

int64_t ManhattanDistance(const State& state);

int64_t ManhattanDistance(uint64_t tile, int64_t position);

class Dijkstra {
 private:
  std::unordered_map<uint64_t, int64_t>& distance_;
  // std::unordered_map<uint64_t, size_t>& h_distance_;
  std::priority_queue<std::pair<int64_t, State>,
                      std::vector<std::pair<int64_t, State>>,
                      std::greater<std::pair<int64_t, State>>>
      queue_;
  // std::unordered_map<uint64_t, std::pair<char, uint64_t>> parents_;
  std::unordered_map<uint64_t, char> parents_;
  int64_t add_, substract_;

  bool Relax(State& from_state, State& to_state, State& source,
             char direction) {
    uint64_t to = to_state.Data();
    uint64_t from = from_state.Data();
    if (distance_[to] == 0 && to != source.Data()) {
      distance_[to] = distance_[from] + 1;
      queue_.push({distance_[to] + add_ - substract_, to_state});
      // parents_[to] = {direction, from};
      parents_[to] = direction;
      return true;
    }
    return false;
  }

 public:
  Dijkstra(std::unordered_map<uint64_t, int64_t>& distance)
      : distance_(distance) {}
  void operator()(State& source, const State& target) {
    // parents_[source.Data()] = {'S', source.Data()};
    parents_[source.Data()] = 'S';
    distance_[source.Data()] = 0;
    queue_.push({distance_[source.Data()] + ManhattanDistance(source), source});
    size_t zero_pos;

    while (!queue_.empty()) {
      auto from = queue_.top().second;
      queue_.pop();
      if (from.Data() == target.Data()) {
        break;
      }
      zero_pos = from.Zero();
      if (zero_pos % 3 > 0) {  // move zero left
        auto left = from;
        substract_ = ManhattanDistance(left.GetTile(zero_pos - 1));
        left.Swap(zero_pos, zero_pos - 1);
        add_ = ManhattanDistance(left.GetTile(zero_pos));
        Relax(from, left, source, 'L');
      }
      if (zero_pos % 3 < 2) {  // move zero right
        auto right = from;
        substract_ = ManhattanDistance(right.GetTile(zero_pos + 1));
        right.Swap(zero_pos, zero_pos + 1);
        add_ = ManhattanDistance(right.GetTile(zero_pos));
        Relax(from, right, source, 'R');
      }
      if (zero_pos > 2) {  // move zero up
        auto up = from;
        substract_ = ManhattanDistance(up.GetTile(zero_pos - 3));
        up.Swap(zero_pos, zero_pos - 3);
        add_ = ManhattanDistance(up.GetTile(zero_pos));
        Relax(from, up, source, 'U');
      }
      if (zero_pos < 6) {  // move zero down
        auto down = from;
        substract_ = ManhattanDistance(down.GetTile(zero_pos + 3));
        down.Swap(zero_pos, zero_pos + 3);
        add_ = ManhattanDistance(down.GetTile(zero_pos));
        Relax(from, down, source, 'D');
      }
    }
  }

  std::string GetPath(State state) {
    std::string path;
    char direction;
    size_t zero_pos = state.Zero();
    while (parents_[state.Data()] != 'S') {
      // direction = parents_[state.Data()].first;
      direction = parents_[state.Data()];
      path += direction;
      switch (direction) {
        case 'L':
          state.Swap(zero_pos, zero_pos + 1);
          break;
        case 'R':
          state.Swap(zero_pos, zero_pos - 1);
          break;
        case 'U':
          state.Swap(zero_pos, zero_pos + 3);
          break;
        case 'D':
          state.Swap(zero_pos, zero_pos - 3);
          break;
        default:
          // return path;
          break;
      }
      // state = parents_[state.Data()].second;
      zero_pos = state.Zero();
    }
    std::reverse(path.begin(), path.end());
    return path;
  }
};

bool CheckZeroAccessibility(std::vector<uint64_t>& board /*, State& state*/);

static constexpr State kTarget(2271560481, 8);

int main() {
  std::vector<uint64_t> source_board(9, 0);

  for (size_t i = 0; i < 3; ++i) {
    for (size_t j = 0; j < 3; ++j) {
      std::cin >> source_board[i * 3 + j];
    }
  }
  State source(source_board);

  if (CheckZeroAccessibility(source_board)) {
    std::unordered_map<uint64_t, int64_t> distance;
    // std::unordered_map<uint64_t, size_t> h_distance;
    Dijkstra dijkstra(distance);
    dijkstra(source, kTarget);
    std::cout << distance[kTarget.Data()] << '\n';
    std::cout << dijkstra.GetPath(kTarget);
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
  return (data_ & k_masks[position]) >> (position * 4);
}

void State::SetTile(size_t position, uint64_t value) {
  value &= k_masks[0];
  if (value == 0) {
    zero_pos_ = position;
  }
  data_ &= k_anti_masks[position];
  value <<= (position * 4);
  data_ |= value;
}

void State::Swap(size_t lhs, size_t rhs) {
  uint64_t src = (data_ & k_masks[lhs]) >> (lhs * 4);
  uint64_t dest = (data_ & k_masks[rhs]) >> (rhs * 4);
  SetTile(lhs, dest);
  SetTile(rhs, src);
}

void State::Print() const {
  for (size_t i = 0; i < 3; ++i) {
    for (size_t j = 0; j < 3; ++j) {
      std::cout << GetTile(3 * i + j) << ' ';
    }
    std::cout << '\n';
  }
}

bool CheckZeroAccessibility(std::vector<uint64_t>& board /*, State& state*/) {
  size_t counter = 0;
  // size_t zero_row = state.Zero() / 3 + 1;
  for (size_t i = 0; i < board.size(); ++i) {
    for (size_t j = 0; j < i; ++j) {
      if (board[i] != 0 && board[i] < board[j]) {
        ++counter;
      }
    }
  }
  return (counter) % 2 == 0;
}

int64_t ManhattanDistance(const State& state) {
  int64_t distance = 0;
  size_t tile;
  int64_t position;
  for (int64_t x = 0; x < 3; ++x) {
    for (int64_t y = 0; y < 3; ++y) {
      position = x * 3 + y;
      tile = state.GetTile(position);
      distance += ManhattanDistance(tile, position);
    }
  }
  return distance;
}

int64_t ManhattanDistance(uint64_t tile, int64_t position) {
  int64_t distance = 0;
  if (tile != 0) {
    --tile;
    int64_t x_pos = tile / 3;
    int64_t y_pos = tile % 3;
    int64_t x = position / 3;
    int64_t y = position % 3;
    distance = std::max(x_pos, x) - std::min(x_pos, x) + std::max(y_pos, y) -
               std::max(y_pos, y);
  }
  return distance;
}