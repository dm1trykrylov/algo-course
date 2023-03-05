#include <iostream>
#include <vector>

class State {
 private:
  uint64_t data_;
  const uint64_t mask = 0xF;
  static constexpr uint64_t AntiMasks[] = {
      0x777777770, 0x777777707, 0x777777077, 0x777770777, 0x777707777,
      0x777077777, 0x770777777, 0x707777777, 0x077777777};

  static constexpr uint64_t Masks[] = {0x00000000F, 0x0000000F0, 0x000000F00,
                                       0x00000F000, 0x0000F0000, 0x000F00000,
                                       0x00F000000, 0x0F0000000, 0xF00000000};

 public:
  State(std::vector<size_t>& tiles) {
    for (size_t i = 0; i < tiles.size(); ++i) {
      data_ &= AntiMasks[i];
      uint64_t tile = tiles[i] & mask;
      tile <<= (4 * i);
      data_ |= tile;
    }
  }
  void Print() {
    uint64_t current_tile;
    for (size_t i = 0; i < 3; ++i) {
      for (size_t j = 0; j < 3; ++j) {
        current_tile = data_ & Masks[3 * i + j];
        current_tile >>= (4 * (3 * i + j));
        std::cout << current_tile << ' ';
      }
      std::cout << '\n';
    }
  }
};

int main() {
  std::vector<uint64_t> board(9, 0);
  for (size_t i = 0; i < 3; ++i) {
    for (size_t j = 0; j < 3; ++j) {
      std::cin >> board[i * 3 + j];
    }
  }
  State state(board);
  state.Print();
  return 0;
}