#include <iostream>
#include <vector>

namespace Constants {
const int64_t kMod = 1e9;
}

template <typename T>
class AVLTree {
 public:
  struct Node {
    Node* left{nullptr};
    Node* right{nullptr};
    T value;
    int height{0};
  };

  AVLTree() : root_(nullptr) {}

  void Insert(T value) { root_ = Insert(root_, value); }

  ~AVLTree() {
    Clear();
    delete root_;
  }

  void Clear() {
    if (root_ != nullptr) {
      Clear(root_);
    }
    root_ = nullptr;
  }

  bool Find(T value) { return Find(root_, value); }

  Node* LowerBound(T value, Node* prev) {
    return LowerBound(root_, value, prev);
  }

  void Erase(T value) { root_ = Erase(root_, value); }

 private:
  static Node* Erase(Node* node, T value) {
    if (node == nullptr) {
      return node;
    }
    if (node->value == value) {
      if (node->right == nullptr) {
        Node* temp = node->left;
        delete node;
        return FixBalance(temp);
      }
      Node* right_min = FindMin(node->right);
      node->right = UnlinkMin(node->right);
      right_min->left = node->left;
      right_min->right = node->right;
      delete node;
      return FixBalance(right_min);
    }
    if (value < node->value) {
      node->left = Erase(node->left, value);
    } else {
      node->right = Erase(node->right, value);
    }
    return FixBalance(node);
  }

  static Node* FindMin(Node* node) {
    if (node->left == nullptr) {
      return node;
    }
    return FindMin(node->left);
  }

  static Node* UnlinkMin(Node* node) {
    if (node->left == nullptr) {
      return node->right;
    }
    node->left = UnlinkMin(node->left);
    return FixBalance(node);
  }

  static bool Find(Node* node, T value) {
    if (node == nullptr) {
      return false;
    }
    if (node->value == value) {
      return true;
    }
    if (value < node->value) {
      return Find(node->left, value);
    } else {
      return Find(node->right, value);
    }
  }

  static Node* LowerBound(Node* node, T value, Node* prev) {
    if (node == nullptr) {
      return prev;
    }
    if (node->value == value) {
      return node;
    }
    if (node->value > value) {
      return LowerBound(node->left, value, node);
    } else {
      return LowerBound(node->right, value, prev);
    }
  }

  static void Clear(Node* node) {
    if (node->left != nullptr) {
      Clear(node->left);
    }
    if (node->right != nullptr) {
      Clear(node->right);
    }
    delete node;
  }

  static Node* Insert(Node* node, T value) {
    if (node == nullptr) {
      Node* new_node = new Node;
      new_node->value = value;
      return new_node;
    }
    // <=
    if (value < node->value) {
      node->left = Insert(node->left, value);
    } else {
      node->right = Insert(node->right, value);
    }
    return FixBalance(node);
  }

  static int Height(Node* node) {
    return node == nullptr ? 0 : static_cast<int>(node->height);
  }

  static int BalanceFactor(Node* node) {
    return node == nullptr ? 0 : Height(node->right) - Height(node->left);
  }

  static void CalcHeight(Node* node) {
    if (node == nullptr) {
      return;
    }
    node->height = std::max(Height(node->left), Height(node->right)) + 1;
  }

  static Node* RightRotate(Node* p) {
    Node* q = p->left;
    p->left = q->right;
    q->right = p;
    CalcHeight(p);
    CalcHeight(q);
    return q;
  }

  static Node* LeftRotate(Node* q) {
    Node* p = q->right;
    q->right = p->left;
    p->left = q;
    CalcHeight(q);
    CalcHeight(p);
    return p;
  }

  static Node* FixBalance(Node* node) {
    CalcHeight(node);
    if (BalanceFactor(node) == 2) {
      if (BalanceFactor(node->right) == -1) {
        node->right = RightRotate(node->right);
      }
      return LeftRotate(node);
    }
    if (BalanceFactor(node) == -2) {
      if (BalanceFactor(node->left) == 1) {
        node->left = LeftRotate(node->left);
      }
      return RightRotate(node);
    }
    return node;
  }

  Node* root_{nullptr};
};

void Insert(int64_t value, int64_t prev, AVLTree<int64_t>* tree) {
  value = (value + prev) % Constants::kMod;
  auto res = tree->Find(value);
  if (!res) {
    tree->Insert(value);
  }
  prev = 0;
}

void Find(int64_t value, int64_t& prev, AVLTree<int64_t>* tree) {
  auto result = tree->LowerBound(value, nullptr);
  if (result == nullptr) {
    prev = -1;
    std::cout << -1 << '\n';
    return;
  }
  prev = result->value;
  std::cout << result->value << '\n';
}

void ProcessRequests(AVLTree<int64_t>* tree) {
  size_t requests;
  std::cin >> requests;
  char request;
  int64_t value, prev = 0;
  for (size_t i = 0; i < requests; ++i) {
    std::cin >> request >> value;
    if (request == '+') {
      Insert(value, prev, tree);
    } else {
      Find(value, prev, tree);
    }
  }
}

int main() {
  auto avl = new AVLTree<int64_t>();

  ProcessRequests(avl);

  delete avl;
  return 0;
}