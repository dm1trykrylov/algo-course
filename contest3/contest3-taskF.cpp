#include <functional>
#include <iostream>
#include <string>

namespace Constants {
const size_t kMod = 1e9;
}

template <typename T, typename V>
class SplayTree {
 private:
  class Node {
   public:
    Node(const T& key, const V& value, Node* left, Node* right, Node* parent)
        : key(key),
          value(value),
          left(left),
          right(right),
          parent(parent),
          sum_(0) {}
    T key;
    V value;
    Node* left;
    Node* right;
    Node* parent;
    V sum_;  // sum on subtree
  };

 public:
  SplayTree() : root_(nullptr), new_node_(nullptr) {}

  ~SplayTree() {
    PostOrder(root_, [](Node* v) { delete v; });
  }

  void PostOrder(Node* v, const std::function<void(Node*)>& callback);

  std::pair<Node*, Node*> Split(Node* v, T key);
  Node* Merge(Node* l, Node* r);

  void Insert(T x, const V& value);
  void Erase(T key);

  V& operator[](T key);
  V Sum(Node* node);
  V Sum(T left_key, T right_key);

 private:
  void InsertInTree(T key, const V& value);

  Node* Max(Node* v);
  Node* Find(Node* v, T key);
  Node* LowerBound(Node* v, T key, Node* node);

  void RotateLeft(Node* v);
  void RotateRight(Node* v);

  void Splay(Node* v);

  Node* root_;
  Node* new_node_;
};

void AddParticipants(SplayTree<std::string, std::string>* tree) {
  size_t count;
  std::cin >> count;
  std::string nickname, car;
  for (size_t i = 0; i < count; ++i) {
    std::cin >> nickname >> car;
    tree->Insert(nickname, car);
    tree->Insert(car, nickname);
  }
}

void EnableFastInput() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(0);
  std::cout.tie(0);
}

void ProcessRequests(SplayTree<std::string, std::string>* tree) {
  size_t count;
  std::cin >> count;
  std::string key;
  for (size_t i = 0; i < count; ++i) {
    std::cin >> key;
    std::cout << (*tree)[key] << '\n';
  }
}

int main() {
  // EnableFastInput();

  auto tree = new SplayTree<std::string, std::string>;
  AddParticipants(tree);
  ProcessRequests(tree);
  delete tree;
  return 0;
}

template <typename T, typename V>
void SplayTree<T, V>::PostOrder(Node* v,
                                const std::function<void(Node*)>& callback) {
  if (v == nullptr) {
    return;
  }
  PostOrder(v->left, callback);
  PostOrder(v->right, callback);
  callback(v);
}

template <typename T, typename V>
std::pair<typename SplayTree<T, V>::Node*, typename SplayTree<T, V>::Node*>
SplayTree<T, V>::Split(Node* v, T key) {
  if (v == nullptr) {
    return std::make_pair(nullptr, nullptr);
  }

  auto lb = LowerBound(v, key, nullptr);
  if (lb == nullptr) {
    lb = Max(root_);
  }
  Splay(lb);
  v = lb;
  Node* left;
  Node* right;
  if (v->key >= key) {
    left = v->left;
    if (left != nullptr) {
      v->left = nullptr;
      left->parent = nullptr;
    }
    right = v;
  } else {
    right = v->right;
    if (right != nullptr) {
      v->right = nullptr;
      right->parent = nullptr;
    }
    left = v;
  }

  return std::make_pair(left, right);
}

template <typename T, typename V>
void SplayTree<T, V>::Insert(T x, const V& value) {
  if (root_ == nullptr) {
    root_ = new Node(x, value, nullptr, nullptr, nullptr);
    root_->sum_ = value;
  } else {
    InsertInTree(x, value);
  }
}

template <typename T, typename V>
typename SplayTree<T, V>::Node* SplayTree<T, V>::Merge(Node* l, Node* r) {
  if (l == nullptr) {
    return r;
  }
  if (r == nullptr) {
    return l;
  }
  Node* m = Max(l);
  Splay(m);
  m->right = r;
  //
  m->sum_ += r->sum_;
  r->parent = m;
  return m;
}

