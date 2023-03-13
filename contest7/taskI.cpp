#include <algorithm>
#include <iostream>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>

static constexpr size_t kSize = 4;
static constexpr size_t kMaxPos = kSize * kSize;
static constexpr size_t kOffset = 4;

static constexpr int64_t kDistWeight = 1;
static constexpr int64_t kMaxDepth = 60 * kDistWeight;
static constexpr int64_t kManhattanWeight = 1;

const static constexpr uint64_t k_anti_masks[] = {
    0xFFFFFFFFFFFFFFF0, 0xFFFFFFFFFFFFFF0F, 0xFFFFFFFFFFFFF0FF,
    0xFFFFFFFFFFFF0FFF, 0xFFFFFFFFFFF0FFFF, 0xFFFFFFFFFF0FFFFF,
    0xFFFFFFFFF0FFFFFF, 0xFFFFFFFF0FFFFFFF, 0xFFFFFFF0FFFFFFFF,
    0xFFFFFF0FFFFFFFFF, 0xFFFFF0FFFFFFFFFF, 0xFFFF0FFFFFFFFFFF,
    0xFFF0FFFFFFFFFFFF, 0xFF0FFFFFFFFFFFFF, 0xF0FFFFFFFFFFFFFF,
    0x0FFFFFFFFFFFFFFF};

const static constexpr uint64_t k_masks[] = {
    0x000000000000000F, 0x00000000000000F0, 0x0000000000000F00,
    0x000000000000F000, 0x00000000000F0000, 0x0000000000F00000,
    0x000000000F000000, 0x00000000F0000000, 0x0000000F00000000,
    0x000000F000000000, 0x00000F0000000000, 0x0000F00000000000,
    0x000F000000000000, 0x00F0000000000000, 0x0F00000000000000,
    0xF000000000000000};

class State {
 private:
  uint64_t data_;
  size_t zero_pos_;
  int64_t manhattan_ = 0;
  const uint64_t kMask = 0xF;

 public:
  State() : State(0, 0){};
  State(uint64_t data);
  State(uint64_t data, size_t zero_pos);
  constexpr State(const uint64_t kData, const size_t kZeroPos,
                  int64_t manhattan)
      : data_(kData), zero_pos_(kZeroPos), manhattan_(manhattan) {}
  State(const State& rhs)
      : data_(rhs.data_),
        zero_pos_(rhs.zero_pos_),
        manhattan_(rhs.manhattan_) {}
  State(std::vector<size_t>& tiles);
  State& operator=(const State& rhs) {
    data_ = rhs.data_;
    zero_pos_ = rhs.zero_pos_;
    manhattan_ = rhs.manhattan_;
    return *this;
  }
  uint64_t Data() const { return data_; }
  size_t Zero() const { return zero_pos_; }
  size_t Manhattan() const { return manhattan_; }
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

int64_t LastMove(State& state) {
  int64_t incr = 0;
  if (state.GetTile(kSize * kSize - 2) != kSize * kSize - 1) {
    ++incr;
  }
  if (state.GetTile(kSize * (kSize - 1) - 1) != kSize * (kSize - 1)) {
    ++incr;
  }
  return incr;
}

int64_t LUcorner(State& state) {
  int64_t incr = 0;
  if (state.GetTile(0) != 1) {
    if (state.GetTile(1) != 2) {
      incr = 2;
    }
    if (state.GetTile(kSize) != kSize + 1) {
      incr = 2;
    }
  }
  return incr;
}

int64_t RUcorner(State& state) {
  int64_t incr = 0;
  if (state.GetTile(kSize - 1) != 1) {
    if (state.GetTile(kSize - 2) != kSize - 1) {
      incr = 2;
    }
    if (state.GetTile(kSize * 2 - 1) != kSize * 2) {
      incr = 2;
    }
  }
  return incr;
}

int64_t LDcorner(State& state) {
  int64_t incr = 0;
  if (state.GetTile(kSize * (kSize - 1)) != kSize * (kSize - 1) + 1) {
    if (state.GetTile(kSize * (kSize - 1) + 1) != kSize * (kSize - 1) + 2) {
      incr = 2;
    }
    if (state.GetTile(kSize * (kSize - 2)) != kSize * (kSize - 2) + 1) {
      incr = 2;
    }
  }
  return incr;
}

class Dijkstra {
 private:
  std::unordered_map<uint64_t, int64_t>& distance_;
  std::priority_queue<std::pair<int64_t, State>,
                      std::vector<std::pair<int64_t, State>>,
                      std::greater<std::pair<int64_t, State>>>
      queue_;
  std::unordered_map<uint64_t, char> parents_;
  int64_t add_, substract_;
  State tmp_;
  char inverse_[256];

