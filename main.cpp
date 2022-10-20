#include <iostream>
#include <optional>
#include <vector>

namespace Constants {
const int64_t kMax = 1 << 30;
}  // namespace Constants

template <typename T>
class MaxHeap {
 public:
  MaxHeap() : data_(kCapacity), size_(0) {}

  std::optional<T> Top();

  void Pop();

  std::optional<T> ExtractTop();

  void Insert(T value);

  size_t Size() const;

  bool Empty() const;

 private:
  static constexpr size_t kCapacity = 8;
  static constexpr size_t kTopIdx = 1;
  std::vector<T> data_;
  size_t size_;

  void SiftUp(size_t index);

  void SiftDown(size_t index);
};

template <typename T>
void ReadInput(size_t& n, size_t& k, T& a_0, T& x, T& y) {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  std::cout.tie(nullptr);
  std::cin >> n >> k;
  std::cin >> a_0 >> x >> y;
}

template <typename T>
void Insert(T val, size_t k, MaxHeap<T>* heap) {
  if (heap->Size() < k) {
    heap->Insert(val);
  } else {
    if (val < heap->Top()) {
      heap->ExtractTop();
      heap->Insert(val);
    }
  }
}

template <typename T>
struct ElemData {
  T x;
  T y;
};

template <typename T>
void FillHeap(size_t& n, size_t& k, T a_0, ElemData<T> dt, MaxHeap<T>* heap) {
  int64_t a_i;
  for (size_t i = 0; i < n; ++i) {
    a_i = (dt.x * a_0 + dt.y) % Constants::kMax;
    a_0 = a_i;
    Insert(a_i, k, heap);
  }
}

template <typename T>
void PrintElements(MaxHeap<T>* heap) {
  std::vector<T> elements;
  while (!(heap->Empty())) {
    elements.push_back(heap->ExtractTop().value());
  }

  for (size_t i = elements.size(); i > 0; --i) {
    std::cout << elements[i - 1] << ' ';
  }
}

int main() {
  size_t n;
  size_t k;
  int64_t a_0;
  int64_t x, y;
  ReadInput(n, k, a_0, x, y);

  auto first_k_elements = new MaxHeap<int64_t>;
  FillHeap(n, k, a_0, {x, y}, first_k_elements);

  PrintElements(first_k_elements);
  delete first_k_elements;
  return 0;
}

template <typename T>
std::optional<T> MaxHeap<T>::Top() {
  if (size_ == 0) {
    return std::nullopt;
  }
  return data_[kTopIdx];
}

template <typename T>
void MaxHeap<T>::Pop() {
  if (size_ == 0) {
    return;
  }
  std::swap(data_[kTopIdx], data_[size_]);
  --size_;
  SiftDown(kTopIdx);
}

template <typename T>
std::optional<T> MaxHeap<T>::ExtractTop() {
  auto top = Top();
  Pop();
  return top;
}

template <typename T>
void MaxHeap<T>::Insert(T value) {
  if (size_ == data_.size() - 1) {
    data_.resize(2 * size_);
  }
  data_[++size_] = value;
  SiftUp(size_);
}

template <typename T>
size_t MaxHeap<T>::Size() const {
  return size_;
}

template <typename T>
bool MaxHeap<T>::Empty() const {
  return size_ == 0;
}

template <typename T>
void MaxHeap<T>::SiftUp(size_t index) {
  if (index > kTopIdx) {
    size_t parent = index / 2;
    if (data_[index] > data_[parent]) {
      std::swap(data_[index], data_[parent]);
      SiftUp(parent);
    }
  }
}

template <typename T>
void MaxHeap<T>::SiftDown(size_t index) {
  size_t i_min = index;
  size_t i_left_child = index * 2;
  size_t i_right_child = index * 2 + 1;
  if (i_left_child <= size_ && data_[i_left_child] > data_[i_min]) {
    i_min = i_left_child;
  }
  if (i_right_child <= size_ && data_[i_right_child] > data_[i_min]) {
    i_min = i_right_child;
  }
  if (index != i_min) {
    std::swap(data_[i_min], data_[index]);
    SiftDown(i_min);
  }
}
