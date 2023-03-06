#include <algorithm>
#include <iostream>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>

static constexpr size_t kSize = 3;
static constexpr size_t kMaxPos = 9;
static constexpr size_t kOffset = 4;

class State {
 private:
  uint64_t data_;
  size_t zero_pos_;
  int64_t manhattan_ = 0;
  const uint64_t kMask = 0xF;

  const static constexpr uint64_t k_anti_masks[] = {
      0xFFFFFFFF0, 0xFFFFFFF0F, 0xFFFFFF0FF, 0xFFFFF0FFF, 0xFFFF0FFFF,
      0xFFF0FFFFF, 0xFF0FFFFFF, 0xF0FFFFFFF, 0x0FFFFFFFF};

  const static constexpr uint64_t k_masks[] = {
      0x00000000F, 0x0000000F0, 0x000000F00, 0x00000F000, 0x0000F0000,
      0x000F00000, 0x00F000000, 0x0F0000000, 0xF00000000};

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
      // queue_.push({distance_[to] + add_ - substract_, to_state});
      queue_.push({distance_[to] + to_state.Manhattan(), to_state});

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
    parents_[source.Data()] = 'S';
    distance_[source.Data()] = 0;
    queue_.push({distance_[source.Data()] + source.Manhattan(), source});
    size_t zero_pos;
    State left, right, up, down;
    while (!queue_.empty()) {
      auto from = queue_.top().second;
      queue_.pop();
      if (from.Data() == target.Data()) {
        break;
      }
      zero_pos = from.Zero();
      if (zero_pos % kSize > 0) {  // move zero left
        left = from;
        left.Swap(zero_pos, zero_pos - 1);
        Relax(from, left, source, 'L');
        if (left.Data() == target.Data()) {
          break;
        }
      }
      if (zero_pos % kSize < kSize - 1) {  // move zero right
        right = from;
        right.Swap(zero_pos, zero_pos + 1);
        Relax(from, right, source, 'R');
        if (right.Data() == target.Data()) {
          break;
        }
      }
      if (zero_pos > kSize - 1) {  // move zero up
        up = from;
        up.Swap(zero_pos, zero_pos - kSize);
        Relax(from, up, source, 'U');
        if (up.Data() == target.Data()) {
          break;
        }
      }
      if (zero_pos < kSize * (kSize - 1)) {  // move zero down
        down = from;
        down.Swap(zero_pos, zero_pos + kSize);
        Relax(from, down, source, 'D');
        if (down.Data() == target.Data()) {
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
      path += direction;
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

bool CheckZeroAccessibility(std::vector<uint64_t>& board /*, State& state*/);

static constexpr State kTarget(2271560481, 8, 0);

int main() {
  std::vector<uint64_t> source_board(kMaxPos, 0);

  for (size_t i = 0; i < kSize; ++i) {
    for (size_t j = 0; j < kSize; ++j) {
      std::cin >> source_board[i * kSize + j];
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