template <typename T, typename V>
void SplayTree<T, V>::Erase(T key) {
  Node* v = Find(root_, key);
  Splay(v);
  if (v->left != nullptr) {
    v->left->parent = nullptr;
  }
  if (v->right != nullptr) {
    v->right->parent = nullptr;
  }
  Node* tree = Merge(v->left, v->right);
  delete v;
  root_ = tree;
  tree->parent = nullptr;
}

template <typename T, typename V>
V& SplayTree<T, V>::operator[](T key) {
  Node* v = Find(root_, key);
  Splay(v);
  root_ = v;
  return v->value;
}

template <typename T, typename V>
V SplayTree<T, V>::Sum(Node* node) {
  return node->sum_;
}

template <typename T, typename V>
V SplayTree<T, V>::Sum(T left_key, T right_key){
  size_t sum = 0;
  auto trees_r = Split(node_, right_key + 1);
  sum = trees_r.first->sum;
  auto trees_l = Split(trees_r.left, left_key);
  if (trees_l.first->left != nullptr) {
    sum -= trees_l.first->left->sum;
  }
  return sum;
}

template <typename T, typename V>
void SplayTree<T, V>::InsertInTree(T key, const V& value) {
  auto trees = Split(root_, key);
  new_node_ = new Node(key, value, nullptr, nullptr, nullptr);
  new_node_->sum_ = value;
  root_ = Merge(Merge(trees.first, new_node_), trees.second);
}

template <typename T, typename V>
typename SplayTree<T, V>::Node* SplayTree<T, V>::Max(Node* v) {
  if (v->right == nullptr) {
    return v;
  }
  return Max(v->right);
}

template <typename T, typename V>
typename SplayTree<T, V>::Node* SplayTree<T, V>::Find(Node* v, T key) {
  while (v->key != key) {
    if (v->key > key) {
      v = v->left;
    } else {
      v = v->right;
    }
  }
  return v;
}

template <typename T, typename V>
typename SplayTree<T, V>::Node* SplayTree<T, V>::LowerBound(Node* v, T key,
                                                            Node* node) {
  auto t = v;

  while (t != nullptr) {
    if (t->key == key) {
      return t;
    }
    if (t->key > key) {
      node = t;
      t = t->left;
    } else {
      t = t->right;
    }
  }
  return node;
}

template <typename T, typename V>
void SplayTree<T, V>::RotateLeft(Node* v) {
  Node* p = v->parent;
  Node* r = v->right;
  if (p != nullptr) {
    (p->left == v ? p->left : p->right) = r;
  }
  if (r != nullptr) {
    Node* tmp = r->left;
    r->left = v;
    v->right = tmp;

    r->parent = p;

    v->parent = r;
  }

  if (v->right != nullptr) {
    v->right->parent = v;
  }
}

template <typename T, typename V>
void SplayTree<T, V>::RotateRight(Node* v) {
  Node* p = v->parent;
  Node* l = v->left;
  if (p != nullptr) {
    (p->right == v ? p->right : p->left) = l;
  }
  if (l != nullptr) {
    Node* tmp = l->right;
    l->right = v;
    v->left = tmp;

    l->parent = p;
    v->parent = l;
  }

  if (v->left != nullptr) {
    v->left->parent = v;
  }
}

template <typename T, typename V>
void SplayTree<T, V>::Splay(Node* v) {
  while (v->parent != nullptr) {
    if (v == v->parent->left) {
      if (v->parent->parent == nullptr) {
        RotateRight(v->parent);  // zig
      } else if (v->parent == v->parent->parent->left) {
        RotateRight(v->parent->parent);  // zig-zig
        RotateRight(v->parent);
      } else {
        RotateRight(v->parent);  // zig-zag
        RotateLeft(v->parent);
      }
    } else {
      if (v->parent->parent == nullptr) {
        RotateLeft(v->parent);  // zig
      } else if (v->parent == v->parent->parent->right) {
        RotateLeft(v->parent->parent);  // zig-zig
        RotateLeft(v->parent);
      } else {
        RotateLeft(v->parent);  // zig-zag
        RotateRight(v->parent);
      }
    }
  }
}