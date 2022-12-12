//#pragma GCC optimize("O3")
//#pragma GCC optimize("unroll-loops")

#include <algorithm>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>
/*
const size_t R = 131072ul;
const size_t N = 262144ul;
const int32_t kMax = 1e7;*/

const size_t R = 1 << 4ul;
const size_t N = 1 << 5ul;  // limit for array size


class TreapSet {
  struct Node {
    Node(int key, int priority)
        : x(key), y(priority), size(1), left(nullptr), right(nullptr) {}

    int x;
    int y;
    int size;  // subtree size
    Node* left;
    Node* right;
  };

 public:
  TreapSet() : root_(nullptr) {}

  ~TreapSet() { Clear(root_); }

  void Erase(int value);

  void EraseFind(int value);
  void Insert(int value);

  bool Find(int value) { return Find(nullptr, root_, value).second != nullptr; }

  Node* LowerBound(int value) { return LowerBound(value, root_, nullptr); }
  Node* UpperBound(int value) { return UpperBound(value, root_, nullptr); }
  int CntLb(int value) {
    auto res = LowerBound(value);
    if (res == nullptr) {
      return 0;
    }
    return Size(res->left) + 1;
  }
  int CntUb(int value) {
    auto res = UpperBound(value);
    if (res == nullptr) {
      return 0;
    }
    return Size(res->left) + 1;
  }
  Node* Kth(int index) { return Kth(index, root_); }

 private:
  std::pair<Node*, Node*> Find(Node* parent, Node* node, int value);

  // greatest element less than value
  Node* LowerBound(int value, Node* node, Node* prev);

  // least element greater than value
  Node* UpperBound(int value, Node* node, Node* prev);

  Node* Kth(int index, Node* node);

  static int Size(Node* node) { return (node == nullptr ? 0 : node->size); }

  static void Update(Node* node);

  void Clear(Node* node);

  Node* Merge(Node* first, Node* second);

  std::pair<Node*, Node*> Split(Node* node, int key);

  Node* root_;
};

std::vector<TreapSet*> tree(N);
std::vector<int32_t> tree1[N];

void Build(int32_t array[], size_t size) {
  for (size_t i = R; i < N; ++i) {
    if (i - R < size) {
      tree[i] = new TreapSet();
      tree[i]->Insert(array[i - R]);
      tree1[i].emplace_back(array[i - R]);
    }
  }
  for (size_t i = R - 1; i > 0; --i) {
    tree[i] = new TreapSet();
    for (auto x : tree1[i << 1]) {
      tree[i]->Insert(x);
    }
    for (auto x : tree1[(i << 1) | 1]) {
      tree[i]->Insert(x);
    }
  }
}

void Upd(size_t p, int32_t array[], int32_t value) {  // set value at position p
  int32_t old_value = array[p];
  array[p] = value;
  p += R;
  tree[p]->EraseFind(old_value);
  tree[p]->Insert(value);
  
  p >>= 1;
  while (p > 0) {
    tree[p]->EraseFind(old_value);
    tree[p]->Insert(value);
    p >>= 1;
  }
}

int32_t Query(size_t l, size_t r, int32_t x,
              int32_t y) {  // sum on interval [l, r)
  l += R;
  r += R;
  int32_t res = 0;
  while (l > 0 && r > 0 && r > l) {
    if (l & 1) {
      res += tree[l]->CntLb(y);
      res -= tree[l]->CntLb(x - 1);
      //res += 1;
      ++l;
    }
    if (r & 1) {
      --r;
      res += tree[r]->CntLb(y);
      res -= tree[r]->CntLb(x - 1);
      //res += 1;
    }
    l >>= 1;
    r >>= 1;
  }
  return res;
}

void FastIO() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  std::cout.tie(nullptr);
}

int main() {
  FastIO();
  srand(time(nullptr));
  int32_t* array = new int32_t[R];
  size_t n, m;  // array size
  size_t x, y, l, r;
  std::cin >> n >> m;
  for (size_t i = 0; i < n; ++i) {
    std::cin >> array[i];
  }
  Build(array, n);
  std::string command;
  for (size_t i = 0; i < m; ++i) {
    std::cin >> command;
    if (command == "GET") {
      std::cin >> l >> r >> x >> y;
      std::cout << Query(l - 1, r, x, y) << '\n';
    } else {
      std::cin >> x >> y;
      Upd(x - 1, array, y);
    }
  }
  delete[] array;
  for (auto t : tree) {
    delete t;
  }
  return 0;
}

