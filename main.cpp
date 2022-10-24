#include <iostream>
#include <iterator>
#include <random>
#include <vector>
//
#include <bits/stl_algo.h>

template <typename T>
size_t Partition(T& array, size_t left, size_t right) {
  auto pivot = array[(right + left) / 2];

  size_t i = left;
  size_t j = right;
  do {
    while (array[i] < pivot) {
      ++i;
    }
    while (array[j] > pivot) {
      --j;
    }
    if (i >= j) {
      return j;
    }
    std::swap(array[i], array[j]);
    ++i;
    --j;
  } while (true);
}

template <typename T>
void Qsort(T& array, size_t left, size_t right) {
  if (left >= right) {
    return;
  }
  size_t p = Partition(array, left, right);
  Qsort(array, left, p);
  Qsort(array, p + 1, right);
}

template <typename T>
void Qsort(T& a, size_t size) {
  Qsort(a, 0, size - 1);
}

void ReadInput(size_t& size, std::vector<int>& array) {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  std::cout.tie(nullptr);

  std::cin >> size;
  array.resize(size);

  for (size_t i = 0; i < size; ++i) {
    std::cin >> array[i];
  }
}

void PrintArray(const std::vector<int>& k_array) {
  for (auto a : k_array) {
    std::cout << a << ' ';
  }
}

int main() {
  size_t size;
  std::vector<int> array;
  ReadInput(size, array);

  std::shuffle(
      array.begin(), array.end(),
      std::default_random_engine{});  // shuffle array to avoid bad cases

  Qsort(array, size);

  PrintArray(array);

  return 0;
}
