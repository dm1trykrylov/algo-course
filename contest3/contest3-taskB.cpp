#include <functional>
#include <iostream>
#include <vector>

namespace Constants {
const int64_t kMod = 1e9;
}

class AVLTree {
  struct Node {
    Node* left{nullptr};
    Node* right{nullptr};
    int value;
    unsigned char height{0};
  };

 public:
  void Insert(int value) { root_ = Insert(root_, value); }

  bool Find(int value) { return Find(root_, value); }

  Node* LowerBound(int value, Node* prev) {
    return LowerBound(value, root_, prev);
  }

  void Erase(int value) { root_ = Erase(root_, value); }

  ~AVLTree() {
    PostOrder(root_, [](Node* v) { delete v; });
  }

 private:
  static Node* Erase(Node* node, int value) {
    if (node == nullptr) {
      return nullptr;
    }
    if (node->value == value) {
      if (node->right == nullptr) {
        Node* temp = node->left;
        delete node;
        return FixBalance(temp);
      }
      Node* min_right = FindMin(node->right);
      node->right = UnlinkMin(node->right);
      min_right->left = node->left;
      min_right->right = node->right;
      delete node;
      return FixBalance(min_right);
    }
    if (value <= node->value) {
      node->left = Erase(node->left, value);
    } else {
      node->right = Erase(node->right, value);
    }
    return FixBalance(node);
  }

  void PostOrder(Node* v, const std::function<void(Node*)>& callback) {
    if (v == nullptr) {
      return;
    }
    PostOrder(v->left, callback);
    PostOrder(v->right, callback);
    callback(v);
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

  static bool Find(Node* node, int value) {
    if (node == nullptr) {
      return false;
    }
    if (node->value == value) {
      return true;
    }
    if (value <= node->value) {
      return Find(node->left, value);
    }
    return Find(node->right, value);
  }

  static Node* LowerBound(int value, Node* node, Node* prev) {
    if (node == nullptr) {
      return prev;
    }
    if (node->value == value) {
      return node;
    }
    if (node->value < value) {
      return LowerBound(value, node->right, prev);
    }
    return LowerBound(value, node->left, node);
  }

  static Node* Insert(Node* node, int value) {
    if (node == nullptr) {
      Node* new_node = new Node;
      new_node->value = value;
      return new_node;
    }
    if (value <= node->value) {
      node->left = Insert(node->left, value);
    } else {
      node->right = Insert(node->right, value);
    }
    return FixBalance(node);
  }

  static void FixHeight(Node* node) {
    if (node == nullptr) {
      return;
    }
    node->height = std::max(Height(node->left), Height(node->right)) + 1;
  }

  static Node* RotateRight(Node* p) {
    Node* q = p->left;
    p->left = q->right;
    q->right = p;
    FixHeight(p);
    FixHeight(q);
    return q;
  }

  static Node* RotateLeft(Node* q) {
    Node* p = q->right;
    q->right = p->left;
    p->left = q;
    FixHeight(q);
    FixHeight(p);
    return p;
  }

  static Node* FixBalance(Node* node) {
    FixHeight(node);
    if (BalanceFactor(node) == 2) {
      if (BalanceFactor(node->right) == -1) {
        node->right = RotateRight(node->right);
      }
      return RotateLeft(node);
    }
    if (BalanceFactor(node) == -2) {
      if (BalanceFactor(node->left) == 1) {
        node->left = RotateLeft(node->left);
      }
      return RotateRight(node);
    }
    return node;
  }

  static int Height(Node* node) { return node == nullptr ? 0 : node->height; }

  static int BalanceFactor(Node* node) {
    return node == nullptr ? 0 : Height(node->right) - Height(node->left);
  }

  Node* root_{nullptr};
};

void Insert(int value, int& prev, AVLTree* tree) {
  value = (value + prev) % Constants::kMod;
  auto res = tree->Find(value);
  if (!res) {
    tree->Insert(value);
  }
  prev = 0;
}

void Find(int value, int& prev, AVLTree* tree) {
  auto result = tree->LowerBound(value, nullptr);
  if (result == nullptr) {
    prev = -1;
    std::cout << -1 << '\n';
    return;
  }
  prev = result->value;
  std::cout << result->value << '\n';
}

void ProcessRequests(AVLTree* tree) {
  size_t requests;
  std::cin >> requests;
  char request;
  int value, prev = 0;
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
  auto avl = new AVLTree();

  ProcessRequests(avl);

  delete avl;
  return 0;
}