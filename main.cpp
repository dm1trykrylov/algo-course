#include <iostream>
#include <optional>
#include <vector>

template <typename T>
class MinHeap {
  static constexpr size_t kCapacity = 8;
  static constexpr size_t kTopIdx = 1;  // heap is 1-indexed
  std::vector<T> data_;
  size_t size_;

 public:
  MinHeap() : data_(kCapacity), size_(0) {}

  std::optional<T> Top() {
    if (size_ == 0) {
      return std::nullopt;
    }
    return data_[kTopIdx];
  }

  std::optional<T> ExtractMin() {
    if (size_ == 0) {
      return std::nullopt;
    }
    std::swap(data_[kTopIdx], data_[size_]);
    --size_;
    SiftDown(kTopIdx);
    return data_[size_ + 1];
  }

  void Insert(T value) {
    if (size_ == data_.size() - 1) {
      data_.resize(2 * size_);
    }
    data_[++size_] = value;
    SiftUp(size_);
  }

  bool Empty() { return size_ == 0; }

 private:
  void SiftUp(size_t index) {
    if (index > kTopIdx) {
      size_t parent = index / 2;
      if (data_[index] < data_[parent]) {
        std::swap(data_[index], data_[parent]);
        SiftUp(parent);
      }
    }
  }

  void SiftDown(size_t index) {
    size_t i_min = index;
    size_t i_left_child = index * 2;
    size_t i_right_child = index * 2 + 1;
    if (i_left_child <= size_ && data_[i_left_child] < data_[i_min]) {
      i_min = i_left_child;
    }
    if (i_right_child <= size_ && data_[i_right_child] < data_[i_min]) {
      i_min = i_right_child;
    }
    if (index != i_min) {
      std::swap(data_[i_min], data_[index]);
      SiftDown(i_min);
    }
  }
};

template <typename T>
inline std::vector<T> Merge(const std::vector<std::vector<T>>& arrays) {
  MinHeap<std::pair<T, size_t>> heap;
  std::vector<size_t> indexes(arrays.size(), 0);
  size_t array_index = 0;
  size_t size = 0;
  for (auto& array : arrays) {
    if (!array.empty()) {
      heap.Insert({array[0], array_index});
    }
    size += array.size();
    ++array_index;
  }
  std::vector<T> result;
  result.reserve(size);

  while (!heap.Empty()) {
    auto[value, array_i] = *heap.ExtractMin();
    result.push_back(value);
    ++indexes[array_i];
    if (indexes[array_i] < arrays[array_i].size()) {
      heap.Insert({arrays[array_i][indexes[array_i]], array_i});
    }
  }

  return result;
}

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  std::cout.tie(nullptr);

  size_t blocks_count, block_size, tmp;
  std::cin >> blocks_count;

  std::vector<std::vector<int>>* arrays =
      new std::vector<std::vector<int>>(blocks_count);

  for (size_t i = 0; i < blocks_count; ++i) {
    std::cin >> block_size;
    for (size_t j = 0; j < block_size; ++j) {
      std::cin >> tmp;
      (*arrays)[i].push_back(tmp);
    }
  }

  auto result = Merge<int>(*arrays);

  for (auto x : result) {
    std::cout << x << ' ';
  }

  delete arrays;
  return 0;
}
