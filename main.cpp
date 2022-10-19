#include <algorithm>
#include <iostream>
#include <vector>

namespace constants {
const uint64_t kByte = 255;  // 11111111 in binary
const size_t kMaxBytes = 8;  // bytes in number
};                           // namespace constants

uint64_t GetByte(uint64_t number, size_t byte_idx) {
  size_t offset = byte_idx * 8;
  uint64_t tmp = constants::kByte << offset;
  tmp = number & tmp;
  tmp >>= offset;
  return tmp;
}

void CountingSort(size_t n, size_t byte_idx, std::vector<uint64_t>& result,
                  std::vector<uint64_t>& array) {
  std::vector<uint64_t> bytes_count(constants::kByte + 1);
  for (auto& number : array) {
    ++bytes_count[GetByte(number, byte_idx)];
  }

  uint64_t count = 0;
  for (size_t i = 0; i <= constants::kByte;
       ++i) {  // write to bytes_count prefix sums
    uint64_t tmp = bytes_count[i];
    bytes_count[i] = count;
    count += tmp;
  }
  uint64_t byte;
  for (size_t i = 0; i < n; ++i) {
    byte = GetByte(array[i], byte_idx);
    result[bytes_count[byte]] = array[i];
    ++bytes_count[byte];
  }
}

void LSDsort(size_t n, std::vector<uint64_t>& array) {
  std::vector<uint64_t> result(array.size());

  for (size_t byte_idx = 0; byte_idx < constants::kMaxBytes; ++byte_idx) {
    CountingSort(n, byte_idx, result, array);

    for (size_t i = 0; i < n; ++i) {
      array[i] = result[i];
    }
  }
  array = result;
}

void ReadInput(size_t& n, std::vector<uint64_t>& array) {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  std::cout.tie(nullptr);

  std::cin >> n;
  array.resize(n);

  for (size_t i = 0; i < n; ++i) {
    std::cin >> array[i];
  }
}

void PrintArray(const std::vector<uint64_t>& kArray) {
  for (auto a : kArray) {
    std::cout << a << '\n';
  }
}

int main() {
  size_t n;
  std::vector<uint64_t> array;
  ReadInput(n, array);

  LSDsort(n, array);

  PrintArray(array);

  return 0;
}