void TreapSet::EraseFind(int value) {
  if (!Find(value)) {
    return;
  }
  auto trees = Find(nullptr, root_, value);  // (parent, value_node)
  auto* parent = trees.first;
  auto* value_node = trees.second;
  if (parent == nullptr && value_node != nullptr) {
    root_ = Merge(root_->left, root_->right);
    Update(root_);
  } else {
    if (parent != nullptr) {
      ((parent->left == value_node) ? parent->left : parent->right) =
          Merge(value_node->left, value_node->right);
      Update(parent->left);
      Update(parent->right);
      Update(parent);
    }
  }
  delete trees.second;
}

void TreapSet::Erase(int value) {
  if (!Find(value)) {
    return;
  }
  auto trees = Split(root_, value);  // (left_with_value, right)
  auto* left_with_value = trees.first;
  auto* right = trees.second;
  trees = Split(left_with_value, value - 1);  // (left, value_tree)
  auto* left = trees.first;
  auto* value_tree = trees.second;
  delete value_tree;
  root_ = Merge(left, right);
}

void TreapSet::Insert(int value) {/*
  if (Find(value)) {
    return;
  }*/
  int priority = rand();
  Node* node = new Node(value, priority);
  auto trees = Split(root_, value);
  root_ = Merge(Merge(trees.first, node), trees.second);
}

std::pair<TreapSet::Node*, TreapSet::Node*> TreapSet::Find(Node* parent,
                                                           Node* node,
                                                           int value) {
  if (node == nullptr) {
    return {parent, node};
  }
  if (value == node->x) {
    return {parent, node};
  }
  if (value <= node->x) {
    return Find(node, node->left, value);
  }
  return Find(node, node->right, value);
}

TreapSet::Node* TreapSet::LowerBound(int value, Node* node, Node* prev) {
  if (node == nullptr) {
    return prev;
  }
  if (node->x == value) {
    return node;
  }
  if (node->x <= value) {
    return LowerBound(value, node->right, node);
  }
  return LowerBound(value, node->left, prev);
}

TreapSet::Node* TreapSet::UpperBound(int value, Node* node, Node* prev) {
  if (node == nullptr) {
    return prev;
  }
  if (node->x == value) {
    return node;
  }
  if (node->x < value) {
    return UpperBound(value, node->right, prev);
  }
  return UpperBound(value, node->left, node);
}

TreapSet::Node* TreapSet::Kth(int index, Node* node) {
  if (node == nullptr) {
    return nullptr;
  }
  if (index < 0 || node->size < index) {
    return nullptr;
  }
  int left = (node->left == nullptr ? 0 : node->left->size);
  if (left + 1 == index) {
    return node;
  }
  if (left + 1 < index) {
    return Kth(index - left - 1, node->right);
  }
  return Kth(index, node->left);
}

void TreapSet::Update(Node* node) {
  if (node != nullptr) {
    node->size = Size(node->left) + Size(node->right) + 1;
  }
}

void TreapSet::Clear(Node* node) {
  if (node == nullptr) {
    return;
  }
  Clear(node->left);
  Clear(node->right);
  delete node;
}

TreapSet::Node* TreapSet::Merge(Node* first, Node* second) {
  if (first == nullptr) {
    return second;
  }
  if (second == nullptr) {
    return first;
  }
  if (first->y > second->y) {
    first->right = Merge(first->right, second);
    Update(first);
    return first;
  }
  second->left = Merge(first, second->left);
  Update(second);
  return second;
}

std::pair<TreapSet::Node*, TreapSet::Node*> TreapSet::Split(Node* node,
                                                            int key) {
  if (node == nullptr) {
    return {nullptr, nullptr};
  }
  if (key <= node->x) {
    auto trees = Split(node->left, key);  // (left, right)
    node->left = trees.second;
    Update(node);
    return {trees.first, node};
  }
  auto trees = Split(node->right, key);
  node->right = trees.first;
  Update(node);
  return {node, trees.second};
}