#include <cstdlib>
#include <iostream>
#include <random>
#include <utility>

namespace constants {
const int64_t k_max_val = 1e9 + 1;
}

template <typename T>
class TreapArray {
  struct Node {
    Node(int64_t priority, const T& value)
        : size(1),
          min(value),
          priority(priority),
          value(value),
          left(nullptr),
          right(nullptr) {}

    int64_t size;
    int64_t min;
    int64_t priority;
    T value;
    Node* left;
    Node* right;
  };

 public:
  TreapArray() : root_(nullptr) {}

  ~TreapArray() { Clear(root_); }

  int64_t Size() { return Size(root_); }

  bool Empty() { return Size(root_) == 0; }

  void Erase(int64_t pos) {
    auto [left, right_with_pos] = Split(root_, pos);
    auto [pos_tree, right] = Split(right_with_pos, 1);
    delete pos_tree;
    root_ = Merge(left, right);
  }

  void Insert(int64_t pos, const T& value) {
    int64_t priority = distribution_(gen_);
    Node* node = new Node(priority, value);
    auto [first, second] = Split(root_, pos);
    root_ = Merge(Merge(first, node), second);
  }

  T GetMin(size_t left, size_t right) {
    auto [first, second_with_value] = Split(root_, left);
    auto [first_with_value, second] =
        Split(second_with_value, right + 1 - left);
    auto ans = first_with_value->min;
    root_ = Merge(first, Merge(first_with_value, second));
    return ans;
  }

  T& operator[](int64_t pos) {
    auto [parent, pos_node] = Find(nullptr, root_, pos);
    return pos_node->value;
  }

 private:
  std::pair<Node*, Node*> Find(Node* parent, Node* node, int64_t pos) {
    if (node == nullptr) {
      return {parent, node};
    }
    int64_t left_size = Size(node->left);
    if (pos == left_size) {
      return {parent, node};
    }
    if (pos < left_size) {
      return Find(node, node->left, pos);
    }
    return Find(node, node->right, pos - left_size - 1);
  }

  void Clear(Node* node) {
    if (node == nullptr) {
      return;
    }
    Clear(node->left);
    Clear(node->right);
    delete node;
  }

  Node* Merge(Node* first, Node* second) {
    if (first == nullptr) {
      return second;
    }
    if (second == nullptr) {
      return first;
    }
    if (first->priority > second->priority) {
      first->right = Merge(first->right, second);
      Update(first);
      return first;
    }
    second->left = Merge(first, second->left);
    Update(second);
    return second;
  }

  std::pair<Node*, Node*> Split(Node* node, int64_t pos) {
    if (node == nullptr) {
      return {nullptr, nullptr};
    }
    int64_t left_size = Size(node->left);
    if (pos <= left_size) {
      auto [left, right] = Split(node->left, pos);
      node->left = right;
      Update(node);
      return {left, node};
    }
    auto [left, right] = Split(node->right, pos - left_size - 1);
    node->right = left;
    Update(node);
    return {node, right};
  }

  void Update(Node* node) {
    if (node == nullptr) {
      return;
    }
    node->size = 1 + Size(node->left) + Size(node->right);
    node->min = std::min(node->value, Min(node->left));
    node->min = std::min(node->min, Min(node->right));
  }

  int64_t Min(Node* node) {
    if (node == nullptr) {
      return constants::k_max_val;
    }
    return node->min;
  }

  int64_t Size(Node* node) {
    if (node == nullptr) {
      return 0;
    }
    return node->size;
  }

  Node* root_;
  std::mt19937 gen_;
  std::uniform_int_distribution<int64_t> distribution_;
};

void ProcessRequests(TreapArray<int64_t>* treap) {
  size_t n;
  std::cin >> n;
  size_t l, r;
  int64_t index;
  int64_t x;
  char command;
  for (size_t i = 0; i < n; ++i) {
    std::cin >> command;
    if (command == '?') {
      std::cin >> l >> r;
      std::cout << treap->GetMin(l - 1, r - 1) << '\n';
    } else {
      std::cin >> index >> x;
      treap->Insert(index, x);
    }
  }
}

void FastIO() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  std::cout.tie(nullptr);
}

int main() {
  FastIO();

  auto treap = new TreapArray<int64_t>();
  ProcessRequests(treap);

  delete treap;
  return 0;
}