  bool Relax(State& from_state, State& to_state, State& source,
             char direction) {
    uint64_t to = to_state.Data();
    uint64_t from = from_state.Data();
    if (distance_[to] == 0 && to != source.Data() &&
        distance_[from] < kMaxDepth) {
      distance_[to] = distance_[from] + 1;
      queue_.push(
          {distance_[to] * kDistWeight +
               (to_state.Manhattan() + LastMove(to_state) + LUcorner(to_state) +
                LDcorner(to_state) + RUcorner(to_state)) *
                   kManhattanWeight,
           to_state});

      // parents_[to] = {direction, from};
      parents_[to] = direction;
      return true;
    }
    return false;
  }

 public:
  Dijkstra(std::unordered_map<uint64_t, int64_t>& distance)
      : distance_(distance) {
    inverse_['D'] = 'U';
    inverse_['U'] = 'D';
    inverse_['R'] = 'L';
    inverse_['L'] = 'R';
  }
  bool CheckState(size_t to, char direction, State& from, State& source,
                  const State& target) {
    tmp_ = from;
    size_t src = from.Zero();
    tmp_.Swap(src, to);
    Relax(from, tmp_, source, direction);
    return tmp_.Data() == target.Data();
  }

  void operator()(State& source, const State& target) {
    parents_[source.Data()] = 'S';
    distance_[source.Data()] = 0;
    queue_.push({distance_[source.Data()] + source.Manhattan() +
                     LastMove(source) + LUcorner(source) + LDcorner(source),
                 source});
    size_t zero_pos;
    while (!queue_.empty()) {
      auto from = queue_.top().second;
      queue_.pop();
      if (from.Data() == target.Data()) {
        break;
      }
      if (distance_[from.Data()] >= kMaxDepth) {
        continue;
      }
      zero_pos = from.Zero();
      if (zero_pos % kSize > 0) {  // move zero left
        if (CheckState(zero_pos - 1, 'L', from, source, target)) {
          break;
        }
      }
      if (zero_pos % kSize < kSize - 1) {  // move zero right
        if (CheckState(zero_pos + 1, 'R', from, source, target)) {
          break;
        }
      }
      if (zero_pos > kSize - 1) {  // move zero up
        if (CheckState(zero_pos - kSize, 'U', from, source, target)) {
          break;
        }
      }
      if (zero_pos < kSize * (kSize - 1)) {  // move zero down
        if (CheckState(zero_pos + kSize, 'D', from, source, target)) {
          break;
        }
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
      path += inverse_[static_cast<size_t>(direction)];
      // path += direction;
      switch (direction) {
        case 'L':
          state.Swap(zero_pos, zero_pos + 1);
          break;
        case 'R':
          state.Swap(zero_pos, zero_pos - 1);
          break;
        case 'U':
          state.Swap(zero_pos, zero_pos + kSize);
          break;
        case 'D':
          state.Swap(zero_pos, zero_pos - kSize);
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

bool CheckZeroAccessibility(std::vector<uint64_t>& board, State& state);

// static constexpr State kTarget(2271560481, 8, 0);
static constexpr State kTarget(1147797409030816545, 15, 0);

int main() {
  std::vector<uint64_t> source_board(kMaxPos, 0);

  for (size_t i = 0; i < kSize; ++i) {
    for (size_t j = 0; j < kSize; ++j) {
      std::cin >> source_board[i * kSize + j];
    }
  }
  State source(source_board);
  if (CheckZeroAccessibility(source_board, source)) {
    std::unordered_map<uint64_t, int64_t> distance;
    Dijkstra dijkstra(distance);
    dijkstra(source, kTarget);
    std::cout << distance[kTarget.Data()] << '\n';
    std::cout << dijkstra.GetPath(kTarget);
  } else {
    std::cout << -1;
  }

  return 0;
}

State::State(uint64_t data, size_t zero_pos)
    : data_(data), zero_pos_(zero_pos) {
  manhattan_ = ManhattanDistance(*this);
}

State::State(uint64_t data) : data_(data) {
  zero_pos_ = 8;
  for (size_t pos = 0; pos < kMaxPos; ++pos) {
    if (GetTile(pos) == 0) {
      zero_pos_ = pos;
      break;
    }
  }
  manhattan_ = ManhattanDistance(*this);
}

State::State(std::vector<size_t>& tiles) {
  for (size_t i = 0; i < tiles.size(); ++i) {
    SetTile(i, tiles[i]);
  }
  manhattan_ = ManhattanDistance(*this);
}

uint64_t State::GetTile(size_t position) const {
  return (data_ & k_masks[position]) >> (position * kOffset);
}

void State::SetTile(size_t position, uint64_t value) {
  value &= k_masks[0];
  if (value == 0) {
    zero_pos_ = position;
  }
  data_ &= k_anti_masks[position];
  value <<= (position * kOffset);
  data_ |= value;
}

void State::Swap(size_t lhs, size_t rhs) {
  uint64_t src = (data_ & k_masks[lhs]) >> (lhs * kOffset);
  uint64_t dest = (data_ & k_masks[rhs]) >> (rhs * kOffset);
  if (src != 0) {
    manhattan_ += ManhattanDistance(src, rhs) - ManhattanDistance(src, lhs);
  }
  if (dest != 0) {
    manhattan_ += ManhattanDistance(dest, lhs) - ManhattanDistance(dest, rhs);
  }
  SetTile(lhs, dest);
  SetTile(rhs, src);
}

void State::Print() const {
  for (size_t i = 0; i < kSize; ++i) {
    for (size_t j = 0; j < kSize; ++j) {
      std::cout << GetTile(kSize * i + j) << ' ';
    }
    std::cout << '\n';
  }
}

bool CheckZeroAccessibility(std::vector<uint64_t>& board, State& state) {
  size_t counter = 0;
  size_t zero_row = state.Zero() / kSize + 1;
  for (size_t i = 0; i < board.size(); ++i) {
    for (size_t j = 0; j < i; ++j) {
      if (board[i] != 0 && board[i] < board[j]) {
        ++counter;
      }
    }
  }
  if (kSize == 4) {
    counter += zero_row;
  }
  return counter % 2 == 0;
}

int64_t ManhattanDistance(const State& state) {
  int64_t distance = 0;
  size_t tile;
  int64_t position;
  for (int64_t x = 0; x < static_cast<int64_t>(kSize); ++x) {
    for (int64_t y = 0; y < static_cast<int64_t>(kSize); ++y) {
      position = x * kSize + y;
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
    int64_t x_pos = tile / static_cast<int64_t>(kSize);
    int64_t y_pos = tile % static_cast<int64_t>(kSize);
    int64_t x = position / static_cast<int64_t>(kSize);
    int64_t y = position % static_cast<int64_t>(kSize);
    distance = std::max(x_pos, x) - std::min(x_pos, x) + std::max(y_pos, y) -
               std::max(y_pos, y);
  }
  return distance;
}