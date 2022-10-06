#include <iostream>

bool BinarySearch(const int* begin, const int* end, int target) {
  const int* middle;

  while (end - begin > 1) {
    middle = begin + (end - begin) / 2;
    if (*middle <= target) {
      begin = middle;
    } else {
      end = middle;
    }
  }

  return *begin == target;
}

int main() {
  size_t numbers_count;
  size_t query_count;

  size_t left;
  size_t right;
  int target;

  std::cin >> numbers_count;
  int* numbers = new int[numbers_count];
  for (size_t i = 0; i < numbers_count; ++i) {
    std::cin >> numbers[i];
  }

  std::cin >> query_count;

  for (size_t i = 0; i < query_count; ++i) {
    std::cin >> left >> right >> target;
    if (BinarySearch(numbers + left - 1, numbers + right - 1, target)) {
      std::cout << "YES\n";
    } else {
      std::cout << "NO\n";
    }
  }

  delete[] numbers;
  return 0;
}
