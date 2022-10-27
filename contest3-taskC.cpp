#include <functional>
#include <iostream>
#include <string>

template <typename T, typename V>
class Node {
 public:
  Node(const T& key, const V& value, Node* left, Node* right, Node* parent)
      : key(key), value(value), left(left), right(right), parent(parent){};
  ~Node();

  T key;
  V value;
  Node* left;
  Node* right;
  Node* parent;
};

template <typename T, typename V>
class SplayTree {
 public:
  SplayTree() { root_ = nullptr; }

  V& operator[](T key);

  Node<T, V>* Merge(Node<T, V>* l, Node<T, V>* r);

  std::pair<Node<T, V>*, Node<T, V>*> Split(Node<T, V>* node, T key);

  void Insert(T key, const V& value);
  void Erase(T key);

  void PostOrder(Node<T, V>* v,
                 const std::function<void(Node<T, V>*)>& callback);

  ~SplayTree() {
    PostOrder(root_, [](Node<T, V>* v) { delete v; });
  }

 private:
  Node<T, V>* root_;
  Node<T, V>* p(Node<T, V>* node) { return node->parent; }

  Node<T, V>* g(Node<T, V>* node) { return node->parent->parent; }

  Node<T, V>* Max(Node<T, V>* node);

  void RotateLeft(Node<T, V>* node);

  void RotateRight(Node<T, V>* node);

  void Splay(Node<T, V>* node);

  void Insert(T key, const V& value, Node<T, V>* node);

  Node<T, V>* Find(Node<T, V>* st_node, T key);
};

template <typename T, typename V>
V& SplayTree<T, V>::operator[](T key) {
  Node<T, V>* node = Find(key);
  Splay(node);
  // root_ = node;
  return node->value;
}

template <typename T, typename V>
Node<T, V>* SplayTree<T, V>::Merge(Node<T, V>* l, Node<T, V>* r) {
  if (l == nullptr) {
    return r;
  }
  if (r == nullptr) {
    return l;
  }
  Node<T, V>* m = Max(l);
  Splay(m);
  m->right = r;
  r->parent = m;
  return m;
}

template <typename T, typename V>
std::pair<Node<T, V>*, Node<T, V>*> Split(typename SplayTree<T, V>::Node* node,
                                          T key) {
  if (node == nullptr) {
    return std::make_pair(nullptr, nullptr);
  }
  Node<T, V>* left;
  Node<T, V>* right;
  if (node->key > key) {
    auto t = split(node->left, key);
    node->left = t.second;
    if (t.second != nullptr) {
      p(t.second) = node;
    }
    left = t.first;
    right = node;
  } else {
    auto t = split(node->right, key);
    node->right = t.first;
    if (t.first != nullptr) {
      p(t.first) = node;
    }
    right = t.second;
    left = node;
  }
  return std::make_pair(left, right);
}

template <typename T, typename V>
void SplayTree<T, V>::Insert(T key, const V& value) {
  if (root_ == nullptr) {
    root_ = new Node<T, V>(key, value, nullptr, nullptr, nullptr);
  } else {
    Insert(key, value, root_);
  }
}

template <typename T, typename V>
void SplayTree<T, V>::Erase(T key) {
  Node<T, V>* node = Find(root_, key);
  Splay(node);
  if (node->left != nullptr) {
    node->left->parent = nullptr;
  }
  if (node->right != nullptr) {
    node->right->parent = nullptr;
  }
  Node<T, V>* new_root = Merge(node->left, node->right);
  new_root->parent = nullptr;
  root_ = new_root;
  delete node;
}

template <typename T, typename V>
void SplayTree<T, V>::PostOrder(
    Node<T, V>* v, const std::function<void(Node<T, V>*)>& callback) {
  if (v == nullptr) {
    return;
  }
  PostOrder(v->left, callback);
  PostOrder(v->right, callback);
  callback(v);
}

template <typename T, typename V>
Node<T, V>* SplayTree<T, V>::Max(Node<T, V>* node) {
  if (node->right == nullptr) {
    return node;
  } else {
    return Max(node->right);
  }
}

template <typename T, typename V>
void SplayTree<T, V>::RotateLeft(Node<T, V>* node) {
  Node<T, V>* p = this->p(node);
  Node<T, V>* r = node->right;

  if (p != nullptr) {
    (p->left == node ? p->left : p->right) = r;
  }
  Node<T, V>* tmp = r->left;
  r->left = node;
  node->right = tmp;
  node->parent = r;
  r->parent = p;
  if (node->right != nullptr) {
    node->right->parent = node;
  }
}

template <typename T, typename V>
void SplayTree<T, V>::RotateRight(Node<T, V>* node) {
  Node<T, V>* p = this->p(node);
  Node<T, V>* l = node->left;

  if (p != nullptr) {
    (p->right == node ? p->right : p->left) = l;
  }
  Node<T, V>* tmp = l->right;
  l->right = node;
  node->left = tmp;
  node->parent = l;
  l->parent = p;
  if (node->left != nullptr) {
    node->left->parent = node;
  }
}

template <typename T, typename V>
void SplayTree<T, V>::Splay(Node<T, V>* node) {
  while (p(node) != nullptr) {
    if (node == p(node)->left) {
      if (g(node) == nullptr) {
        RotateRight(p(node));  // zig
      } else if (p(node) == g(node)->left) {
        RotateRight(g(node));  // zig-zig
        RotateRight(p(node));
      } else {
        RotateRight(p(node));  // zig-zag
        RotateLeft(p(node));
      }
    } else {
      if (g(node) == nullptr) {
        RotateLeft(p(node));  // zig
      } else if (p(node) == g(node)->right) {
        RotateLeft(g(node));  // zig-zig
        RotateLeft(p(node));
      } else {
        RotateLeft(p(node));  // zig-zag
        RotateRight(p(node));
      }
    }
  }
}

template <typename T, typename V>
void SplayTree<T, V>::Insert(T key, const V& value, Node<T, V>* node) {
  auto trees = Split(node, key);
  auto x = new Node<T, V>(key, value, nullptr, nullptr, nullptr);
  root_ = Merge(Merge(trees.first, x), trees.second);
}

template <typename T, typename V>
Node<T, V>* SplayTree<T, V>::Find(Node<T, V>* st_node,
                                                      T key) {
  auto node = root_;
  while (node != nullptr) {
    if (node->key == key) {
      break;
    }
    if (node->key > key) {
      node = node->left;
    } else {
      node = node->right;
    }
  }
  return node;
}

void AddParticipants(SplayTree<std::string, std::string>* tree) {
  size_t count;
  std::cin >> count;
  std::string nick, car;
  for (size_t i = 0; i < count; ++i) {
    std::cin >> nick >> car;
    tree->Insert(nick, car);
  }
}

void ProcessRequests(SplayTree<std::string, std::string>* tree) {
  size_t count;
  std::cin >> count;
  std::cout << "Ok";
}

int main() {
  auto tree = new SplayTree<std::string, std::string>();

  AddParticipants(tree);
  ProcessRequests(tree);

  delete tree;
  return 0;
}